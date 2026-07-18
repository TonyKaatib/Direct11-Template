#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "window.h"

#include <d2d1.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi1_6.h>
#include <string>
#include <wincodec.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")

class Graphics {
public:
	Graphics();
	~Graphics();
	HRESULT initialize(HWND g_hwnd);
	void update();
	void render();
	void cleanup();
protected:
	ID3D11Device* p_d3d_device = nullptr;
	ID3D11DeviceContext* p_d3d_device_context = nullptr;
	IDXGISwapChain1* p_swap_chain = nullptr;
	IDXGIAdapter* p_dxgi_adapter = nullptr;
	IDXGIFactory2* p_dxgi_factory = nullptr;
	ID2D1Factory4* p_d2d_factory = nullptr;
	IDXGIDevice1* p_dxgi_device = nullptr;
	ID2D1Device3* p_d2d_device = nullptr;
	ID2D1DeviceContext3* p_d2d_device_context = nullptr;
	ID3D11Texture2D* p_dxgi_back_buffer = nullptr;
	IDXGISurface* p_dxgi_back_buffer_surface = nullptr;
	ID2D1Bitmap1* p_d2d_target_bitmap = nullptr;
	RECT g_window_rect = {};
};
#endif // !GRAPHICS_H
