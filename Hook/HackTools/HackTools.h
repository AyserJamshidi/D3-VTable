#pragma once

#include <Windows.h>

class HackTools {

public:
	bool dllTargetIsForeground(int givenProcessID);
	static HMODULE GetThisDllHandle();

};