#pragma once

#include "DeviceResources.h"
#include "GameTimer.h"
#include "Camera.h"
#include "ShaderBuffer.h"
#include "Mesh.h"
#include "ObjectManager.h"
#include "RenderComponentManager.h"
#include "PlayerController.h"
#include "SocketManager.h"
#include "GameMap/GameMap.h"
#include "UI/UICharacterListing.h"
#include "UI/UIInput.h"
#include "UI/UIInputGroup.h"
#include "UI/UIButton.h"
#include "UI/UIPanel.h"
#include "UI/UILabel.h"
#include "EventHandling/EventHandler.h"

static const int BUFFER_SIZE = 120;
static const float UPDATE_FREQUENCY = 0.01666666666f;

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

	~Game();

private:
	int m_playerUpdateIdCounter{ 0 };
	float updateTimer{ 0.0f };
	float m_mousePosX{ 0.0f };
	float m_mousePosY{ 0.0f };
	int m_clientWidth{ 800 };
	int m_clientHeight{ 600 };
	std::vector<std::unique_ptr<UICharacterListing>> m_characterList;
	std::string m_characterNamePendingDeletion{};
	XMMATRIX m_worldTransform{ XMMatrixIdentity() };
	XMMATRIX m_viewTransform{ XMMatrixIdentity() };
	XMMATRIX m_projectionTransform{ XMMatrixIdentity() };
	Layer m_activeLayer{ Login };
	std::unique_ptr<DX::DeviceResources> m_deviceResources;
	ObjectManager m_objectManager;
	RenderComponentManager m_renderComponentManager{ m_objectManager };
	GameTimer m_timer;
	Camera m_camera;
	std::unique_ptr<GameMap> m_gameMap;
	std::unique_ptr<PlayerController> m_playerController;
	GameObject* m_player;
	std::vector<UIComponent*> uiComponents;
	std::unique_ptr<PlayerUpdate> playerUpdates[BUFFER_SIZE];
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::vector<ComPtr<ID3D11ShaderResourceView>> textures;

	void Update();
	void Render(const float updateTimer);
	void Clear();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	ShaderBuffer LoadShader(const std::wstring filename);
	virtual const bool HandleEvent(const Event* const event);
	void SetActiveLayer(const Layer layer);
	void SyncWithServer(const float deltaTime);

	void InitializeBrushes();
	void InitializeTextFormats();
	void InitializeInputs();
	void InitializeButtons();
	void InitializeLabels();
	void InitializePanels();
	void InitializeShaders();
	void InitializeBuffers();
	void InitializeTextures();
	void InitializeMeshes();
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

	// Raster States
	ComPtr<ID3D11RasterizerState> wireframeRasterState;
	ComPtr<ID3D11RasterizerState> solidRasterState;

	// Inputs
	std::unique_ptr<UIInput> loginScreen_accountNameInput;
	std::unique_ptr<UIInput> loginScreen_passwordInput;
	std::unique_ptr<UIInput> loginScreen_newAccountAccountNameInput;
	std::unique_ptr<UIInput> loginScreen_newAccountPasswordInput;
	std::unique_ptr<UIInput> createAccount_accountNameInput;
	std::unique_ptr<UIInput> createAccount_passwordInput;
	std::unique_ptr<UIInput> createCharacter_characterNameInput;

	// InputGroups
	std::unique_ptr<UIInputGroup> loginScreen_inputGroup;
	std::unique_ptr<UIInputGroup> createAccount_inputGroup;
	std::unique_ptr<UIInputGroup> createCharacter_inputGroup;

	// Buttons
	std::unique_ptr<UIButton> loginScreen_loginButton;
	std::unique_ptr<UIButton> loginScreen_createAccountButton;
	std::unique_ptr<UIButton> createAccount_createAccountButton;
	std::unique_ptr<UIButton> createAccount_cancelButton;
	std::unique_ptr<UIButton> characterSelect_newCharacterButton;
	std::unique_ptr<UIButton> characterSelect_enterWorldButton;
	std::unique_ptr<UIButton> characterSelect_deleteCharacterButton;
	std::unique_ptr<UIButton> characterSelect_logoutButton;
	std::unique_ptr<UIButton> createCharacter_createCharacterButton;
	std::unique_ptr<UIButton> createCharacter_backButton;
	std::unique_ptr<UIButton> deleteCharacter_confirmButton;
	std::unique_ptr<UIButton> deleteCharacter_cancelButton;
	std::unique_ptr<UIButton> gameSettings_logoutButton;

	// Labels
	std::unique_ptr<UILabel> loginScreen_successMessageLabel;
	std::unique_ptr<UILabel> loginScreen_errorMessageLabel;
	std::unique_ptr<UILabel> createAccount_errorMessageLabel;
	std::unique_ptr<UILabel> connecting_statusLabel;
	std::unique_ptr<UILabel> characterSelect_successMessageLabel;
	std::unique_ptr<UILabel> characterSelect_errorMessageLabel;
	std::unique_ptr<UILabel> characterSelect_headerLabel;
	std::unique_ptr<UILabel> createCharacter_errorMessageLabel;
	std::unique_ptr<UILabel> deleteCharacter_headerLabel;
	std::unique_ptr<UILabel> enteringWorld_statusLabel;
	std::unique_ptr<UILabel> fpsTextLabel;
	std::unique_ptr<UILabel> mousePosLabel;
	std::unique_ptr<UILabel> gameSettingsPanelHeader;
	std::unique_ptr<UILabel> gameEditorPanelHeader;
	std::unique_ptr<UILabel> diagnosticsPanelHeader;

	// Panels
	std::unique_ptr<UIPanel> gameSettingsPanel;
	std::unique_ptr<UIPanel> gameEditorPanel;
	std::unique_ptr<UIPanel> diagnosticsPanel;
};