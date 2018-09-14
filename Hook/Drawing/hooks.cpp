#include "Drawing.h"
#include "..\hooks.h"
#include "..\MinHook\include\MinHook.h"
#include "..\HackTools\HackTools.h"

#include "..\memoryLoop.h"
#include "..\Globals\globalVariables.h"

#include <iostream>
#include <TlHelp32.h> // getmodule
#include <tchar.h> // getmodule

#define MIN_SELECTION_INDEX 0
#define MAX_SELECTION_INDEX 2

Drawing draw;
HackTools hTools;
HANDLE MemoryLoopThread = NULL, HotkeyLoopThread = NULL;
//bool keepLooping = TRUE;

// START Menu Variables --------------------------------------------------
int opacity = 0;

int currentSelectionIndex = 0;
//bool noAnimationStatus, noFlightStatus, noGravityStatus;

// Offsets and splits
static int mainMenuBackgroundXOffset = 10;
static int mainMenuBackgroundYOffset = 10;
static int mainMenuBackgroundWidth = 300;
static int mainMenuBackgroundHeight = 400;

static int titleBackgroundHeight = 30;

// Boxes
D3DRECT mainMenuRect = { mainMenuBackgroundXOffset, mainMenuBackgroundYOffset + titleBackgroundHeight, mainMenuBackgroundWidth, mainMenuBackgroundHeight }; // Do not touch
D3DRECT titleBackgroundD3Rect = { mainMenuRect.x1, mainMenuRect.y1 - titleBackgroundHeight, mainMenuRect.x2, titleBackgroundHeight }; // Do not touch
D3DRECT playerMenuBox = { mainMenuRect.x1 + 20, mainMenuRect.y1 + 20, 100, 20 };

// Text boxes
RECT mainMenuTitleRect = { mainMenuRect.x1 + 200, mainMenuRect.y1 + 5, 20, 100 };
RECT titleBackgroundRect = { titleBackgroundD3Rect.x1, titleBackgroundD3Rect.y1, titleBackgroundD3Rect.x2, titleBackgroundD3Rect.y2 };
//RECT testrect, rect;

LPD3DXFONT titleFont, pFont;
// END Menu Variable------------------------------------------------------

// START Memory Changing Variables ---------------------------------------
std::wstring playerName = L"???";
// END Memory Changing Variables -----------------------------------------

// START Variables changed by us -----------------------------------------
bool firstEndScene = TRUE;
bool initializedPresent = FALSE;
D3DDEVICE_CREATION_PARAMETERS cparams;

//HMODULE dllHandle; // DLL HMODULE
HWND processHandle; // Target's window handle
bool showD3D;
// END Variables changed by us -------------------------------------------

