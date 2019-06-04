#include "stdafx.h"
#include "Game.h"
#include "ConstantBufferOnce.h"
#include "Camera.h"
#include "Components/RenderComponent.h"
#include <OpCodes.h>
#include "Events/UIAbilityDroppedEvent.h"
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
#include "EventHandling/Events/ActivateAbilityEvent.h"

SocketManager g_socketManager;

extern EventHandler g_eventHandler;

Game::Game() noexcept(false)
{
	deviceResources = std::make_unique<DX::DeviceResources>();
	deviceResources->RegisterDeviceNotify(this);

	g_eventHandler.Subscribe(*this);
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
	while (g_socketManager.TryRecieveMessage()) {}

	timer.Tick();

	updateTimer += timer.DeltaTime();
	if (updateTimer >= UPDATE_FREQUENCY)
	{
		if (activeLayer == InGame)
		{
			playerController->Update();
			camera.Update(player->GetWorldPosition(), UPDATE_FREQUENCY);
			if (g_socketManager.Connected())
			{
				auto playerUpdate = playerController->GeneratePlayerUpdate();

				g_socketManager.SendPacket(
					OPCODE_PLAYER_UPDATE,
					9,
					std::to_string(playerUpdate->id),
					std::to_string(player->id),
					std::to_string(playerUpdate->position.x),
					std::to_string(playerUpdate->position.y),
					std::to_string(playerUpdate->position.z),
					std::to_string(playerUpdate->isRightClickHeld),
					std::to_string(playerUpdate->currentMouseDirection.x),
					std::to_string(playerUpdate->currentMouseDirection.y),
					std::to_string(playerUpdate->currentMouseDirection.z)
				);
			}
			objectManager.Update();
		}
		
		g_eventHandler.PublishEvents();

		updateTimer -= UPDATE_FREQUENCY;
	}
	
	Render(updateTimer);
}

void Game::Update()
{
	float elapsedTime = float(timer.DeltaTime());

	// TODO: Add your game logic here.
	elapsedTime;
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

		gameMap->Draw(d3dContext, viewTransform, projectionTransform);

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

	CreateWindowSizeDependentResources();

	clientWidth = width;
	clientHeight = height;

	if (playerController)
		playerController->SetClientDimensions(width, height);
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	InitializeBrushes();
	InitializeTextFormats();
	InitializeInputs();
	InitializeButtons();
	InitializeLabels();
	InitializeTextures();
	InitializeShaders();
	InitializeBuffers();
	InitializeMeshes();
	InitializeRasterStates();
	InitializeSprites();
	InitializePanels();
	
	auto d3dDevice = deviceResources->GetD3DDevice();
	auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto writeFactory = deviceResources->GetWriteFactory();

	gameMap = std::make_unique<GameMap>(d3dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader.Get(), pixelShader.Get(), textures[2].Get());

	// initialize tree. TODO: game world needs to be stored on disk somewhere and initialized on startup
	GameObject& tree = objectManager.CreateGameObject(XMFLOAT3{ 90.0f, 0.0f, 90.0f }, XMFLOAT3{ 14.0f, 14.0f, 14.0f });
	auto treeRenderComponent = renderComponentManager.CreateRenderComponent(tree.id, meshes[1].get(), vertexShader.Get(), pixelShader.Get(), textures[1].Get());
	tree.renderComponentId = treeRenderComponent.GetId();

	// init hotbar
	hotbar = std::make_unique<UIHotbar>(uiComponents, XMFLOAT3{ 5.0f, clientHeight - 45.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, blackBrush.Get(), d2dDeviceContext, d2dFactory, (float)clientHeight);

	// init targetHUD
	targetHUD = std::make_unique<UITargetHUD>(uiComponents, XMFLOAT3{ 260.0f, 12.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, d2dDeviceContext, writeFactory, textFormatSuccessMessage.Get(), d2dFactory, healthBrush.Get(), manaBrush.Get(), staminaBrush.Get(), statBackgroundBrush.Get(), blackBrush.Get(), blackBrush.Get(), whiteBrush.Get());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	// TODO: Initialize windows-size dependent objects here.
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

#pragma region Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 800;
	height = 600;
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

void Game::InitializeBrushes()
{
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f), grayBrush.ReleaseAndGetAddressOf());
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.22f, 0.22f, 1.0f), blackBrush.ReleaseAndGetAddressOf());
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
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.9f, 0.9f, 0.9f, 1.0f), statBackgroundBrush.ReleaseAndGetAddressOf());
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
}

