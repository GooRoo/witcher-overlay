#include <Utils/Windows.h>
#include <Utils/Hook.h>
#include <Utils/Pattern.h>
#include <Utils/PipeServer.h>

#include "Game.h"
#include "Messagehandler.h"

#include "Rendering/Renderer.h"

#include "Witcher/Overlay.h"

#include <d3dx9.h>

#define BIND(T) PaketHandler[PipeMessages::T] = T;
#include <dllmain.h>

Hook<CallConvention::stdcall_t, HRESULT, LPDIRECT3DDEVICE9, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *> g_presentHook;
Hook<CallConvention::stdcall_t, HRESULT, LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS *> g_resetHook;
Hook<CallConvention::stdcall_t, BOOL, LPMSG, HWND, UINT, UINT, UINT> g_peekMessageHook;
Hook<CallConvention::stdcall_t, BOOL, LPMSG, HWND, UINT, UINT, UINT> g_diPeekMessageHook;
Hook<CallConvention::stdcall_t, HCURSOR, HCURSOR> g_setCursorHook;
Hook<CallConvention::stdcall_t, BOOL, LPPOINT> g_getCursorPosHook;
Hook<CallConvention::stdcall_t, BOOL, int, int> g_setCursorPosHook;

Renderer g_pRenderer;
Overlay g_Overlay;

bool g_bEnabled = false;

extern "C" __declspec(dllexport) void enable()
{
	g_bEnabled = true;
}

void initGame()
{
	HMODULE hMod = NULL;

	while ((hMod = GetModuleHandle("d3d9.dll")) == NULL || g_bEnabled == false)
		Sleep(200);

	DWORD *vtbl = 0;
	DWORD dwDevice = findPattern((DWORD) hMod, 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");
	memcpy(&vtbl, (void *) (dwDevice + 0x2), 4);

	g_presentHook.apply(vtbl[17], [](LPDIRECT3DDEVICE9 dev, CONST RECT * a1, CONST RECT * a2, HWND a3, CONST RGNDATA *a4) -> HRESULT
	{
		__asm pushad
		g_pRenderer.draw(dev);
		__asm popad

		return g_presentHook.callOrig(dev, a1, a2, a3, a4);
	});

	g_resetHook.apply(vtbl[16], [](LPDIRECT3DDEVICE9 dev, D3DPRESENT_PARAMETERS *pp) -> HRESULT
	{
		__asm pushad
		g_pRenderer.reset(dev);
		__asm popad

		return g_resetHook.callOrig(dev, pp);
	});

	g_peekMessageHook.applyEx("user32.dll", "PeekMessageA", [](LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) -> BOOL
	{
		BOOL res = g_peekMessageHook.callOrig(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
		if (res)
		{
			if (g_Overlay.handleMessage(lpMsg))
			{
				// try to remove this message from queue (not sure this actually works)
				g_peekMessageHook.callOrig(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, PM_REMOVE);
				return FALSE;
			}

			return res;
		}
		else
			return 0;
	});

	g_diPeekMessageHook.applyEx("user32.dll", "PeekMessageW", [](LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) -> BOOL
	{
		BOOL res = g_diPeekMessageHook.callOrig(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
		if (res)
		{
			if (g_Overlay.handleMessage(lpMsg))
				return 0;
			else
				return res;
		}
		else
			return 0;
	});

	g_setCursorHook.applyEx("user32.dll", "SetCursor", [](HCURSOR hCursor) -> HCURSOR
	{
		return g_setCursorHook.callOrig(g_Overlay.isActive() ? LoadCursor(nullptr, IDC_ARROW) : hCursor);
	});

	g_getCursorPosHook.applyEx("user32.dll", "GetCursorPos", [](LPPOINT lpPoint) -> BOOL
	{
		if (g_Overlay.isActive())
			return FALSE;
		else
			return g_getCursorPosHook.callOrig(lpPoint);
	});

	g_setCursorPosHook.applyEx("user32.dll", "SetCursorPos", [](int x, int y) -> BOOL
	{
		if (g_Overlay.isActive())
			return FALSE;
		else
			return g_setCursorPosHook.callOrig(x, y);
	});

	using MessagePaketHandler = std::map<PipeMessages, std::function<void(Serializer&, Serializer&)>>;
	MessagePaketHandler PaketHandler;

	// disable all the messages 'cause we don't need them for current application
#if FALSE
	BIND(TextCreate);
	BIND(TextDestroy);
	BIND(TextSetShadow);
	BIND(TextSetShown);
	BIND(TextSetColor);
	BIND(TextSetPos);
	BIND(TextSetString);
	BIND(TextUpdate);

	BIND(BoxCreate);
	BIND(BoxDestroy);
	BIND(BoxSetShown);
	BIND(BoxSetBorder);
	BIND(BoxSetBorderColor);
	BIND(BoxSetColor);
	BIND(BoxSetHeight);
	BIND(BoxSetPos);
	BIND(BoxSetWidth);

	BIND(LineCreate);
	BIND(LineDestroy);
	BIND(LineSetShown);
	BIND(LineSetColor);
	BIND(LineSetWidth);
	BIND(LineSetPos);

	BIND(ImageCreate);
	BIND(ImageDestroy);
	BIND(ImageSetShown);
	BIND(ImageSetAlign);
	BIND(ImageSetPos);
	BIND(ImageSetRotation);

	BIND(DestroyAllVisual);
	BIND(ShowAllVisual);
	BIND(HideAllVisual);

	BIND(GetFrameRate);
	BIND(GetScreenSpecs);

	BIND(SetCalculationRatio);
	BIND(SetOverlayPriority);
#endif

	BIND(SetOverlayAssetsPath); // the only thing we need 

	new PipeServer([&](Serializer& serializerIn, Serializer& serializerOut)
	{
		SERIALIZATION_READ(serializerIn, PipeMessages, eMessage);

		try
		{
			auto it = PaketHandler.find(eMessage);
			if (it == PaketHandler.end())
				return;

			if (!PaketHandler[eMessage])
				return;

			PaketHandler[eMessage](serializerIn, serializerOut);
		}
		catch (...)
		{
		}
	});

	// postponed initialization of overlay
	g_Overlay.init();

	while (true){
		Sleep(100);
	}
}
