#include "stdafx.h"
#include "build_version.h"

gw2al_addon_dsc gAddonDeps[] = {
	{
		L"loader_core",
		L"whatever",
		0, 1, 1, 0
	},
	{
		L"d3d9_wrapper",
		L"Wrapper for d3d9 API that includes hooking and custom d3d9 loading",
		1, 0, 2, 0
	},
	{0,0,0,0,0,0}
};

gw2al_addon_dsc gAddonDsc = {
	L"lib_imgui",
	L"provides imgui context that can be used in other addons",
	1,
	0,
	BUILD_VERSION_REV,
	gAddonDeps
};

gw2al_core_vtable* gAPI;

const wchar_t* getContextExportName = L"lib_imgui_get_context";
const wchar_t* guiDrawEventName = L"lib_imgui_draw_event";

gw2al_hashed_name getContextExportNameHash = 0;
gw2al_hashed_name guiDrawEventNameHash = 0;
gw2al_event_id guiDrawEventId = 0;


void* getImGuiContext()
{
	return lib_imgui::Core::i()->imguiContext();
}

void SendDrawEvent()
{
	gAPI->trigger_event(guiDrawEventId, nullptr);
}

gw2al_addon_dsc* gw2addon_get_description()
{
	return &gAddonDsc;
}

#define LOG_INFO(a) gAPI->log_text(LL_INFO, (wchar_t*)L"lib_imgui", (wchar_t*)a)

gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api)
{
	gAPI = core_api;
	
	lib_imgui::Direct3D9Hooks::i()->InitHooks(core_api);

	getContextExportNameHash = gAPI->hash_name((wchar_t*)getContextExportName);
	guiDrawEventNameHash = gAPI->hash_name((wchar_t*)guiDrawEventName);

	gAPI->register_function(&getImGuiContext, getContextExportNameHash);
	guiDrawEventId = gAPI->query_event(guiDrawEventNameHash);

	return GW2AL_OK;
}

gw2al_api_ret gw2addon_unload(int gameExiting)
{
	gAPI->unregister_function(getContextExportNameHash);
	return GW2AL_OK;
}

bool WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		while (!IsDebuggerPresent());
#endif
		lib_imgui::Core::Init(hModule);
		break;
	case DLL_PROCESS_DETACH:
		lib_imgui::Core::Shutdown();
		break;
	}

	return true;
}