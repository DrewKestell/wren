#include "stdafx.h"
#include "Game.h"
#include "ConstantBufferOnce.h"
#include "Camera.h"
#include "Components/RenderComponent.h"
#include <OpCodes.h>
#include "Events/UIAbilityDroppedEvent.h"
#include "Events/AttackHitEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/CreateAccountFailedEvent.h"
#include "EventHandling/Events/LoginSuccessEvent.h"
#include "EventHandling/Events/LoginFailedEvent.h"
#include "EventHandling/Events/CreateCharacterFailedEvent.h"
#include "EventHandling/Events/CreateCharacterSuccessEvent.h"
#include "EventHandling/Events/DeleteCharacterSuccessEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/MouseEvent.h"
#include "EventHandling/Events/EnterWorldSuccessEvent.h"
#include "EventHandling/Events/GameObjectUpdateEvent.h"
#include "EventHandling/Events/OtherPlayerUpdateEvent.h"
#include "EventHandling/Events/ActivateAbilityEvent.h"
#include "EventHandling/Events/SetTargetEvent.h"
#include "EventHandling/Events/SendChatMessage.h"
#include "EventHandling/Events/PropagateChatMessage.h"
#include "EventHandling/Events/ServerMessageEvent.h"

SocketManager g_socketManager;
unsigned int g_zIndex{ 0 };

extern EventHandler g_eventHandler;

bool CompareUIComponents(UIComponent* a, UIComponent* b) { return (a->zIndex < b->zIndex); }

Game::Game(ClientRepository repository, CommonRepository commonRepository) noexcept(false)
	: repository{ repository },
	  commonRepository{ commonRepository }
{
	deviceResources = std::make_unique<DX::DeviceResources>();
	deviceResources->RegisterDeviceNotify(this);

	g_eventHandler.Subscribe(*this);
}

