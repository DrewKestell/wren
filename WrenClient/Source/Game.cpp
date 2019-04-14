#include "stdafx.h"
#include "Game.h"
#include "ConstantBufferOnce.h"
#include "Camera.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/CreateAccountFailedEvent.h"
#include "EventHandling/Events/LoginSuccessEvent.h"
#include "EventHandling/Events/LoginFailedEvent.h"
#include "EventHandling/Events/CreateCharacterFailedEvent.h"
#include "EventHandling/Events/CreateCharacterSuccessEvent.h"
#include "EventHandling/Events/DeleteCharacterSuccessEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"
#include "EventHandling/Events/MouseEvent.h"

extern EventHandler g_eventHandler;

Game::Game() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

// General initialization that would likely be shared between any D3D application
//   should go in DeviceResources.cpp
// Game-specific initialization should go in Game.cpp
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	SetActiveLayer(Layer::InGame);
}

#pragma region Frame Update
void Game::Tick()
{
	while (m_socketManager.TryRecieveMessage()) {}

	m_timer.Tick();

	m_playerController->Update();

	g_eventHandler.PublishEvents();

	Render();
}

void Game::Update()
{
	float elapsedTime = float(m_timer.DeltaTime());

	// TODO: Add your game logic here.
	elapsedTime;
}
#pragma endregion

#pragma region Frame Render
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.TotalTime() == 0)
	{
		return;
	}

	Clear();

	//m_deviceResources->PIXBeginEvent(L"Render");
	auto d3dContext = m_deviceResources->GetD3DDeviceContext();
	auto d2dContext = m_deviceResources->GetD2DDeviceContext();

	d2dContext->BeginDraw();

	// used for FPS text
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;

	// update FPS text
	if (m_timer.TotalTime() - timeElapsed >= 1)
	{
		float mspf = (float)1000 / frameCnt;
		const auto fpsText = "FPS: " + std::to_string(frameCnt) + ", MSPF: " + std::to_string(mspf);
		fpsTextLabel->SetText(fpsText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;

		if (m_token != "")
			m_socketManager.SendPacket(OPCODE_HEARTBEAT, 1, m_token);
	}

	// update MousePos text
	const auto mousePosText = "MousePosX: " + std::to_string((int)m_mousePosX) + ", MousePosY: " + std::to_string((int)m_mousePosY);
	mousePosLabel->SetText(mousePosText.c_str());

	// Draw all objects registered in the ObjectManager
	m_objectManager.Draw();

	if (m_activeLayer == InGame)
	{
		auto camPos = m_camera.GetPosition();
		XMVECTORF32 s_Eye = { camPos.x, camPos.y, camPos.z, 0.0f };
		XMVECTORF32 s_At = { camPos.x - 500.0f, 0.0f, camPos.z + 500.0f, 0.0f };
		XMVECTORF32 s_Up = { 0.0f, 1.0f, 0.0f, 0.0f };
		m_viewTransform = XMMatrixLookAtLH(s_Eye, s_At, s_Up);

		d3dContext->RSSetState(solidRasterState.Get());

		//immediateContext->RSSetState(wireframeRasterState);
		m_gameMap->Draw(d3dContext, m_viewTransform, m_projectionTransform);

		m_sphereModel->Draw(d3dContext, m_viewTransform, m_projectionTransform);
		m_treeModel->Draw(d3dContext, m_viewTransform, m_projectionTransform);
	}

	d2dContext->EndDraw();

	//m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

void Game::Clear()
{
	//m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	//m_deviceResources->PIXEndEvent();
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
	m_timer.Reset();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();

	m_clientWidth = width;
	m_clientHeight = height;
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
	InitializePanels();
	InitializeShaders();
	InitializeBuffers();
	InitializeTextures();
	InitializeRasterStates();

	auto d2dDevice = m_deviceResources->GetD3DDevice();
	
	m_gameMap = new GameMap(d2dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader.Get(), pixelShader.Get(), grass01SRV.Get());
	
	std::string path = "../../WrenClient/Models/sphere.blend";
	m_sphereModel = new Model(d2dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader.Get(), pixelShader.Get(), color02SRV.Get(), path);
	
	path = "../../WrenClient/Models/tree.blend";
	m_treeModel = new Model(d2dDevice, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader.Get(), pixelShader.Get(), color01SRV.Get(), path);
	m_treeModel->Translate(XMMatrixTranslation(90.0f, 0.0f, 90.0f));
	
	m_playerController = new PlayerController{ m_timer, *m_sphereModel, m_camera };
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

ShaderBuffer Game::LoadShader(std::wstring filename)
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
	auto d2dContext = m_deviceResources->GetD2DDeviceContext();

	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f), &grayBrush);
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.22f, 0.22f, 1.0f), &blackBrush);
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &whiteBrush);
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.619f, 0.854f, 1.0f, 1.0f), &blueBrush);
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.301f, 0.729f, 1.0f, 1.0f), &darkBlueBrush);
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.137f, 0.98f, 0.117f, 1.0f), &successMessageBrush);
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.98f, 0.117f, 0.156f, 1.0f), &errorMessageBrush);
	d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.921f, 1.0f, 0.921f, 1.0f), &selectedCharacterBrush);
}

