#include "stdafx.h"
#include "DirectXManager.h"
#include "ConstantBufferOnce.h"
#include "EventHandling/Events/MouseMoveEvent.h"
#include "EventHandling/Events/ChangeActiveLayerEvent.h"
#include "EventHandling/Events/CreateAccountFailedEvent.h"
#include "EventHandling/Events/LoginSuccessEvent.h"
#include "EventHandling/Events/LoginFailedEvent.h"
#include "EventHandling/Events/CreateCharacterFailedEvent.h"
#include "EventHandling/Events/CreateCharacterSuccessEvent.h"
#include "EventHandling/Events/DeleteCharacterSuccessEvent.h"
#include "EventHandling/Events/KeyDownEvent.h"

extern EventHandler* g_eventHandler;

void DirectXManager::Initialize(HWND hWnd)
{
    HRESULT hr;
    D3D_FEATURE_LEVEL featureLevelsRequested[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT numFeatureLevelsRequested = 1;
    D3D_FEATURE_LEVEL featureLevelsSupported;
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        createDeviceFlags,
        featureLevelsRequested,
        numFeatureLevelsRequested,
        D3D11_SDK_VERSION,
        &device,
        &featureLevelsSupported,
        &immediateContext
    );
    if (FAILED(hr))
        throw std::exception("Failed to create device.");

    UINT msaaCount = 8;
    UINT m4xMsaaQuality = 0;
    hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, msaaCount, &m4xMsaaQuality);
    if (FAILED(hr))
        throw std::exception("Failed to check MSAA quality.");

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
	sd.Flags = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    RECT rect;
    GetClientRect(hWnd, &rect);
    clientWidth = rect.right - rect.left;
    clientHeight = rect.bottom - rect.top;

    // Get the dxgi device
    IDXGIDevice* dxgiDevice;
    hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Device.");

    // Get the dxgi adapter
    IDXGIAdapter* dxgiAdapter;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Adapter.");

    // Get the dxgi factory interface
    IDXGIFactory* dxgiFactory;
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    if (FAILED(hr))
        throw std::exception("Failed to get DXGI Factory.");

    // Create swap chain
    hr = dxgiFactory->CreateSwapChain(device, &sd, &swapChain);
    if (FAILED(hr))
        throw std::exception("Failed to create swap chain.");

    // Get pointer to back buffer
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
        throw std::exception("Failed to get pointer to back buffer.");

    // Create a render-target view 
    device->CreateRenderTargetView(backBuffer, NULL, &backBufferRenderTargetView);

    // Create Depth/Stencil Buffer and View
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = clientWidth;
    depthStencilDesc.Height = clientHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = msaaCount;
    depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D* depthStencilBuffer;   
    hr = device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
    if (FAILED(hr))
        throw std::exception("Failed to create texture 2d.");

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
    hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
    if (FAILED(hr))
        throw std::exception("Failed to create texture depth stencil view.");

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (float)clientWidth;
    vp.Height = (float)clientHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    immediateContext->RSSetViewports(1, &vp);

    // DWriteFactory
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
    if (FAILED(hr))
        throw std::exception("Failed to initialize DirectWrite Factory.");

    // Direct2DFactory    
    D2D1_FACTORY_OPTIONS options;
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, _uuidof(ID2D1Factory), &options, reinterpret_cast<void**>(&d2dFactory));
    if (FAILED(hr))
        throw std::exception("Failed to create Direct2D Factory.");

    // Create D2D Device
    ID2D1Device1* d2dDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
    if (FAILED(hr))
        throw std::exception("Failed to get Direct2D Device.");

    // Create D2D Device Context   
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &d2dDeviceContext);
    if (FAILED(hr))
        throw std::exception("Failed to get Direct2D Device Context.");

    // Set D2D Render Target to back buffer
    D2D1_BITMAP_PROPERTIES1 bp;
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bp.dpiX = 96.0f;
    bp.dpiY = 96.0f;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.colorContext = nullptr;

    // Get reference to DXGI back-buffer
    IDXGISurface1* dxgiBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&dxgiBuffer));
    if (FAILED(hr))
        throw std::exception("Failed to retrieve the back buffer.");

    // create the bitmap
    ID2D1Bitmap1* targetBitmap;
    hr = d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer, &bp, &targetBitmap);
    if (FAILED(hr))
        throw std::exception("Failed to create the Direct2D bitmap from the DXGI surface.");

    // set the newly created bitmap as render target
    d2dDeviceContext->SetTarget(targetBitmap);

	// create offscreen render target
	D3D11_TEXTURE2D_DESC offScreenSurfaceDesc;
	ZeroMemory(&offScreenSurfaceDesc, sizeof(D3D11_TEXTURE2D_DESC));

	offScreenSurfaceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	offScreenSurfaceDesc.Width = static_cast<unsigned int>(clientWidth);
	offScreenSurfaceDesc.Height = static_cast<unsigned int>(clientHeight);
	offScreenSurfaceDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	offScreenSurfaceDesc.MipLevels = 1;
	offScreenSurfaceDesc.ArraySize = 1;
	offScreenSurfaceDesc.SampleDesc.Count = msaaCount;;
	offScreenSurfaceDesc.SampleDesc.Quality = m4xMsaaQuality - 1;

	hr = device->CreateTexture2D(&offScreenSurfaceDesc, nullptr, &offscreenRenderTarget);
	if (FAILED(hr))
		throw std::exception("Failed to create offscreen render target.");

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS);
	hr = device->CreateRenderTargetView(offscreenRenderTarget, &renderTargetViewDesc, &offscreenRenderTargetView);
	if (FAILED(hr))
		throw std::exception("Failed to create offscreen render target view.");

    InitializeBrushes();
    InitializeTextFormats();
    InitializeInputs();
    InitializeButtons();
    InitializeLabels();
    InitializePanels();
    InitializeShaders();
	InitializeBuffers();
	InitializeRasterStates();
	InitializeTextures();

	gameMap = new GameMap(device, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader, pixelShader, grass01SRV);

	std::string path = "../../WrenClient/Models/sphere.blend";
	sphereModel = new Model(device, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader, pixelShader, color02SRV, path);

	path = "../../WrenClient/Models/tree.blend";
	treeModel = new Model(device, vertexShaderBuffer.buffer, vertexShaderBuffer.size, vertexShader, pixelShader, color01SRV, path);
	treeModel->SetPosition(3, 3);

	SetActiveLayer(InGame);
}

