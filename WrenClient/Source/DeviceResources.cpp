#include "stdafx.h"
#include "DeviceResources.h"

using namespace DX;

DeviceResources::DeviceResources() noexcept
	: m_deviceNotify(nullptr)
{
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DeviceResources::CreateDeviceResources()
{

}

// These resources need to be recreated every time the window size is changed.
void DeviceResources::CreateWindowSizeDependentResources()
{

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
}

bool DeviceResources::WindowSizeChanged(int width, int height)
{
	RECT newRc;
	newRc.left = newRc.top = 0;
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