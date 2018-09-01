#pragma once

#include <Windows.h>

class globalVariables{
private:
public:
	static VOID setProcessID(DWORD pID);
	static DWORD getProcessID();
	static VOID setLoggedIn(BOOL decision);
	static BOOL getLoggedIn();
	static VOID setKeepLooping(BOOL decision);
	static BOOL getKeepLooping();
	static VOID setNoAnimation(BOOL decision);
	static BOOL getNoAnimation();
	static VOID setNoFlight(BOOL decision);
	static BOOL getNoFlight();
	static VOID setNoGravity(BOOL decision);
	static BOOL getNoGravity();
};