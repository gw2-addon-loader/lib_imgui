#include "stdafx.h"

namespace lib_imgui
{
DEFINE_SINGLETON(Direct3D9Hooks);

Direct3D9Hooks::Direct3D9Hooks()
{
}

void Direct3D9Hooks::DevPostRelease(IDirect3DDevice9 * sThis, ULONG refs)
{
	if (refs == 1)
	{
		preResetCallback_();
		sThis->Release();
	}
}

void Direct3D9Hooks::DevPrePresent(IDirect3DDevice9 *sThis)
{
	drawOverCallback_(sThis, true, true);
}

void Direct3D9Hooks::DevPreReset()
{
	preResetCallback_();
}

void Direct3D9Hooks::DevPostReset(IDirect3DDevice9 *sThis, D3DPRESENT_PARAMETERS *pPresentationParameters, HRESULT hr)
{
	if (FAILED(hr))
		return;

	postResetCallback_(sThis, pPresentationParameters);	
}

void Direct3D9Hooks::ObjPreCreateDevice(HWND hFocusWindow)
{
	preCreateDeviceCallback_(hFocusWindow);
}

void Direct3D9Hooks::ObjPostCreateDevice(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	postCreateDeviceCallback_(pDevice, pPresentationParameters);	
	pDevice->AddRef();
}

#pragma pack(push, 1)
typedef struct d3d9_api_call {
	union {
		IDirect3D9* obj;
		IDirect3DDevice9* dev;
	};
	union {
		struct {
			UINT pad0;
			UINT v1;
			UINT pad1;
			D3DDEVTYPE v2;
			HWND v3;
			UINT pad2;
			DWORD v4;
			D3DPRESENT_PARAMETERS* v5;
			IDirect3DDevice9** ret;
		} CreateDevice;
		struct {
			DWORD            *pFunction;
			union {
				IDirect3DPixelShader9 **ppShader;
				IDirect3DVertexShader9 **pvShader;
			};
		} CreateShader;
		struct {
			union {
				IDirect3DVertexShader9 *vs;
				IDirect3DPixelShader9 *ps;
			};
		} SetShader;
		struct {
			D3DPRESENT_PARAMETERS* pPresentationParameters;
		} Reset;
	};
} d3d9_api_call;
#pragma pack(pop)

void OnDevPostRelease(D3D9_wrapper_event_data* evd)
{
	lib_imgui::Direct3D9Hooks::i()->DevPostRelease((IDirect3DDevice9*)*evd->stackPtr, *((ULONG*)evd->ret));
}

void OnDevPrePresent(D3D9_wrapper_event_data* evd)
{
	lib_imgui::Direct3D9Hooks::i()->DevPrePresent((IDirect3DDevice9*)*evd->stackPtr);
}

void OnDevPreReset(D3D9_wrapper_event_data* evd)
{
	lib_imgui::Direct3D9Hooks::i()->DevPreReset();
}

void OnDevPostReset(D3D9_wrapper_event_data* evd)
{
	d3d9_api_call* dx_api_cp = (d3d9_api_call*)evd->stackPtr;
	lib_imgui::Direct3D9Hooks::i()->DevPostReset(dx_api_cp->dev, dx_api_cp->Reset.pPresentationParameters, *((HRESULT*)evd->ret));
}

void OnObjPreCreateDevice(D3D9_wrapper_event_data* evd)
{
	d3d9_api_call* dx_api_cp = (d3d9_api_call*)evd->stackPtr;
	lib_imgui::Direct3D9Hooks::i()->ObjPreCreateDevice(dx_api_cp->CreateDevice.v3);
}

void OnObjPostCreateDevice(D3D9_wrapper_event_data* evd)
{
	d3d9_api_call* dx_api_cp = (d3d9_api_call*)evd->stackPtr;
	lib_imgui::Direct3D9Hooks::i()->ObjPostCreateDevice(*dx_api_cp->CreateDevice.ret, dx_api_cp->CreateDevice.v5);
}

void Direct3D9Hooks::InitHooks(gw2al_core_vtable* gAPI)
{
	D3D9_wrapper d3d9_wrap;
	d3d9_wrap.enable_event = (pD3D9_wrapper_enable_event)gAPI->query_function(gAPI->hash_name((wchar_t*)D3D9_WRAPPER_ENABLE_EVENT_FNAME));

	d3d9_wrap.enable_event(METH_OBJ_CreateDevice, WRAP_CB_PRE_POST);
	d3d9_wrap.enable_event(METH_DEV_Release, WRAP_CB_POST);
	d3d9_wrap.enable_event(METH_DEV_Present, WRAP_CB_PRE);
	d3d9_wrap.enable_event(METH_DEV_Reset, WRAP_CB_PRE_POST);
	d3d9_wrap.enable_event(METH_DEV_CreateVertexShader, WRAP_CB_POST);
	d3d9_wrap.enable_event(METH_DEV_CreatePixelShader, WRAP_CB_POST);
	d3d9_wrap.enable_event(METH_DEV_SetVertexShader, WRAP_CB_POST);
	d3d9_wrap.enable_event(METH_DEV_SetPixelShader, WRAP_CB_POST);

	D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_POST_DEV_Release", OnDevPostRelease, 0);
	D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_PRE_DEV_Present", OnDevPrePresent, 0);
	D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_PRE_DEV_Reset", OnDevPreReset, 0);
	D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_POST_DEV_Reset", OnDevPostReset, 0);
	D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_PRE_OBJ_CreateDevice", OnObjPreCreateDevice, 0);
	D3D9_WRAPPER_WATCH_EVENT(L"gw2radial", L"D3D9_POST_OBJ_CreateDevice", OnObjPostCreateDevice, 0);
}

}