void Game::PublishEvents()
{
	std::sort(uiComponents.begin(), uiComponents.end(), CompareUIComponents);
	auto eventQueue = g_eventHandler.eventQueue;
	while (!eventQueue->empty())
	{
		const auto event = eventQueue->front();
		eventQueue->pop();

		// We pass events to the UIComponents first, because those are usually overlaid on top
		//   of 3D GameObjects, and therefore we want certain events like clicks, etc to hit
		//   the UIComponents first.
		// These UIComponents are sorted in ascending order of their z-index.
		auto stopPropagation = false;

		for (int i = (int)uiComponents.size() - 1; i >= 0; i--)
		{
			stopPropagation = uiComponents.at(i)->HandleEvent(event);
			if (stopPropagation)
				break;
		}

		// There are times where we want to avoid having events propagate to GameObjects if they're
		//   handled by a UIComponent, so if any UIComponent returns true, we skip passing that event
		//   to the GameObjects and move to the next iteration of the while loop.
		if (stopPropagation)
			continue;

		for (auto observer : *g_eventHandler.observers)
		{
			stopPropagation = observer->HandleEvent(event);
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

	while (g_socketManager.TryRecieveMessage()) {}

	// to get an accurate ping, we should handle this outside of the main update loop which is locked at 60 updates / second
	if (activeLayer == InGame)
	{
		if (pingStart == 0.0f)
		{
			pingStart = timer.TotalTime();

			std::string args[]{ std::to_string(pingId) };
			g_socketManager.SendPacket(OPCODE_PING, args, 1);
		}
	}
	
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
		float mspf = (float)1000 / frameCnt;
		const auto fpsText = "FPS: " + std::to_string(frameCnt) + ", MSPF: " + std::to_string(mspf);
		fpsTextLabel->SetText(fpsText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;

		if (g_socketManager.Connected())
			g_socketManager.SendPacket(OPCODE_HEARTBEAT);
	}

	// update ping
	const auto pingText = "Ping: " + std::to_string(ping) + " ms";
	pingTextLabel->SetText(pingText.c_str());

	// update MousePos text
	const auto mousePosText = "MousePosX: " + std::to_string((int)mousePosX) + ", MousePosY: " + std::to_string((int)mousePosY);
	mousePosLabel->SetText(mousePosText.c_str());

	if (activeLayer == InGame)
	{
		auto camPos = camera.GetPosition();
		XMVECTORF32 s_Eye = { camPos.x, camPos.y, camPos.z, 0.0f };
		XMVECTORF32 s_At = { camPos.x - 500.0f, 0.0f, camPos.z + 500.0f, 0.0f };
		XMVECTORF32 s_Up = { 0.0f, 1.0f, 0.0f, 0.0f };
		viewTransform = XMMatrixLookAtLH(s_Eye, s_At, s_Up);

		d3dContext->RSSetState(solidRasterState.Get());
		//d3dContext->RSSetState(wireframeRasterState);

		gameMapRenderComponent->Draw(d3dContext, viewTransform, projectionTransform);

		renderComponentManager.Render(d3dContext, viewTransform, projectionTransform, updateTimer);
	}

	// foreach RenderComponent -> Draw
	for (auto i = 0; i < uiComponents.size(); i++)
		uiComponents.at(i)->Draw();

	d2dContext->EndDraw();

	// this stuff has to be drawn after EndDraw is called to make sure it's on top of the d2d stuff
	abilitiesContainer->DrawSprites();
	hotbar->DrawSprites();

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
	auto viewport = deviceResources->GetScreenViewport();
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
	auto r = deviceResources->GetOutputSize();
	deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!deviceResources->WindowSizeChanged(width, height))
		return;

	clientWidth = width;
	clientHeight = height;

	abilitiesContainer->clientWidth = (float)width;
	abilitiesContainer->clientHeight = (float)height;

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
	InitializeStaticObjects();
	InitializeRasterStates();
	InitializeSprites();
	
	auto d3dDevice = deviceResources->GetD3DDevice();
	auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto writeFactory = deviceResources->GetWriteFactory();

	gameMapRenderComponent = std::make_unique<GameMapRenderComponent>(d3dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader.Get(), pixelShader.Get(), textures[2].Get());

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
	hotbar = std::make_unique<UIHotbar>(uiComponents, XMFLOAT2{ 5.0f, clientHeight - 45.0f }, InGame, 0, blackBrush.Get(), d2dDeviceContext, d2dFactory, (float)clientHeight);

	// init textWindow
	textWindow = std::make_unique<UITextWindow>(uiComponents, XMFLOAT2{ 5.0f, clientHeight - 300.0f }, InGame, 0, textWindowMessages, textWindowMessageIndex, statBackgroundBrush.Get(), blackBrush.Get(), darkGrayBrush.Get(), whiteBrush.Get(), mediumGrayBrush.Get(), blackBrush.Get(), scrollBarBackgroundBrush.Get(), scrollBarBrush.Get(), d2dDeviceContext, writeFactory, textFormatTextWindow.Get(), textFormatTextWindowInactive.Get(), d2dFactory);

	if (skills)
		InitializeSkills();

	if (abilities)
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
	ShaderBuffer sb;
	byte* fileData = nullptr;

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
	npcs = repository.ListNpcs();
}

void Game::InitializeStaticObjects()
{
	auto staticObjects = commonRepository.ListStaticObjects();

	for (auto staticObject : staticObjects)
	{
		const auto pos = staticObject->GetPosition();
		GameObject& gameObject = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, 0.0f, GameObjectType::StaticObject, staticObject->GetId(), true);
		auto renderComponent = renderComponentManager.CreateRenderComponent(gameObject.id, meshes[staticObject->GetModelId()].get(), vertexShader.Get(), pixelShader.Get(), textures[staticObject->GetTextureId()].Get());
		gameObject.renderComponentId = renderComponent.GetId();
		StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(gameObject.id, 100, 100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10, staticObject->GetName());
		gameObject.statsComponentId = statsComponent.id;
		gameMap.SetTileOccupied(gameObject.localPosition, true);

		delete staticObject;
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
	auto arialFontFamily = L"Arial";
	auto locale = L"en-US";

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
	loginScreen_inputGroup = std::make_unique<UIInputGroup>(Login);
	loginScreen_inputGroup->AddInput(loginScreen_accountNameInput.get());
	loginScreen_inputGroup->AddInput(loginScreen_passwordInput.get());

	// CreateAccount
	createAccount_accountNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 20.0f }, CreateAccount, 0, false, 120.0f, 260.0f, 24.0f, "Account Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_passwordInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 50.0f  }, CreateAccount, 0, true, 120.0f, 260.0f, 24.0f, "Password:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_inputGroup = std::make_unique<UIInputGroup>(CreateAccount);
	createAccount_inputGroup->AddInput(createAccount_accountNameInput.get());
	createAccount_inputGroup->AddInput(createAccount_passwordInput.get());

	// CreateCharacter
	createCharacter_characterNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT2{ 15.0f, 20.0f }, CreateCharacter, 0, false, 140.0f, 260.0f, 24.0f, "Character Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createCharacter_inputGroup = std::make_unique<UIInputGroup>(CreateCharacter);
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

		std::string args[]{ accountName, password };
		g_socketManager.SendPacket(OPCODE_CONNECT, args, 2);
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

		std::string args[]{ accountName, password };
		g_socketManager.SendPacket(OPCODE_CREATE_ACCOUNT, args, 2);
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

		auto characterInput = GetCurrentlySelectedCharacterListing();
		if (characterInput == nullptr)
			characterSelect_errorMessageLabel->SetText("You must select a character before entering the game.");
		else
		{
			std::string args[]{ characterInput->GetName() };
			g_socketManager.SendPacket(OPCODE_ENTER_WORLD, args, 1);
			SetActiveLayer(EnteringWorld);
		}
	};

	const auto onClickCharacterSelectDeleteCharacterButton = [this]()
	{
		characterSelect_successMessageLabel->SetText("");
		characterSelect_errorMessageLabel->SetText("");

		auto characterInput = GetCurrentlySelectedCharacterListing();
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
		g_socketManager.Logout();

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

		std::string args[]{ characterName };
		g_socketManager.SendPacket(OPCODE_CREATE_CHARACTER, args, 1);
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
		std::string args[]{ characterNamePendingDeletion };
		g_socketManager.SendPacket(OPCODE_DELETE_CHARACTER, args, 1);
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
	auto d2dContext = deviceResources->GetD2DDeviceContext();
	auto d3dDevice = deviceResources->GetD3DDevice();
	auto d3dDeviceContext = deviceResources->GetD3DDeviceContext();

	// Game Settings
	const auto gameSettingsPanelX = (clientWidth - 400.0f) / 2.0f;
	const auto gameSettingsPanelY = (clientHeight - 200.0f) / 2.0f;
	const auto onClickGameSettingsLogoutButton = [this]()
	{
		g_socketManager.SendPacket(OPCODE_DISCONNECT);

		SetActiveLayer(Login);
	};
	gameSettingsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ gameSettingsPanelX, gameSettingsPanelY }, InGame, 1, false, 400.0f, 200.0f, VK_ESCAPE, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);
	gameSettings_logoutButton = std::make_unique<UIButton>(uiComponents, XMFLOAT2{ 10.0f, 26.0f }, InGame, 2, 80.0f, 24.0f, "LOGOUT", onClickGameSettingsLogoutButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	gameSettingsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 2, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	gameSettingsPanelHeader->SetText("Game Settings");
	gameSettingsPanel->AddChildComponent(*gameSettingsPanelHeader);
	gameSettingsPanel->AddChildComponent(*gameSettings_logoutButton);

	// Game Editor
	const auto gameEditorPanelX = 580.0f;
	const auto gameEditorPanelY = 5.0f;
	gameEditorPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ gameEditorPanelX, gameEditorPanelY }, InGame, 1, true, 200.0f, 400.0f, VK_F1, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);
	gameEditorPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 2, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	gameEditorPanelHeader->SetText("Game Editor");
	gameEditorPanel->AddChildComponent(*gameEditorPanelHeader);

	// Diagnostics
	const auto diagnosticsPanelX = 580.0f;
	const auto diagnosticsPanelY = 336.0f;
	diagnosticsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ diagnosticsPanelX, diagnosticsPanelY }, InGame, 1, true, 200.0f, 200.0f, VK_F2, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);

	diagnosticsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanelHeader->SetText("Diagnostics");
	diagnosticsPanel->AddChildComponent(*diagnosticsPanelHeader);

	mousePosLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 22.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*mousePosLabel);

	fpsTextLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 36.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*fpsTextLabel);

	pingTextLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 50.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*pingTextLabel);

	// Skills
	const auto skillsPanelX = 200.0f;
	const auto skillsPanelY = 200.0f;
	skillsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ skillsPanelX, skillsPanelY }, InGame, 1, true, 200.0f, 200.0f, VK_F3, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);

	skillsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 2, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	skillsPanelHeader->SetText("Skills");
	skillsPanel->AddChildComponent(*skillsPanelHeader);

	// Abilities
	const auto abilitiesPanelX = 10.0f;
	const auto abilitiesPanelY = 10.0f;
	abilitiesPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT2{ abilitiesPanelX, abilitiesPanelY }, InGame, 1, true, 240.0f, 400.0f, VK_F4, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);

	abilitiesPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT2{ 2.0f, 2.0f }, InGame, 3, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	abilitiesPanelHeader->SetText("Abilities");
	abilitiesPanel->AddChildComponent(*abilitiesPanelHeader);

	abilitiesContainer = std::make_unique<UIAbilitiesContainer>(uiComponents, XMFLOAT2{ 0.0f, 0.0f }, InGame, 2, d2dContext, d2dFactory, d3dDevice, d3dDeviceContext, writeFactory, blackBrush.Get(), abilityHighlightBrush.Get(), blackBrush.Get(), abilityPressedBrush.Get(), errorMessageBrush.Get(), textFormatHeaders.Get(), spriteVertexShader.Get(), spritePixelShader.Get(), spriteVertexShaderBuffer.buffer, spriteVertexShaderBuffer.size, projectionTransform, (float)clientWidth, (float)clientHeight);
	abilitiesPanel->AddChildComponent(*abilitiesContainer);
}

