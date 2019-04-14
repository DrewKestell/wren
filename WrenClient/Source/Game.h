#pragma once

#include "DeviceResources.h"
#include "GameTimer.h"
#include "SocketManager.h"
#include "ObjectManager.h"
#include "Camera.h"
#include "ShaderBuffer.h"
#include "Model.h"
#include "PlayerController.h"
#include "GameMap/GameMap.h"
#include "UI/UICharacterListing.h"
#include "UI/UIInput.h"
#include "UI/UIInputGroup.h"
#include "UI/UIButton.h"
#include "UI/UIPanel.h"
#include "UI/UILabel.h"
#include "EventHandling/EventHandler.h"

class Game : public DX::IDeviceNotify, public Observer
{
public:
	Game() noexcept(false) ;

	void Initialize(HWND window, int width, int height);
	void Tick();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);

	// Properties
	void GetDefaultSize(int& width, int& height) const;

private:
	float m_mousePosX = 0.0f;
	float m_mousePosY = 0.0f;
	int m_clientWidth = 800;
	int m_clientHeight = 600;
	std::vector<UICharacterListing*>* m_characterList = new std::vector<UICharacterListing*>;
	std::string m_token = "";
	std::string m_characterNamePendingDeletion = "";
	XMMATRIX m_worldTransform{ XMMatrixIdentity() };
	XMMATRIX m_viewTransform{ XMMatrixIdentity() };
	XMMATRIX m_projectionTransform{ XMMatrixIdentity() };
	Layer m_activeLayer = Login;
	std::unique_ptr<DX::DeviceResources> m_deviceResources;
	GameTimer m_timer{};
	Camera m_camera{};
	SocketManager m_socketManager{};
	ObjectManager m_objectManager{};
	GameMap* m_gameMap = nullptr;
	PlayerController* m_playerController = nullptr;
	Model* m_sphereModel = nullptr;
	Model* m_treeModel = nullptr;

	void Update();
	void Render();
	void Clear();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	ShaderBuffer LoadShader(std::wstring filename);
	virtual bool HandleEvent(const Event* event);
	void SetActiveLayer(const Layer layer);

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
	UICharacterListing* GetCurrentlySelectedCharacterListing();

	// TextFormats
	ComPtr<IDWriteTextFormat> textFormatFPS;
	ComPtr<IDWriteTextFormat> textFormatAccountCreds;
	ComPtr<IDWriteTextFormat> textFormatHeaders;
	ComPtr<IDWriteTextFormat> textFormatAccountCredsInputValue;
	ComPtr<IDWriteTextFormat> textFormatButtonText;
	ComPtr<IDWriteTextFormat> textFormatSuccessMessage;
	ComPtr<IDWriteTextFormat> textFormatErrorMessage;

	// TextLayouts
	ComPtr<IDWriteTextLayout> textLayoutFPS;
	ComPtr<IDWriteTextLayout> textLayoutMousePos;
	ComPtr<IDWriteTextLayout> textLayoutSuccessMessage;
	ComPtr<IDWriteTextLayout> textLayoutErrorMessage;

	// Brushes
	ComPtr<ID2D1SolidColorBrush> grayBrush;
	ComPtr<ID2D1SolidColorBrush> blackBrush;
	ComPtr<ID2D1SolidColorBrush> whiteBrush;
	ComPtr<ID2D1SolidColorBrush> blueBrush;
	ComPtr<ID2D1SolidColorBrush> darkBlueBrush;
	ComPtr<ID2D1SolidColorBrush> successMessageBrush;
	ComPtr<ID2D1SolidColorBrush> errorMessageBrush;
	ComPtr<ID2D1SolidColorBrush> selectedCharacterBrush;

	// Shaders
	ShaderBuffer vertexShaderBuffer{};
	ComPtr<ID3D11VertexShader> vertexShader;
	ShaderBuffer pixelShaderBuffer{};
	ComPtr<ID3D11PixelShader> pixelShader;

	// InputLayouts
	ComPtr<ID3D11InputLayout> inputLayout;

	// Textures
	ComPtr<ID3D11ShaderResourceView> color01SRV;
	ComPtr<ID3D11ShaderResourceView> color02SRV;
	ComPtr<ID3D11ShaderResourceView> grass01SRV;

	// Raster States
	ComPtr<ID3D11RasterizerState> wireframeRasterState;
	ComPtr<ID3D11RasterizerState> solidRasterState;

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
	UILabel* fpsTextLabel = nullptr;
	UILabel* mousePosLabel = nullptr;
	UILabel* gameSettingsPanelHeader = nullptr;
	UILabel* gameEditorPanelHeader = nullptr;
	UILabel* diagnosticsPanelHeader = nullptr;

	// Panels
	UIPanel* gameSettingsPanel;
	UIPanel* gameEditorPanel;
	UIPanel* diagnosticsPanel;
};