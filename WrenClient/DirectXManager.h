#include <d3d11.h>

class DirectXManager
{
    IDXGISwapChain* swapChain;
    ID3D11DeviceContext* immediateContext;
    ID3D11RenderTargetView* renderTargetView;
public:
    void Initialize(HWND hWnd);
    void DrawScene();
};