void Game::InitializeTextFormats()
{
	auto arialFontFamily = L"Arial";
	auto locale = L"en-US";

	auto writeFactory = m_deviceResources->GetWriteFactory();

	// FPS / MousePos
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, locale, &textFormatFPS);
	textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// Account Creds Input Values
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, &textFormatAccountCredsInputValue);
	textFormatAccountCredsInputValue->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatAccountCredsInputValue->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// Account Creds Labels
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, &textFormatAccountCreds);
	textFormatAccountCreds->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	textFormatAccountCreds->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// Headers
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, &textFormatHeaders);
	textFormatHeaders->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatHeaders->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// Button Text
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, &textFormatButtonText);
	textFormatButtonText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	textFormatButtonText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// SuccessMessage Text
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, &textFormatSuccessMessage);
	textFormatSuccessMessage->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatSuccessMessage->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// ErrorMessage Message
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, &textFormatErrorMessage);
	textFormatErrorMessage->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatErrorMessage->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

void Game::InitializeInputs()
{
	auto writeFactory = m_deviceResources->GetWriteFactory();
	auto d2dFactory = m_deviceResources->GetD2DFactory();
	auto d2dContext = m_deviceResources->GetD2DDeviceContext();

	// LoginScreen
	loginScreen_accountNameInput = new UIInput(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, m_objectManager, Login, false, 120.0f, 260.0f, 24.0f, blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, "Account Name:", writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	loginScreen_passwordInput = new UIInput(XMFLOAT3{ 15.0f, 50.0f, 0.0f }, m_objectManager, Login, true, 120.0f, 260.0f, 24.0f, blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, "Password:", writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	loginScreen_inputGroup = new UIInputGroup(Login);
	loginScreen_inputGroup->AddInput(loginScreen_accountNameInput);
	loginScreen_inputGroup->AddInput(loginScreen_passwordInput);

	// CreateAccount
	createAccount_accountNameInput = new UIInput(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, m_objectManager, CreateAccount, false, 120.0f, 260.0f, 24.0f, blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, "Account Name:", writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_passwordInput = new UIInput(XMFLOAT3{ 15.0f, 50.0f, 0.0f }, m_objectManager, CreateAccount, true, 120.0f, 260.0f, 24.0f, blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, "Password:", writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createAccount_inputGroup = new UIInputGroup(CreateAccount);
	createAccount_inputGroup->AddInput(createAccount_accountNameInput);
	createAccount_inputGroup->AddInput(createAccount_passwordInput);

	// CreateCharacter
	createCharacter_characterNameInput = new UIInput(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, m_objectManager, CreateCharacter, false, 140.0f, 260.0f, 24.0f, blackBrush.Get(), whiteBrush.Get(), grayBrush.Get(), blackBrush.Get(), textFormatAccountCredsInputValue.Get(), d2dContext, "Character Name:", writeFactory, textFormatAccountCreds.Get(), d2dFactory);
	createCharacter_inputGroup = new UIInputGroup(CreateCharacter);
	createCharacter_inputGroup->AddInput(createCharacter_characterNameInput);
}

void Game::InitializeButtons()
{
	auto writeFactory = m_deviceResources->GetWriteFactory();
	auto d2dFactory = m_deviceResources->GetD2DFactory();
	auto d2dContext = m_deviceResources->GetD2DDeviceContext();
	
	const auto onClickLoginButton = [this]()
	{
		loginScreen_successMessageLabel->SetText("");
		loginScreen_errorMessageLabel->SetText("");

		const auto accountName = ws2s(std::wstring(loginScreen_accountNameInput->GetInputValue()));
		const auto password = ws2s(std::wstring(loginScreen_passwordInput->GetInputValue()));

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

		m_socketManager.SendPacket(OPCODE_CONNECT, 2, accountName, password);
		SetActiveLayer(Connecting);
	};

	const auto onClickLoginScreenCreateAccountButton = [this]()
	{
		loginScreen_successMessageLabel->SetText("");
		loginScreen_errorMessageLabel->SetText("");
		SetActiveLayer(CreateAccount);
	};

	// LoginScreen
	loginScreen_loginButton = new UIButton(XMFLOAT3{ 145.0f, 96.0f, 0.0f }, m_objectManager, Login, 80.0f, 24.0f, onClickLoginButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "LOGIN", writeFactory, textFormatButtonText.Get(), d2dFactory);
	loginScreen_createAccountButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, m_objectManager, Login, 160.0f, 24.0f, onClickLoginScreenCreateAccountButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "CREATE ACCOUNT", writeFactory, textFormatButtonText.Get(), d2dFactory);

	const auto onClickCreateAccountCreateAccountButton = [this]()
	{
		createAccount_errorMessageLabel->SetText("");

		const auto accountName = ws2s(std::wstring(createAccount_accountNameInput->GetInputValue()));
		const auto password = ws2s(std::wstring(createAccount_passwordInput->GetInputValue()));

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

		m_socketManager.SendPacket(OPCODE_CREATE_ACCOUNT, 2, accountName, password);
	};

	const auto onClickCreateAccountCancelButton = [this]()
	{
		createAccount_errorMessageLabel->SetText("");
		SetActiveLayer(Login);
	};

	// CreateAccount
	createAccount_createAccountButton = new UIButton(XMFLOAT3{ 145.0f, 96.0f, 0.0f }, m_objectManager, CreateAccount, 80.0f, 24.0f, onClickCreateAccountCreateAccountButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "CREATE", writeFactory, textFormatButtonText.Get(), d2dFactory);
	createAccount_cancelButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, m_objectManager, CreateAccount, 80.0f, 24.0f, onClickCreateAccountCancelButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "CANCEL", writeFactory, textFormatButtonText.Get(), d2dFactory);

	const auto onClickCharacterSelectNewCharacterButton = [this]()
	{
		characterSelect_successMessageLabel->SetText("");

		if (m_characterList->size() == 5)
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
			m_socketManager.SendPacket(OPCODE_ENTER_WORLD, 2, m_token, characterInput->GetName());
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
			m_characterNamePendingDeletion = characterInput->GetName();
			SetActiveLayer(DeleteCharacter);
		}
	};

	const auto onClickCharacterSelectLogoutButton = [this]()
	{
		m_token = "";
		SetActiveLayer(Login);
	};

	// CharacterSelect
	characterSelect_newCharacterButton = new UIButton(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, m_objectManager, CharacterSelect, 140.0f, 24.0f, onClickCharacterSelectNewCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "NEW CHARACTER", writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_enterWorldButton = new UIButton(XMFLOAT3{ 170.0f, 20.0f, 0.0f }, m_objectManager, CharacterSelect, 120.0f, 24.0f, onClickCharacterSelectEnterWorldButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "ENTER WORLD", writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_deleteCharacterButton = new UIButton(XMFLOAT3{ 305.0f, 20.0f, 0.0f }, m_objectManager, CharacterSelect, 160.0f, 24.0f, onClickCharacterSelectDeleteCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "DELETE CHARACTER", writeFactory, textFormatButtonText.Get(), d2dFactory);
	characterSelect_logoutButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, m_objectManager, CharacterSelect, 80.0f, 24.0f, onClickCharacterSelectLogoutButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "LOGOUT", writeFactory, textFormatButtonText.Get(), d2dFactory);

	const auto onClickCreateCharacterCreateCharacterButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");

		const auto characterName = ws2s(std::wstring(createCharacter_characterNameInput->GetInputValue()));

		if (characterName.length() == 0)
		{
			createCharacter_errorMessageLabel->SetText("Character name can't be empty.");
			return;
		}

		m_socketManager.SendPacket(OPCODE_CREATE_CHARACTER, 2, m_token, characterName);
	};

	const auto onClickCreateCharacterBackButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");
		SetActiveLayer(CharacterSelect);
	};

	// CreateCharacter
	createCharacter_createCharacterButton = new UIButton(XMFLOAT3{ 165.0f, 64.0f, 0.0f }, m_objectManager, CreateCharacter, 160.0f, 24.0f, onClickCreateCharacterCreateCharacterButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "CREATE CHARACTER", writeFactory, textFormatButtonText.Get(), d2dFactory);
	createCharacter_backButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, m_objectManager, CreateCharacter, 80.0f, 24.0f, onClickCreateCharacterBackButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "BACK", writeFactory, textFormatButtonText.Get(), d2dFactory);

	// DeleteCharacter

	const auto onClickDeleteCharacterConfirm = [this]()
	{
		m_socketManager.SendPacket(OPCODE_DELETE_CHARACTER, 2, m_token, m_characterNamePendingDeletion);
	};

	const auto onClickDeleteCharacterCancel = [this]()
	{
		m_characterNamePendingDeletion = "";
		SetActiveLayer(CharacterSelect);
	};

	deleteCharacter_confirmButton = new UIButton(XMFLOAT3{ 10.0f, 30.0f, 0.0f }, m_objectManager, DeleteCharacter, 100.0f, 24.0f, onClickDeleteCharacterConfirm, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "CONFIRM", writeFactory, textFormatButtonText.Get(), d2dFactory);
	deleteCharacter_cancelButton = new UIButton(XMFLOAT3{ 120.0f, 30.0f, 0.0f }, m_objectManager, DeleteCharacter, 100.0f, 24.0f, onClickDeleteCharacterCancel, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "CANCEL", writeFactory, textFormatButtonText.Get(), d2dFactory);
}