void Game::InitializeInputs()
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	// LoginScreen
	loginScreen_accountNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT3{ 15.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, Login, false, 120.0f, 260.0f, 24.0f, "Account Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	loginScreen_passwordInput = std::make_unique<UIInput>(uiComponents, XMFLOAT3{ 15.0f, 50.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, Login, true, 120.0f, 260.0f, 24.0f, "Password:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	loginScreen_inputGroup = std::make_unique<UIInputGroup>(Login);
	loginScreen_inputGroup->AddInput(loginScreen_accountNameInput.get());
	loginScreen_inputGroup->AddInput(loginScreen_passwordInput.get());

	// CreateAccount
	createAccount_accountNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT3{ 15.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateAccount, false, 120.0f, 260.0f, 24.0f, "Account Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_passwordInput = std::make_unique<UIInput>(uiComponents, XMFLOAT3{ 15.0f, 50.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateAccount, true, 120.0f, 260.0f, 24.0f, "Password:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_inputGroup = std::make_unique<UIInputGroup>(CreateAccount);
	createAccount_inputGroup->AddInput(createAccount_accountNameInput.get());
	createAccount_inputGroup->AddInput(createAccount_passwordInput.get());

	// CreateCharacter
	createCharacter_characterNameInput = std::make_unique<UIInput>(uiComponents, XMFLOAT3{ 15.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateCharacter, false, 140.0f, 260.0f, 24.0f, "Character Name:", blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, writeFactory, textFormatAccountCreds.Get(), d2dFactory);
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

		g_socketManager.SendPacket(OPCODE_CONNECT, 2, accountName, password);
		SetActiveLayer(Connecting);
	};

	const auto onClickLoginScreenCreateAccountButton = [this]()
	{
		loginScreen_successMessageLabel->SetText("");
		loginScreen_errorMessageLabel->SetText("");
		SetActiveLayer(CreateAccount);
	};

	// LoginScreen
	loginScreen_loginButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 145.0f, 96.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, Login, 80.0f, 24.0f, "LOGIN", onClickLoginButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	loginScreen_createAccountButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 15.0f, 522.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, Login, 160.0f, 24.0f, "CREATE ACCOUNT", onClickLoginScreenCreateAccountButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);

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

		g_socketManager.SendPacket(OPCODE_CREATE_ACCOUNT, 2, accountName, password);
	};

	const auto onClickCreateAccountCancelButton = [this]()
	{
		createAccount_errorMessageLabel->SetText("");
		SetActiveLayer(Login);
	};

	// CreateAccount
	createAccount_createAccountButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 145.0f, 96.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateAccount, 80.0f, 24.0f, "CREATE", onClickCreateAccountCreateAccountButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	createAccount_cancelButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 15.0f, 522.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateAccount, 80.0f, 24.0f, "CANCEL", onClickCreateAccountCancelButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);

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
			g_socketManager.SendPacket(OPCODE_ENTER_WORLD, 1, characterInput->GetName());
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
		SetActiveLayer(Login);
	};

	// CharacterSelect
	characterSelect_newCharacterButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 15.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 140.0f, 24.0f, "NEW CHARACTER", onClickCharacterSelectNewCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_enterWorldButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 170.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 120.0f, 24.0f, "ENTER WORLD", onClickCharacterSelectEnterWorldButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_deleteCharacterButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 305.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 160.0f, 24.0f, "DELETE CHARACTER", onClickCharacterSelectDeleteCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_logoutButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 15.0f, 522.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 80.0f, 24.0f, "LOGOUT", onClickCharacterSelectLogoutButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext,  writeFactory, textFormatButtonText.Get(), d2dFactory);

	const auto onClickCreateCharacterCreateCharacterButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");

		const auto characterName = Utility::ws2s(std::wstring(createCharacter_characterNameInput->GetInputValue()));

		if (characterName.length() == 0)
		{
			createCharacter_errorMessageLabel->SetText("Character name can't be empty.");
			return;
		}

		g_socketManager.SendPacket(OPCODE_CREATE_CHARACTER, 1, characterName);
	};

	const auto onClickCreateCharacterBackButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");
		SetActiveLayer(CharacterSelect);
	};

	// CreateCharacter
	createCharacter_createCharacterButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 165.0f, 64.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateCharacter, 160.0f, 24.0f, "CREATE CHARACTER", onClickCreateCharacterCreateCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	createCharacter_backButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 15.0f, 522.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateCharacter, 80.0f, 24.0f, "BACK", onClickCreateCharacterBackButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);

	// DeleteCharacter

	const auto onClickDeleteCharacterConfirm = [this]()
	{
		g_socketManager.SendPacket(OPCODE_DELETE_CHARACTER, 1, characterNamePendingDeletion);
	};

	const auto onClickDeleteCharacterCancel = [this]()
	{
		characterNamePendingDeletion = "";
		SetActiveLayer(CharacterSelect);
	};

	deleteCharacter_confirmButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 10.0f, 30.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DeleteCharacter, 100.0f, 24.0f, "CONFIRM", onClickDeleteCharacterConfirm, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext,  writeFactory, textFormatButtonText.Get(), d2dFactory);
	deleteCharacter_cancelButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 120.0f, 30.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DeleteCharacter, 100.0f, 24.0f, "CANCEL", onClickDeleteCharacterCancel, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
}