UICharacterListing* Game::GetCurrentlySelectedCharacterListing()
{
	UICharacterListing* characterListing = nullptr;
	for (auto i = 0; i < characterList.size(); i++)
	{
		if (characterList.at(i)->IsSelected())
			characterListing = characterList.at(i).get();
	}
	return characterListing;
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

	ID3D11Buffer* constantBufferOnce = nullptr;
	d3dDevice->CreateBuffer(&bufferDesc, nullptr, &constantBufferOnce);

	// map ConstantBuffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	d3dContext->Map(constantBufferOnce, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto pCB = reinterpret_cast<ConstantBufferOnce*>(mappedResource.pData);
	XMStoreFloat4(&pCB->directionalLight, XMVECTOR{ 0.0f, -1.0f, 0.5f, 0.0f });
	d3dContext->Unmap(constantBufferOnce, 0);

	d3dContext->PSSetConstantBuffers(0, 1, &constantBufferOnce);
}

void Game::InitializeRasterStates()
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	CD3D11_RASTERIZER_DESC wireframeRasterStateDesc(D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	d3dDevice->CreateRasterizerState(&wireframeRasterStateDesc, wireframeRasterState.ReleaseAndGetAddressOf());

	CD3D11_RASTERIZER_DESC solidRasterStateDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	d3dDevice->CreateRasterizerState(&solidRasterStateDesc, solidRasterState.ReleaseAndGetAddressOf());
}