void DirectXManager::InitializeBrushes()
{
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f), &grayBrush);
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.22f, 0.22f, 1.0f), &blackBrush);
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &whiteBrush);
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.619f, 0.854f, 1.0f, 1.0f), &blueBrush);
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.301f, 0.729f, 1.0f, 1.0f), &darkBlueBrush);
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.137f, 0.98f, 0.117f, 1.0f), &successMessageBrush);
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.98f, 0.117f, 0.156f, 1.0f), &errorMessageBrush);
	d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.921f, 1.0f, 0.921f, 1.0f), &selectedCharacterBrush);
}

void DirectXManager::InitializeTextFormats()
{
    auto arialFontFamily = L"Arial";
    auto locale = L"en-US";

    // FPS / MousePos
	writeFactory->CreateTextFormat(arialFontFamily, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, locale, &textFormatFPS);
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

void DirectXManager::InitializeInputs()
{
    // LoginScreen
    loginScreen_accountNameInput = new UIInput(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, objectManager, Login, false, 120.0f, 260.0f, 24.0f, blackBrush, whiteBrush, grayBrush, blackBrush, textFormatAccountCredsInputValue, d2dDeviceContext, "Account Name:", writeFactory, textFormatAccountCreds, d2dFactory);
    loginScreen_passwordInput = new UIInput(XMFLOAT3{ 15.0f, 50.0f, 0.0f}, objectManager, Login, true, 120.0f, 260.0f, 24.0f, blackBrush, whiteBrush, grayBrush, blackBrush, textFormatAccountCredsInputValue, d2dDeviceContext, "Password:", writeFactory, textFormatAccountCreds, d2dFactory);
	loginScreen_inputGroup = new UIInputGroup(Login);
	loginScreen_inputGroup->AddInput(loginScreen_accountNameInput);
	loginScreen_inputGroup->AddInput(loginScreen_passwordInput);

    // CreateAccount
    createAccount_accountNameInput = new UIInput(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, objectManager, CreateAccount, false, 120.0f, 260.0f, 24.0f, blackBrush, whiteBrush, grayBrush, blackBrush, textFormatAccountCredsInputValue, d2dDeviceContext, "Account Name:", writeFactory, textFormatAccountCreds, d2dFactory);
    createAccount_passwordInput = new UIInput(XMFLOAT3{ 15.0f, 50.0f, 0.0f }, objectManager, CreateAccount, true, 120.0f, 260.0f, 24.0f, blackBrush, whiteBrush, grayBrush, blackBrush, textFormatAccountCredsInputValue, d2dDeviceContext, "Password:", writeFactory, textFormatAccountCreds, d2dFactory);
	createAccount_inputGroup = new UIInputGroup(CreateAccount);
	createAccount_inputGroup->AddInput(createAccount_accountNameInput);
	createAccount_inputGroup->AddInput(createAccount_passwordInput);

    // CreateCharacter
    createCharacter_characterNameInput = new UIInput(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, objectManager, CreateCharacter, false, 140.0f, 260.0f, 24.0f, blackBrush, whiteBrush, grayBrush, blackBrush, textFormatAccountCredsInputValue, d2dDeviceContext, "Character Name:", writeFactory, textFormatAccountCreds, d2dFactory);
	createCharacter_inputGroup = new UIInputGroup(CreateCharacter);
	createCharacter_inputGroup->AddInput(createCharacter_characterNameInput);
}

void DirectXManager::InitializeButtons()
{
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

		socketManager.SendPacket(OPCODE_CONNECT, 2, accountName, password);
		SetActiveLayer(Connecting);
	};

	const auto onClickLoginScreenCreateAccountButton = [this]()
	{
		loginScreen_successMessageLabel->SetText("");
		loginScreen_errorMessageLabel->SetText("");
		SetActiveLayer(CreateAccount);
	};

    // LoginScreen
	loginScreen_loginButton = new UIButton(XMFLOAT3{ 145.0f, 96.0f, 0.0f }, objectManager, Login, 80.0f, 24.0f, onClickLoginButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "LOGIN", writeFactory, textFormatButtonText, d2dFactory);
    loginScreen_createAccountButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, objectManager, Login, 160.0f, 24.0f, onClickLoginScreenCreateAccountButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "CREATE ACCOUNT", writeFactory, textFormatButtonText, d2dFactory);

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

		socketManager.SendPacket(OPCODE_CREATE_ACCOUNT, 2, accountName, password);
	};

	const auto onClickCreateAccountCancelButton = [this]()
	{
		createAccount_errorMessageLabel->SetText("");
		SetActiveLayer(Login);
	};

    // CreateAccount
    createAccount_createAccountButton = new UIButton(XMFLOAT3{ 145.0f, 96.0f, 0.0f }, objectManager, CreateAccount, 80.0f, 24.0f, onClickCreateAccountCreateAccountButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "CREATE", writeFactory, textFormatButtonText, d2dFactory);
    createAccount_cancelButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, objectManager, CreateAccount, 80.0f, 24.0f, onClickCreateAccountCancelButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "CANCEL", writeFactory, textFormatButtonText, d2dFactory);

	const auto onClickCharacterSelectNewCharacterButton = [this]()
	{
		characterSelect_successMessageLabel->SetText("");

		if (characterList->size() == 5)
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
			socketManager.SendPacket(OPCODE_ENTER_WORLD, 2, token, characterInput->GetName());
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
		token = "";
		SetActiveLayer(Login);
	};

    // CharacterSelect
    characterSelect_newCharacterButton = new UIButton(XMFLOAT3{ 15.0f, 20.0f, 0.0f }, objectManager, CharacterSelect, 140.0f, 24.0f, onClickCharacterSelectNewCharacterButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "NEW CHARACTER", writeFactory, textFormatButtonText, d2dFactory);
    characterSelect_enterWorldButton = new UIButton(XMFLOAT3{ 170.0f, 20.0f, 0.0f }, objectManager, CharacterSelect, 120.0f, 24.0f, onClickCharacterSelectEnterWorldButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "ENTER WORLD", writeFactory, textFormatButtonText, d2dFactory);
	characterSelect_deleteCharacterButton = new UIButton(XMFLOAT3{ 305.0f, 20.0f, 0.0f }, objectManager, CharacterSelect, 160.0f, 24.0f, onClickCharacterSelectDeleteCharacterButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "DELETE CHARACTER", writeFactory, textFormatButtonText, d2dFactory);
	characterSelect_logoutButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, objectManager, CharacterSelect, 80.0f, 24.0f, onClickCharacterSelectLogoutButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "LOGOUT", writeFactory, textFormatButtonText, d2dFactory);
    
	const auto onClickCreateCharacterCreateCharacterButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");

		const auto characterName = ws2s(std::wstring(createCharacter_characterNameInput->GetInputValue()));

		if (characterName.length() == 0)
		{
			createCharacter_errorMessageLabel->SetText("Character name can't be empty.");
			return;
		}

		socketManager.SendPacket(OPCODE_CREATE_CHARACTER, 2, token, characterName);
	};

	const auto onClickCreateCharacterBackButton = [this]()
	{
		createCharacter_errorMessageLabel->SetText("");
		SetActiveLayer(CharacterSelect);
	};

    // CreateCharacter
    createCharacter_createCharacterButton = new UIButton(XMFLOAT3{ 165.0f, 64.0f, 0.0f }, objectManager, CreateCharacter, 160.0f, 24.0f, onClickCreateCharacterCreateCharacterButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "CREATE CHARACTER", writeFactory, textFormatButtonText, d2dFactory);
    createCharacter_backButton = new UIButton(XMFLOAT3{ 15.0f, 522.0f, 0.0f }, objectManager, CreateCharacter, 80.0f, 24.0f, onClickCreateCharacterBackButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "BACK", writeFactory, textFormatButtonText, d2dFactory);

	// DeleteCharacter

	const auto onClickDeleteCharacterConfirm = [this]()
	{
		socketManager.SendPacket(OPCODE_DELETE_CHARACTER, 2, token, characterNamePendingDeletion);
	};

	const auto onClickDeleteCharacterCancel = [this]()
	{
		characterNamePendingDeletion = "";
		SetActiveLayer(CharacterSelect);
	};

	deleteCharacter_confirmButton = new UIButton(XMFLOAT3{ 10.0f, 30.0f, 0.0f }, objectManager, DeleteCharacter, 100.0f, 24.0f, onClickDeleteCharacterConfirm, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "CONFIRM", writeFactory, textFormatButtonText, d2dFactory);
	deleteCharacter_cancelButton = new UIButton(XMFLOAT3{ 120.0f, 30.0f, 0.0f }, objectManager, DeleteCharacter, 100.0f, 24.0f, onClickDeleteCharacterCancel, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "CANCEL", writeFactory, textFormatButtonText, d2dFactory);
}

