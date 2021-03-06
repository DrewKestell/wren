#pragma once

#include <CommonRepository.h>
#include "ClientRepository.h"
#include <Models/Skill.h>
#include <Models/StaticObject.h>
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
#include "Components/InventoryComponentManager.h"
#include "Components/GameMapRenderComponent.h"
#include "Sprite.h"
#include "Models/Npc.h"
#include "Models/Item.h"
#include "UI/UICharacterListing.h"
#include "UI/UIInput.h"
#include "UI/UIInputGroup.h"
#include "UI/UIButton.h"
#include "UI/UIPanel.h"
#include "UI/UILabel.h"
#include "UI/UISkillListing.h"
#include "UI/UIHotbar.h"
#include "UI/UIAbilitiesContainer.h"
#include "UI/UILootContainer.h"
#include "UI/UISkillsContainer.h"
#include "UI/UICharacterHUD.h"
#include "UI/UITargetHUD.h"
#include "UI/UITextWindow.h"
#include "UI/UIInventory.h"
#include "EventHandling/EventHandler.h"
#include "ClientSocketManager.h"
#include "ClientSettingsManager.h"

static constexpr auto ARIAL_FONT_FAMILY{ L"Arial" };
static constexpr auto LOCALE{ L"en-US" };

class Game : public DX::IDeviceNotify, public Observer
{
public:
	Game(
		EventHandler& eventHandler,
		ObjectManager& objectManager,
		RenderComponentManager& renderComponentManager,
		StatsComponentManager& statsComponentManager,
		InventoryComponentManager& inventoryComponentManager,
		ClientRepository& clientRepository,
		CommonRepository& commonRepository,
		ClientSocketManager& socketManager);

	void Initialize(HWND window, int width, int height);
	void Tick();

	// IDeviceNotify
	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);
	void OnPong(unsigned int pingId);

	~Game();

