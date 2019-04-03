#pragma once

#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <string>
#include <vector>
#include "GameTimer.h"
#include "Layer.h"
#include "UI/UIInput.h"
#include "UI/UILabel.h"
#include "UI/UIPanel.h"
#include "UI/UICharacterListing.h"
#include "UI/UIInputGroup.h"
#include "SocketManager.h"
#include "EventHandling/EventHandler.h"
#include "EventHandling/Observer.h"
#include "UI/UIButton.h"
#include "GameMap/GameMap.h"

struct CBChangesEveryFrame
{
	DirectX::XMFLOAT4X4 mWorldViewProj;
};

struct VERTEX
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ShaderBuffer
{
    BYTE* buffer;
    int size;
};

class DirectXManager : public Observer
{
	DirectX::XMMATRIX worldTransform;
	DirectX::XMMATRIX viewTransform;
	DirectX::XMMATRIX projectionTransform;

	Layer activeLayer = Login;
	std::string characterNamePendingDeletion;
	ObjectManager& objectManager;
	GameMap& gameMap;
	std::vector<Observer*>* observers;
    ID3D11Buffer* buffer;
	ID3D11Buffer* constantBuffer;
    std::vector<UICharacterListing*>* characterList = new std::vector<UICharacterListing*>;
    std::string token = "";
    UINT clientWidth;
    UINT clientHeight;
    GameTimer& timer;
    SocketManager& socketManager;
	float mousePosX;
	float mousePosY;

    // DXGI
    IDXGISwapChain* swapChain;

    // D3D
    ID3D11Device* device;
    ID3D11DeviceContext* immediateContext;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;

    // D2D
    ID2D1Factory2* d2dFactory;
    ID2D1DeviceContext1* d2dDeviceContext;

    // DirectWrite
    IDWriteFactory2* writeFactory;

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
    ID2D1SolidColorBrush* grayBrush;
    ID2D1SolidColorBrush* blackBrush;
    ID2D1SolidColorBrush* whiteBrush;
    ID2D1SolidColorBrush* blueBrush;
    ID2D1SolidColorBrush* darkBlueBrush;
    ID2D1SolidColorBrush* successMessageBrush;
    ID2D1SolidColorBrush* errorMessageBrush;
    ID2D1SolidColorBrush* selectedCharacterBrush;

    // Inputs
    UIInput* loginScreen_accountNameInput;
    UIInput* loginScreen_passwordInput;
    UIInput* loginScreen_newAccountAccountNameInput;
    UIInput* loginScreen_newAccountPasswordInput;
    UIInput* createAccount_accountNameInput;
    UIInput* createAccount_passwordInput;
    UIInput* createCharacter_characterNameInput;

	// InputGroups
	UIInputGroup* loginScreen_inputGroup;
	UIInputGroup* createAccount_inputGroup;
	UIInputGroup* createCharacter_inputGroup;

    // Buttons
    UIButton* loginScreen_loginButton;
    UIButton* loginScreen_createAccountButton;
    UIButton* createAccount_createAccountButton;
    UIButton* createAccount_cancelButton;
    UIButton* characterSelect_newCharacterButton;
	UIButton* characterSelect_enterWorldButton;
	UIButton* characterSelect_deleteCharacterButton;
    UIButton* characterSelect_logoutButton;
    UIButton* createCharacter_createCharacterButton;
    UIButton* createCharacter_backButton;
	UIButton* deleteCharacter_confirmButton;
	UIButton* deleteCharacter_cancelButton;
	UIButton* gameSettings_logoutButton;

    // Labels
    UILabel* loginScreen_successMessageLabel;
    UILabel* loginScreen_errorMessageLabel;
    UILabel* createAccount_errorMessageLabel;
    UILabel* connecting_statusLabel;
    UILabel* characterSelect_successMessageLabel;
	UILabel* characterSelect_errorMessageLabel;
    UILabel* characterSelect_headerLabel;
    UILabel* createCharacter_errorMessageLabel;
	UILabel* deleteCharacter_headerLabel;
    UILabel* enteringWorld_statusLabel;

    // Panels
    UIPanel* gameSettingsPanel;
    UIPanel* gameEditorPanel;

	// Shaders
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

    void InitializeBrushes();
    void InitializeTextFormats();
    void InitializeInputs();
    void InitializeButtons();
    void InitializeLabels();
    void InitializePanels();
    void RecreateCharacterListings(const std::vector<std::string*>* characterNames);
    void InitializeGameWorld();
    ShaderBuffer LoadShader(std::wstring filename);
	UICharacterListing* GetCurrentlySelectedCharacterListing();
public:
	DirectXManager(GameTimer& timer, SocketManager& socketManager, ObjectManager& objectManager, GameMap& gameMap)
		: objectManager{ objectManager },
		timer{ timer },
		socketManager{ socketManager },
		gameMap{ gameMap }
	{
	};
    void Initialize(HWND hWnd);
    void DrawScene();
	virtual bool HandleEvent(const Event* event);
	void SetActiveLayer(const Layer layer);
};
