#include "stdafx.h"
#include "Game.h"
#include "ConstantBufferOnce.h"
#include "Events/SkillIncreaseEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/CreateAccountFailedEvent.h"
#include "EventHandling/Events/LoginSuccessEvent.h"
#include "EventHandling/Events/LoginFailedEvent.h"
#include "EventHandling/Events/CreateCharacterFailedEvent.h"
#include "EventHandling/Events/CreateCharacterSuccessEvent.h"
#include "EventHandling/Events/DeleteCharacterSuccessEvent.h"
#include "EventHandling/Events/EnterWorldSuccessEvent.h"
#include "EventHandling/Events/NpcUpdateEvent.h"
#include "EventHandling/Events/PlayerUpdateEvent.h"
#include "EventHandling/Events/ActivateAbilityEvent.h"
#include "EventHandling/Events/SetTargetEvent.h"
#include "EventHandling/Events/SendChatMessage.h"
#include "EventHandling/Events/PropagateChatMessageEvent.h"
#include "EventHandling/Events/ServerMessageEvent.h"
#include "Events/DoubleLeftMouseDownEvent.h"

unsigned int g_zIndex{ 0 };

bool CompareUIComponents(UIComponent* a, UIComponent* b) { return (a->zIndex < b->zIndex); }

Game::Game(
	EventHandler& eventHandler,
	ObjectManager& objectManager,
	RenderComponentManager& renderComponentManager,
	StatsComponentManager& statsComponentManager,
	InventoryComponentManager& inventoryComponentManager,
	ClientRepository& clientRepository,
	CommonRepository& commonRepository,
	ClientSocketManager& socketManager)
	: eventHandler{ eventHandler },
	  objectManager{ objectManager },
	  renderComponentManager{ renderComponentManager },
	  statsComponentManager{ statsComponentManager },
	  inventoryComponentManager{ inventoryComponentManager },
	  clientRepository{ clientRepository },
	  commonRepository{ commonRepository },
	  socketManager{ socketManager }
{
	deviceResources = std::make_unique<DX::DeviceResources>();
	deviceResources->RegisterDeviceNotify(this);

	eventHandler.Subscribe(*this);

	InitializeEventHandlers();
}

void Game::PublishEvents()
{
	std::sort(uiComponents.begin(), uiComponents.end(), CompareUIComponents);
	std::queue<std::unique_ptr<const Event>>& eventQueue = eventHandler.GetEventQueue();
	while (!eventQueue.empty())
	{
		auto event = std::move(eventQueue.front());
		eventQueue.pop();

		// We pass events to the UIComponents first, because those are usually overlaid on top
		//   of 3D GameObjects, and therefore we want certain events like clicks, etc to hit
		//   the UIComponents first.
		// These UIComponents are sorted in ascending order of their z-index.
		auto stopPropagation = false;

		for (auto i = (int)uiComponents.size() - 1; i >= 0; i--)
		{
			stopPropagation = uiComponents.at(i)->HandleEvent(event.get());
			if (stopPropagation)
				break;
		}

		// There are times where we want to avoid having events propagate to GameObjects if they're
		//   handled by a UIComponent, so if any UIComponent returns true, we skip passing that event
		//   to the GameObjects and move to the next iteration of the while loop.
		if (stopPropagation)
			continue;

		std::list<Observer*>& observers = eventHandler.GetObservers();
		for (auto observer : observers)
		{
			stopPropagation = observer->HandleEvent(event.get());
			if (stopPropagation)
				break;
		}
	}
}

// General initialization that would likely be shared between any D3D application
//   should go in DeviceResources.cpp
// Game-specific initialization should go in Game.cpp
void Game::Initialize(HWND window, int width, int height)
{
	deviceResources->SetWindow(window, width, height);

	deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	timer.Reset();
	SetActiveLayer(Login);
}

#pragma region Frame Update
void Game::Tick()
{
	timer.Tick();

	socketManager.ProcessPackets();

	// to get an accurate ping, we should handle this outside of the main update loop which is locked at 60 updates / second
	if (activeLayer == InGame)
	{
		if (pingStart == 0.0f)
		{
			pingStart = timer.TotalTime();

			std::vector<std::string> args{ std::to_string(pingId) };
			socketManager.SendPacket(OpCode::Ping, args);
		}
	}

	// reset double click timer if necessary
	if (timer.TotalTime() - doubleClickStart > 0.5f)
		doubleClickStart = 0.0f;
	
	updateTimer += timer.DeltaTime();
	if (updateTimer >= UPDATE_FREQUENCY)
	{
		if (activeLayer == InGame)
		{
			camera.Update(player->GetWorldPosition(), UPDATE_FREQUENCY);
			
			textWindow->Update(); // this should be handled by objectManager.Update()...
			objectManager.Update();
		}
		
		PublishEvents();

		updateTimer -= UPDATE_FREQUENCY;
	}
	
	Render(updateTimer);
}
#pragma endregion

#pragma region Frame Render
void Game::Render(const float updateTimer)
{
	// Don't try to render anything before the first Update.
	if (timer.TotalTime() == 0)
		return;

	Clear();

	auto d3dContext = deviceResources->GetD3DDeviceContext();
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	d2dContext->BeginDraw();

	// used for FPS text
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;
	
	// update FPS text
	if (timer.TotalTime() - timeElapsed >= 1)
	{
		const float mspf = 1000.0f / frameCnt;
		const auto fpsText = "FPS: " + std::to_string(frameCnt) + ", MSPF: " + std::to_string(mspf);
		fpsTextLabel->SetText(fpsText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;

		if (socketManager.Connected())
			socketManager.SendPacket(OpCode::Heartbeat);
	}

	// update ping
	const auto pingText = "Ping: " + std::to_string(ping) + " ms";
	pingTextLabel->SetText(pingText.c_str());

	// update MousePos text
	const auto mousePosText = "MousePosX: " + std::to_string((int)mousePosX) + ", MousePosY: " + std::to_string((int)mousePosY);
	mousePosLabel->SetText(mousePosText.c_str());

	if (activeLayer == InGame)
	{
		const auto camPos{ camera.GetPosition() };
		const XMVECTORF32 s_Eye{ camPos.x, camPos.y, camPos.z, 0.0f };
		const XMVECTORF32 s_At{ camPos.x - 500.0f, 0.0f, camPos.z + 500.0f, 0.0f };
		const XMVECTORF32 s_Up{ 0.0f, 1.0f, 0.0f, 0.0f };
		viewTransform = XMMatrixLookAtLH(s_Eye, s_At, s_Up);

		d3dContext->RSSetState(solidRasterState.Get());
		//d3dContext->RSSetState(wireframeRasterState);

		gameMapRenderComponent->Draw(d3dContext, viewTransform, projectionTransform);

		renderComponentManager.Update(d3dContext, viewTransform, projectionTransform, updateTimer);
	}

	// foreach RenderComponent -> Draw
	for (auto i = 0; i < uiComponents.size(); i++)
		uiComponents.at(i)->Draw();

	d2dContext->EndDraw();

	d3dContext->ResolveSubresource(deviceResources->GetBackBufferRenderTarget(), 0, deviceResources->GetOffscreenRenderTarget(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);

	// Show the new frame.
	deviceResources->Present();
}

void Game::Clear()
{
	// Clear the views.
	auto context = deviceResources->GetD3DDeviceContext();
	auto renderTarget = deviceResources->GetOffscreenRenderTargetView();
	auto depthStencil = deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	const auto viewport = deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);
}
#pragma endregion

#pragma region Message Handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	timer.Reset();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	const auto r = deviceResources->GetOutputSize();
	deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!deviceResources->WindowSizeChanged(width, height))
		return;

	clientWidth = width;
	clientHeight = height;

	abilitiesContainer->clientWidth = static_cast<float>(width);
	abilitiesContainer->clientHeight = static_cast<float>(height);

	CreateWindowSizeDependentResources();

	SetActiveLayer(activeLayer);
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	InitializeBrushes();
	InitializeTextFormats();
	InitializeInputs();
	InitializeLabels();
	InitializeTextures();
	InitializeShaders();
	InitializeBuffers();
	InitializeMeshes();
	InitializeNpcs();
	InitializeItems();
	InitializeStaticObjects();
	InitializeRasterStates();
	InitializeSprites();
	
	auto d3dDevice = deviceResources->GetD3DDevice();
	auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto writeFactory = deviceResources->GetWriteFactory();

	gameMapRenderComponent = std::make_unique<GameMapRenderComponent>(d3dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader.Get(), pixelShader.Get(), textures.at(2).Get());

	// init targetHUD
	targetHUD = std::make_unique<UITargetHUD>(uiComponents, XMFLOAT2{ 260.0f, 12.0f }, InGame, 0, d2dDeviceContext, writeFactory, textFormatSuccessMessage.Get(), d2dFactory, healthBrush.Get(), manaBrush.Get(), staminaBrush.Get(), statBackgroundBrush.Get(), blackBrush.Get(), blackBrush.Get(), whiteBrush.Get());
}

