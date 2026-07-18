#include "graphics.h"

Graphics::Graphics() {};

Graphics::~Graphics() {};

HRESULT Graphics::initialize(HWND g_hwnd) {
	HRESULT hr = S_OK;

	GetClientRect(g_hwnd, &g_window_rect);

	const UINT width = g_window_rect.right - g_window_rect.left;
	const UINT height = g_window_rect.bottom - g_window_rect.top;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p_d2d_factory);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create D2D factory.", L"Error", MB_OK);
		return hr;
	}

	D3D_DRIVER_TYPE driver_types[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_SOFTWARE,
	};

	const D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	for (UINT driver_type_index = 0; driver_type_index < ARRAYSIZE(driver_types); driver_type_index++) {
		D3D_DRIVER_TYPE driver_type = driver_types[driver_type_index];
		hr = D3D11CreateDevice(
			nullptr,
			driver_type,
			nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&p_d3d_device,
			nullptr,
			&p_d3d_device_context
		);
		if (SUCCEEDED(hr)) {
			break;
		}
	}

	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create D3D device.", L"Error", MB_OK);
		return hr;
	}

	hr = p_d3d_device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&p_dxgi_device));
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to query DXGI device.", L"Error", MB_OK);
		return hr;
	}

	hr = p_dxgi_device->GetAdapter(&p_dxgi_adapter);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to get DXGI adapter.", L"Error", MB_OK);
		return hr;
	}

	hr = p_dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&p_dxgi_factory));
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to get DXGI factory.", L"Error", MB_OK);
		return hr;
	}

	hr = p_d2d_factory->CreateDevice(p_dxgi_device, &p_d2d_device);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create D2D device.", L"Error", MB_OK);
		return hr;
	}

	hr = p_d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &p_d2d_device_context);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create D2D device context.", L"Error", MB_OK);
		return hr;
	}

	DXGI_SWAP_CHAIN_DESC1 sd = {};
	ZeroMemory(&sd, sizeof(sd));

	sd.Width = width;
	sd.Height = height;
	sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Stereo = FALSE;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.Scaling = DXGI_SCALING_NONE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	sd.Flags = 0;

	hr = p_dxgi_factory->CreateSwapChainForHwnd(
		p_d3d_device,
		g_hwnd,
		&sd,
		nullptr,
		nullptr,
		&p_swap_chain
	);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create swap chain.", L"Error", MB_OK);
		return hr;
	}

	hr = p_dxgi_device->SetMaximumFrameLatency(1);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to set maximum frame latency.", L"Error", MB_OK);
		return hr;
	}

	hr = p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&p_dxgi_back_buffer));
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to get back buffer.", L"Error", MB_OK);
		return hr;
	}

	hr = p_dxgi_back_buffer->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(&p_dxgi_back_buffer_surface));
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to query back buffer surface.", L"Error", MB_OK);
		return hr;
	}

	hr = p_d2d_device_context->CreateBitmapFromDxgiSurface(
		p_dxgi_back_buffer_surface,
		&bitmap_properties,
		&p_d2d_target_bitmap
	);
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to create D2D target bitmap.", L"Error", MB_OK);
		return hr;
	}

	p_d2d_device_context->SetTarget(p_d2d_target_bitmap);

	return hr;
}

void Graphics::update() {
	// Update logic here
}

void Graphics::render() {
	p_d2d_device_context->BeginDraw();
	p_d2d_device_context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	// Rendering logic here
	HRESULT hr = p_d2d_device_context->EndDraw();
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to end draw.", L"Error", MB_OK);
	}
	p_swap_chain->Present(1, 0);
}

void Graphics::cleanup() {
	if (p_d2d_target_bitmap) p_d2d_target_bitmap->Release();
	if (p_dxgi_back_buffer_surface) p_dxgi_back_buffer_surface->Release();
	if (p_dxgi_back_buffer) p_dxgi_back_buffer->Release();
	if (p_swap_chain) p_swap_chain->Release();
	if (p_d2d_device_context) p_d2d_device_context->Release();
	if (p_d2d_device) p_d2d_device->Release();
	if (p_dxgi_device) p_dxgi_device->Release();
	if (p_dxgi_factory) p_dxgi_factory->Release();
	if (p_dxgi_adapter) p_dxgi_adapter->Release();
	if (p_d3d_device_context) p_d3d_device_context->Release();
	if (p_d3d_device) p_d3d_device->Release();
	if (p_d2d_factory) p_d2d_factory->Release();
}