void DirectXManager::InitializeLabels()
{
    loginScreen_successMessageLabel = new UILabel(XMFLOAT3{30.0f, 170.0f, 0.0f}, objectManager, Login, 400.0f, successMessageBrush, textFormatSuccessMessage, d2dDeviceContext, writeFactory, d2dFactory);
    loginScreen_errorMessageLabel = new UILabel(XMFLOAT3{30.0f, 170.0f, 0.0f}, objectManager, Login, 400.0f, errorMessageBrush, textFormatErrorMessage, d2dDeviceContext, writeFactory, d2dFactory);

    createAccount_errorMessageLabel = new UILabel(XMFLOAT3{30.0f, 170.0f, 0.0f}, objectManager, CreateAccount, 400.0f, errorMessageBrush, textFormatErrorMessage, d2dDeviceContext, writeFactory, d2dFactory);

    connecting_statusLabel = new UILabel(XMFLOAT3{15.0f, 20.0f, 0.0f}, objectManager, Connecting, 400.0f,  blackBrush, textFormatAccountCreds, d2dDeviceContext, writeFactory, d2dFactory);
    connecting_statusLabel->SetText("Connecting...");

    characterSelect_successMessageLabel = new UILabel(XMFLOAT3{30.0f, 400.0f, 0.0f}, objectManager, CharacterSelect, 400.0f, successMessageBrush, textFormatSuccessMessage, d2dDeviceContext, writeFactory, d2dFactory);
	characterSelect_errorMessageLabel = new UILabel(XMFLOAT3{ 30.0f, 400.0f, 0.0f }, objectManager, CharacterSelect, 400.0f, errorMessageBrush, textFormatErrorMessage, d2dDeviceContext, writeFactory, d2dFactory);
	characterSelect_headerLabel = new UILabel(XMFLOAT3{15.0f, 60.0f, 0.0f}, objectManager, CharacterSelect, 200.0f, blackBrush, textFormatHeaders, d2dDeviceContext, writeFactory, d2dFactory);
    characterSelect_headerLabel->SetText("Character List:");

    createCharacter_errorMessageLabel = new UILabel(XMFLOAT3{30.0f, 170.0f, 0.0f}, objectManager, CreateCharacter, 400.0f, errorMessageBrush, textFormatErrorMessage, d2dDeviceContext, writeFactory, d2dFactory);

	deleteCharacter_headerLabel = new UILabel(XMFLOAT3{ 10.0f, 10.0f, 0.0f }, objectManager, DeleteCharacter, 400.0f, errorMessageBrush, textFormatErrorMessage, d2dDeviceContext, writeFactory, d2dFactory);
	deleteCharacter_headerLabel->SetText("Are you sure you want to delete this character?");

    enteringWorld_statusLabel = new UILabel(XMFLOAT3{ 5.0f, 20.0f, 0.0f}, objectManager, EnteringWorld, 400.0f, blackBrush, textFormatAccountCreds, d2dDeviceContext, writeFactory, d2dFactory);
    enteringWorld_statusLabel->SetText("Entering World...");
}

