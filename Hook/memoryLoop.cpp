//#include <Windows.h>
#include "memoryLoop.h"
#include "Globals/globalVariables.h"
#include <TlHelp32.h> // getmodule
#include <tchar.h> // getmodule
#include <iostream>

// -------------------------------------------------------------------
// --                           Addresses                           --
// -------------------------------------------------------------------
DWORD gameDllAddition = 0x014062F8; // 8/29/2018

// Player Base
DWORD pBaseOS[] = { 0x58, 0x10, 0x28, 0x378 }; // 8/29/2018

// Hack Offsets
DWORD playerNameOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], pBaseOS[3], 0x46 }; // 8/29/2018
DWORD playerNoteOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], pBaseOS[3], 0x4A0 }; // 8/29/2018
DWORD playerMovementOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], pBaseOS[3], 0x80C }; // 8/29/2018
DWORD playerAnimOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], pBaseOS[3], 0x536 }; // 8/29/2018
DWORD playerXOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], 0x190, 0xA4 }; // 8/29/2018
DWORD playerYOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], 0x190, 0xA0 }; // 8/29/2018
DWORD playerZOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], 0x190, 0xA8 }; // 8/29/2018
DWORD playerGravityOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], pBaseOS[3], 0xA18 }; // 8/29/2018
DWORD playerEyeOffsets[] = { pBaseOS[0], pBaseOS[1], pBaseOS[2], pBaseOS[3], 0x3A1 }; // 8/29/2018

// Hack Variable Containers
HANDLE pHandle;
DWORD noAniAddress, noGravityAddress, finalGameAddress;
BOOL returnNormalGravity = FALSE;

DWORD dwGetModuleBaseAddress(DWORD dwProcessIdentifier, const wchar_t* lpszModuleName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessIdentifier);
	DWORD dwModuleBaseAddress = 0;
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 ModuleEntry32 = { 0 };
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32)) {
			do {
				if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) {
					dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	return dwModuleBaseAddress;
}

DWORD GetPointerAddress(int PointerLevel, DWORD Offsets[], DWORD BaseAddress) {
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, globalVariables::getProcessID());
	DWORD Ptr = *(DWORD*)(BaseAddress);
	DWORD oldPtr;
	if (Ptr == 0) return NULL;

	for (int i = 0; i < PointerLevel; i++) {
		if (i == PointerLevel - 1) {
			oldPtr = Ptr;
			Ptr = (DWORD)(Ptr + Offsets[i]);
			if (Ptr == 0 || oldPtr == Ptr) return NULL;
			return Ptr;
		} else {
			oldPtr = Ptr;
			Ptr = *(DWORD*)(Ptr + Offsets[i]);
			if (Ptr == 0 || oldPtr == Ptr) return NULL;
		}
	}

	return Ptr;
}

void memoryLoop::MemoryLoop() {
	DWORD gameDllBaseAddress = dwGetModuleBaseAddress(globalVariables::getProcessID(), L"Game.dll");

	pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, globalVariables::getProcessID());

	while (globalVariables::getKeepLooping()) {

		if (pHandle != NULL) {
			if (gameDllBaseAddress != NULL) {
				finalGameAddress = gameDllBaseAddress + gameDllAddition;

				animationCheck();
				gravityCheck();
				/*if (globalVariables::getNoFlight()) {
					DWORD noFlightAddress = gameDllBaseAddress + 0x14029F4;
					short noFlightValue = 0; // 2 bytes
					WriteProcessMemory(pHandle, (LPVOID)noFlightAddress, &noFlightValue, sizeof(noFlightValue), NULL);
					std::cout << "Flight overwritten." << std::endl;
				}*/
			}
		} else {
			std::cout << "pHandle is NULL!" << std::endl;
		}
		std::cout << getXPosition() << ", " << getYPosition() << ", " << getZPosition() << std::endl;
		Sleep(250);
	}
}
/* ------------------------------------------------------------------------
 * --                               Checks                               --
 * ------------------------------------------------------------------------ */

void memoryLoop::animationCheck() {
	if (globalVariables::getNoAnimation()) {
		setAnimationSpeed(1);
	}
}

