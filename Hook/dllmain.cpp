#include "dllmain.h"
#include "hooks.h"
#include "Globals/globalVariables.h"
#include "Login/login.h"

#include <Windows.h>
#include <iostream>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//==========================================================================================================================

void displayError(const std::wstring message) {
	MessageBox(NULL, message.c_str(), L"Lmfaoown's Aion Beta Hack", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
}

//==========================================================================================================================

void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD Reason, LPVOID Reserved) { // Called upon injection
	DisableThreadLibraryCalls(hinstDll); // Disabled DLL_THREAD_ messages.  We aren't doing anything thread critical here.

	if (Reason == DLL_PROCESS_ATTACH) {

	//#ifdef _DEBUG
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to AllocConsole while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONIN$", "r", stdin);
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		freopen_s(&pCout, "CONOUT$", "w", stderr);
	//#endif // DEBUG

		/*std::string username, password;

		std::cout << "Login!\n" << std::endl;

		while (!globalVariables::getLoggedIn()) {
			std::cout << "Please enter the following information:" << std::endl;

			std::cout << "Username: ";
			std::cin >> username;

			std::cout << "Password: ";
			std::cin >> password;

			if (loginSystem::attemptLogin(username, password)) { // TODO verify login information
				globalVariables::setLoggedIn(TRUE);
				std::cout << "Welcome " << username << "!\n" << std::endl;
			} else {
				clear();
				std::cout << "Incorrect login information!" << std::endl;
			}
		}*/

		while (!GetModuleHandle(L"d3d9.dll"))
			Sleep(100);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DirectXInit, NULL, 0, NULL);
	}
	std::cout << "Done and returning true now.." << std::endl; 
	return TRUE;
}