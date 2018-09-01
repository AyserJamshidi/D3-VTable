#include "Drawing.h"


/*void Drawing::Text(LPD3DXFONT font, int x, int y, DWORD color, LPCSTR Message) {
	vertex argbColor[4];
	argbColor[0].Color = argbColor[1].Color = argbColor[2].Color = argbColor[3].Color = color;

	RECT rect;
	rect.left = x; // xOffset
	rect.right = 300;
	rect.top = y; // yOffset
	rect.bottom = y+30; // yMaxLength, we will put it as yOffset + 30

	font->DrawTextA(0, Message, strlen(Message), &rect, 0, color);
}*/
struct QuadVertex {
	float x, y, z, rhw;
	DWORD dwColor;
};

void Drawing::Box(float x, float y, float w, float h, DWORD color, LPDIRECT3DDEVICE9 pDevice) {
	QuadVertex qV[4] = {
		{ (float)x, (float)(y + h), 0.0f, 0.0f, color },
		{ (float)x, (float)y, 0.0f, 0.0f, color },
		{ (float)(x + w), (float)(y + h), 0.0f, 0.0f, color },
		{ (float)(x + w), (float)y, 0.0f, 0.0f, color }
	};

	pDevice->SetTexture(0, NULL);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(qV[0]));

	/*pDevice->SetTexture(0, NULL);
	pDevice->SetPixelShader(NULL);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);*/
	//pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, q, sizeof(vertex));
}