void Game::InitializeLabels()
{
	auto writeFactory = deviceResources->GetWriteFactory();
	auto d2dFactory = deviceResources->GetD2DFactory();
	auto d2dContext = deviceResources->GetD2DDeviceContext();

	loginScreen_successMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 30.0f, 170.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, Login, 400.0f, successMessageBrush.Get(), textFormatSuccessMessage.Get(), d2dContext, writeFactory, d2dFactory);
	loginScreen_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 30.0f, 170.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, Login, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	createAccount_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 30.0f, 170.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateAccount, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	connecting_statusLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 15.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, Connecting, 400.0f, blackBrush.Get(), textFormatAccountCreds.Get(), d2dContext, writeFactory, d2dFactory);
	connecting_statusLabel->SetText("Connecting...");

	characterSelect_successMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 30.0f, 400.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 400.0f, successMessageBrush.Get(), textFormatSuccessMessage.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 30.0f, 400.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_headerLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 15.0f, 60.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_headerLabel->SetText("Character List:");

	createCharacter_errorMessageLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 30.0f, 170.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CreateCharacter, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	deleteCharacter_headerLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 10.0f, 10.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DeleteCharacter, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);
	deleteCharacter_headerLabel->SetText("Are you sure you want to delete this character?");

	enteringWorld_statusLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 5.0f, 20.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, EnteringWorld, 400.0f, blackBrush.Get(), textFormatAccountCreds.Get(), d2dContext, writeFactory, d2dFactory);
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
	gameSettingsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT3{2.0f, 2.0f, 0.0f}, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	gameSettingsPanelHeader->SetText("Game Settings");
	const auto onClickGameSettingsLogoutButton = [this]()
	{
		g_socketManager.SendPacket(OPCODE_DISCONNECT);
		SetActiveLayer(Login);
	};
	gameSettings_logoutButton = std::make_unique<UIButton>(uiComponents, XMFLOAT3{ 10.0f, 26.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 80.0f, 24.0f, "LOGOUT", onClickGameSettingsLogoutButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatButtonText.Get(), d2dFactory);
	gameSettingsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT3{ gameSettingsPanelX, gameSettingsPanelY, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, false, 400.0f, 200.0f, VK_ESCAPE, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);
	gameSettingsPanel->AddChildComponent(*gameSettingsPanelHeader);
	gameSettingsPanel->AddChildComponent(*gameSettings_logoutButton);

	// Game Editor
	const auto gameEditorPanelX = 580.0f;
	const auto gameEditorPanelY = 5.0f;
	gameEditorPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 2.0f, 2.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	gameEditorPanelHeader->SetText("Game Editor");
	gameEditorPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT3{ gameEditorPanelX, gameEditorPanelY, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, true, 200.0f, 400.0f, VK_F1, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);
	gameEditorPanel->AddChildComponent(*gameEditorPanelHeader);

	// Diagnostics
	const auto diagnosticsPanelX = 580.0f;
	const auto diagnosticsPanelY = 336.0f;
	diagnosticsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT3{ diagnosticsPanelX, diagnosticsPanelY, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, true, 200.0f, 200.0f, VK_F2, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);

	diagnosticsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 2.0f, 2.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanelHeader->SetText("Diagnostics");
	diagnosticsPanel->AddChildComponent(*diagnosticsPanelHeader);

	mousePosLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 2.0f, 22.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*mousePosLabel);

	fpsTextLabel = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 2.0f, 36.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*fpsTextLabel);

	// Skills
	const auto skillsPanelX = 200.0f;
	const auto skillsPanelY = 200.0f;
	skillsPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT3{ skillsPanelX, skillsPanelY, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, true, 200.0f, 200.0f, VK_F3, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);

	skillsPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 2.0f, 2.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	skillsPanelHeader->SetText("Skills");
	skillsPanel->AddChildComponent(*skillsPanelHeader);

	// Abilities
	const auto abilitiesPanelX = 10.0f;
	const auto abilitiesPanelY = 10.0f;
	abilitiesPanel = std::make_unique<UIPanel>(uiComponents, XMFLOAT3{ abilitiesPanelX, abilitiesPanelY, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, true, 240.0f, 400.0f, VK_F4, darkBlueBrush.Get(), lightGrayBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);

	abilitiesPanelHeader = std::make_unique<UILabel>(uiComponents, XMFLOAT3{ 2.0f, 2.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	abilitiesPanelHeader->SetText("Abilities");
	abilitiesPanel->AddChildComponent(*abilitiesPanelHeader);

	abilitiesContainer = std::make_unique<UIAbilitiesContainer>(uiComponents, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, d2dContext, d2dFactory, d3dDevice, d3dDeviceContext, writeFactory, blackBrush.Get(), abilityHighlightBrush.Get(), blackBrush.Get(), abilityPressedBrush.Get(), textFormatHeaders.Get(), spriteVertexShader.Get(), spritePixelShader.Get(), spriteVertexShaderBuffer.buffer, spriteVertexShaderBuffer.size, projectionTransform, (float)clientWidth, (float)clientHeight);
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

	projectionTransform = XMMatrixOrthographicLH((float)clientWidth, (float)clientHeight, 0.0f, 5000.0f);
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
		L"../../WrenClient/Textures/texture03.dds"      // 4
	};

	// clear calls the destructor of its elements, and ComPtr's destructor handles calling Release()
	textures.clear();

	for (auto i = 0; i < 5; i++)
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
		characterList.push_back(std::make_unique<UICharacterListing>(uiComponents, XMFLOAT3{ 25.0f, y, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, CharacterSelect, 260.0f, 30.0f, (*characterNames->at(i)).c_str(), whiteBrush.Get(), selectedCharacterBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, writeFactory, textFormatAccountCredsInputValue.Get(), d2dFactory));
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

			if (playerController)
				playerController->OnRightMouseDownEvent(derivedEvent);

			break;
		}
		case EventType::RightMouseUp:
		{
			const auto derivedEvent = (MouseEvent*)event;

			if (playerController)
				playerController->OnRightMouseUpEvent(derivedEvent);

			break;
		}
		case EventType::MouseMove:
		{
			const auto derivedEvent = (MouseEvent*)event;

			mousePosX = derivedEvent->mousePosX;
			mousePosY = derivedEvent->mousePosY;

			if (playerController)
				playerController->OnMouseMoveEvent(derivedEvent);

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

			GameObject& player = objectManager.CreateGameObject(derivedEvent->position, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, (long)derivedEvent->characterId);
			this->player = &player;
			auto sphereRenderComponent = renderComponentManager.CreateRenderComponent(player.id, meshes[derivedEvent->modelId].get(), vertexShader.Get(), pixelShader.Get(), textures[derivedEvent->textureId].Get());
			player.renderComponentId = sphereRenderComponent.GetId();
			auto statsComponent = statsComponentManager.CreateStatsComponent(player.id, 100, 100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10);
			player.statsComponentId = statsComponent.id;
			playerController = std::make_unique<PlayerController>(player);

			auto d2dFactory = deviceResources->GetD2DFactory();
			auto d2dDeviceContext = deviceResources->GetD2DDeviceContext();
			auto writeFactory = deviceResources->GetWriteFactory();

			skills = derivedEvent->skills;
			auto xOffset = 5.0f;
			auto yOffset = 25.0f;
			for (auto i = 0; i < derivedEvent->skills->size(); i++)
			{
				auto skill = derivedEvent->skills->at(i);
				skillList[skill->skillId] = std::make_unique<UISkillListing>(uiComponents, XMFLOAT3{ xOffset, yOffset + (18.0f * i), 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, *skill, blackBrush.Get(), d2dDeviceContext, writeFactory, textFormatFPS.Get());
				skillsPanel->AddChildComponent(*skillList[skill->skillId]);
			}

			abilities = derivedEvent->abilities;
			for (auto i = 0; i < derivedEvent->abilities->size(); i++)
			{
				auto ability = abilities->at(i);
				abilitiesContainer->AddAbility(ability, textures[ability->spriteId].Get());
			}

			// init characterHUD
			characterHUD = std::make_unique<UICharacterHUD>(uiComponents, XMFLOAT3{ 10.0f, 12.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, InGame, d2dDeviceContext, writeFactory, textFormatSuccessMessage.Get(), d2dFactory, statsComponent, healthBrush.Get(), manaBrush.Get(), staminaBrush.Get(), statBackgroundBrush.Get(), blackBrush.Get(), blackBrush.Get(), whiteBrush.Get(), derivedEvent->name->c_str());

			SetActiveLayer(InGame);

			break;
		}
		case EventType::GameObjectUpdate:
		{
			const auto derivedEvent = (GameObjectUpdateEvent*)event;

			const auto gameObjectId = derivedEvent->characterId;
			const auto pos = XMFLOAT3{ derivedEvent->posX, derivedEvent->posY, derivedEvent->posZ };
			const auto mov = XMFLOAT3{ derivedEvent->movX, derivedEvent->movY, derivedEvent->movZ };
			const auto modelId = derivedEvent->modelId;
			const auto textureId = derivedEvent->textureId;

			if (!objectManager.GameObjectExists(gameObjectId))
			{
				GameObject& obj = objectManager.CreateGameObject(pos, XMFLOAT3{ 14.0f, 14.0f, 14.0f }, gameObjectId);
				obj.movementVector = mov;
				auto sphereRenderComponent = renderComponentManager.CreateRenderComponent(gameObjectId, meshes[modelId].get(), vertexShader.Get(), pixelShader.Get(), textures[textureId].Get());
				obj.renderComponentId = sphereRenderComponent.GetId();
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

			g_socketManager.SendPacket(OPCODE_ACTIVATE_ABILITY, 1, std::to_string(derivedEvent->abilityId));

			break;
		}
		case EventType::PlayerCorrection:
		{
			const auto derivedEvent = (PlayerCorrectionEvent*)event;

			if (playerController)
				playerController->OnPlayerCorrectionEvent(derivedEvent);

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