#ifndef DIRECTXMANAGER_H
#define DIRECTXMANAGER_H

#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <string>
#include <vector>
#include "GameTimer.h"
#include "LoginState.h"
#include "UIInput.h"
#include "UIButton.h"
#include "UILabel.h"
#include "SocketManager.h"

class DirectXManager
{
    std::vector<std::string>* characterList = nullptr;
    std::string token = "";
    UINT clientWidth;
    UINT clientHeight;
    GameTimer& timer;
    SocketManager& socketManager;

    // DXGI
    IDXGISwapChain* swapChain;

    // D3D
    ID3D11DeviceContext* immediateContext;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* mDepthStencilView;

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

    // Inputs
    UIInput* loginScreen_accountNameInput;
    UIInput* loginScreen_passwordInput;
    UIInput* loginScreen_newAccountAccountNameInput;
    UIInput* loginScreen_newAccountPasswordInput;
    UIInput* createAccount_accountNameInput;
    UIInput* createAccount_passwordInput;
    UIInput* createCharacter_characterNameInput;

    // Buttons
    UIButton* loginScreen_loginButton;
    UIButton* loginScreen_createAccountButton;
    UIButton* createAccount_createAccountButton;
    UIButton* createAccount_cancelButton;
    UIButton* characterSelect_newCharacterButton;
    UIButton* characterSelect_logoutButton;
    UIButton* createCharacter_createCharacterButton;
    UIButton* createCharacter_backButton;

    // Labels
    UILabel* loginScreen_successMessageLabel;
    UILabel* loginScreen_errorMessageLabel;
    UILabel* createAccount_errorMessageLabel;
    UILabel* connecting_statusLabel;
    UILabel* characterSelect_successMessageLabel;
    UILabel* characterSelect_headerLabel;
    UILabel* createCharacter_errorMessageLabel;

    void InitializeBrushes();
    void InitializeTextFormats();
    void InitializeInputs();
    void InitializeButtons();
    void InitializeLabels();
public:
    DirectXManager(GameTimer& timer, SocketManager& socketManager);
    void Initialize(HWND hWnd);
    void OnBackspace();
    void OnKeyPress(TCHAR c);
    void MouseDown(FLOAT mousePosX, FLOAT mousePosY);
    void MouseUp();
    void OnTab();
    void DrawScene(FLOAT mouseX, FLOAT mouseY);
    void HandleMessage(std::tuple<std::string, std::string, std::vector<std::string>*> message);
};

#endif