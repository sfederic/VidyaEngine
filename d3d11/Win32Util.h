#pragma once

#include <Windows.h>

extern HWND mainWindow; 
extern HWND renderWindow; 
extern const int windowWidth;
extern const int windowHeight;
extern MSG msg;
extern bool mouseUp;

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
HWND CreateSimpleToolbar(HWND hWndParent);

class Win32Util
{
public:
	void SetupWindow(HINSTANCE instance, int cmdShow);
	void SetTimerFrequency();
	void StartTimer();
	void EndTimer();
	void HandleMessages(MSG msg);
	static float GetAspectRatio();

	__int64 start, end, freq;
	double delta, deltaAccum, ticks;
	int frameAccum;
};