typedef HRESULT(APIENTRY* Reset) (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT APIENTRY hkReset(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset oReset = FALSE;

typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
HRESULT APIENTRY hkEndScene(IDirect3DDevice9*);
EndScene oEndScene = FALSE;

bool opacityCheck() {
	if (showD3D) {
		if (opacity < 187) {
			opacity += 17;
		}
	} else if (opacity > 0) {
		opacity -= 17;
	}
	return opacity > 0;
}

HRESULT __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "Reset Hooked!" << std::endl;

	if (pFont)
		pFont->OnLostDevice();
	if (titleFont)
		titleFont->OnLostDevice();

	HRESULT returnValue = oReset(pDevice, pPresentationParameters);

	if (returnValue == D3D_OK) {
		if (pFont)
			pFont->OnResetDevice();
		if (titleFont)
			titleFont->OnResetDevice();
		//setWindowRect();
	}

	return returnValue;
}

int menuSpacer = 20;
RECT noAnimationRect = { mainMenuRect.x1 + 10, mainMenuRect.y1 + menuSpacer, mainMenuRect.x2, 100 };
RECT noAnimationStatusRect = { mainMenuRect.x1 + 140, mainMenuRect.y1 + menuSpacer, mainMenuRect.x2, 100 };

RECT noFlightRect = { mainMenuRect.x1 + 10, mainMenuRect.y1 + (menuSpacer * 2), mainMenuRect.x2, 100 };
RECT noFlightStatusRect = { mainMenuRect.x1 + 140, mainMenuRect.y1 + (menuSpacer * 2), mainMenuRect.x2, 100 };

RECT noGravitytRect = { mainMenuRect.x1 + 10, mainMenuRect.y1 + (menuSpacer * 3), mainMenuRect.x2, 100 };
RECT noGravityStatusRect = { mainMenuRect.x1 + 140, mainMenuRect.y1 + (menuSpacer * 3), mainMenuRect.x2, 100 };

// NOTE - Draw boxes with primitives instead of clear, much better performance.
HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice) { // TODO 1 Move all calculations out of EndScene into a separate thread to keep FPS 1:1.

	if (globalVariables::getLoggedIn()) {
		if (opacityCheck()) {
			LPDIRECT3DSTATEBLOCK9 oStateBlock; // Keep so Aion doesn't look fucked up when D3D Transparency is up
			pDevice->CreateStateBlock(D3DSBT_ALL, &oStateBlock); // Keep so Aion doesn't look fucked up when D3D Transparency is up
			if (firstEndScene) {
				firstEndScene = FALSE;

				pDevice->GetCreationParameters(&cparams);
				processHandle = cparams.hFocusWindow;

				if (!pFont)
					D3DXCreateFontW(pDevice, 16, 0, FW_DEMIBOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pFont);

				if (!titleFont)
					D3DXCreateFontW(pDevice, 26, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI"), &titleFont);
			}

			// Menu
			//pDevice->Clear(1, &mainMenuRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 0, 0);
			draw.Box(mainMenuRect.x1, mainMenuRect.y1, mainMenuRect.x2, mainMenuRect.y2, D3DCOLOR_ARGB(opacity, 0, 0, 0), pDevice); // Main menu background
			draw.Box(titleBackgroundD3Rect.x1, titleBackgroundD3Rect.y1, titleBackgroundD3Rect.x2, titleBackgroundD3Rect.y2, D3DCOLOR_ARGB(opacity, 255, 0, 0), pDevice); // Main menu background
			//draw.Box(playerMenuBox.x1, playerMenuBox.y1, playerMenuBox.x2, playerMenuBox.y2, D3DCOLOR_ARGB(opacity, 222, 30, 58), pDevice);


			// Draw Text
			titleFont->DrawTextW(0, L"Lmfaoown's Aion D3D Hack", 24, &titleBackgroundRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB(opacity, 0, 0, 0));
			//pFont->DrawTextW(0, (L"Hello " + playerName).c_str(), -1, &mainMenuTitleRect, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB(opacity, 255, 0, 0));

			D3DCOLOR selectionColor = D3DCOLOR_ARGB(opacity, 255, 255, 255);

			pFont->DrawTextW(0, L"No Animation", -1, &noAnimationRect, DT_NOCLIP | DT_LEFT, currentSelectionIndex == 0 ? selectionColor : D3DCOLOR_ARGB(opacity, 255, 0, 0));
			pFont->DrawTextW(0, globalVariables::getNoAnimation() ? L"TRUE" : L"FALSE", -1, &noAnimationStatusRect, DT_NOCLIP | DT_LEFT, globalVariables::getNoAnimation() ? D3DCOLOR_ARGB(opacity, 0, 255, 0) : D3DCOLOR_ARGB(opacity, 255, 0, 0));

			pFont->DrawTextW(0, L"No Flight CD", -1, &noFlightRect, DT_NOCLIP | DT_LEFT, currentSelectionIndex == 1 ? selectionColor : D3DCOLOR_ARGB(opacity, 255, 0, 0));
			pFont->DrawTextW(0, globalVariables::getNoFlight() ? L"TRUE" : L"FALSE", -1, &noFlightStatusRect, DT_NOCLIP | DT_LEFT, globalVariables::getNoFlight() ? D3DCOLOR_ARGB(opacity, 0, 255, 0) : D3DCOLOR_ARGB(opacity, 255, 0, 0));

			pFont->DrawTextW(0, L"No Gravity", -1, &noGravitytRect, DT_NOCLIP | DT_LEFT, currentSelectionIndex == 2 ? selectionColor : D3DCOLOR_ARGB(opacity, 255, 0, 0));
			pFont->DrawTextW(0, globalVariables::getNoGravity() ? L"TRUE" : L"FALSE", -1, &noGravityStatusRect, DT_NOCLIP | DT_LEFT, globalVariables::getNoGravity() ? D3DCOLOR_ARGB(opacity, 0, 255, 0) : D3DCOLOR_ARGB(opacity, 255, 0, 0));

			std::cout << "selection = " << currentSelectionIndex << std::endl;

			oStateBlock->Apply(); // Keep so Aion doesn't look fucked up when D3D Transparency is up
			oStateBlock->Release(); // Keep so Aion doesn't look fucked up when D3D Transparency is up
		}
	}

	return oEndScene(pDevice);
}

void HotkeyLoop() {
	if (globalVariables::getLoggedIn()) {
		int possibleKeys[] = {
			VK_DELETE,
			VK_INSERT,
			VK_UP,
			VK_DOWN,
			VK_LEFT,
			VK_RIGHT };

		std::cout << "Now reading for hotkeys..." << std::endl;
		while (globalVariables::getKeepLooping()) {
			if (hTools.dllTargetIsForeground(globalVariables::getProcessID()))
				for (int i : possibleKeys) {
					if (GetAsyncKeyState(i)) {
						switch (i) {
						case VK_DELETE:
							showD3D = !showD3D;
							break;
						case VK_INSERT:
						{
							globalVariables::setKeepLooping(FALSE);

							MH_DisableHook(MH_ALL_HOOKS); // Unhook everything before unloading DLL so we don't crash.
							if (pFont) {
								pFont->Release();
								pFont = NULL;
							}

							if (titleFont) {
								titleFont->Release();
								titleFont = NULL;
							}

							HWND console = GetConsoleWindow(); // Keep this here.  After freeing console this is no longer possible.
							Sleep(200);
							FreeConsole(); // Free the console.
							SendMessage(console, WM_CLOSE, NULL, NULL); // Send console window handle the close message.
							Sleep(1000); // Keep this sleep otherwise we crash.  Possible race condition??
							FreeLibraryAndExitThread(hTools.GetThisDllHandle(), 0); // Unload DLL.
							break;
						}
						case VK_UP:
						{
							if (showD3D && currentSelectionIndex > MIN_SELECTION_INDEX)
								currentSelectionIndex--;
							break;
						}
						case VK_DOWN:
							if (showD3D && currentSelectionIndex < MAX_SELECTION_INDEX)
								currentSelectionIndex++;
							break;
						case VK_RIGHT:
						case VK_LEFT:
						{
							if (showD3D) {
								BOOL setStatus = i == VK_RIGHT ? TRUE : FALSE;
								switch (currentSelectionIndex) {
								case 0: // NoAnimation
									globalVariables::setNoAnimation(setStatus);
									break;
								case 1: // NoFlight
									globalVariables::setNoFlight(setStatus);
									break;
								case 2: // NoGravity
									globalVariables::setNoGravity(setStatus);
									break;
								}
							}
							break;
						}
						default:
							std::cout << "Unset button: " << i << std::endl;
							break;
						}
						while (GetAsyncKeyState(i) & 0x8000) // While key is held down, we wait...
							Sleep(10);
						Sleep(10);
					}
				}
			Sleep(10);
		}
	}
	std::cout << "Ending HotkeyLoop()" << std::endl;
}

DWORD WINAPI DirectXInit(__in  LPVOID lpParameter) {
	IDirect3D9* d3d = NULL;
	IDirect3DDevice9* d3ddev = NULL;

	//HWND tmpWnd = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, dllHandle, NULL);
	HWND tmpWnd = CreateWindowA("BUTTON", "Temp Window", NULL, NULL, NULL, 1, 1, NULL, NULL, hTools.GetThisDllHandle(), NULL); /*find_main_window(6532);// TODO 1 - REPLACE 6532 WITH FUNCTION TO GET PID*/
	DWORD retrievedPID;
	GetWindowThreadProcessId(tmpWnd, &retrievedPID); // Save hooked process' ID

	globalVariables::setProcessID(retrievedPID);

	if (tmpWnd == NULL) {
#ifdef _DEBUG
		std::cout << "Failed to create temporary window." << std::endl;
#else
		displayError(L"Failed to create temporary window.");
#endif
		return 0;
	}

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL) {
		DestroyWindow(tmpWnd);
#ifdef _DEBUG
		std::cout << "Failed to create Direct3D interface." << std::endl;
#else // Release
		displayError(L"Failed to create Direct3D interface.");
		//MessageBox(NULL, L"Failed to create Direct3D interface.  Closing.", L"Lmfaoown's Aion Beta Hack", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
#endif
		return 0;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // Fills d3dpp with necessary amount of 00 bytes
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (result != D3D_OK) {
		d3d->Release();
		DestroyWindow(tmpWnd);
#if defined _DEBUG
		std::cout << "Failed to create temporary Direct3D device." << std::endl;
#else
		displayError(L"Failed to create temporary Direct3D device.");
#endif
		//Log("[DirectX] Failed to create temp Direct3D device");
		return 0;
	}

	// We have the device, so walk the vtable to get the address of all the dx functions in d3d9.dll
#if defined _M_X64
	DWORD64* dVtable = (DWORD64*)d3ddev;
	dVtable = (DWORD64*)dVtable[0];
#elif defined _M_IX86
	DWORD* dVtable = (DWORD*)d3ddev;
	dVtable = (DWORD*)dVtable[0];
#endif

	oReset = (Reset)dVtable[16]; // Set oReset to the original Reset.
	oEndScene = (EndScene)dVtable[42]; // Set oEndScene to the original EndScene.

	// TODO 1 - Properly label this and add errors instead of return 1's...
	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[16], &hkReset, reinterpret_cast<void**>(&oReset)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[16]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[42], &hkEndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[42]) != MH_OK) { return 1; }

	processHandle = tmpWnd; 
	d3ddev->Release();
	d3d->Release();
	DestroyWindow(tmpWnd);

	std::cout << "===== " << GetWindowText;
	HotkeyLoopThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&HotkeyLoop, NULL, 0, NULL);
	MemoryLoopThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&memoryLoop::MemoryLoop, NULL, 0, NULL);
	return 1;
}