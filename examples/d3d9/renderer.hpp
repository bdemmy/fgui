//
// FGUI - DirectX 9 Renderer
//

#pragma once

#define FGUI_IMPLEMENTATION
#include "../../FGUI/FGUI.hpp"
#include <atlstr.h>

// NOTE: make sure to tell FGUI to use D3D9:
// go to: https://github.com/otvv/fgui/wiki/First-Steps#renderer-helpers for more info.

namespace FGUI_D3D9
{
	// NOTE: you still need to initialize the device.
	inline IDirect3DDevice9* m_pDevice;
	inline IDirect3DTexture9* ppTexture;
	inline float g_flRenderAlpha;

	inline void InitIcon() {
		const auto result = D3DXCreateTextureFromFileA(m_pDevice, "C:\\Users\\bdemm\\Desktop\\blackicon.jpg", &ppTexture);

		assert(result == D3D_OK);
	}

	inline void RenderIcon(int _x, int _y, int _width, int _height) {
		IDirect3DBaseTexture9* orig;
		m_pDevice->GetTexture(0, &orig);
		
		if (m_pDevice->SetTexture(0, ppTexture) == D3D_OK) {
			m_pDevice->SetRenderState(D3DRS_ZENABLE, false);
			
			DWORD state1, state2, state3;
			m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &state1);
			m_pDevice->GetRenderState(D3DRS_SRCBLEND, &state2);
			m_pDevice->GetRenderState(D3DRS_DESTBLEND, &state3);
			
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			
			DWORD stage1, stage2, stage3;
			m_pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &stage1);
			m_pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &stage2);
			m_pDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &stage3);
			
			m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			//d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
			m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 16);
			
			DWORD fvf;
			m_pDevice->GetFVF(&fvf);

			const float vertices[4][6] =
			{
				{_x, _y + _height, 0.f, 1.f, 0.0, 1.0 },
				{_x, _y, 0.f, 1.f, 0.0, 0.0},
				{_x + _width, _y + _height, 0.f, 1.f, 1.0, 1.0},
				{_x + _width, _y, 0.f, 1.f, 1.0, 0.0},
			};
			
			m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(float) * 6);

			m_pDevice->SetFVF(fvf);
			m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, stage1);
			m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, stage2);
			m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, stage3);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, state1);
			m_pDevice->SetRenderState(D3DRS_SRCBLEND, state2);
			m_pDevice->SetRenderState(D3DRS_DESTBLEND, state3);
			
			m_pDevice->SetTexture(0, orig);
		}
	}

	inline void CreateFont(FGUI::FONT &_font, std::string _family, int _size, int _flags, bool _bold) // TODO: handle font flags
	{
		CString str(_family.c_str());
		CStringW strw(str);
		LPCWSTR ptr = strw;

		D3DXCreateFont(m_pDevice, _size, 0, _bold ? FW_BOLD : FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, ptr, &_font);
	}

	inline FGUI::DIMENSION GetScreenSize()
	{
		static int iWidth = GetSystemMetrics(SM_CXSCREEN);
		static int iHeight = GetSystemMetrics(SM_CYSCREEN);

		return { iWidth, iHeight };
	}

	inline FGUI::DIMENSION GetTextSize(FGUI::FONT _font, std::string _text)
	{
		RECT rectFontSize = { 0, 0 };

		CString str(_text.c_str());
		CStringW strw(str);
		LPCWSTR ptr = strw;

		// there's probably a better way of getting the size of a text, this will do for the moment.
		_font->DrawTextA(0, _text.c_str(), _text.length(), &rectFontSize, DT_CALCRECT, D3DCOLOR_RGBA(0, 0, 0, 0));

		return { (rectFontSize.right - rectFontSize.left), (rectFontSize.bottom - rectFontSize.top) };
	}

	inline void Text(int _x, int _y, FGUI::FONT _font, FGUI::COLOR _color, std::string _text)
	{
		// Use our global alpha
		_color.m_ucAlpha *= (g_flRenderAlpha / 255.f);
		
		D3DCOLOR dwColor = D3DCOLOR_RGBA(_color.m_ucRed, _color.m_ucGreen, _color.m_ucBlue, _color.m_ucAlpha);

		RECT rectFontSize = { _x, _y };

		CString str(_text.c_str());
		CStringW strw(str);
		LPCWSTR ptr = strw;

		_font->DrawText(0, ptr, _text.length(), &rectFontSize, DT_NOCLIP, dwColor);
	}

	inline void Rectangle(int _x, int _y, int _width, int _height, FGUI::COLOR _color)
	{
		// Use our global alpha
		_color.m_ucAlpha *= (g_flRenderAlpha / 255.f);
		
		D3DCOLOR dwColor = D3DCOLOR_RGBA(_color.m_ucRed, _color.m_ucGreen, _color.m_ucBlue, _color.m_ucAlpha);

		const FGUI::VERTEX vtxVertices[4] =
		{
			{_x, _y + _height, 0.f, 1.f, dwColor },
			{_x, _y, 0.f, 1.f, dwColor},
			{_x + _width, _y + _height, 0.f, 1.f, dwColor},
			{_x + _width, _y, 0.f, 1.f, dwColor},
		};

		m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtxVertices, sizeof(FGUI::VERTEX));
	}

	inline void Outline(int _x, int _y, int _width, int _height, FGUI::COLOR _color)
	{
		// WARNING: do not repeat this at home.
		Rectangle(_x, _y, _width, 1, _color);
		Rectangle(_x, _y, 1, _height, _color);
		Rectangle(_x + _width - 1, _y, 1, _height, _color);
		Rectangle(_x, _y + _height - 1, _width, 1, _color);
	}

	inline void Line(int _from_x, int _from_y, int _to_x, int _to_y, FGUI::COLOR _color)
	{
		// Use our global alpha
		_color.m_ucAlpha *= (g_flRenderAlpha / 255.f);
		
		D3DCOLOR dwColor = D3DCOLOR_RGBA(_color.m_ucRed, _color.m_ucGreen, _color.m_ucBlue, _color.m_ucAlpha);
		
		const FGUI::VERTEX vtxVertices[2] =
		{
			{_from_x, _from_y, 0.f, 0.f, dwColor },
			{_from_x + _to_x, _from_y + _to_y, 0.f, 0.f, dwColor}
		};

		m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vtxVertices, sizeof(FGUI::VERTEX));
	}

	inline void Gradient(int _x, int _y, int _width, int _height, FGUI::COLOR _color1, FGUI::COLOR _color2, bool _horizontal)
	{
		_color1.m_ucAlpha *= (g_flRenderAlpha / 255.f);
		_color2.m_ucAlpha *= (g_flRenderAlpha / 255.f);
		
		// there's probably a better way to do this kind of gradient as well.
		D3DCOLOR dwColor1 = D3DCOLOR_RGBA(_color1.m_ucRed, _color1.m_ucGreen, _color1.m_ucBlue, _color1.m_ucAlpha);
		D3DCOLOR dwColor2 = D3DCOLOR_RGBA(_color2.m_ucRed, _color2.m_ucGreen, _color2.m_ucBlue, _color2.m_ucAlpha);

		const FGUI::VERTEX vtxVertices[4] = 
		{ 
			{ _x, _y, 0.0f, 1.0f, dwColor1 },
			{ _x + _width, _y, 0.0f, 1.0f, _horizontal ? dwColor2 : dwColor1 },
			{ _x, _y + _height, 0.0f, 1.0f, _horizontal ? dwColor1 : dwColor2 },
			{ _x + _width, _y + _height, 0.0f, 1.0f, dwColor2 }
		};

		m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtxVertices, sizeof(FGUI::VERTEX));
	}

	inline void SetAlpha(float alpha) {
		if (alpha < 0) {
			alpha = 0;
		}

		if (alpha > 255) {
			alpha = 255;
		}

		g_flRenderAlpha = alpha;
	}

	inline float GetAlpha() {
		return g_flRenderAlpha;
	}
  
  	// NOTE: call this function only once (preferably when you initialize your application)
	inline void OnEntryPoint()
	{
		FGUI::RENDER.CreateFont = FGUI_D3D9::CreateFont;
		FGUI::RENDER.GetScreenSize = FGUI_D3D9::GetScreenSize;
		FGUI::RENDER.GetTextSize = FGUI_D3D9::GetTextSize;
		FGUI::RENDER.Rectangle = FGUI_D3D9::Rectangle;
		FGUI::RENDER.Outline = FGUI_D3D9::Outline;
		FGUI::RENDER.Line = FGUI_D3D9::Line;
		FGUI::RENDER.Text = FGUI_D3D9::Text;
		FGUI::RENDER.Gradient = FGUI_D3D9::Gradient;
		FGUI::RENDER.SetAlpha = FGUI_D3D9::SetAlpha;
		FGUI::RENDER.GetAlpha = FGUI_D3D9::GetAlpha;

		InitIcon();
	}
}