void Game::CreatePlayerDependentResources()
{
	// i think you need to move certain things into here (ui elements that depend on the player existing, etc)
	// you should call this when receiving the EnterWorldSuccess event, and also conditionally when
	// calling CreateDeviceResources, call this IF the player exists (this will happen when resizing the window
	// after entering the game)
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	projectionTransform = XMMatrixOrthographicLH((float)clientWidth, (float)clientHeight, 0.0f, 5000.0f);

	InitializeButtons();
	InitializePanels();

	auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto writeFactory = deviceResources->GetWriteFactory();

	// init hotbar
	hotbar = std::make_unique<UIHotbar>(uiComponents, XMFLOAT2{ 5.0f, clientHeight - 45.0f }, InGame, 0, eventHandler, blackBrush.Get(), d2dDeviceContext, d2dFactory, (float)clientHeight);

	// init textWindow
	textWindow = std::make_unique<UITextWindow>(uiComponents, XMFLOAT2{ 5.0f, clientHeight - 300.0f }, InGame, 0, eventHandler, objectManager, items, textWindowMessages, textWindowMessageIndex.get(), statBackgroundBrush.Get(), blackBrush.Get(), darkGrayBrush.Get(), whiteBrush.Get(), mediumGrayBrush.Get(), blackBrush.Get(), scrollBarBackgroundBrush.Get(), scrollBarBrush.Get(), d2dDeviceContext, writeFactory, textFormatTextWindow.Get(), textFormatTextWindowInactive.Get(), d2dFactory);

	if (skills.size() > 0)
		skillsContainer->Initialize(skills);

	if (abilities.size() > 0)
		InitializeAbilitiesContainer();

	std::sort(uiComponents.begin(), uiComponents.end(), CompareUIComponents);
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();

	SetActiveLayer(activeLayer);
}
#pragma endregion

ShaderBuffer Game::LoadShader(const std::wstring filename)
{
	// load precompiled shaders from .cso objects
	ShaderBuffer sb{ nullptr, 0 };
	byte* fileData{ nullptr };

	// open the file
	std::ifstream csoFile(filename, std::ios::in | std::ios::binary | std::ios::ate);

	if (csoFile.is_open())
	{
		// get shader size
		sb.size = (unsigned int)csoFile.tellg();

		// collect shader data
		fileData = new byte[sb.size];
		csoFile.seekg(0, std::ios::beg);
		csoFile.read(reinterpret_cast<char*>(fileData), sb.size);
		csoFile.close();
		sb.buffer = fileData;
	}
	else
		throw std::exception("Critical error: Unable to open the compiled shader object!");

	return sb;
}

void Game::InitializeNpcs()
{
	npcs = clientRepository.ListNpcs();
}

void Game::InitializeItems()
{
	items = clientRepository.ListItems();
}

void Game::InitializeStaticObjects()
{
	auto staticObjects = commonRepository.ListStaticObjects();

	for (auto i = 0; i < staticObjects.size(); i++)
	{
		const StaticObject* staticObject = staticObjects.at(i).get();
		const auto pos = staticObject->GetPosition();
		GameObject& gameObject = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, 0.0f, GameObjectType::StaticObject, staticObject->GetName(), staticObject->GetId(), true);
		const auto gameObjectId = gameObject.GetId();
		
		const RenderComponent& renderComponent = renderComponentManager.CreateRenderComponent(gameObjectId, meshes.at(staticObject->GetModelId()).get(), vertexShader.Get(), pixelShader.Get(), textures.at(staticObject->GetTextureId()).Get());
		gameObject.renderComponentId = renderComponent.GetId();

		const StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(gameObjectId, 100, 100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10);
		gameObject.statsComponentId = statsComponent.GetId();
		gameMap.SetTileOccupied(gameObject.localPosition, true);
	}	
}

void Game::InitializeBrushes()
{
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f), grayBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f), blackBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), whiteBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.619f, 0.854f, 1.0f, 1.0f), blueBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.301f, 0.729f, 1.0f, 1.0f), darkBlueBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.137f, 0.98f, 0.117f, 1.0f), successMessageBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.98f, 0.117f, 0.156f, 1.0f), errorMessageBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.921f, 1.0f, 0.921f, 1.0f), selectedCharacterBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.9f, 0.9f, 0.9f, 1.0f), lightGrayBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.619f, 0.854f, 1.0f, 0.75f), abilityHighlightBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.619f, 0.854f, 1.0f, 0.95f), abilityPressedBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f), healthBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f), manaBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f), staminaBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.95f, 0.95f, 1.0f), statBackgroundBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.3f, 0.3f, 1.0f), darkGrayBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.6f, 0.6f, 0.6f, 1.0f), mediumGrayBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.85f, 0.85f, 1.0f), scrollBarBackgroundBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.65f, 0.65f, 0.65f, 1.0f), scrollBarBrush.ReleaseAndGetAddressOf());
}