void Game::InitializeTextures()
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	const wchar_t* paths[] =
	{
		L"../../WrenClient/Textures/texture01.dds",     // 0
		L"../../WrenClient/Textures/texture02.dds",     // 1
		L"../../WrenClient/Textures/grass01.dds",       // 2
		L"../../WrenClient/Textures/abilityicon01.dds", // 3
		L"../../WrenClient/Textures/texture03.dds",     // 4
		L"../../WrenClient/Textures/abilityicon02.dds", // 5
		L"../../WrenClient/Textures/abilityicon03.dds"  // 6
	};

	// clear calls the destructor of its elements, and ComPtr's destructor handles calling Release()
	textures.clear();

	for (auto i = 0; i < 7; i++)
	{
		ComPtr<ID3D11ShaderResourceView> ptr;
		CreateDDSTextureFromFile(d3dDevice, paths[i], nullptr, ptr.ReleaseAndGetAddressOf());
		textures.push_back(ptr);
	}
}

void Game::InitializeMeshes()
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	std::string paths[] = 
	{
		"../../WrenClient/Models/sphere.blend",  // 0
		"../../WrenClient/Models/tree.blend",    // 1
		"../../WrenClient/Models/dummy.blend"    // 2
	};

	// clear calls the destructor of its elements, and unique_ptr's destructor handles cleaning itself up
	meshes.clear();

	for (auto i = 0; i < sizeof(paths) / sizeof(std::string); i++)
		meshes.push_back(std::make_unique<Mesh>(paths[i], d3dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size));
}

