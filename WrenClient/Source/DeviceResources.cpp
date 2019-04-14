#include "stdafx.h"
#include "DeviceResources.h"

using namespace DX;

DeviceResources::DeviceResources() noexcept
{
}

// Create and configure D3DDevice and D2DDevice, and all associated
//   resources that aren't dependent on the size of the window.
void DeviceResources::CreateDeviceResources()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	CreateFactory();

	static const D3D_FEATURE_LEVEL s_featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	HRESULT hr;
	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,
		s_featureLevels,
		1,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),  // Returns the Direct3D device created.
		&m_d3dFeatureLevel,     // Returns feature level of device created.
		context.GetAddressOf()  // Returns the device immediate context.
	);
	ThrowIfFailed(hr);

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	ThrowIfFailed(device.As(&m_d3dDevice));
	ThrowIfFailed(context.As(&m_d3dContext));
	//ThrowIfFailed(context.As(&m_d3dAnnotation));

	// WriteFactory is used for Text related tasks, such as CreateTextLayout.
	// Text rendering happens through the D2DContext, for example:
	//   writeFactory->CreateTextLayout...
	//   d2dContext->DrawTextLayout...
	ComPtr<IDWriteFactory> writeFactory;
	ThrowIfFailed(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		&writeFactory
	));
	ThrowIfFailed(writeFactory.As(&m_writeFactory));

	// D2DFactory is used to create 2D geometry, for example:
	//   d2dFactory->CreateRoundedRectangleGeometry...
	//   d2dContext->DrawGeometry...
	D2D1_FACTORY_OPTIONS options;
#ifdef _DEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	ComPtr<ID2D1Factory> d2dFactory;
	ThrowIfFailed(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_MULTI_THREADED,
		__uuidof(ID2D1Factory),
		&options,
		&d2dFactory
	));
	ThrowIfFailed(d2dFactory.As(&m_d2dFactory));

	// Get a reference to the DXGIDevice to use in creation of the D2DDevice
	ComPtr<IDXGIDevice> dxgiDevice;
	ThrowIfFailed(device.As(&dxgiDevice));

	// Create D2DDevice
	ComPtr<ID2D1Device> d2dDevice;
	ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf()));
	ThrowIfFailed(d2dDevice.As(&m_d2dDevice));

	// Create D2DContext
	ComPtr<ID2D1DeviceContext1> d2dDeviceContext;
	ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &d2dDeviceContext));
	ThrowIfFailed(d2dDeviceContext.As(&m_d2dContext));
}

void DeviceResources::CreateFactory()
{
#if defined(_DEBUG) && (_WIN32_WINNT >= 0x0603 /*_WIN32_WINNT_WINBLUE*/)
	bool debugDXGI = false;
	{
		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			debugDXGI = true;

			ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

			DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
			{
				80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
			};
			DXGI_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
		}
	}

	if (!debugDXGI)
#endif
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
}