void Game::InitializeTextFormats()
{
	auto arialFontFamily{ L"Arial" };
	auto locale{ L"en-US" };

	auto writeFactory = deviceResources->GetWriteFactory();

	// FPS / MousePos
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, locale, textFormatFPS.ReleaseAndGetAddressOf());
	textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// Account Creds Input Values
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, textFormatAccountCredsInputValue.ReleaseAndGetAddressOf());
	textFormatAccountCredsInputValue->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatAccountCredsInputValue->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// Account Creds Labels
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, textFormatAccountCreds.ReleaseAndGetAddressOf());
	textFormatAccountCreds->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	textFormatAccountCreds->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// Headers
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, textFormatHeaders.ReleaseAndGetAddressOf());
	textFormatHeaders->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatHeaders->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// Button Text
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, textFormatButtonText.ReleaseAndGetAddressOf());
	textFormatButtonText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	textFormatButtonText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// SuccessMessage Text
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, textFormatSuccessMessage.ReleaseAndGetAddressOf());
	textFormatSuccessMessage->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatSuccessMessage->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// ErrorMessage Message
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, textFormatErrorMessage.ReleaseAndGetAddressOf());
	textFormatErrorMessage->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatErrorMessage->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// UITextWindow
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, textFormatTextWindow.ReleaseAndGetAddressOf());
	textFormatTextWindow->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatTextWindow->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// UITextWindow inactive
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, textFormatTextWindowInactive.ReleaseAndGetAddressOf());
	textFormatTextWindowInactive->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatTextWindowInactive->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

void Game::InitializeInputs()
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	// LoginScreen
	loginScreen_accountNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 20.0f }, Login, 0, false, 120.0f, 260.0f, 24.0f, "Account Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	loginScreen_passwordInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 50.0f }, Login, 0, true, 120.0f, 260.0f, 24.0f, "Password:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	loginScreen_inputGroup = std::make_unique<UIInputGroup>(Login, eventHandler);
	loginScreen_inputGroup->AddInput(loginScreen_accountNameInput.get());
	loginScreen_inputGroup->AddInput(loginScreen_passwordInput.get());

	// CreateAccount
	createAccount_accountNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 20.0f }, CreateAccount, 0, false, 120.0f, 260.0f, 24.0f, "Account Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_passwordInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 50.0f  }, CreateAccount, 0, true, 120.0f, 260.0f, 24.0f, "Password:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_inputGroup = std::make_unique<UIInputGroup>(CreateAccount, eventHandler);
	createAccount_inputGroup->AddInput(createAccount_accountNameInput.get());
	createAccount_inputGroup->AddInput(createAccount_passwordInput.get());

	// CreateCharacter
	createCharacter_characterNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 20.0f }, CreateCharacter, 0, false, 140.0f, 260.0f, 24.0f, "Character Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createCharacter_inputGroup = std::make_unique<UIInputGroup>(CreateCharacter, eventHandler);
	createCharacter_inputGroup->AddInput(createCharacter_characterNameInput.get());
}

void Game::InitializeButtons()
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dContext = deviceResources->GetD2DDeviceContext();
	
	const auto onClickLoginButton = [this]()
	{
		loginScreen_successMessageLabel->SetText("");
		loginScreen_errorMessageLabel->SetText("");

		const auto accountName = Utility::ws2s(std::wstring(loginScreen_accountNameInput->GetInputValue()));
		const auto password = Utility::ws2s(std::wstring(loginScreen_passwordInput->GetInputValue()));

		if (accountName.length() == 0)
		{
			loginScreen_errorMessageLabel->SetText("Username field can't be empty.");
			return;
		}
		if (password.length() == 0)
		{
			loginScreen_errorMessageLabel->SetText("Password field can't be empty.");
			return;
		}

		std::vector<std::string> args{ accountName, password };
		socketManager.SendPacket(OpCode::Connect, args);
		SetActiveLayer(Connecting);
	};

	const auto onClickLoginScreenCreateAccountButton = [this]()
	{
		loginScreen_successMessageLabel->SetText("");
		loginScreen_errorMessageLabel->SetText("");
		SetActiveLayer(CreateAccount);
	};

	const auto onClickLoginScreeQuitGameButton = [this]()
	{
		QuitGame();
	};

	// LoginScreen
	loginScreen_loginButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 145.0f, 96.0f }, Login, 0, 80.0f, 24.0f, "LOGIN", onClickLoginButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	loginScreen_createAccountButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 15.0f, clientHeight - 40.0f }, Login, 0, 160.0f, 24.0f, "CREATE ACCOUNT", onClickLoginScreenCreateAccountButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	loginScreen_quitGameButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ clientWidth - 95.0f, clientHeight - 40.0f }, Login, 0, 80.0f, 24.0f, "QUIT", onClickLoginScreeQuitGameButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);

	const auto onClickCreateAccountCreateAccountButton = [this]()
	{
		createAccount_errorMessageLabel->SetText("");

		const auto accountName = Utility::ws2s(std::wstring(createAccount_accountNameInput->GetInputValue()));
		const auto password = Utility::ws2s(std::wstring(createAccount_passwordInput->GetInputValue()));

		if (accountName.length() == 0)
		{
			createAccount_errorMessageLabel->SetText("Username field can't be empty.");
			return;
		}
		if (password.length() == 0)
		{
			createAccount_errorMessageLabel->SetText("Password field can't be empty.");
			return;
		}

		std::vector<std::string> args{ accountName, password };
		socketManager.SendPacket(OpCode::CreateAccount, args);
	};

	const auto onClickCreateAccountCancelButton = [this]()
	{
		createAccount_errorMessageLabel->SetText("");
		SetActiveLayer(Login);
	};

	// CreateAccount
	createAccount_createAccountButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 145.0f, 96.0f }, CreateAccount, 0, 80.0f, 24.0f, "CREATE", onClickCreateAccountCreateAccountButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	createAccount_cancelButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 15.0f, clientHeight - 40.0f }, CreateAccount, 0, 80.0f, 24.0f, "CANCEL", onClickCreateAccountCancelButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);

	const auto onClickCharacterSelectNewCharacterButton = [this]()
	{
		characterSelect_successMessageLabel->SetText("");

		if (characterList.size() == 5)
			characterSelect_errorMessageLabel->SetText("You can not have more than 5 characters.");
		else
			SetActiveLayer(CreateCharacter);
	};

	const auto onClickCharacterSelectEnterWorldButton = [this]()
	{
		characterSelect_successMessageLabel->SetText("");
		characterSelect_errorMessageLabel->SetText("");

		const auto* const characterInput = GetCurrentlySelectedCharacterListing();
		if (characterInput == nullptr)
			characterSelect_errorMessageLabel->SetText("You must select a character before entering the game.");
		else
		{
			std::vector<std::string> args{ characterInput->GetName() };
			socketManager.SendPacket(OpCode::EnterWorld, args);
			SetActiveLayer(EnteringWorld);
		}
	};

	const auto onClickCharacterSelectDeleteCharacterButton = [this]()
	{
		characterSelect_successMessageLabel->SetText("");
		characterSelect_errorMessageLabel->SetText("");

		const auto* const characterInput = GetCurrentlySelectedCharacterListing();
		if (characterInput == nullptr)
			characterSelect_errorMessageLabel->SetText("You must select a character to delete.");
		else
		{
			characterNamePendingDeletion = characterInput->GetName();
			SetActiveLayer(DeleteCharacter);
		}
	};

	const auto onClickCharacterSelectLogoutButton = [this]()
	{
		socketManager.Logout();

		SetActiveLayer(Login);
	};

	// CharacterSelect
	characterSelect_newCharacterButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 15.0f, 20.0f }, CharacterSelect, 0, 140.0f, 24.0f, "NEW CHARACTER", onClickCharacterSelectNewCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_enterWorldButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 170.0f, 20.0f }, CharacterSelect, 0, 120.0f, 24.0f, "ENTER WORLD", onClickCharacterSelectEnterWorldButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_deleteCharacterButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 305.0f, 20.0f }, CharacterSelect, 0, 160.0f, 24.0f, "DELETE CHARACTER", onClickCharacterSelectDeleteCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_logoutButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 15.0f, clientHeight - 40.0f }, CharacterSelect, 0, 80.0f, 24.0f, "LOGOUT", onClickCharacterSelectLogoutButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext,  writeFactory, textFormatButtonText.Get(), d2dFactory);

	const auto onClickCreateCharacterCreateCharacterButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");

		const auto characterName = Utility::ws2s(std::wstring(createCharacter_characterNameInput->GetInputValue()));

		if (characterName.length() == 0)
		{
			createCharacter_errorMessageLabel->SetText("Character name can't be empty.");
			return;
		}

		std::vector<std::string> args{ characterName };
		socketManager.SendPacket(OpCode::CreateCharacter, args);
	};

	const auto onClickCreateCharacterBackButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");
		SetActiveLayer(CharacterSelect);
	};

	// CreateCharacter
	createCharacter_createCharacterButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 165.0f, 64.0f }, CreateCharacter, 0, 160.0f, 24.0f, "CREATE CHARACTER", onClickCreateCharacterCreateCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	createCharacter_backButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 15.0f, clientHeight - 40.0f }, CreateCharacter, 0, 80.0f, 24.0f, "BACK", onClickCreateCharacterBackButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);

	// DeleteCharacter
	const auto onClickDeleteCharacterConfirm = [this]()
	{
		std::vector<std::string> args{ characterNamePendingDeletion };
		socketManager.SendPacket(OpCode::DeleteCharacter, args);
	};

	const auto onClickDeleteCharacterCancel = [this]()
	{
		characterNamePendingDeletion = "";
		SetActiveLayer(CharacterSelect);
	};

	deleteCharacter_confirmButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 10.0f, 30.0f }, DeleteCharacter, 0, 100.0f, 24.0f, "CONFIRM", onClickDeleteCharacterConfirm, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext,  writeFactory, textFormatButtonText.Get(), d2dFactory);
	deleteCharacter_cancelButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 120.0f, 30.0f }, DeleteCharacter, 0, 100.0f, 24.0f, "CANCEL", onClickDeleteCharacterCancel, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
}