void Game::InitializeSprites()
{
	auto d3dDevice = deviceResources->GetD3DDevice();
}

void Game::RecreateCharacterListings(const std::vector<std::string*>* characterNames)
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	characterList.clear();

	for (auto i = 0; i < characterNames->size(); i++)
	{
		const float y = 100.0f + (i * 40.0f);
		characterList.push_back(std::make_unique<UICharacterListing>(uiComponents, XMFLOAT2{ 25.0f, y }, CharacterSelect, 1, 260.0f, 30.0f, (*characterNames->at(i)).c_str(), whiteBrush.Get(), selectedCharacterBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatAccountCredsInputValue.Get(), d2dFactory));
	}
}

const bool Game::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::KeyDown:
		{
			const auto derivedEvent = (KeyDownEvent*)event;

			break;
		}
		case EventType::RightMouseDown:
		{
			const auto derivedEvent = (MouseEvent*)event;

			const auto dir = Utility::MousePosToDirection(clientWidth, clientHeight, derivedEvent->mousePosX, derivedEvent->mousePosY);
			std::string args[]{ std::to_string(dir.x), std::to_string(dir.y), std::to_string(dir.z) };
			g_socketManager.SendPacket(OPCODE_PLAYER_RIGHT_MOUSE_DOWN, args, 3);

			rightMouseDownDir = dir;

			break;
		}
		case EventType::RightMouseUp:
		{
			const auto derivedEvent = (MouseEvent*)event;

			g_socketManager.SendPacket(OPCODE_PLAYER_RIGHT_MOUSE_UP);

			rightMouseDownDir = VEC_ZERO;

			break;
		}
		case EventType::MouseMove:
		{
			const auto derivedEvent = (MouseEvent*)event;

			mousePosX = derivedEvent->mousePosX;
			mousePosY = derivedEvent->mousePosY;

			if (activeLayer == Layer::InGame && rightMouseDownDir != VEC_ZERO)
			{
				const auto dir = Utility::MousePosToDirection(clientWidth, clientHeight, derivedEvent->mousePosX, derivedEvent->mousePosY);
				if (dir != rightMouseDownDir)
				{
					std::string args[]{ std::to_string(dir.x), std::to_string(dir.y), std::to_string(dir.z) };
					g_socketManager.SendPacket(OPCODE_PLAYER_RIGHT_MOUSE_DOWN_DIR_CHANGE, args, 3);
					rightMouseDownDir = dir;
				}
			}
			
			break;
		}
		case EventType::CreateAccountFailed:
		{
			const auto derivedEvent = (CreateAccountFailedEvent*)event;

			createAccount_errorMessageLabel->SetText(("Failed to create account. Reason: " + *(derivedEvent->error)).c_str());

			break;
		}
		case EventType::CreateAccountSuccess:
		{
			createAccount_errorMessageLabel->SetText("");
			loginScreen_successMessageLabel->SetText("Account created successfully.");
			SetActiveLayer(Login);

			break;
		}
		case EventType::LoginFailed:
		{
			const auto derivedEvent = (LoginFailedEvent*)event;

			loginScreen_errorMessageLabel->SetText(("Login failed. Reason: " + *(derivedEvent->error)).c_str());
			SetActiveLayer(Login);

			break;
		}
		case EventType::LoginSuccess:
		{
			const auto derivedEvent = (LoginSuccessEvent*)event;

			RecreateCharacterListings(derivedEvent->characterList);
			SetActiveLayer(CharacterSelect);

			break;
		}
		case EventType::CreateCharacterFailed:
		{
			const auto derivedEvent = (CreateCharacterFailedEvent*)event;

			createCharacter_errorMessageLabel->SetText(("Character creation failed. Reason: " + *(derivedEvent->error)).c_str());

			break;
		}
		case EventType::CreateCharacterSuccess:
		{
			const auto derivedEvent = (CreateCharacterSuccessEvent*)event;

			RecreateCharacterListings(derivedEvent->characterList);
			createCharacter_errorMessageLabel->SetText("");
			characterSelect_successMessageLabel->SetText("Character created successfully.");
			SetActiveLayer(CharacterSelect);

			break;
		}
		case EventType::DeleteCharacterSuccess:
		{
			const auto derivedEvent = (DeleteCharacterSuccessEvent*)event;

			RecreateCharacterListings(derivedEvent->characterList);
			characterNamePendingDeletion = "";
			createCharacter_errorMessageLabel->SetText("");
			characterSelect_successMessageLabel->SetText("Character deleted successfully.");
			SetActiveLayer(CharacterSelect);

			break;
		}
		case EventType::EnterWorldSuccess:
		{
			const auto derivedEvent = (EnterWorldSuccessEvent*)event;

			GameObject& player = objectManager.CreateGameObject(derivedEvent->position, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, PLAYER_SPEED, GameObjectType::Player, (long)derivedEvent->accountId);
			this->player = &player;
			auto sphereRenderComponent = renderComponentManager.CreateRenderComponent(player.id, meshes[derivedEvent->modelId].get(), vertexShader.Get(), pixelShader.Get(), textures[derivedEvent->textureId].Get());
			player.renderComponentId = sphereRenderComponent.GetId();
			StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(player.id, 100, 100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10, derivedEvent->name);
			player.statsComponentId = statsComponent.id;
			gameMap.SetTileOccupied(player.localPosition, true);

			auto d2dFactory = deviceResources->GetD2DFactory();
			auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
			auto writeFactory = deviceResources->GetWriteFactory();

			if (skills)
				skills->clear();
			skills = derivedEvent->skills;

			// logging out doesn't empty the skillsPanel's skills list, 
			// so check for it to avoid duplicating skill listings on successive logins
			if (skillsPanel->GetChildren().size() == 1) 
				InitializeSkills();

			if (abilities)
				abilities->clear();
			abilities = derivedEvent->abilities;
			InitializeAbilitiesContainer();

			// init characterHUD
			characterHUD = std::make_unique<UICharacterHUD>(uiComponents, XMFLOAT2{ 10.0f, 12.0f }, InGame, 0, d2dDeviceContext, writeFactory, textFormatSuccessMessage.Get(), d2dFactory, statsComponent, healthBrush.Get(), manaBrush.Get(), staminaBrush.Get(), statBackgroundBrush.Get(), blackBrush.Get(), blackBrush.Get(), whiteBrush.Get(), derivedEvent->name->c_str());

			std::sort(uiComponents.begin(), uiComponents.end(), CompareUIComponents);

			textWindow->AddMessage(new std::string("Welcome to Wren!"));

			SetActiveLayer(InGame);

			break;
		}
		case EventType::GameObjectUpdate:
		{
			const auto derivedEvent = (GameObjectUpdateEvent*)event;

			const auto gameObjectId = derivedEvent->characterId;
			const auto type = derivedEvent->type;
			const auto pos = XMFLOAT3{ derivedEvent->posX, derivedEvent->posY, derivedEvent->posZ };
			const auto mov = XMFLOAT3{ derivedEvent->movX, derivedEvent->movY, derivedEvent->movZ };

			int modelId{ 0 };
			int textureId{ 0 };
			std::string* name{ nullptr };
			float speed{ 0.0f };
			if (type == GameObjectType::Npc)
			{
				auto npc = *find_if(npcs->begin(), npcs->end(), [&gameObjectId](Npc* npc) { return npc->GetId() == gameObjectId; });
				modelId = npc->GetModelId();
				textureId = npc->GetTextureId();
				name = npc->GetName();
				speed = npc->GetSpeed();
			}
			if (!objectManager.GameObjectExists(gameObjectId))
			{
				GameObject& obj = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, speed, type, gameObjectId);
				obj.movementVector = mov;
				auto sphereRenderComponent = renderComponentManager.CreateRenderComponent(gameObjectId, meshes[modelId].get(), vertexShader.Get(), pixelShader.Get(), textures[textureId].Get());
				obj.renderComponentId = sphereRenderComponent.GetId();
				StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(obj.id, 100, 100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10, name);
				obj.statsComponentId = statsComponent.id;
				gameMap.SetTileOccupied(obj.localPosition, true);
			}
			else
			{
				GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->characterId);

				// TODO: this might not be right. "pos" may not correctly refer to the destination tile.
				gameMap.SetTileOccupied(gameObject.localPosition, false);
				gameMap.SetTileOccupied(pos, true);

				gameObject.localPosition = pos;
				gameObject.movementVector = mov;
			}

			break;
		}
		case EventType::OtherPlayerUpdate:
		{
			const auto derivedEvent = (OtherPlayerUpdateEvent*)event;

			const auto gameObjectId = derivedEvent->characterId;
			const auto type = derivedEvent->type;
			const auto pos = XMFLOAT3{ derivedEvent->posX, derivedEvent->posY, derivedEvent->posZ };
			const auto mov = XMFLOAT3{ derivedEvent->movX, derivedEvent->movY, derivedEvent->movZ };
			const auto modelId = derivedEvent->modelId;
			const auto textureId = derivedEvent->textureId;
			const auto name = derivedEvent->name;
			
			if (!objectManager.GameObjectExists(gameObjectId))
			{
				GameObject& obj = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, PLAYER_SPEED, GameObjectType::Player, gameObjectId);
				obj.movementVector = mov;
				auto sphereRenderComponent = renderComponentManager.CreateRenderComponent(gameObjectId, meshes[modelId].get(), vertexShader.Get(), pixelShader.Get(), textures[textureId].Get());
				obj.renderComponentId = sphereRenderComponent.GetId();
				StatsComponent& statsComponent = statsComponentManager.CreateStatsComponent(obj.id, 100, 100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10, name);
				obj.statsComponentId = statsComponent.id;
			}
			else
			{
				GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->characterId);
				gameObject.localPosition = pos;
				gameObject.movementVector = mov;
			}

			break;
		}
		case EventType::ActivateAbility:
		{
			const auto derivedEvent = (ActivateAbilityEvent*)event;

			std::string args[]{ std::to_string(derivedEvent->abilityId) };
			g_socketManager.SendPacket(OPCODE_ACTIVATE_ABILITY, args, 1);

			break;
		}
		case EventType::ReorderUIComponents:
		{
			std::sort(uiComponents.begin(), uiComponents.end(), CompareUIComponents);

			break;
		}
		case EventType::LeftMouseDown:
		{
			if (activeLayer != Layer::InGame) break;

			const auto derivedEvent = (MouseEvent*)event;

			GameObject* clickedGameObject{ nullptr };
			float smallestDist = FLT_MAX;

			auto gameObjects = objectManager.GetGameObjects();
			for (unsigned int j = 0; j < objectManager.GetGameObjectIndex(); j++)
			{
				GameObject& gameObject = gameObjects[j];
				auto pos = gameObject.GetWorldPosition();
				auto scale = gameObject.scale;
				auto worldTransform = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);

				XMVECTOR roScreen = XMVectorSet(derivedEvent->mousePosX, derivedEvent->mousePosY, 0.0f, 1.0f);
				XMVECTOR rdScreen = XMVectorSet(derivedEvent->mousePosX, derivedEvent->mousePosY, 1.0f, 1.0f);
				XMVECTOR ro = XMVector3Unproject(roScreen, 0.0f, 0.0f, (float)clientWidth, (float)clientHeight, 0.0f, 1000.0f, projectionTransform, viewTransform, worldTransform);
				XMVECTOR rd = XMVector3Unproject(rdScreen, 0.0f, 0.0f, (float)clientWidth, (float)clientHeight, 0.0f, 1000.0f, projectionTransform, viewTransform, worldTransform);
				rd = XMVector3Normalize(rd - ro);

				auto renderComponent = renderComponentManager.GetRenderComponentById(gameObject.renderComponentId);
				auto vertices = renderComponent.mesh->vertices;
				auto indices = renderComponent.mesh->indices;

				float dist;
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
			StatsComponent& playerStats = statsComponentManager.GetStatsComponentById(player->statsComponentId);
			if (clickedGameObject)
			{
				StatsComponent& statsComponent = statsComponentManager.GetStatsComponentById(clickedGameObject->statsComponentId);
				g_eventHandler.QueueEvent(new SetTargetEvent{ clickedGameObject->id, &statsComponent });
				playerStats.targetId = clickedGameObject->id;
				std::string args[]{ std::to_string(clickedGameObject->id) };
				g_socketManager.SendPacket(OPCODE_SET_TARGET, args, 1);
			}
			else
			{
				g_eventHandler.QueueEvent(new Event{ EventType::UnsetTarget });
				playerStats.targetId = -1;
				g_socketManager.SendPacket(OPCODE_UNSET_TARGET);
			}

			break;
		}
		case EventType::SendChatMessage:
		{
			const auto derivedEvent = (SendChatMessage*)event;

			auto statsComponent = statsComponentManager.GetStatsComponentById(player->statsComponentId);

			std::string args[]{ *statsComponent.name, *derivedEvent->message };
			g_socketManager.SendPacket(OPCODE_SEND_CHAT_MESSAGE, args, 2);

			break;
		}
		case EventType::PropagateChatMessage:
		{
			const auto derivedEvent = (PropagateChatMessage*)event;

			std::string* msg = new std::string("(" + *derivedEvent->senderName + ") " + *derivedEvent->message);
			textWindow->AddMessage(msg);

			break;
		}
		case EventType::ServerMessage:
		{
			const auto derivedEvent = (ServerMessageEvent*)event;

			std::string* msg = new std::string(*derivedEvent->message);
			textWindow->AddMessage(msg);

			break;
		}
		case EventType::AttackHit:
		{
			const auto derivedEvent = (AttackHitEvent*)event;

			const auto gameObject = objectManager.GetGameObjectById(derivedEvent->targetId);
			StatsComponent& comp = statsComponentManager.GetStatsComponentById(gameObject.statsComponentId);
			comp.health -= derivedEvent->damage;

			break;
		}
	}

	return false;
}