void DirectXManager::InitializePanels()
{
    const auto gameSettingsPanelX = (clientWidth - 400.0f) / 2.0f;
    const auto gameSettingsPanelY = (clientHeight - 200.0f) / 2.0f;
    auto gameSettingsPanelHeader = new UILabel{XMFLOAT3{2.0f, 2.0f, 0.0f}, objectManager, InGame, 200.0f, blackBrush, textFormatHeaders, d2dDeviceContext, writeFactory, d2dFactory};
    gameSettingsPanelHeader->SetText("Game Settings");
	const auto onClickGameSettingsLogoutButton = [this]()
	{
		socketManager.SendPacket(OPCODE_DISCONNECT, 1, token);
		token = "";
		SetActiveLayer(Login);
	};
	auto gameSettings_logoutButton = new UIButton(XMFLOAT3{ 10.0f, 26.0f, 0.0f }, objectManager, InGame, 80.0f, 24.0f, onClickGameSettingsLogoutButton, blueBrush, darkBlueBrush, grayBrush, blackBrush, d2dDeviceContext, "LOGOUT", writeFactory, textFormatButtonText, d2dFactory);
    gameSettingsPanel = new UIPanel(XMFLOAT3{gameSettingsPanelX, gameSettingsPanelY, 0.0f}, objectManager, InGame, false, 400.0f, 200.0f, VK_ESCAPE, darkBlueBrush, whiteBrush, grayBrush, d2dDeviceContext, d2dFactory);
    gameSettingsPanel->AddChildComponent(*gameSettingsPanelHeader);
	gameSettingsPanel->AddChildComponent(*gameSettings_logoutButton);

    const auto gameEditorPanelX = 580.0f;
    const auto gameEditorPanelY = 5.0f;
    auto gameEditorPanelHeader = new UILabel(XMFLOAT3{2.0f, 2.0f, 0.0f}, objectManager, InGame, 200.0f, blackBrush, textFormatHeaders, d2dDeviceContext, writeFactory, d2dFactory);
    gameEditorPanelHeader->SetText("Game Editor");
    gameEditorPanel = new UIPanel(XMFLOAT3{gameEditorPanelX, gameEditorPanelY, 0.0f}, objectManager, InGame, true, 200.0f, 400.0f, VK_F1, darkBlueBrush, whiteBrush, grayBrush, d2dDeviceContext, d2dFactory);
    gameEditorPanel->AddChildComponent(*gameEditorPanelHeader);
}