void Game::InitializeLabels()
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	loginScreen_successMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 30.0f, 170.0f }, Login, 0, 400.0f, successMessageBrush.Get(), textFormatSuccessMessage.Get(), d2dContext, writeFactory, d2dFactory);
	loginScreen_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 30.0f, 170.0f }, Login, 0, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	createAccount_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 30.0f, 170.0f }, CreateAccount, 0, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	connecting_statusLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 15.0f, 20.0f }, Connecting, 0, 400.0f, blackBrush.Get(), textFormatAccountCreds.Get(), d2dContext, writeFactory, d2dFactory);
	connecting_statusLabel->SetText("Connecting...");

	characterSelect_successMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 30.0f, 400.0f }, CharacterSelect, 0, 400.0f, successMessageBrush.Get(), textFormatSuccessMessage.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 30.0f, 400.0f }, CharacterSelect, 0, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_headerLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 15.0f, 60.0f }, CharacterSelect, 0, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_headerLabel->SetText("Character List:");

	createCharacter_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 30.0f, 170.0f }, CreateCharacter, 0, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	deleteCharacter_headerLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 10.0f, 10.0f }, DeleteCharacter, 0, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);
	deleteCharacter_headerLabel->SetText("Are you sure you want to delete this character?");

	enteringWorld_statusLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 5.0f, 20.0f }, EnteringWorld, 0, 400.0f, blackBrush.Get(), textFormatAccountCreds.Get(), d2dContext, writeFactory, d2dFactory);
	enteringWorld_statusLabel->SetText("Entering World...");
}

void Game::InitializePanels()
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	auto d3dDevice = deviceResources->GetD3DDevice();
	auto d3dDeviceContext = deviceResources->GetD3DDeviceContext();

	// Game Settings
	const auto gameSettingsPanelX{ (clientWidth - 400.0f) / 2.0f };
	const auto gameSettingsPanelY{ (clientHeight - 200.0f) / 2.0f };
	const auto onClickGameSettingsLogoutButton = [this]()
	{
		socketManager.SendPacket(OpCode::Disconnect);
		socketManager.Logout();
		SetActiveLayer(Login);
	};
	gameSettingsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ gameSettingsPanelX, gameSettingsPanelY }, InGame, 1, eventHandler, false, 400.0f, 200.0f, VK_ESCAPE, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dDeviceContext, d2dFactory);
	gameSettings_logoutButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 10.0f, 26.0f }, InGame, 2, 80.0f, 24.0f, "LOGOUT", onClickGameSettingsLogoutButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dDeviceContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	gameSettingsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 2, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	gameSettingsPanelHeader->SetText("Game Settings");
	gameSettingsPanel->AddChildComponent(*gameSettingsPanelHeader);
	gameSettingsPanel->AddChildComponent(*gameSettings_logoutButton);

	// Game Editor
	const auto gameEditorPanelX{ 580.0f };
	const auto gameEditorPanelY{ 5.0f };
	gameEditorPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ gameEditorPanelX, gameEditorPanelY }, InGame, 1, eventHandler, true, 200.0f, 400.0f, VK_F1, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dDeviceContext, d2dFactory);
	gameEditorPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 2, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	gameEditorPanelHeader->SetText("Game Editor");
	gameEditorPanel->AddChildComponent(*gameEditorPanelHeader);

	// Diagnostics
	const auto diagnosticsPanelX{ 580.0f };
	const auto diagnosticsPanelY{ 336.0f };
	diagnosticsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ diagnosticsPanelX, diagnosticsPanelY }, InGame, 1, eventHandler, true, 200.0f, 200.0f, VK_F2, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dDeviceContext, d2dFactory);

	diagnosticsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	diagnosticsPanelHeader->SetText("Diagnostics");
	diagnosticsPanel->AddChildComponent(*diagnosticsPanelHeader);

	mousePosLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 22.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*mousePosLabel);

	fpsTextLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 36.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*fpsTextLabel);

	pingTextLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 50.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*pingTextLabel);

	// Skills
	const auto skillsPanelX{ 200.0f };
	const auto skillsPanelY{ 200.0f };
	skillsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ skillsPanelX, skillsPanelY }, InGame, 1, eventHandler, true, 200.0f, 200.0f, VK_F3, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dDeviceContext, d2dFactory);

	skillsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 3, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	skillsPanelHeader->SetText("Skills");
	skillsPanel->AddChildComponent(*skillsPanelHeader);

	skillsContainer = std::make_unique<UISkillsContainer>(uiComponents, XMFLOAT2{ 0.0f, 0.0f }, InGame, 2, d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, writeFactory, blackBrush.Get(), textFormatFPS.Get());
	skillsPanel->AddChildComponent(*skillsContainer);

	// Abilities
	const auto abilitiesPanelX{ 10.0f };
	const auto abilitiesPanelY{ 10.0f };
	abilitiesPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ abilitiesPanelX, abilitiesPanelY }, InGame, 1, eventHandler, true, 240.0f, 400.0f, VK_F4, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dDeviceContext, d2dFactory);

	abilitiesPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 3, 240.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	abilitiesPanelHeader->SetText("Abilities");
	abilitiesPanel->AddChildComponent(*abilitiesPanelHeader);

	abilitiesContainer = std::make_unique<UIAbilitiesContainer>(uiComponents, XMFLOAT2{ 0.0f, 0.0f }, InGame, 2, eventHandler, d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, writeFactory, blackBrush.Get(), abilityHighlightBrush.Get(), blackBrush.Get(), abilityPressedBrush.Get(), errorMessageBrush.Get(), textFormatHeaders.Get(), spriteVertexShader.Get(), spritePixelShader.Get(), spriteVertexShaderBuffer.buffer, spriteVertexShaderBuffer.size, projectionTransform, (float)clientWidth, (float)clientHeight);
	abilitiesPanel->AddChildComponent(*abilitiesContainer);

	// Loot
	const auto lootPanelX{ 400.0f };
	const auto lootPanelY{ 300.0f };
	lootPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ lootPanelX, lootPanelY }, InGame, 1, eventHandler, true, 140.0f, 185.0f, 0, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dDeviceContext, d2dFactory);

	lootPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 3, 140.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	lootPanelHeader->SetText("Loot");
	lootPanel->AddChildComponent(*lootPanelHeader);

	lootContainer = std::make_unique<UILootContainer>(uiComponents, XMFLOAT2{ 0.0f, 0.0f }, InGame, 2, eventHandler, socketManager, statsComponentManager, inventoryComponentManager, items, textures, blackBrush.Get(), d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, abilityHighlightBrush.Get(), spriteVertexShader.Get(), spritePixelShader.Get(), spriteVertexShaderBuffer.buffer, spriteVertexShaderBuffer.size, projectionTransform, (float)clientWidth, (float)clientHeight);
	lootPanel->AddChildComponent(*lootContainer);

	// Inventory
	const auto inventoryPanelX{ 500.0f };
	const auto inventoryPanelY{ 350.0f };
	inventoryPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ lootPanelX, lootPanelY }, InGame, 1, eventHandler, true, 185.0f, 185.0f, VK_F5, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dDeviceContext, d2dFactory);

	inventoryPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 3, 185.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dDeviceContext, writeFactory, d2dFactory);
	inventoryPanelHeader->SetText("Inventory");
	inventoryPanel->AddChildComponent(*inventoryPanelHeader);

	inventory = std::make_unique<UIInventory>(uiComponents, XMFLOAT2{ 0.0f, 0.0f }, InGame, 2, eventHandler, socketManager, items, textures, blackBrush.Get(), d2dDeviceContext, d2dFactory, d3dDevice, d3dDeviceContext, abilityHighlightBrush.Get(), spriteVertexShader.Get(), spritePixelShader.Get(), spriteVertexShaderBuffer.buffer, spriteVertexShaderBuffer.size, projectionTransform, (float)clientWidth, (float)clientHeight);
	inventoryPanel->AddChildComponent(*inventory);

	if (player)
		inventory->playerId = player->GetId();
}

