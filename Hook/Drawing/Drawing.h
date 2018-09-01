#pragma once

#include <d3dx9.h>

class Drawing {

	public:
	void Box(float xOffset, float yOffset, float width, float height, DWORD color, LPDIRECT3DDEVICE9 pDevice);
};