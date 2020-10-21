#pragma once
#include "CmdThreadDef.h"

extern "C" class cmdListener
{
public:
	cmdListener() {};
	cmdListener(const cmdListener& rhs) = delete;
	cmdListener& operator=(const cmdListener& rhs) = delete;

	static bool Alloc();
	static bool Init(const HANDLE hCmd, HWND hWnd);
	static void Free();
	static void GetCmd(TCHAR* pData, UINT sizeofData);

	static HANDLE GetHandle();
	static void  PushCmdQue(std::wstring cmdStr, HANDLE hEvent);
	static std::queue<std::wstring> GetVector();
	static void PopVector();
	static size_t GetSizeVector();
	static HWND GetHWND();
private:
	static HANDLE _hListener;					//이 리스너의 핸들
	static HANDLE _hCmdConsole;					//여기다 커맨드를 날리는 콘솔창안의 스레드 핸들
	static HWND _hWndTarget;
	static DWORD  _id;					
	static std::mutex _mu;						//명령어를 팝할때 데이터를 건드리지 못하도록
	static std::queue<std::wstring> _CmdQue;	//명령어 Queue
	

};

extern HANDLE g_hWaitObj;			//처리를 기다리는 핸들.