UICharacterListing* Game::GetCurrentlySelectedCharacterListing()
{
	for (auto i = 0; i < characterList.size(); i++)
	{
		if (characterList.at(i)->IsSelected())
			return characterList.at(i).get();
	}
	return nullptr;
}

void Game::InitializeShaders()
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	vertexShaderBuffer = LoadShader(L"VertexShader.cso");
	d3dDevice->CreateVertexShader(vertexShaderBuffer.buffer, vertexShaderBuffer.size, nullptr, vertexShader.ReleaseAndGetAddressOf());

	pixelShaderBuffer = LoadShader(L"PixelShader.cso");
	d3dDevice->CreatePixelShader(pixelShaderBuffer.buffer, pixelShaderBuffer.size, nullptr, pixelShader.ReleaseAndGetAddressOf());

	spriteVertexShaderBuffer = LoadShader(L"SpriteVertexShader.cso");
	d3dDevice->CreateVertexShader(spriteVertexShaderBuffer.buffer, spriteVertexShaderBuffer.size, nullptr, spriteVertexShader.ReleaseAndGetAddressOf());

	spritePixelShaderBuffer = LoadShader(L"SpritePixelShader.cso");
	d3dDevice->CreatePixelShader(spritePixelShaderBuffer.buffer, spritePixelShaderBuffer.size, nullptr, spritePixelShader.ReleaseAndGetAddressOf());
}

void Game::InitializeBuffers()
{
	auto d3dDevice = deviceResources->GetD3DDevice();
	auto d3dContext = deviceResources->GetD3DDeviceContext();

	// create constant buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.MiscFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(ConstantBufferOnce);

	ID3D11Buffer* constantBufferOnce{ nullptr };
	DX::ThrowIfFailed(d3dDevice->CreateBuffer(&bufferDesc, nullptr, &constantBufferOnce));

	// map ConstantBuffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	d3dContext->Map(constantBufferOnce, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto pCB{ reinterpret_cast<ConstantBufferOnce*>(mappedResource.pData) };
	XMStoreFloat4(&pCB->directionalLight, XMVECTOR{ 0.0f, -1.0f, 0.5f, 0.0f });
	d3dContext->Unmap(constantBufferOnce, 0);

	d3dContext->PSSetConstantBuffers(0, 1, &constantBufferOnce);
}

void Game::InitializeRasterStates()
{
	auto d3dDevice{ deviceResources->GetD3DDevice() };

	CD3D11_RASTERIZER_DESC wireframeRasterStateDesc{ D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE };
	d3dDevice->CreateRasterizerState(&wireframeRasterStateDesc, wireframeRasterState.ReleaseAndGetAddressOf());

	CD3D11_RASTERIZER_DESC solidRasterStateDesc{ D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE };
	d3dDevice->CreateRasterizerState(&solidRasterStateDesc, solidRasterState.ReleaseAndGetAddressOf());
}

void Game::InitializeTextures()
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	const std::vector<const wchar_t*> paths
	{
		L"../../WrenClient/Textures/texture01.dds",     // 0
		L"../../WrenClient/Textures/texture02.dds",     // 1
		L"../../WrenClient/Textures/grass01.dds",       // 2
		L"../../WrenClient/Textures/abilityicon01.dds", // 3
		L"../../WrenClient/Textures/texture03.dds",     // 4
		L"../../WrenClient/Textures/abilityicon02.dds", // 5
		L"../../WrenClient/Textures/abilityicon03.dds", // 6
		L"../../WrenClient/Textures/jade.dds",          // 7
		L"../../WrenClient/Textures/jade_gray.dds",     // 8
		L"../../WrenClient/Textures/ruby.dds",          // 9
		L"../../WrenClient/Textures/ruby_gray.dds",     // 10
		L"../../WrenClient/Textures/sapphire.dds",      // 11
		L"../../WrenClient/Textures/sapphire_gray.dds", // 12
	};

	// clear calls the destructor of its elements, and ComPtr's destructor handles calling Release()
	textures.clear();

	for (auto i = 0; i < paths.size(); i++)
	{
		ComPtr<ID3D11ShaderResourceView> ptr;
		CreateDDSTextureFromFile(d3dDevice, paths.at(i), nullptr, ptr.ReleaseAndGetAddressOf());
		textures.push_back(ptr);
	}
}

