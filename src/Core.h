#pragma once
#include "stdafx.h"

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void SendDrawEvent();

namespace lib_imgui
{

	class Core : public Singleton<Core>
	{
	public:
		static void Init(HMODULE dll);
		static void Shutdown();

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		Core() = default;
		~Core();

		HWND gameWindow() const { return gameWindow_; }
		HMODULE dllModule() const { return dllModule_; }
		WNDPROC baseWndProc() const { return baseWndProc_; }
		int screenWidth() const { return screenWidth_; }
		int screenHeight() const { return screenHeight_; }
		void* imguiContext() const { return (void*)imguiContext_; }

		void SwapchainCreate11(ID3D11Device5* device, ID3D11DeviceContext* context, IDXGISwapChain4* swc, DXGI_SWAP_CHAIN_DESC* desc);
		void DrawOver11();
		void PreReset11();
		void PostReset11();
protected:
	void InternalInit();

	void OnDeviceSet(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters);
	void OnDeviceUnset();


	void SetWindowHandle(HWND hFocusWindow);
	void PostCreateDevice(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters);

	void PreReset();
	void PostReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS *presentationParameters);
	
	void DrawOver(IDirect3DDevice9* device, bool frameDrawn, bool sceneEnded);

	HWND gameWindow_ = nullptr;
	HMODULE dllModule_ = nullptr;
	WNDPROC baseWndProc_ = nullptr;

	int screenWidth_ = 0;
	int screenHeight_ = 0;
	bool firstFrame_ = true;

	IDXGISwapChain4* dxSwapChain = nullptr;
	ID3D11Device5* pD11Device = nullptr;
	ID3D11DeviceContext* pD11DeviceContext = nullptr;

	ImGuiContext* imguiContext_ = nullptr;
};
}
