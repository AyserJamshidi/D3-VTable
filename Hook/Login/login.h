#pragma once
#include <Windows.h>
#include <string>

class loginSystem {
private:
public:
	static BOOL attemptLogin(std::string user, std::string pass);
};