void Game::InitializeMeshes()
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	const std::vector<std::string> paths
	{
		"../../WrenClient/Models/sphere.blend",  // 0
		"../../WrenClient/Models/tree.blend",    // 1
		"../../WrenClient/Models/dummy.blend"    // 2
	};

	// clear calls the destructor of its elements, and unique_ptr's destructor handles cleaning itself up
	meshes.clear();

	for (auto i = 0; i < paths.size(); i++)
		meshes.push_back(std::make_unique<Mesh>(paths.at(i), d3dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size));
}

void Game::InitializeSprites()
{
	auto d3dDevice = deviceResources->GetD3DDevice();
	
	// TODO
}

void Game::RecreateCharacterListings(const std::vector<std::unique_ptr<std::string>>& characterNames)
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	characterList.clear();

	for (auto i = 0; i < characterNames.size(); i++)
	{
		characterList.push_back(std::make_unique<UICharacterListing>(uiComponents, XMFLOAT2{ 25.0f, 100.0f + (i * 40.0f) }, CharacterSelect, 1, eventHandler, 260.0f, 30.0f, characterNames.at(i)->c_str(), whiteBrush.Get(), selectedCharacterBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatAccountCredsInputValue.Get(), d2dFactory));
	}
}

const bool Game::HandleEvent(const Event* const event)
{
	const auto fun = eventHandlers[event->type];
	if (fun)
		fun(event);

	return false;
}

void Game::SetActiveLayer(const Layer layer)
{
	activeLayer = layer;

	std::unique_ptr<Event> e = std::make_unique<ChangeActiveLayerEvent>(layer);
	eventHandler.QueueEvent(e);
}

Game::~Game()
{
	eventHandler.Unsubscribe(*this);
}

void Game::InitializeAbilitiesContainer()
{
	abilitiesContainer->ClearAbilities();
	for (auto i = 0; i < abilities.size(); i++)
	{
		Ability* ability = abilities.at(i).get();
		abilitiesContainer->AddAbility(ability, textures.at(ability->spriteId).Get());
	}
}

void Game::QuitGame()
{
	DestroyWindow(deviceResources->GetWindow());
}

void Game::OnPong(unsigned int pingId)
{
	const auto delta = timer.TotalTime() - pingStart;
	ping = static_cast<int>(std::round(delta * 1000));
	pingStart = 0.0f;
	this->pingId++;
}