UICharacterListing* DirectXManager::GetCurrentlySelectedCharacterListing()
{
	UICharacterListing* characterListing = nullptr;
	for (auto i = 0; i < characterList->size(); i++)
	{
		if (characterList->at(i)->IsSelected())
			characterListing = characterList->at(i);
	}
	return characterListing;
}

void DirectXManager::DrawScene()
{
    HRESULT hr;
    float color[4];
    color[3] = 1.0f;
    if (activeLayer == InGame)
    {
        color[0] = 0.9f;
        color[1] = 0.9f;
        color[2] = 0.9f;
    }
    else
    {
        color[0] = 1.0f;
        color[1] = 0.5f;
        color[2] = 0.3f;
    }
    immediateContext->ClearRenderTargetView(offscreenRenderTargetView, color);
    immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	immediateContext->OMSetRenderTargets(1, &offscreenRenderTargetView, depthStencilView);

	if (activeLayer == InGame)
	{
		XMVECTORF32 s_Eye = { camX, camY, camZ, 0.0f };
		XMVECTORF32 s_At = { camX - 500.0f, 0.0f, camZ + 500.0f, 0.0f };
		XMVECTORF32 s_Up = { 0.0f, 1.0f, 0.0f, 0.0f };
		viewTransform = XMMatrixLookAtLH(s_Eye, s_At, s_Up);

		immediateContext->RSSetState(solidRasterState);

		//immediateContext->RSSetState(wireframeRasterState);
		gameMap->Draw(immediateContext, viewTransform, projectionTransform);

		sphereModel->Draw(immediateContext, viewTransform, projectionTransform);
		treeModel->Draw(immediateContext, viewTransform, projectionTransform);
	}
	
	immediateContext->ResolveSubresource(backBuffer, 0, offscreenRenderTarget, 0, DXGI_FORMAT_B8G8R8A8_UNORM);

	immediateContext->OMSetRenderTargets(1, &backBufferRenderTargetView, depthStencilView);

	d2dDeviceContext->BeginDraw();

	// draw FPS
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;

	if (timer.TotalTime() - timeElapsed >= 1)
	{
		float mspf = (float)1000 / frameCnt;

		std::wostringstream outFPS;
		outFPS.precision(6);
		outFPS << "FPS: " << frameCnt << ", MSPF: " << mspf;

		if (textLayoutFPS != nullptr)
			textLayoutFPS->Release();
		writeFactory->CreateTextLayout(outFPS.str().c_str(), (unsigned int)outFPS.str().size(), textFormatFPS, (float)clientWidth, (float)clientHeight, &textLayoutFPS);

		frameCnt = 0;
		timeElapsed += 1.0f;

		if (token != "")
			socketManager.SendPacket(OPCODE_HEARTBEAT, 1, token);
	}

	if (textLayoutFPS != nullptr)
		d2dDeviceContext->DrawTextLayout(D2D1::Point2F(540.0f, 540.0f), textLayoutFPS, blackBrush);

	// draw MousePos
	std::wostringstream outMousePos;
	outMousePos.precision(6);
	outMousePos << "MousePosX: " << mousePosX << ", MousePosY: " << mousePosY;
	if (textLayoutMousePos != nullptr)
		textLayoutMousePos->Release();
	writeFactory->CreateTextLayout(outMousePos.str().c_str(), (unsigned int)outMousePos.str().size(), textFormatFPS, (float)clientWidth, (float)clientHeight, &textLayoutMousePos);
	d2dDeviceContext->DrawTextLayout(D2D1::Point2F(540.0f, 520.0f), textLayoutMousePos, blackBrush);

	// draw all GameObjects
	objectManager.Draw();

	d2dDeviceContext->EndDraw();
    
    hr = swapChain->Present(0, 0);
    if (FAILED(hr))
        throw std::exception("Present failed.");
}