// These resources need to be recreated every time the window size is changed.
void DeviceResources::CreateWindowSizeDependentResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_d2dContext->SetTarget(nullptr);
	m_d3dRenderTargetView.Reset();
	m_d3dDepthStencilView.Reset();
	m_renderTarget.Reset();
	m_depthStencil.Reset();
	m_d3dContext->Flush();
	//m_d2dContext->Flush();

	// Determine the render target size in pixels.
	UINT backBufferWidth = std::max<UINT>(m_outputSize.right - m_outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(m_outputSize.bottom - m_outputSize.top, 1);
	DXGI_FORMAT backBufferFormat = m_backBufferFormat;

	if (m_swapChain)
	{
		// If the swap chain already exists, resize it.
		HRESULT hr = m_swapChain->ResizeBuffers(
			m_backBufferCount,
			backBufferWidth,
			backBufferHeight,
			backBufferFormat,
			0
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
#ifdef _DEBUG
			char buff[64] = {};
			sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
			OutputDebugStringA(buff);
#endif
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}
	else
	{
		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_backBufferCount;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			m_swapChain.ReleaseAndGetAddressOf()
		));

		// This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
		ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
	}

	// Create a render target view of the swap chain back buffer.
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_renderTarget.ReleaseAndGetAddressOf())));

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat);
	ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(
		m_renderTarget.Get(),
		&renderTargetViewDesc,
		m_d3dRenderTargetView.ReleaseAndGetAddressOf()
	));

	// Create a depth stencil view for use with 3D rendering.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		m_depthBufferFormat,
		backBufferWidth,
		backBufferHeight,
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	ThrowIfFailed(m_d3dDevice->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		m_depthStencil.ReleaseAndGetAddressOf()
	));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(
		m_depthStencil.Get(),
		&depthStencilViewDesc,
		m_d3dDepthStencilView.ReleaseAndGetAddressOf()
	));

	// Get reference to DXGISurface and create a bitmap from that surface
	// to use as a rendering target for our D2DContext.
	D2D1_BITMAP_PROPERTIES1 bp;
	bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	bp.dpiX = 96.0f;
	bp.dpiY = 96.0f;
	bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bp.colorContext = nullptr;

	ComPtr<IDXGISurface1> dxgiBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer));

	ComPtr<ID2D1Bitmap1> targetBitmap;
	ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), &bp, &targetBitmap));

	m_d2dContext->SetTarget(targetBitmap.Get());

	// Set the 3D rendering viewport to target the entire window.
	m_screenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		static_cast<float>(backBufferWidth),
		static_cast<float>(backBufferHeight)
	);
}

// Recreate all device resources and set them back to the current state.
void DeviceResources::HandleDeviceLost()
{
	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceLost();
	}

	m_d2dContext.Reset();
	m_d2dDevice.Reset();
	m_d2dFactory.Reset();
	m_writeFactory.Reset();
	m_d3dDepthStencilView.Reset();
	m_d3dRenderTargetView.Reset();
	m_renderTarget.Reset();
	m_depthStencil.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();	
	//m_d3dAnnotation.Reset();

#ifdef _DEBUG
	{
		ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
		{
			d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
		}
	}
#endif

	m_d3dDevice.Reset();
	m_dxgiFactory.Reset();

	CreateDeviceResources();
	CreateWindowSizeDependentResources();

	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceRestored();
	}
}

// This method is called when the Win32 window is created (or re-created).
void DeviceResources::SetWindow(HWND window, int width, int height)
{
	m_window = window;

	m_outputSize.top = 0;
	m_outputSize.left = 0;
	m_outputSize.right = width;
	m_outputSize.bottom = height;
}

void DeviceResources::Present()
{
	HRESULT hr;

	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	hr = m_swapChain->Present(1, 0);

	// Discard the contents of the render target.
	// This is a valid operation only when the existing contents will be entirely
	// overwritten. If dirty or scroll rects are used, this call should be removed.
	m_d3dContext->DiscardView(m_d3dRenderTargetView.Get());
	m_d3dContext->DiscardView(m_d3dDepthStencilView.Get());

	// If the device was removed either by a disconnection or a driver upgrade, we 
	// must recreate all device resources.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buff[64] = {};
		sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buff);
#endif
		HandleDeviceLost();
	}
	else
	{
		ThrowIfFailed(hr);

		if (!m_dxgiFactory->IsCurrent())
		{
			// Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
			CreateFactory();
		}
	}
}

bool DeviceResources::WindowSizeChanged(int width, int height)
{
	RECT newRc;
	newRc.left = 0;
	newRc.top = 0;
	newRc.right = width;
	newRc.bottom = height;
	if (newRc == m_outputSize)
	{
		// Handle color space settings for HDR
		//UpdateColorSpace();

		return false;
	}

	m_outputSize = newRc;
	CreateWindowSizeDependentResources();
	return true;
}