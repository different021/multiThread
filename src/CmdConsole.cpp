#include "cmdLinstener.h"
#include "cmdConsole.h"

HANDLE cmdConsole::_hConsole = NULL;
HANDLE cmdConsole::_hListener = NULL;
DWORD  cmdConsole::_id = 0;

BOOL   cmdConsole::_bUse = true;
INT	   cmdConsole::_x = 0;
INT	   cmdConsole::_y = 0;

TCHAR  cmdConsole::_SendBuffer[1024] = L"";
TCHAR  cmdConsole::_ReceiveBuffer[1024] = L"";



void __stdcall ResultWait(DWORD dw)
{
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	TCHAR buf[1024] = L"Done\n";
	DWORD bufLen;
	int a = 0;
	switch (dw)
	{
	case WAIT_OBJECT_0:
		//프로세스 종료
		wsprintf(buf, L"PROCESS DONE\n");
		WriteConsole(output, buf, _tcslen(buf), &bufLen, 0);
		break;
	case WAIT_TIMEOUT:
		//프로세스가 5000밀리초 이내 종료되지 않았다.
		wsprintf(buf, L"PROCESS TIME OUT\n");
		WriteConsole(output, buf, _tcslen(buf), &bufLen, 0);
		break;
	case WAIT_FAILED:
		//실패?
		wsprintf(buf, L"PROCESS FAIL\n");
		WriteConsole(output, buf, _tcslen(buf), &bufLen, 0);
		break;
	}
}

unsigned int WINAPI ConsoleThread(LPVOID p)
//void __stdcall ConsoleThread(void* p)
{
	HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	if (input == INVALID_HANDLE_VALUE)
		DebugBreak();

	TCHAR cmd[1024];

	DWORD written = 0;

	while (1)
	{
		ZeroMemory(cmd, sizeof(cmd));
		DWORD dwResult = ReadConsole(input, cmd, 1024, &written, 0);

		//HANDLE hCon = FindWindow(NULL, L"Command Console");
		//HANDLE hWin = FindWindow(NULL, L"ConsoleCmd");

		if (dwResult == 1)
		{
			HANDLE eventHandle = CreateEventEx(
				nullptr,
				L"Command",
				false,
				EVENT_ALL_ACCESS);

			if (eventHandle != 0)
			{
				cmdListener::PushCmdQue(cmd, eventHandle);
				SetEvent(g_hWaitObj);
				DWORD dw = WaitForSingleObject(eventHandle, INFINITE);
				ResultWait(dw);
				CloseHandle(eventHandle);
			}
			else
			{
				DWORD dwErr = GetLastError();
			}
		}

	}
}


bool cmdConsole::Alloc()
{

	if (AllocConsole() != 0)
	{
		unsigned id;
		//_hConsole = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)ConsoleThread, NULL, 0, &id);
		_hConsole = (HANDLE)_beginthreadex(NULL, 0, &ConsoleThread, NULL, 0, &id);
		/*
		_hConsole = CreateThread(
			NULL,									//defalt security
			0,										//default stack size
			(LPTHREAD_START_ROUTINE)ConsoleThread,  //name of the thread function
			0,										//no Thread parameter
			0,										//default startup flags
			&_id);
		*/
		SetConsoleTitle(L"Command Console");
		if (_hConsole)
		{
			return true;
		}

	}
	else
	{
		MessageBox(NULL, L"FAIL!", L"FAIL to Create Console", MB_OK);
		return false;
	}

}

bool cmdConsole::Init(HANDLE hListener)
{
	if (!hListener)
	{
		_hListener = NULL;
		//MessageBox(NULL, L"ERROR", L"Target handle is NULL", MB_OK);
		return false;
	}
	else {
		_hListener = hListener;
		return true;
	}

}

bool cmdConsole::Free()
{
	if (_hConsole)
	{
		//스레드마다 고유의 static 값을 갇고싶다?>
		//__declspec(thread) static int a;

		//_beginThreadEx 종료.
		CloseHandle(_hConsole);
		//unsigned int uiReturn;
		//_endthreadex(uiReturn);

		//_endthread();
		//TerminateThread(_hConsole, 0);
	}


	return FreeConsole() != 0;
}

//Cmd를 처리해주는 객체에게 Cmd메세지와 이벤트 객체를 전달. 이벤트 객체를 해제하는 것은 해당 객체가 해줄 것이다.
void cmdConsole::SendCmd(std::wstring cmdMsg, HANDLE hEvent)
{
	//::GetCurrentDirectory(MAXPATH_, dir);				//현재 폴더 저장.
	//::SetCurrentDirectory(ynLogGetPath());			//로그 폴더로 변경.

	//_ftprintf(fp, message);							//파일에 출력
	//_ftprintf(fp, L"\r\n");

	//_stprintf(pBuffer, _SendBuffer);
	//length = _len;

	cmdListener::PushCmdQue(cmdMsg, hEvent);

}