void DirectXManager::RecreateCharacterListings(const std::vector<std::string*>* characterNames)
{
    for (auto i = 0; i < characterList->size(); i++)
        delete(characterList->at(i));
    characterList->clear();

    for (auto i = 0; i < characterNames->size(); i++)
    {
        const float y = 100.0f + (i * 40.0f);
        characterList->push_back(new UICharacterListing(XMFLOAT3{ 25.0f, y, 0.0f }, objectManager, CharacterSelect, 260.0f, 30.0f, whiteBrush, selectedCharacterBrush, grayBrush, blackBrush, d2dDeviceContext, (*characterNames->at(i)).c_str(), writeFactory, textFormatAccountCredsInputValue, d2dFactory));
    }
}

void DirectXManager::InitializeShaders()
{
    vertexShaderBuffer = LoadShader(L"VertexShader.cso");
    device->CreateVertexShader(vertexShaderBuffer.buffer, vertexShaderBuffer.size, nullptr, &vertexShader);

    pixelShaderBuffer = LoadShader(L"PixelShader.cso");
    device->CreatePixelShader(pixelShaderBuffer.buffer, pixelShaderBuffer.size, nullptr, &pixelShader);

	projectionTransform = XMMatrixOrthographicLH((float)clientWidth, (float)clientHeight, 0.1f, 5000.0f);
}

