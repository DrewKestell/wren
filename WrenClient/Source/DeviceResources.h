#pragma once

namespace DX
{
	// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
	struct IDeviceNotify
	{
		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceRestored() = 0;
	};

	class DeviceResources
	{
	public:
		DeviceResources() noexcept;

		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		HWND GetWindow() { return window; }
		void SetWindow(HWND window, int width, int height);
		bool WindowSizeChanged(int width, int height);
		void HandleDeviceLost();
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify) { deviceNotify = deviceNotify; }
		void Present();

		// Device Accessors.
		RECT GetOutputSize() const { return outputSize; }

		// Direct3D Accessors.
		ID3D11Device1* GetD3DDevice() const { return d3dDevice.Get(); }
		ID3D11DeviceContext1* GetD3DDeviceContext() const { return d3dContext.Get(); }
		ID2D1Device1* GetD2DDevice() const { return d2dDevice.Get(); }
		ID2D1DeviceContext1* GetD2DDeviceContext() const { return d2dContext.Get(); }
		IDWriteFactory2* GetWriteFactory() const { return writeFactory.Get(); }
		ID2D1Factory2* GetD2DFactory() const { return d2dFactory.Get(); }
		ID3D11Texture2D* GetBackBufferRenderTarget() const { return backBufferRenderTarget.Get(); };
		ID3D11Texture2D* GetOffscreenRenderTarget() const { return offscreenRenderTarget.Get(); };
		ID3D11RenderTargetView*	GetOffscreenRenderTargetView() const { return offscreenRenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }
		D3D11_VIEWPORT GetScreenViewport() const { return screenViewport; }
	private:
		void CreateFactory();

		// Direct2D objects.
		ComPtr<IDWriteFactory2> writeFactory;
		ComPtr<ID2D1Factory2> d2dFactory;
		ComPtr<ID2D1Device1> d2dDevice;
		ComPtr<ID2D1DeviceContext1> d2dContext;

		// Direct3D objects.
		ComPtr<IDXGIFactory2> dxgiFactory;
		ComPtr<ID3D11Device1> d3dDevice;
		ComPtr<ID3D11DeviceContext1> d3dContext;
		ComPtr<IDXGISwapChain1> swapChain;

		// Direct3D rendering objects. Required for 3D.
		ComPtr<ID3D11Texture2D> backBufferRenderTarget;
		ComPtr<ID3D11Texture2D> offscreenRenderTarget;
		ComPtr<ID3D11Texture2D> depthStencil;
		ComPtr<ID3D11RenderTargetView> backBufferRenderTargetView;
		ComPtr<ID3D11RenderTargetView> offscreenRenderTargetView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
		ComPtr<ID3D11DepthStencilState> depthStencilState;
		D3D11_VIEWPORT screenViewport{};

		// Direct3D Properties.
		DXGI_FORMAT backBufferFormat{ DXGI_FORMAT_B8G8R8A8_UNORM };
		DXGI_FORMAT depthBufferFormat{ DXGI_FORMAT_D32_FLOAT };
		UINT backBufferCount{ 2 };
		UINT msaaCount{ 8 };
		UINT msaaQuality{ 0 };

		// Cached device properties.
		HWND window{ nullptr };
		D3D_FEATURE_LEVEL d3dFeatureLevel{ D3D_FEATURE_LEVEL_9_1 };
		RECT outputSize{ 0, 0, 1, 1 };

		// The IDeviceNotify can be held directly as it owns the DeviceResources.
		IDeviceNotify* deviceNotify{ nullptr };
	};
}