void Game::SetActiveLayer(const Layer layer)
{
	activeLayer = layer;
	g_eventHandler.QueueEvent(new ChangeActiveLayerEvent{ layer });
}

Game::~Game()
{
	g_eventHandler.Unsubscribe(*this);
}

void Game::InitializeSkills()
{
	auto xOffset = 5.0f;
	auto yOffset = 25.0f;

	auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	auto writeFactory = deviceResources->GetWriteFactory();

	for (auto i = 0; i < skills->size(); i++)
	{
		auto skill = skills->at(i);

		if (!skillList[skill->skillId])
			skillList[skill->skillId] = std::make_unique<UISkillListing>(uiComponents, XMFLOAT2{ xOffset, yOffset + (18.0f * i) }, InGame, 2, *skill, blackBrush.Get(), d2dDeviceContext, writeFactory, textFormatFPS.Get());

		skillsPanel->AddChildComponent(*skillList[skill->skillId]);
	}
}

void Game::InitializeAbilitiesContainer()
{
	abilitiesContainer->ClearAbilities();
	for (auto i = 0; i < abilities->size(); i++)
	{
		auto ability = abilities->at(i);
		abilitiesContainer->AddAbility(ability, textures[ability->spriteId].Get());
	}
}

void Game::QuitGame()
{
	auto hWnd = deviceResources->GetWindow();
	DestroyWindow(hWnd);
}

void Game::OnPong(unsigned int pingId)
{
	const auto delta = timer.TotalTime() - pingStart;
	//std::cout << "Delta: " << delta << std::endl;
	ping = (int)std::round(delta * 1000);
	pingStart = 0.0f;
	this->pingId++;
}