#pragma once
#include "CmdThreadDef.h"

extern "C" class cmdConsole
{
public:
	static bool Alloc();
	static bool Init(HANDLE hWnd);
	static bool Free();
	static void SendCmd(std::wstring cmdMsg, HANDLE hEvent);

	static HANDLE GetHandle()	{ return _hConsole; };
	static HWND	  GetWindow()	{ return GetConsoleWindow(); };
	static TCHAR* GetBuffer()	{ return _SendBuffer; };
	static UINT   GetBufferSize() { return MAXSIZE; };
	static DWORD  GetSendLength() { return (DWORD)_tcslen(_SendBuffer); };

private:
	static HANDLE _hConsole;
	static HANDLE _hListener;					//커맨드 보낼 대상 핸들
	static DWORD  _id;

	static BOOL   _bUse;
	static INT	  _x;
	static INT	  _y;

	static TCHAR _SendBuffer[MAXSIZE];				//보내기용 버퍼.
	static TCHAR _ReceiveBuffer[MAXSIZE];			//받기용 버퍼.
};


//extern HANDLE g_EventHandle;
