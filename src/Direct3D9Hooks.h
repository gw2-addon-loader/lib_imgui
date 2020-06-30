#pragma once
#include "stdafx.h"

namespace lib_imgui
{

class Direct3D9Hooks : public Singleton<Direct3D9Hooks>
{
public:
	using DrawCallback = std::function<void(IDirect3DDevice9*, bool, bool)>;
	using PreResetCallback = std::function<void()>;
	using PostResetCallback = std::function<void(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>;
	using PreCreateDeviceCallback = std::function<void(HWND)>;
	using PostCreateDeviceCallback = std::function<void(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>;
	
	Direct3D9Hooks();

	const DrawCallback & drawOverCallback() const { return drawOverCallback_; }
	void drawOverCallback(const DrawCallback &drawOverCallback) { drawOverCallback_ = drawOverCallback; }

	const PreResetCallback & preResetCallback() const { return preResetCallback_; }
	void preResetCallback(const PreResetCallback &preResetCallback) { preResetCallback_ = preResetCallback; }

	const PostResetCallback & postResetCallback() const { return postResetCallback_; }
	void postResetCallback(const PostResetCallback &postResetCallback) { postResetCallback_ = postResetCallback; }

	const PreCreateDeviceCallback & preCreateDeviceCallback() const { return preCreateDeviceCallback_; }
	void preCreateDeviceCallback(const PreCreateDeviceCallback &preCreateDeviceCallback)
	{
		preCreateDeviceCallback_ = preCreateDeviceCallback;
	}

	const PostCreateDeviceCallback & postCreateDeviceCallback() const { return postCreateDeviceCallback_; }
	void postCreateDeviceCallback(const PostCreateDeviceCallback &postCreateDeviceCallback)
	{
		postCreateDeviceCallback_ = postCreateDeviceCallback;
	}
	
	void DevPostRelease(IDirect3DDevice9 *sThis, ULONG refs);
	void DevPrePresent(IDirect3DDevice9 *sThis);
	void DevPreReset();
	void DevPostReset(IDirect3DDevice9 *sThis, D3DPRESENT_PARAMETERS *pPresentationParameters, HRESULT hr);
	void ObjPreCreateDevice(HWND hFocusWindow);
	void ObjPostCreateDevice(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters);

	void InitHooks(gw2al_core_vtable* gAPI);

protected:	
	DrawCallback drawOverCallback_;
	PreResetCallback preResetCallback_;
	PostResetCallback postResetCallback_;
	PreCreateDeviceCallback preCreateDeviceCallback_;
	PostCreateDeviceCallback postCreateDeviceCallback_;
};

}
