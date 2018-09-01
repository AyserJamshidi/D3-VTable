#include "globalVariables.h"

DWORD processID = 0;
BOOL loggedIn = FALSE;
BOOL keepLooping = TRUE;
BOOL noAnimation = FALSE;
BOOL noFlight = FALSE;
BOOL noGravity = FALSE;

VOID globalVariables::setProcessID(DWORD pID) {
	processID = pID;
}

DWORD globalVariables::getProcessID() {
	return processID;
}

VOID globalVariables::setLoggedIn(BOOL decision) {
	loggedIn = decision;
}

BOOL globalVariables::getLoggedIn() {
	return loggedIn;
}

VOID globalVariables::setKeepLooping(BOOL decision) {
	keepLooping = decision;
}

BOOL globalVariables::getKeepLooping() {
	return keepLooping;
}


VOID globalVariables::setNoAnimation(BOOL decision) {
	noAnimation = decision;
}

BOOL globalVariables::getNoAnimation() {
	return noAnimation;
}


VOID globalVariables::setNoFlight(BOOL decision) {
	noFlight = decision;
}

BOOL globalVariables::getNoFlight() {
	return noFlight;
}


VOID globalVariables::setNoGravity(BOOL decision) {
	noGravity = decision;
}

BOOL globalVariables::getNoGravity() {
	return noGravity;
}