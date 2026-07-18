#pragma once

#ifndef PROGRAM_H
#define PROGRAM_H

#include "graphics.h"

class Program {
public:
	Program();
	~Program();
	HRESULT initialize(HWND g_hwnd);
	void update();
	void render();
	void cleanup();
protected:
	Graphics g_graphics;
};

#endif // !PROGRAM_H
