#pragma once

#include "DeviceResources.h"
#include "GameTimer.h"
#include "SocketManager.h"
#include "ObjectManager.h"
#include "UI/UILabel.h"
#include "EventHandling/EventHandler.h"

class Game : public DX::IDeviceNotify
{
public:
	Game() noexcept(false);

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

private:
	void Update();
	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	std::unique_ptr<DX::DeviceResources> m_deviceResources;
	GameTimer m_timer{};
	SocketManager m_socketManager{};
	ObjectManager m_objectManager{};

	ComPtr<ID2D1SolidColorBrush> grayBrush;
	ComPtr<IDWriteTextFormat> textFormatHeaders;

	UILabel* testLabel = nullptr;
};