void DirectXManager::InitializeBuffers()
{
	// create constant buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.MiscFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(ConstantBufferOnce);

	ID3D11Buffer* constantBufferOnce = nullptr;
	device->CreateBuffer(&bufferDesc, nullptr, &constantBufferOnce);

	// map ConstantBuffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	immediateContext->Map(constantBufferOnce, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto pCB = reinterpret_cast<ConstantBufferOnce*>(mappedResource.pData);
	XMStoreFloat4(&pCB->directionalLight, XMVECTOR{ 0.0f, -1.0f, 0.5f, 0.0f });
	immediateContext->Unmap(constantBufferOnce, 0);

	immediateContext->PSSetConstantBuffers(1, 1, &constantBufferOnce);
}

void DirectXManager::InitializeRasterStates()
{
	CD3D11_RASTERIZER_DESC wireframeRasterStateDesc(D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	device->CreateRasterizerState(&wireframeRasterStateDesc, &wireframeRasterState);

	CD3D11_RASTERIZER_DESC solidRasterStateDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	device->CreateRasterizerState(&solidRasterStateDesc, &solidRasterState);
}

void DirectXManager::InitializeTextures()
{
	CreateDDSTextureFromFile(device, L"../../WrenClient/Textures/texture01.dds", nullptr, &color01SRV);
	CreateDDSTextureFromFile(device, L"../../WrenClient/Textures/texture02.dds", nullptr, &color02SRV);
	CreateDDSTextureFromFile(device, L"../../WrenClient/Textures/grass01.dds", nullptr, &grass01SRV);
}

ShaderBuffer DirectXManager::LoadShader(std::wstring filename)
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

bool DirectXManager::HandleEvent(const Event* event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::KeyDownEvent:
		{
			const auto derivedEvent = (KeyDownEvent*)event;

			if (derivedEvent->charCode == '7' && currentX > 0)
			{
				currentX--;
				camX -= 60.0f;
			}
			if (derivedEvent->charCode == '8' && currentX > 0 && currentZ < 100)
			{
				currentX--;
				currentZ++;
				camX -= 60.0f;
				camZ += 60.0f;
			}
			if (derivedEvent->charCode == '9' && currentZ < 100)
			{
				currentZ++;
				camZ += 60.0f;
			}
			if (derivedEvent->charCode == '6' && currentX < 100 && currentZ < 100)
			{
				currentX++;
				currentZ++;
				camX += 60.0f;
				camZ += 60.0f;
			}
			if (derivedEvent->charCode == '3' && currentX < 100)
			{
				currentX++;
				camX += 60.0f;
			}
			if (derivedEvent->charCode == '2' && currentX < 100 && currentZ > 0)
			{
				currentX++;
				currentZ--;
				camX += 60.0f;
				camZ -= 60.0f;
			}
			if (derivedEvent->charCode == '1' && currentZ > 0)
			{
				currentZ--;
				camZ -= 60.0f;
			}
			if (derivedEvent->charCode == '4' && currentX > 0 && currentZ > 0)
			{
				currentX--;
				currentZ--;
				camX -= 60.0f;
				camZ -= 60.0f;
			}

			sphereModel->SetPosition(currentZ, currentX);
			
			break;
		}
		case EventType::MouseMoveEvent:
		{
			const auto derivedEvent = (MouseMoveEvent*)event;

			mousePosX = derivedEvent->mousePosX;
			mousePosY = derivedEvent->mousePosY;

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

			token = *(derivedEvent->token);
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
			SetActiveLayer(InGame);

			break;
		}
	}

	return false;
}

void DirectXManager::SetActiveLayer(const Layer layer)
{
	activeLayer = layer;
	g_eventHandler->QueueEvent(new ChangeActiveLayerEvent{ layer });
}