void memoryLoop::gravityCheck() {
	if (globalVariables::getNoGravity() || (returnNormalGravity && !globalVariables::getNoGravity())) {
		returnNormalGravity = TRUE;
		noGravityAddress = GetPointerAddress(5, playerGravityOffsets, finalGameAddress);

		if (noGravityAddress != NULL) {
			if (globalVariables::getNoGravity()) {
				setGravityStatus(5);
				std::cout << "Gravity overwritten." << std::endl;
			} else {
				returnNormalGravity = FALSE;
				setGravityStatus(0);
			}
		}
	}
}


/* ------------------------------------------------------------------------
 * --                               Methods                              --
 * ------------------------------------------------------------------------ */

void memoryLoop::setAnimationSpeed(short num) {
	noAniAddress = GetPointerAddress(5, playerAnimOffsets, finalGameAddress);

	if (noAniAddress != NULL) {
		//short noAnimationValue = num; // 2 bytes
		WriteProcessMemory(pHandle, (LPVOID)noAniAddress, &num, sizeof(num), NULL);
	}
}

void memoryLoop::setGravityStatus(int num) {
	noGravityAddress = GetPointerAddress(5, playerGravityOffsets, finalGameAddress);

	if (noGravityAddress != NULL) {
		WriteProcessMemory(pHandle, (LPVOID)noGravityAddress, &num, sizeof(num), NULL);
	}
}

void memoryLoop::setPosition(float x, float y, float z) {
	DWORD xPosAddress = GetPointerAddress(5, playerXOffsets, finalGameAddress);
	DWORD yPosAddress = GetPointerAddress(5, playerYOffsets, finalGameAddress);
	DWORD zPosAddress = GetPointerAddress(5, playerZOffsets, finalGameAddress);

	if (xPosAddress != NULL && yPosAddress != NULL && zPosAddress != NULL) {
		WriteProcessMemory(pHandle, (LPVOID)xPosAddress, &x, sizeof(x), NULL);
		WriteProcessMemory(pHandle, (LPVOID)yPosAddress, &y, sizeof(y), NULL);
		WriteProcessMemory(pHandle, (LPVOID)zPosAddress, &z, sizeof(z), NULL);
	}
}

void memoryLoop::setXPosition(float x) {
	DWORD xPosAddress = GetPointerAddress(5, playerXOffsets, finalGameAddress);

	if (xPosAddress != NULL) {
		WriteProcessMemory(pHandle, (LPVOID)xPosAddress, &x, sizeof(x), NULL);
	}
}

void memoryLoop::setYPosition(float y) {
	DWORD yPosAddress = GetPointerAddress(5, playerYOffsets, finalGameAddress);

	if (yPosAddress != NULL) {
		WriteProcessMemory(pHandle, (LPVOID)yPosAddress, &y, sizeof(y), NULL);
	}
}

void memoryLoop::setZPosition(float z) {
	DWORD zPosAddress = GetPointerAddress(5, playerZOffsets, finalGameAddress);

	if (zPosAddress != NULL) {
		WriteProcessMemory(pHandle, (LPVOID)zPosAddress, &z, sizeof(z), NULL);
	}
}

float memoryLoop::getXPosition() {
	DWORD xPosAddress = GetPointerAddress(5, playerXOffsets, finalGameAddress);

	if (xPosAddress != NULL) {
		float xPos;
		ReadProcessMemory(pHandle, (LPVOID)xPosAddress, &xPos, sizeof(xPos), NULL);
		return xPos;
	}
}

float memoryLoop::getYPosition() {
	DWORD yPosAddress = GetPointerAddress(5, playerYOffsets, finalGameAddress);

	if (yPosAddress != NULL) {
		float yPos;
		ReadProcessMemory(pHandle, (LPVOID)yPosAddress, &yPos, sizeof(yPos), NULL);
		return yPos;
	}
}

float memoryLoop::getZPosition() {
	DWORD zPosAddress = GetPointerAddress(5, playerZOffsets, finalGameAddress);

	if (zPosAddress != NULL) {
		float zPos;
		ReadProcessMemory(pHandle, (LPVOID)zPosAddress, &zPos, sizeof(zPos), NULL);
		return zPos;
	}
}