#pragma once

#include <CommonRepository.h>
#include "ClientRepository.h"
#include <Models/Skill.h>
#include <Models/StaticObject.h>
#include <PlayerController.h>
#include <Constants.h>
#include <GameMap/GameMap.h>
#include "DeviceResources.h"
#include "GameTimer.h"
#include "Camera.h"
#include "ShaderBuffer.h"
#include "Mesh.h"
#include "ObjectManager.h"
#include "Components/RenderComponentManager.h"
#include "Components/StatsComponentManager.h"
#include "SocketManager.h"
#include "Components/GameMapRenderComponent.h"
#include "Sprite.h"
#include "Models/Npc.h"
#include "UI/UICharacterListing.h"
#include "UI/UIInput.h"
#include "UI/UIInputGroup.h"
#include "UI/UIButton.h"
#include "UI/UIPanel.h"
#include "UI/UILabel.h"
#include "UI/UISkillListing.h"
#include "UI/UIHotbar.h"
#include "UI/UIAbilitiesContainer.h"
#include "UI/UICharacterHUD.h"
#include "UI/UITargetHUD.h"
#include "UI/UITextWindow.h"
#include "EventHandling/EventHandler.h"

class Game : public DX::IDeviceNotify, public Observer
{
public:
	Game(ClientRepository repository, CommonRepository commonRepository) noexcept(false) ;

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

	~Game();

private:
	float updateTimer{ 0.0f };
	float mousePosX{ 0.0f };
	float mousePosY{ 0.0f };
	int clientWidth{ CLIENT_WIDTH };
	int clientHeight{ CLIENT_HEIGHT };
	std::vector<std::unique_ptr<UICharacterListing>> characterList;
	std::map<int, std::unique_ptr<UISkillListing>> skillList;
	std::string characterNamePendingDeletion{};
	XMMATRIX worldTransform{ XMMatrixIdentity() };
	XMMATRIX viewTransform{ XMMatrixIdentity() };
	XMMATRIX projectionTransform{ XMMatrixIdentity() };
	Layer activeLayer{ Login };
	ClientRepository repository;
	CommonRepository commonRepository;
	std::unique_ptr<DX::DeviceResources> deviceResources;
	ObjectManager objectManager;
	RenderComponentManager renderComponentManager{ objectManager };
	StatsComponentManager statsComponentManager{ objectManager };
	GameTimer timer;
	Camera camera;
	GameMap gameMap;
	std::unique_ptr<GameMapRenderComponent> gameMapRenderComponent;
	std::unique_ptr<PlayerController> playerController;
	GameObject* player;
	std::vector<UIComponent*> uiComponents; // TODO: i think these should use smart pointers
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::vector<ComPtr<ID3D11ShaderResourceView>> textures;
	std::vector<std::shared_ptr<Sprite>> sprites;
	std::unique_ptr<UIHotbar> hotbar;
	std::unique_ptr<UITargetHUD> targetHUD;
	std::unique_ptr<UICharacterHUD> characterHUD;
	std::unique_ptr<UITextWindow> textWindow;
	std::vector<Skill*>* skills;
	std::vector<Ability*>* abilities;
	unsigned int* textWindowMessageIndex = new unsigned int{ 0 };
	std::string* textWindowMessages[MESSAGE_BUFFER_SIZE];
	std::vector<Npc*>* npcs;

	void Render(const float updateTimer);
	void Clear();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
	void CreatePlayerDependentResources();

	ShaderBuffer LoadShader(const std::wstring filename);
	virtual const bool HandleEvent(const Event* const event);
	void SetActiveLayer(const Layer layer);
	void PublishEvents();
	void QuitGame();

	void InitializeNpcs();
	void InitializeStaticObjects();
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
	void InitializeSprites();
	void InitializeSkills();
	void InitializeAbilitiesContainer();
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
	ComPtr<IDWriteTextFormat> textFormatTextWindow;
	ComPtr<IDWriteTextFormat> textFormatTextWindowInactive;

	// Brushes
	ComPtr<ID2D1SolidColorBrush> grayBrush;
	ComPtr<ID2D1SolidColorBrush> blackBrush;
	ComPtr<ID2D1SolidColorBrush> whiteBrush;
	ComPtr<ID2D1SolidColorBrush> blueBrush;
	ComPtr<ID2D1SolidColorBrush> darkBlueBrush;
	ComPtr<ID2D1SolidColorBrush> successMessageBrush;
	ComPtr<ID2D1SolidColorBrush> errorMessageBrush;
	ComPtr<ID2D1SolidColorBrush> selectedCharacterBrush;
	ComPtr<ID2D1SolidColorBrush> lightGrayBrush;
	ComPtr<ID2D1SolidColorBrush> abilityHighlightBrush;
	ComPtr<ID2D1SolidColorBrush> abilityPressedBrush;
	ComPtr<ID2D1SolidColorBrush> healthBrush;
	ComPtr<ID2D1SolidColorBrush> manaBrush;
	ComPtr<ID2D1SolidColorBrush> staminaBrush;
	ComPtr<ID2D1SolidColorBrush> statBackgroundBrush;
	ComPtr<ID2D1SolidColorBrush> darkGrayBrush;
	ComPtr<ID2D1SolidColorBrush> mediumGrayBrush;
	ComPtr<ID2D1SolidColorBrush> scrollBarBackgroundBrush;
	ComPtr<ID2D1SolidColorBrush> scrollBarBrush;

	// Shaders
	ShaderBuffer vertexShaderBuffer{};
	ComPtr<ID3D11VertexShader> vertexShader;
	ShaderBuffer pixelShaderBuffer{};
	ComPtr<ID3D11PixelShader> pixelShader;
	ShaderBuffer spriteVertexShaderBuffer{};
	ComPtr<ID3D11VertexShader> spriteVertexShader;
	ShaderBuffer spritePixelShaderBuffer{};
	ComPtr<ID3D11PixelShader> spritePixelShader;

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
	std::unique_ptr<UIButton> loginScreen_quitGameButton;
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
	std::unique_ptr<UILabel> skillsPanelHeader;
	std::unique_ptr<UILabel> abilitiesPanelHeader;

	// Panels
	std::unique_ptr<UIPanel> gameSettingsPanel;
	std::unique_ptr<UIPanel> gameEditorPanel;
	std::unique_ptr<UIPanel> diagnosticsPanel;
	std::unique_ptr<UIPanel> skillsPanel;
	std::unique_ptr<UIPanel> abilitiesPanel;

	// Abilities
	std::unique_ptr<UIAbilitiesContainer> abilitiesContainer;
};