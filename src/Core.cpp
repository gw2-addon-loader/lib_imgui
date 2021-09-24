#include "stdafx.h"

namespace lib_imgui
{
DEFINE_SINGLETON(Core);

void Core::Init(HMODULE dll)
{
	i()->dllModule_ = dll;
	i()->InternalInit();
}

void Core::Shutdown()
{
	i_.reset();

	// We'll just leak a bunch of things and let the driver/OS take care of it, since we have no clean exit point
	// and calling FreeLibrary in DllMain causes deadlocks
}

Core::~Core()
{
	ImGui::DestroyContext();

	auto i = Direct3D9Hooks::iNoInit();
	if(i != nullptr)
	{
		i->preCreateDeviceCallback(nullptr);
		i->postCreateDeviceCallback(nullptr);
		
		i->preResetCallback(nullptr);
		i->postResetCallback(nullptr);
		
		i->drawOverCallback(nullptr);
	}
}

void Core::SwapchainCreate11(ID3D11Device5* device, ID3D11DeviceContext* context, IDXGISwapChain4* swc, DXGI_SWAP_CHAIN_DESC* desc)
{
	ImGui_ImplDX11_Shutdown();

	SetWindowHandle(desc->OutputWindow);
	ImGui_ImplWin32_Init(gameWindow_);

	// Initialize graphics
	screenWidth_ = desc->BufferDesc.Width;
	screenHeight_ = desc->BufferDesc.Height;
	firstFrame_ = true;

	ImGui_ImplDX11_Init(device, context);
}

void Core::DrawOver11()
{
	if (firstFrame_)
	{
		firstFrame_ = false;
	}
	else
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//this sends event to all other addons to draw their UI
		SendDrawEvent();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

void Core::PreReset11()
{
	ImGui_ImplDX11_InvalidateDeviceObjects();
}

void Core::PostReset11()
{
	ImGui_ImplDX11_CreateDeviceObjects();
}


void Core::InternalInit()
{	
	Direct3D9Hooks::i()->preCreateDeviceCallback([this](HWND hWnd){ SetWindowHandle(hWnd); });
	Direct3D9Hooks::i()->postCreateDeviceCallback([this](IDirect3DDevice9* d, D3DPRESENT_PARAMETERS* pp){ PostCreateDevice(d, pp); });
	
	Direct3D9Hooks::i()->preResetCallback([this](){ PreReset(); });
	Direct3D9Hooks::i()->postResetCallback([this](IDirect3DDevice9* d, D3DPRESENT_PARAMETERS* pp){ PostReset(d, pp); });
	
	Direct3D9Hooks::i()->drawOverCallback([this](IDirect3DDevice9* d, bool frameDrawn, bool sceneEnded){ DrawOver(d, frameDrawn, sceneEnded); });
		
	imguiContext_ = ImGui::CreateContext();
}

LRESULT Core::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	if (msg == WM_MOUSEMOVE)
	{
		auto& io = ImGui::GetIO();
		io.MousePos.x = static_cast<signed short>(lParam);
		io.MousePos.y = static_cast<signed short>(lParam >> 16);
	}

	// Prevent game from receiving input if ImGui requests capture
	const auto& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
		if (io.WantCaptureMouse)
			return 0;
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		if (io.WantCaptureKeyboard)
			return 0;
		break;
	case WM_CHAR:
		if (io.WantTextInput)
			return 0;
		break;
	}

	// Whatever's left should be sent to the game
	return CallWindowProc(i()->baseWndProc_, hWnd, msg, wParam, lParam);
}

void Core::SetWindowHandle(HWND hFocusWindow)
{
	gameWindow_ = hFocusWindow;

	// Hook WndProc
	if (!baseWndProc_)
	{
		baseWndProc_ = WNDPROC(GetWindowLongPtr(hFocusWindow, GWLP_WNDPROC));
		SetWindowLongPtr(hFocusWindow, GWLP_WNDPROC, LONG_PTR(&WndProc));
	}
}

void Core::PostCreateDevice(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters)
{
	// Init ImGui
	ImGui_ImplWin32_Init(gameWindow_);

	OnDeviceSet(device, presentationParameters);
}

void Core::OnDeviceSet(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters)
{
	// Initialize graphics
	screenWidth_ = presentationParameters->BackBufferWidth;
	screenHeight_ = presentationParameters->BackBufferHeight;
	firstFrame_ = true;

	ImGui_ImplDX9_Init(device);
}

void Core::OnDeviceUnset()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Core::PreReset()
{
	OnDeviceUnset();
}

void Core::PostReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS *presentationParameters)
{
	OnDeviceSet(device, presentationParameters);
}

void Core::DrawOver(IDirect3DDevice9* device, bool frameDrawn, bool sceneEnded)
{
	if (firstFrame_)
	{
		firstFrame_ = false;
	}
	else
	{
		// We have to use Present rather than hooking EndScene because the game seems to do final UI compositing after EndScene
		// This unfortunately means that we have to call Begin/EndScene before Present so we can render things, but thankfully for modern GPUs that doesn't cause bugs
		if (sceneEnded)
			device->BeginScene();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		//this sends event to all other addons to draw their UI
		SendDrawEvent();

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());	

		if (sceneEnded)
			device->EndScene();
	}
}

}
