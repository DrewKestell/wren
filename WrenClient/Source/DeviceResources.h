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
		void SetWindow(HWND window, int width, int height);
		bool WindowSizeChanged(int width, int height);
		void HandleDeviceLost();
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify) { m_deviceNotify = deviceNotify; }
		void Present();

		// Device Accessors.
		RECT GetOutputSize() const { return m_outputSize; }

		// Direct3D Accessors.
		ID3D11Device1* GetD3DDevice() const { return m_d3dDevice.Get(); }
		ID3D11DeviceContext1* GetD3DDeviceContext() const { return m_d3dContext.Get(); }
		ID3D11RenderTargetView*	GetRenderTargetView() const { return m_d3dRenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_d3dDepthStencilView.Get(); }
		D3D11_VIEWPORT GetScreenViewport() const { return m_screenViewport; }
	private:
		void CreateFactory();

		// Direct3D objects.
		ComPtr<IDXGIFactory2> m_dxgiFactory;
		ComPtr<ID3D11Device1> m_d3dDevice;
		ComPtr<ID3D11DeviceContext1> m_d3dContext;
		ComPtr<IDXGISwapChain1> m_swapChain;

		// Direct3D rendering objects. Required for 3D.
		ComPtr<ID3D11Texture2D> m_renderTarget;
		ComPtr<ID3D11Texture2D> m_depthStencil;
		ComPtr<ID3D11RenderTargetView> m_d3dRenderTargetView;
		ComPtr<ID3D11DepthStencilView> m_d3dDepthStencilView;
		D3D11_VIEWPORT m_screenViewport;

		// Direct3D Properties.
		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		DXGI_FORMAT m_depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
		UINT m_backBufferCount = 2;

		// Cached device properties.
		HWND m_window;
		D3D_FEATURE_LEVEL m_d3dFeatureLevel;
		RECT m_outputSize;

		// The IDeviceNotify can be held directly as it owns the DeviceResources.
		IDeviceNotify* m_deviceNotify;
	};
}