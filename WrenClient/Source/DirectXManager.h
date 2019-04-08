#pragma once

#include "Model.h"
#include "Layer.h"
#include "GameTimer.h"
#include "SocketManager.h"
#include "GameMap/GameMap.h"
#include "PlayerController.h"
#include "Camera.h"
#include "UI/UICharacterListing.h"
#include "UI/UIInput.h"
#include "UI/UIInputGroup.h"
#include "UI/UIButton.h"
#include "UI/UIPanel.h"
#include "UI/UILabel.h"

struct ShaderBuffer
{
    BYTE* buffer;
    int size;
};

class DirectXManager : public Observer
{
	int currentX = 0;
	int currentZ = 0;
	
	XMMATRIX worldTransform;
	XMMATRIX viewTransform;
	XMMATRIX projectionTransform;

	Model* sphereModel = nullptr;
	Model* treeModel = nullptr;

	Layer activeLayer = Login;
	std::string characterNamePendingDeletion;
	ObjectManager& objectManager;
	GameMap* gameMap;
	std::vector<Observer*>* observers;
    std::vector<UICharacterListing*>* characterList = new std::vector<UICharacterListing*>;
    std::string token = "";
    GameTimer& timer;
    SocketManager& socketManager;
	Camera& camera;
	PlayerController& playerController;
	float mousePosX;
	float mousePosY;

    // DXGI
    IDXGISwapChain* swapChain = nullptr;

    // D3D
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;
    ID3D11RenderTargetView* backBufferRenderTargetView = nullptr;
	ID3D11RenderTargetView* offscreenRenderTargetView = nullptr;
    ID3D11DepthStencilView* multisampledDepthStencilView = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11Texture2D* backBuffer = nullptr;
	ID3D11Texture2D* offscreenRenderTarget = nullptr;

    // D2D
    ID2D1Factory2* d2dFactory = nullptr;
    ID2D1DeviceContext1* d2dDeviceContext = nullptr;

    // DirectWrite
    IDWriteFactory2* writeFactory = nullptr;

    // TextFormats
    IDWriteTextFormat* textFormatFPS = nullptr;
    IDWriteTextFormat* textFormatAccountCreds = nullptr;
    IDWriteTextFormat* textFormatHeaders = nullptr;
    IDWriteTextFormat* textFormatAccountCredsInputValue = nullptr;
    IDWriteTextFormat* textFormatButtonText = nullptr;
    IDWriteTextFormat* textFormatSuccessMessage = nullptr;
    IDWriteTextFormat* textFormatErrorMessage = nullptr;

    // TextLayouts
    IDWriteTextLayout* textLayoutFPS = nullptr;
    IDWriteTextLayout* textLayoutMousePos = nullptr;
    IDWriteTextLayout* textLayoutSuccessMessage = nullptr;
    IDWriteTextLayout* textLayoutErrorMessage = nullptr;

    // Brushes
    ID2D1SolidColorBrush* grayBrush = nullptr;
    ID2D1SolidColorBrush* blackBrush = nullptr;
    ID2D1SolidColorBrush* whiteBrush = nullptr;
    ID2D1SolidColorBrush* blueBrush = nullptr;
    ID2D1SolidColorBrush* darkBlueBrush = nullptr;
    ID2D1SolidColorBrush* successMessageBrush = nullptr;
    ID2D1SolidColorBrush* errorMessageBrush = nullptr;
    ID2D1SolidColorBrush* selectedCharacterBrush = nullptr;

    // Inputs
    UIInput* loginScreen_accountNameInput = nullptr;
    UIInput* loginScreen_passwordInput = nullptr;
    UIInput* loginScreen_newAccountAccountNameInput = nullptr;
    UIInput* loginScreen_newAccountPasswordInput = nullptr;
    UIInput* createAccount_accountNameInput = nullptr;
    UIInput* createAccount_passwordInput = nullptr;
    UIInput* createCharacter_characterNameInput = nullptr;

	// InputGroups
	UIInputGroup* loginScreen_inputGroup = nullptr;
	UIInputGroup* createAccount_inputGroup = nullptr;
	UIInputGroup* createCharacter_inputGroup = nullptr;

    // Buttons
    UIButton* loginScreen_loginButton = nullptr;
    UIButton* loginScreen_createAccountButton = nullptr;
    UIButton* createAccount_createAccountButton = nullptr;
    UIButton* createAccount_cancelButton = nullptr;
    UIButton* characterSelect_newCharacterButton = nullptr;
	UIButton* characterSelect_enterWorldButton = nullptr;
	UIButton* characterSelect_deleteCharacterButton = nullptr;
    UIButton* characterSelect_logoutButton = nullptr;
    UIButton* createCharacter_createCharacterButton = nullptr;
    UIButton* createCharacter_backButton = nullptr;
	UIButton* deleteCharacter_confirmButton = nullptr;
	UIButton* deleteCharacter_cancelButton = nullptr;
	UIButton* gameSettings_logoutButton = nullptr;

    // Labels
    UILabel* loginScreen_successMessageLabel = nullptr;
    UILabel* loginScreen_errorMessageLabel = nullptr;
    UILabel* createAccount_errorMessageLabel = nullptr;
    UILabel* connecting_statusLabel = nullptr;
    UILabel* characterSelect_successMessageLabel = nullptr;
	UILabel* characterSelect_errorMessageLabel = nullptr;
    UILabel* characterSelect_headerLabel = nullptr;
    UILabel* createCharacter_errorMessageLabel = nullptr;
	UILabel* deleteCharacter_headerLabel = nullptr;
    UILabel* enteringWorld_statusLabel = nullptr;

    // Panels
    UIPanel* gameSettingsPanel = nullptr;
    UIPanel* gameEditorPanel = nullptr;

	// Shaders
	ShaderBuffer vertexShaderBuffer;
	ID3D11VertexShader* vertexShader = nullptr;
	ShaderBuffer pixelShaderBuffer;
	ID3D11PixelShader* pixelShader = nullptr;

	// InputLayouts
	ID3D11InputLayout* inputLayout = nullptr;

	// Textures
	ID3D11ShaderResourceView* color01SRV = nullptr;
	ID3D11ShaderResourceView* color02SRV = nullptr;
	ID3D11ShaderResourceView* grass01SRV = nullptr;

	// Raster States
	ID3D11RasterizerState* wireframeRasterState = nullptr;
	ID3D11RasterizerState* solidRasterState = nullptr;

    void InitializeBrushes();
    void InitializeTextFormats();
    void InitializeInputs();
    void InitializeButtons();
    void InitializeLabels();
    void InitializePanels();
    void InitializeShaders();
	void InitializeBuffers();
	void InitializeTextures();
	void InitializeRasterStates();
	void RecreateCharacterListings(const std::vector<std::string*>* characterNames);
    ShaderBuffer LoadShader(std::wstring filename);
	UICharacterListing* GetCurrentlySelectedCharacterListing();
public:
	DirectXManager(GameTimer& timer, SocketManager& socketManager, ObjectManager& objectManager, PlayerController& playerController, Camera& camera)
		: objectManager{ objectManager },
		  timer{ timer },
		  socketManager{ socketManager },
		  playerController{ playerController },
		  camera{ camera }
	{
	};
    void Initialize(HWND hWnd);
    void DrawScene();
	virtual bool HandleEvent(const Event* event);
	void SetActiveLayer(const Layer layer);
};