void Game::InitializeEventHandlers()
{
	eventHandlers[EventType::RightMouseDown] = [this](const Event* const event)
	{
		if (activeLayer != Layer::InGame)
			return;

		const auto derivedEvent = (MouseEvent*)event;

		const auto dir = Utility::MousePosToDirection(static_cast<float>(clientWidth), static_cast<float>(clientHeight), derivedEvent->mousePosX, derivedEvent->mousePosY);
		std::vector<std::string> args{ std::to_string(dir.x), std::to_string(dir.y), std::to_string(dir.z) };
		socketManager.SendPacket(OpCode::PlayerRightMouseDown, args);

		rightMouseDownDir = dir;
	};

	eventHandlers[EventType::RightMouseUp] = [this](const Event* const event)
	{
		if (activeLayer != Layer::InGame)
			return;

		socketManager.SendPacket(OpCode::PlayerRightMouseUp);

		rightMouseDownDir = VEC_ZERO;
	};

	eventHandlers[EventType::MouseMove] = [this](const Event* const event)
	{
		const auto derivedEvent = (MouseEvent*)event;

		mousePosX = derivedEvent->mousePosX;
		mousePosY = derivedEvent->mousePosY;

		if (activeLayer == Layer::InGame && rightMouseDownDir != VEC_ZERO)
		{
			const auto dir = Utility::MousePosToDirection(static_cast<float>(clientWidth), static_cast<float>(clientHeight), derivedEvent->mousePosX, derivedEvent->mousePosY);
			if (dir != rightMouseDownDir)
			{
				std::vector<std::string> args{ std::to_string(dir.x), std::to_string(dir.y), std::to_string(dir.z) };
				socketManager.SendPacket(OpCode::PlayerRightMouseDirChange, args);
				rightMouseDownDir = dir;
			}
		}
	};

	eventHandlers[EventType::CreateAccountFailed] = [this](const Event* const event)
	{
		const auto derivedEvent = (CreateAccountFailedEvent*)event;

		createAccount_errorMessageLabel->SetText(("Failed to create account. Reason: " + derivedEvent->error).c_str());
	};

	eventHandlers[EventType::CreateAccountSuccess] = [this](const Event* const event)
	{
		createAccount_errorMessageLabel->SetText("");
		loginScreen_successMessageLabel->SetText("Account created successfully.");
		SetActiveLayer(Login);
	};

	eventHandlers[EventType::LoginFailed] = [this](const Event* const event)
	{
		const auto derivedEvent = (LoginFailedEvent*)event;

		loginScreen_errorMessageLabel->SetText(("Login failed. Reason: " + derivedEvent->error).c_str());
		SetActiveLayer(Login);
	};

	eventHandlers[EventType::LoginSuccess] = [this](const Event* const event)
	{
		const auto derivedEvent = (LoginSuccessEvent*)event;

		RecreateCharacterListings(derivedEvent->characterList);
		SetActiveLayer(CharacterSelect);
	};

	eventHandlers[EventType::CreateCharacterFailed] = [this](const Event* const event)
	{
		const auto derivedEvent = (CreateCharacterFailedEvent*)event;

		createCharacter_errorMessageLabel->SetText(("Character creation failed. Reason: " + derivedEvent->error).c_str());
	};

	eventHandlers[EventType::CreateCharacterSuccess] = [this](const Event* const event)
	{
		const auto derivedEvent = (CreateCharacterSuccessEvent*)event;

		RecreateCharacterListings(derivedEvent->characterList);
		createCharacter_errorMessageLabel->SetText("");
		characterSelect_successMessageLabel->SetText("Character created successfully.");
		SetActiveLayer(CharacterSelect);
	};

	eventHandlers[EventType::DeleteCharacterSuccess] = [this](const Event* const event)
	{
		const auto derivedEvent = (DeleteCharacterSuccessEvent*)event;

		RecreateCharacterListings(derivedEvent->characterList);
		characterNamePendingDeletion = "";
		createCharacter_errorMessageLabel->SetText("");
		characterSelect_successMessageLabel->SetText("Character deleted successfully.");
		SetActiveLayer(CharacterSelect);
	};

	eventHandlers[EventType::EnterWorldSuccess] = [this](const Event* const event)
	{
		const auto derivedEvent = (EnterWorldSuccessEvent*)event;

		const auto agility = derivedEvent->agility;
		const auto strength = derivedEvent->strength;
		const auto wisdom = derivedEvent->wisdom;
		const auto intelligence = derivedEvent->intelligence;
		const auto charisma = derivedEvent->charisma;
		const auto luck = derivedEvent->luck;
		const auto endurance = derivedEvent->endurance;
		const auto health = derivedEvent->health;
		const auto maxHealth = derivedEvent->maxHealth;
		const auto mana = derivedEvent->mana;
		const auto maxMana = derivedEvent->maxMana;
		const auto stamina = derivedEvent->stamina;
		const auto maxStamina = derivedEvent->maxStamina;

		GameObject& player = objectManager.CreateGameObject(derivedEvent->position, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, PLAYER_SPEED, GameObjectType::Player, derivedEvent->name, derivedEvent->accountId);
		const auto playerId = player.GetId();
		this->player = &player;

		RenderComponent& sphereRenderComponent = renderComponentManager.CreateRenderComponent(playerId, meshes[derivedEvent->modelId].get(), vertexShader.Get(), pixelShader.Get(), textures[derivedEvent->textureId].Get());
		player.renderComponentId = sphereRenderComponent.GetId();
		
		StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(playerId, agility, strength, wisdom, intelligence, charisma, luck, endurance, health, maxHealth, mana, maxMana, stamina, maxStamina);
		player.statsComponentId = statsComponent.GetId();

		InventoryComponent& inventoryComponent = inventoryComponentManager.CreateInventoryComponent(playerId);
		player.inventoryComponentId = inventoryComponent.GetId();

		gameMap.SetTileOccupied(player.localPosition, true);

		auto d2dFactory = deviceResources->GetD2DFactory();
		auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
		auto writeFactory = deviceResources->GetWriteFactory();

		if (skills.size() > 0)
			skills.clear();
		skills = std::move(derivedEvent->skills);
		skillsContainer->Initialize(skills);

		if (abilities.size() > 0)
			abilities.clear();
		abilities = std::move(derivedEvent->abilities);
		InitializeAbilitiesContainer();

		inventory->playerId = player.GetId();

		// init characterHUD
		characterHUD = std::make_unique<UICharacterHUD>(uiComponents, XMFLOAT2{ 10.0f, 12.0f }, InGame, 0, d2dDeviceContext, writeFactory, textFormatSuccessMessage.Get(), d2dFactory, statsComponent, healthBrush.Get(), manaBrush.Get(), staminaBrush.Get(), statBackgroundBrush.Get(), blackBrush.Get(), blackBrush.Get(), whiteBrush.Get(), derivedEvent->name.c_str());

		std::sort(uiComponents.begin(), uiComponents.end(), CompareUIComponents);

		textWindow->AddMessage("Welcome to Wren!");

		SetActiveLayer(InGame);
	};

	eventHandlers[EventType::NpcUpdate] = [this](const Event* const event)
	{
		const auto derivedEvent = (NpcUpdateEvent*)event;

		const auto gameObjectId = derivedEvent->gameObjectId;
		const auto type = derivedEvent->type;
		const auto pos = derivedEvent->pos;
		const auto mov = derivedEvent->mov;
		const auto agility = derivedEvent->agility;
		const auto strength = derivedEvent->strength;
		const auto wisdom = derivedEvent->wisdom;
		const auto intelligence = derivedEvent->intelligence;
		const auto charisma = derivedEvent->charisma;
		const auto luck = derivedEvent->luck;
		const auto endurance = derivedEvent->endurance;
		const auto health = derivedEvent->health;
		const auto maxHealth = derivedEvent->maxHealth;
		const auto mana = derivedEvent->mana;
		const auto maxMana = derivedEvent->maxMana;
		const auto stamina = derivedEvent->stamina;
		const auto maxStamina = derivedEvent->maxStamina;

		auto npc = find_if(npcs.begin(), npcs.end(), [&gameObjectId](std::unique_ptr<Npc>& npc) { return npc->GetId() == gameObjectId; });
		const auto modelId = npc->get()->GetModelId();
		const auto textureId = npc->get()->GetTextureId();
		const auto name = npc->get()->GetName();
		const auto speed = npc->get()->GetSpeed();

		if (!objectManager.GameObjectExists(gameObjectId))
		{
			GameObject& obj = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, speed, GameObjectType::Npc, name, gameObjectId);
			obj.movementVector = mov;
			const RenderComponent& sphereRenderComponent = renderComponentManager.CreateRenderComponent(gameObjectId, meshes[modelId].get(), vertexShader.Get(), pixelShader.Get(), textures[textureId].Get());
			obj.renderComponentId = sphereRenderComponent.GetId();

			const StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(gameObjectId, agility, strength, wisdom, intelligence, charisma, luck, endurance, health, maxHealth, mana, maxMana, stamina, maxStamina);
			obj.statsComponentId = statsComponent.GetId();

			const InventoryComponent& inventoryComponent = inventoryComponentManager.CreateInventoryComponent(gameObjectId);
			obj.inventoryComponentId = inventoryComponent.GetId();

			gameMap.SetTileOccupied(obj.localPosition, true);
		}
		else
		{
			GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			gameObject.localPosition = pos;
			gameObject.movementVector = mov;

			StatsComponent& statsComponent = statsComponentManager.GetComponentById(gameObject.statsComponentId);
			statsComponent.agility = agility;
			statsComponent.strength = strength;
			statsComponent.wisdom = wisdom;
			statsComponent.intelligence = intelligence;
			statsComponent.charisma = charisma;
			statsComponent.luck = luck;
			statsComponent.endurance = endurance;
			statsComponent.health = health;
			statsComponent.maxHealth = maxHealth;
			statsComponent.mana = mana;
			statsComponent.maxMana = maxMana;
			statsComponent.stamina = stamina;
			statsComponent.maxStamina = maxStamina;
		}
	};

	eventHandlers[EventType::PlayerUpdate] = [this](const Event* const event)
	{
		const auto derivedEvent = (PlayerUpdateEvent*)event;

		const auto gameObjectId = derivedEvent->accountId;
		const auto type = derivedEvent->type;
		const auto pos = derivedEvent->pos;
		const auto mov = derivedEvent->mov;
		const auto modelId = derivedEvent->modelId;
		const auto textureId = derivedEvent->textureId;
		const auto name = derivedEvent->name;
		const auto agility = derivedEvent->agility;
		const auto strength = derivedEvent->strength;
		const auto wisdom = derivedEvent->wisdom;
		const auto intelligence = derivedEvent->intelligence;
		const auto charisma = derivedEvent->charisma;
		const auto luck = derivedEvent->luck;
		const auto endurance = derivedEvent->endurance;
		const auto health = derivedEvent->health;
		const auto maxHealth = derivedEvent->maxHealth;
		const auto mana = derivedEvent->mana;
		const auto maxMana = derivedEvent->maxMana;
		const auto stamina = derivedEvent->stamina;
		const auto maxStamina = derivedEvent->maxStamina;

		if (!objectManager.GameObjectExists(gameObjectId))
		{
			GameObject& obj = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, PLAYER_SPEED, GameObjectType::Player, name, gameObjectId);
			obj.movementVector = mov;
			const RenderComponent& sphereRenderComponent = renderComponentManager.CreateRenderComponent(gameObjectId, meshes.at(modelId).get(), vertexShader.Get(), pixelShader.Get(), textures.at(textureId).Get());
			obj.renderComponentId = sphereRenderComponent.GetId();

			const StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(gameObjectId, agility, strength, wisdom, intelligence, charisma, luck, endurance, health, maxHealth, mana, maxMana, stamina, maxStamina);
			obj.statsComponentId = statsComponent.GetId();

			const InventoryComponent& inventoryComponent = inventoryComponentManager.CreateInventoryComponent(gameObjectId);
			obj.inventoryComponentId = inventoryComponent.GetId();
		}
		else
		{
			GameObject& obj = objectManager.GetGameObjectById(derivedEvent->accountId);
			obj.localPosition = pos;
			obj.movementVector = mov;

			StatsComponent& statsComponent = statsComponentManager.GetComponentById(obj.statsComponentId);
			statsComponent.agility = agility;
			statsComponent.strength = strength;
			statsComponent.wisdom = wisdom;
			statsComponent.intelligence = intelligence;
			statsComponent.charisma = charisma;
			statsComponent.luck = luck;
			statsComponent.endurance = endurance;
			statsComponent.health = health;
			statsComponent.maxHealth = maxHealth;
			statsComponent.mana = mana;
			statsComponent.maxMana = maxMana;
			statsComponent.stamina = stamina;
			statsComponent.maxStamina = maxStamina;
		}
	};

	eventHandlers[EventType::ActivateAbility] = [this](const Event* const event)
	{
		const auto derivedEvent = (ActivateAbilityEvent*)event;

		std::vector<std::string> args{ std::to_string(derivedEvent->abilityId) };
		socketManager.SendPacket(OpCode::ActivateAbility, args);
	};

	eventHandlers[EventType::ReorderUIComponents] = [this](const Event* const event)
	{
		std::sort(uiComponents.begin(), uiComponents.end(), CompareUIComponents);
	};

	eventHandlers[EventType::LeftMouseDown] = [this](const Event* const event)
	{
		if (activeLayer != Layer::InGame)
			return;

		const auto derivedEvent = (MouseEvent*)event;

		GameObject* clickedGameObject{ nullptr };
		float smallestDist = FLT_MAX;

		auto gameObjects = objectManager.GetGameObjects();
		for (auto j = 0; j < objectManager.GetGameObjectIndex(); j++)
		{
			GameObject& gameObject = gameObjects[j];
			const auto pos = gameObject.GetWorldPosition();
			const auto scale = gameObject.scale;
			const auto worldTransform = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);

			XMVECTOR roScreen = XMVectorSet(derivedEvent->mousePosX, derivedEvent->mousePosY, 0.0f, 1.0f);
			XMVECTOR rdScreen = XMVectorSet(derivedEvent->mousePosX, derivedEvent->mousePosY, 1.0f, 1.0f);
			XMVECTOR ro = XMVector3Unproject(roScreen, 0.0f, 0.0f, (float)clientWidth, (float)clientHeight, 0.0f, 1000.0f, projectionTransform, viewTransform, worldTransform);
			XMVECTOR rd = XMVector3Unproject(rdScreen, 0.0f, 0.0f, (float)clientWidth, (float)clientHeight, 0.0f, 1000.0f, projectionTransform, viewTransform, worldTransform);
			rd = XMVector3Normalize(rd - ro);

			RenderComponent& renderComponent = renderComponentManager.GetComponentById(gameObject.renderComponentId);
			auto vertices = renderComponent.mesh->vertices;
			auto indices = renderComponent.mesh->indices;

			auto dist = 0.0f;
			unsigned int i = 0;
			while (i < indices.size())
			{
				auto ver1 = vertices[indices[i]];
				auto ver2 = vertices[indices[i + 1]];
				auto ver3 = vertices[indices[i + 2]];
				XMVECTOR v1 = XMVectorSet(ver1.Position.x, ver1.Position.y, ver1.Position.z, 1.0f);
				XMVECTOR v2 = XMVectorSet(ver2.Position.x, ver2.Position.y, ver2.Position.z, 1.0f);
				XMVECTOR v3 = XMVectorSet(ver3.Position.x, ver3.Position.y, ver3.Position.z, 1.0f);
				auto result = TriangleTests::Intersects(ro, rd, v1, v2, v3, dist);
				if (result)
				{
					if (dist < smallestDist)
					{
						smallestDist = dist;
						clickedGameObject = &gameObject;
					}
				}
				i += 3;
			}
		}
		if (clickedGameObject)
		{
			const auto objectId = clickedGameObject->GetId();
			StatsComponent& statsComponent = statsComponentManager.GetComponentById(clickedGameObject->statsComponentId);
			std::unique_ptr<Event> e = std::make_unique<SetTargetEvent>(objectId, clickedGameObject->name, &statsComponent);
			eventHandler.QueueEvent(e);
			std::vector<std::string> args{ std::to_string(objectId) };
			socketManager.SendPacket(OpCode::SetTarget, args);
		}
		else
		{
			std::unique_ptr<Event> e = std::make_unique<Event>(EventType::UnsetTarget);
			eventHandler.QueueEvent(e);
			socketManager.SendPacket(OpCode::UnsetTarget);
		}

		// check for double click
		if (doubleClickStart == 0.0f)
			doubleClickStart = timer.TotalTime();
		else if (timer.TotalTime() - doubleClickStart <= 0.5f)
		{
			std::unique_ptr<Event> e = std::make_unique<DoubleLeftMouseDownEvent>(mousePosX, mousePosY, clickedGameObject);
			eventHandler.QueueEvent(e);
			doubleClickStart = 0.0f;
		}
	};

	eventHandlers[EventType::SendChatMessage] = [this](const Event* const event)
	{
		const auto derivedEvent = (SendChatMessage*)event;

		std::vector<std::string> args{ player->name, derivedEvent->message };
		socketManager.SendPacket(OpCode::SendChatMessage, args);
	};

	eventHandlers[EventType::PropagateChatMessage] = [this](const Event* const event)
	{
		const auto derivedEvent = (PropagateChatMessageEvent*)event;
		
		textWindow->AddMessage("(" + derivedEvent->senderName + ") " + derivedEvent->message);
	};

	eventHandlers[EventType::ServerMessage] = [this](const Event* const event)
	{
		const auto derivedEvent = (ServerMessageEvent*)event;

		textWindow->AddMessage(derivedEvent->message);
	};

	eventHandlers[EventType::SkillIncrease] = [this](const Event* const event)
	{
		const auto derivedEvent = (SkillIncreaseEvent*)event;

		// TODO: refactors skills on ClientSide to be somewhere easier to access, use array, etc.
		for (auto i = 0; i < skills.size(); i++)
		{
			auto skill{ skills.at(i).get() };

			if (skill->skillId == derivedEvent->skillId)
			{
				skill->value = derivedEvent->newValue;
				textWindow->AddMessage("Your skill in " + skill->name + " has increased to " + std::to_string(derivedEvent->newValue));
			}
		}
	};

	eventHandlers[EventType::DoubleLeftMouseDown] = [this](const Event* const event)
	{
		const auto derivedEvent = (DoubleLeftMouseDownEvent*)event;
		
		if (derivedEvent->clickedObject && !lootPanel->isVisible)
		{
			const auto clickedObject = derivedEvent->clickedObject;
			const StatsComponent& statsComponent = statsComponentManager.GetComponentById(clickedObject->statsComponentId);

			if (!statsComponent.alive)
				lootPanel->ToggleVisibility();
		}
	};
}