void Game::InitializeLabels()
{
	auto writeFactory = m_deviceResources->GetWriteFactory();
	auto d2dFactory = m_deviceResources->GetD2DFactory();
	auto d2dContext = m_deviceResources->GetD2DDeviceContext();

	loginScreen_successMessageLabel = new UILabel(XMFLOAT3{ 30.0f, 170.0f, 0.0f }, m_objectManager, Login, 400.0f, successMessageBrush.Get(), textFormatSuccessMessage.Get(), d2dContext, writeFactory, d2dFactory);
	loginScreen_errorMessageLabel = new UILabel(XMFLOAT3{ 30.0f, 170.0f, 0.0f }, m_objectManager, Login, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	createAccount_errorMessageLabel = new UILabel(XMFLOAT3{ 30.0f, 170.0f, 0.0f }, m_objectManager, CreateAccount, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	connecting_statusLabel = new UILabel(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, m_objectManager, Connecting, 400.0f, blackBrush.Get(), textFormatAccountCreds.Get(), d2dContext, writeFactory, d2dFactory);
	connecting_statusLabel->SetText("Connecting...");

	characterSelect_successMessageLabel = new UILabel(XMFLOAT3{ 30.0f, 400.0f, 0.0f }, m_objectManager, CharacterSelect, 400.0f, successMessageBrush.Get(), textFormatSuccessMessage.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_errorMessageLabel = new UILabel(XMFLOAT3{ 30.0f, 400.0f, 0.0f }, m_objectManager, CharacterSelect, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_headerLabel = new UILabel(XMFLOAT3{ 15.0f, 60.0f, 0.0f }, m_objectManager, CharacterSelect, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	characterSelect_headerLabel->SetText("Character List:");

	createCharacter_errorMessageLabel = new UILabel(XMFLOAT3{ 30.0f, 170.0f, 0.0f }, m_objectManager, CreateCharacter, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);

	deleteCharacter_headerLabel = new UILabel(XMFLOAT3{ 10.0f, 10.0f, 0.0f }, m_objectManager, DeleteCharacter, 400.0f, errorMessageBrush.Get(), textFormatErrorMessage.Get(), d2dContext, writeFactory, d2dFactory);
	deleteCharacter_headerLabel->SetText("Are you sure you want to delete this character?");

	enteringWorld_statusLabel = new UILabel(XMFLOAT3{ 5.0f, 20.0f, 0.0f }, m_objectManager, EnteringWorld, 400.0f, blackBrush.Get(), textFormatAccountCreds.Get(), d2dContext, writeFactory, d2dFactory);
	enteringWorld_statusLabel->SetText("Entering World...");
}

void Game::InitializePanels()
{
	auto writeFactory = m_deviceResources->GetWriteFactory();
	auto d2dFactory = m_deviceResources->GetD2DFactory();
	auto d2dContext = m_deviceResources->GetD2DDeviceContext();

	// Game Settings
	const auto gameSettingsPanelX = (m_clientWidth - 400.0f) / 2.0f;
	const auto gameSettingsPanelY = (m_clientHeight - 200.0f) / 2.0f;
	gameSettingsPanelHeader = new UILabel{ XMFLOAT3{2.0f, 2.0f, 0.0f}, m_objectManager, InGame, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory };
	gameSettingsPanelHeader->SetText("Game Settings");
	const auto onClickGameSettingsLogoutButton = [this]()
	{
		m_socketManager.SendPacket(OPCODE_DISCONNECT, 1, m_token);
		m_token = "";
		SetActiveLayer(Login);
	};
	gameSettings_logoutButton = new UIButton(XMFLOAT3{ 10.0f, 26.0f, 0.0f }, m_objectManager, InGame, 80.0f, 24.0f, onClickGameSettingsLogoutButton, blueBrush.Get(), darkBlueBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, "LOGOUT", writeFactory, textFormatButtonText.Get(), d2dFactory);
	gameSettingsPanel = new UIPanel(XMFLOAT3{ gameSettingsPanelX, gameSettingsPanelY, 0.0f }, m_objectManager, InGame, false, 400.0f, 200.0f, VK_ESCAPE, darkBlueBrush.Get(), whiteBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);
	gameSettingsPanel->AddChildComponent(*gameSettingsPanelHeader);
	gameSettingsPanel->AddChildComponent(*gameSettings_logoutButton);

	// Game Editor
	const auto gameEditorPanelX = 580.0f;
	const auto gameEditorPanelY = 5.0f;
	gameEditorPanelHeader = new UILabel(XMFLOAT3{ 2.0f, 2.0f, 0.0f }, m_objectManager, InGame, 200.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	gameEditorPanelHeader->SetText("Game Editor");
	gameEditorPanel = new UIPanel(XMFLOAT3{ gameEditorPanelX, gameEditorPanelY, 0.0f }, m_objectManager, InGame, true, 200.0f, 400.0f, VK_F1, darkBlueBrush.Get(), whiteBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);
	gameEditorPanel->AddChildComponent(*gameEditorPanelHeader);

	// Diagnostics
	const auto diagnosticsPanelX = 580.0f;
	const auto diagnosticsPanelY = 336.0f;
	diagnosticsPanel = new UIPanel(XMFLOAT3{ diagnosticsPanelX, diagnosticsPanelY, 0.0f }, m_objectManager, InGame, true, 200.0f, 200.0f, VK_F2, darkBlueBrush.Get(), whiteBrush.Get(), grayBrush.Get(), d2dContext, d2dFactory);

	diagnosticsPanelHeader = new UILabel(XMFLOAT3{ 2.0f, 2.0f, 0.0f }, m_objectManager, InGame, 280.0f, blackBrush.Get(), textFormatHeaders.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanelHeader->SetText("Diagnostics");
	diagnosticsPanel->AddChildComponent(*diagnosticsPanelHeader);

	mousePosLabel = new UILabel(XMFLOAT3{ 2.0f, 22.0f, 0.0f }, m_objectManager, InGame, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*mousePosLabel);

	fpsTextLabel = new UILabel(XMFLOAT3{ 2.0f, 36.0f, 0.0f }, m_objectManager, InGame, 280.0f, blackBrush.Get(), textFormatFPS.Get(), d2dContext, writeFactory, d2dFactory);
	diagnosticsPanel->AddChildComponent(*fpsTextLabel);
}

UICharacterListing* Game::GetCurrentlySelectedCharacterListing()
{
	UICharacterListing* characterListing = nullptr;
	for (auto i = 0; i < m_characterList->size(); i++)
	{
		if (m_characterList->at(i)->IsSelected())
			characterListing = m_characterList->at(i);
	}
	return characterListing;
}

void Game::InitializeShaders()
{
	auto d3dDevice = m_deviceResources->GetD3DDevice();

	vertexShaderBuffer = LoadShader(L"VertexShader.cso");
	d3dDevice->CreateVertexShader(vertexShaderBuffer.buffer, vertexShaderBuffer.size, nullptr, &vertexShader);

	pixelShaderBuffer = LoadShader(L"PixelShader.cso");
	d3dDevice->CreatePixelShader(pixelShaderBuffer.buffer, pixelShaderBuffer.size, nullptr, &pixelShader);

	m_projectionTransform = XMMatrixOrthographicLH((float)m_clientWidth, (float)m_clientHeight, 0.1f, 5000.0f);
}

void Game::InitializeBuffers()
{
	auto d3dDevice = m_deviceResources->GetD3DDevice();
	auto d3dContext = m_deviceResources->GetD3DDeviceContext();

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

	d3dContext->PSSetConstantBuffers(1, 1, &constantBufferOnce);
}

void Game::InitializeRasterStates()
{
	auto d3dDevice = m_deviceResources->GetD3DDevice();

	CD3D11_RASTERIZER_DESC wireframeRasterStateDesc(D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	d3dDevice->CreateRasterizerState(&wireframeRasterStateDesc, &wireframeRasterState);

	CD3D11_RASTERIZER_DESC solidRasterStateDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	d3dDevice->CreateRasterizerState(&solidRasterStateDesc, &solidRasterState);
}

void Game::InitializeTextures()
{
	auto d3dDevice = m_deviceResources->GetD3DDevice();

	CreateDDSTextureFromFile(d3dDevice, L"../../WrenClient/Textures/texture01.dds", nullptr, &color01SRV);
	CreateDDSTextureFromFile(d3dDevice, L"../../WrenClient/Textures/texture02.dds", nullptr, &color02SRV);
	CreateDDSTextureFromFile(d3dDevice, L"../../WrenClient/Textures/grass01.dds", nullptr, &grass01SRV);
}

void Game::RecreateCharacterListings(const std::vector<std::string*>* characterNames)
{
	auto writeFactory = m_deviceResources->GetWriteFactory();
	auto d2dFactory = m_deviceResources->GetD2DFactory();
	auto d2dContext = m_deviceResources->GetD2DDeviceContext();

	for (auto i = 0; i < m_characterList->size(); i++)
		delete(m_characterList->at(i));
	m_characterList->clear();

	for (auto i = 0; i < characterNames->size(); i++)
	{
		const float y = 100.0f + (i * 40.0f);
		m_characterList->push_back(new UICharacterListing(XMFLOAT3{ 25.0f, y, 0.0f }, m_objectManager, CharacterSelect, 260.0f, 30.0f, whiteBrush.Get(), selectedCharacterBrush.Get(), grayBrush.Get(), blackBrush.Get(), d2dContext, (*characterNames->at(i)).c_str(), writeFactory, textFormatAccountCredsInputValue.Get(), d2dFactory));
	}
}

bool Game::HandleEvent(const Event* event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::KeyDownEvent:
		{
			const auto derivedEvent = (KeyDownEvent*)event;

			break;
		}
		case EventType::MouseMoveEvent:
		{
			const auto derivedEvent = (MouseEvent*)event;

			m_mousePosX = derivedEvent->mousePosX;
			m_mousePosY = derivedEvent->mousePosY;

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

			m_token = *(derivedEvent->token);
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
			m_characterNamePendingDeletion = "";
			createCharacter_errorMessageLabel->SetText("");
			characterSelect_successMessageLabel->SetText("Character deleted successfully.");
			SetActiveLayer(CharacterSelect);

			break;
		}
		case EventType::EnterWorldSuccess:
		{
			SetActiveLayer(InGame);

			break;
		}
	}

	return false;
}

void Game::SetActiveLayer(const Layer layer)
{
	m_activeLayer = layer;
	g_eventHandler.QueueEvent(new ChangeActiveLayerEvent{ layer });
}