private:
	ObjectManager& objectManager;
	RenderComponentManager& renderComponentManager;
	StatsComponentManager& statsComponentManager;
	InventoryComponentManager& inventoryComponentManager;
	EventHandler& eventHandler;
	ClientRepository& clientRepository;
	CommonRepository& commonRepository;
	ClientSocketManager& socketManager;
	int ping{ 0 };
	float pingStart{ 0.0f };
	unsigned int pingId{ 0 };
	float doubleClickStart{ 0.0f };
	float updateTimer{ 0.0f };
	XMFLOAT3 rightMouseDownDir{ VEC_ZERO };
	std::string characterNamePendingDeletion{};
	XMMATRIX worldTransform{ XMMatrixIdentity() };
	XMMATRIX viewTransform{ XMMatrixIdentity() };
	XMMATRIX projectionTransform{ XMMatrixIdentity() };
	Layer activeLayer{ Login };
	std::unique_ptr<DX::DeviceResources> deviceResources;
	GameTimer timer;
	Camera camera;
	GameMap gameMap;
	GameObject* player;
	std::vector<UIComponent*> uiComponents;
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::vector<ComPtr<ID3D11ShaderResourceView>> textures;
	std::vector<std::shared_ptr<Sprite>> sprites;
	std::vector<std::unique_ptr<WrenCommon::Skill>> skills;
	std::vector<std::unique_ptr<Ability>> abilities;
	std::unique_ptr<unsigned int> textWindowMessageIndex = std::make_unique<unsigned int>(0);
	std::array<std::string, MESSAGE_BUFFER_SIZE> textWindowMessages;
	std::vector<std::unique_ptr<Npc>> npcs;
	std::vector<std::unique_ptr<Item>> items;
	std::map<EventType, std::function<void(const Event* const event)>> eventHandlers;
	std::unique_ptr<ClientSettingsManager> clientSettingsManager;

	void CreateEventHandlers();

	void CreateInputs();
	void CreateButtons();
	void CreateLabels();
	void CreatePanels();
	void CreateCharacterListings(const std::vector<std::unique_ptr<std::string>>& characterNames);

	void InitializeBrushes();
	void InitializeTextFormats();
	void InitializeShaders();
	void InitializeBuffers();
	void InitializeRasterStates();
	void InitializeTextures();
	void InitializeMeshes();
	void InitializeInputs();
	void InitializeButtons();
	void InitializeLabels();
	void InitializePanels();
	void InitializeCharacterListings();
	void InitializeStaticObjects();

	void Render(const float updateTimer);
	void Clear();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	ShaderBuffer LoadShader(const std::wstring filename);
	virtual const bool HandleEvent(const Event* const event);
	void SetActiveLayer(const Layer layer);
	void PublishEvents();
	void QuitGame();

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
	ComPtr<IDWriteTextFormat> textFormatTooltipTitle;
	ComPtr<IDWriteTextFormat> textFormatTooltipDescription;
	ComPtr<IDWriteTextFormat> textFormat_size12_leading_centered_bold;
	ComPtr<IDWriteTextFormat> textFormat_size12_leading_centered;

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

	// Constant Buffers
	ComPtr<ID3D11Buffer> constantBufferPerFrame;

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
	std::unique_ptr<UIInput> gameEditorPanelDirectionalLightColorRInput;
	std::unique_ptr<UIInput> gameEditorPanelDirectionalLightColorGInput;
	std::unique_ptr<UIInput> gameEditorPanelDirectionalLightColorBInput;
	std::unique_ptr<UIInput> gameEditorPanelDirectionalLightXPosInput;
	std::unique_ptr<UIInput> gameEditorPanelDirectionalLightYPosInput;
	std::unique_ptr<UIInput> gameEditorPanelDirectionalLightZPosInput;
	std::unique_ptr<UIInput> gameEditorPanelDirectionalLightIntensityInput;
	std::unique_ptr<UIInput> gameEditorPanelAmbientLightIntensityInput;

	// InputGroups
	std::unique_ptr<UIInputGroup> loginScreen_inputGroup;
	std::unique_ptr<UIInputGroup> createAccount_inputGroup;
	std::unique_ptr<UIInputGroup> createCharacter_inputGroup;
	std::unique_ptr<UIInputGroup> gameEditorPanelInputGroup;

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
	std::unique_ptr<UILabel> pingTextLabel;
	std::unique_ptr<UILabel> mousePosLabel;
	std::unique_ptr<UILabel> gameSettingsPanelHeader;
	std::unique_ptr<UILabel> gameEditorPanelHeader;
	std::unique_ptr<UILabel> gameEditorPanelLightingHeader;
	std::unique_ptr<UILabel> diagnosticsPanelHeader;
	std::unique_ptr<UILabel> skillsPanelHeader;
	std::unique_ptr<UILabel> abilitiesPanelHeader;
	std::unique_ptr<UILabel> lootPanelHeader;
	std::unique_ptr<UILabel> inventoryPanelHeader;

	// Panels
	std::unique_ptr<UIPanel> gameSettingsPanel;
	std::unique_ptr<UIPanel> gameEditorPanel;
	std::unique_ptr<UIPanel> diagnosticsPanel;
	std::unique_ptr<UIPanel> abilitiesPanel;
	std::unique_ptr<UIPanel> lootPanel;
	std::unique_ptr<UIPanel> skillsPanel;
	std::unique_ptr<UIPanel> inventoryPanel;

	// Containers
	std::unique_ptr<UIAbilitiesContainer> abilitiesContainer;
	std::unique_ptr<UILootContainer> lootContainer;
	std::unique_ptr<UISkillsContainer> skillsContainer;
	std::unique_ptr<UIInventory> inventory;

	std::unique_ptr<UIHotbar> hotbar;
	std::unique_ptr<UITargetHUD> targetHUD;
	std::unique_ptr<UICharacterHUD> characterHUD;
	std::unique_ptr<UITextWindow> textWindow;
	std::vector<std::unique_ptr<UICharacterListing>> characterList;
	std::map<int, std::unique_ptr<UISkillListing>> skillList;
	std::unique_ptr<GameMapRenderComponent> gameMapRenderComponent;

	friend class ClientSettingsManager;
};