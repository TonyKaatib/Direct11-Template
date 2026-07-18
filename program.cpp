#include "program.h"

Program::Program() {};

Program::~Program() {};

HRESULT Program::initialize(HWND g_hwnd) {
	HRESULT hr = S_OK;
	hr = g_graphics.initialize(g_hwnd);
	if (FAILED(hr)) {
		return hr;
	}
	return hr;
}

void Program::update() {
	g_graphics.update();
}

void Program::render() {
	g_graphics.render();
}

void Program::cleanup() {
	g_graphics.cleanup();
}
