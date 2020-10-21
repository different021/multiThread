

#include "cmdConsole.h"
#include "cmdLinstener.h"

HANDLE cmdListener::_hListener = NULL;
HANDLE cmdListener::_hCmdConsole = NULL;
HWND   cmdListener::_hWndTarget;
DWORD  cmdListener::_id = 0;
std::queue<std::wstring> cmdListener::_CmdQue;		//명령어 Queue

std::wstring caption;

CRITICAL_SECTION g_CritSec;

HANDLE g_hWaitObj = NULL;			//처리를 기다리는 핸들.

void __stdcall ListenThread(void* p)
{
	g_hWaitObj = CreateEventEx(
		nullptr,
		L"WaitObj",
		false,
		EVENT_ALL_ACCESS
	);

	if (g_hWaitObj == NULL)
	{
		DebugBreak();
	}

	while (1)
	{
		//std::mutex m;
		WaitForSingleObject(g_hWaitObj, INFINITE);
		while (!cmdListener::GetVector().empty())
		{
			//m.lock();

			std::wstring cmd = cmdListener::GetVector().front();		//큐의 가장 앞 읽어오기

			if(_tcscmp(cmd.data(), L"exit"))
			{
				SendMessage(cmdListener::GetHWND(), WM_DESTROY, 0, 0);
			}

			caption = cmd;												//읽어서 저장.
			cmdListener::PopVector();									//제거
			SetWindowText(cmdListener::GetHWND(), caption.data());		//Window Caption 업데이트

			//m.unlock();
		}
		//Sleep(500);
	}
}

bool cmdListener::Alloc()
{
	_hListener = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenThread, 0, 0, &_id);
	if (!_hListener)
		return false;

	InitializeCriticalSection(&g_CritSec);

}

bool cmdListener::Init(const HANDLE hCmd, HWND hWnd)
{
	if (hWnd != NULL)
	{
		_hWndTarget = hWnd;
	}
	else
	{
		MessageBox(NULL, L"FAIL", L"Not exist Target Window(hWnd)", MB_OK);
		return false;
	}

	if(hCmd != 0)
	{ 
		_hCmdConsole = hCmd; 
		return true;
	}
	else
	{
		MessageBox(NULL, L"FAIL", L"Not exist CmdConsole", MB_OK);
		return false;
	}
	
}

void cmdListener::Free()
{
	if (_hListener)
		CloseHandle(_hListener);

	DeleteCriticalSection(&g_CritSec);
}

void cmdListener::GetCmd(TCHAR* pData, UINT sizeofData)
{
	std::wstring str(pData);
	
}

HANDLE cmdListener::GetHandle()
{
	return _hListener;
}

void cmdListener::PushCmdQue(std::wstring cmdStr, HANDLE hEvent)
{
	//__try
	{
		EnterCriticalSection(&g_CritSec);
		_CmdQue.push(cmdStr);

		if (hEvent != NULL)
		{
			SetEvent(hEvent);
			//ResetEvent(hEvent);
		}
			
	}
	//__finally
	{
		LeaveCriticalSection(&g_CritSec);
	}

}

std::queue<std::wstring> cmdListener::GetVector()
{
	return _CmdQue;
}

void cmdListener::PopVector()
{
	_CmdQue.pop();
}

size_t cmdListener::GetSizeVector()
{
	return _CmdQue.size();
}

HWND cmdListener::GetHWND()
{
	return _hWndTarget;
}
