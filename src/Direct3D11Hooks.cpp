#include "stdafx.h"

namespace lib_imgui
{
	DEFINE_SINGLETON(Direct3D11Hooks);

	Direct3D11Hooks::Direct3D11Hooks()
	{
	}
	
	void On11PostSwapchainCreate(D3D9_wrapper_event_data* evd)
	{
		typedef struct dxgi_CreateSwapChain_cp {
			IDXGIFactory5* dxgi;
			IUnknown* inDevice;
			DXGI_SWAP_CHAIN_DESC* desc;
			IDXGISwapChain4** ppSwapchain;
		} dxgi_CreateSwapChain_cp;

		dxgi_CreateSwapChain_cp* pars = (dxgi_CreateSwapChain_cp*)evd->stackPtr;

		ID3D11Device5* dev = (ID3D11Device5*)pars->inDevice;
		ID3D11DeviceContext* ctx;
		dev->GetImmediateContext(&ctx);	

		lib_imgui::Core::i()->SwapchainCreate11(dev, ctx, *pars->ppSwapchain, pars->desc);
	}


	void On11PrePresent(D3D9_wrapper_event_data* evd)
	{
		lib_imgui::Core::i()->DrawOver11();
	}

	void Direct3D11Hooks::InitHooks(gw2al_core_vtable* gAPI)
	{
		D3D9_wrapper d3d9_wrap;
		d3d9_wrap.enable_event = (pD3D9_wrapper_enable_event)gAPI->query_function(gAPI->hash_name((wchar_t*)D3D9_WRAPPER_ENABLE_EVENT_FNAME));

		d3d9_wrap.enable_event(METH_DXGI_CreateSwapChain, WRAP_CB_POST);
		d3d9_wrap.enable_event(METH_SWC_Present, WRAP_CB_PRE);

		D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_POST_DXGI_CreateSwapChain", On11PostSwapchainCreate, 0);
		D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_PRE_SWC_Present", On11PrePresent, 0);
	}

}