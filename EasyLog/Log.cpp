#include "Log.h"
#include <stdio.h>
#include <vector>
#include <time.h>
const char* strLogDebug = "/logd";
const char* strLogFinal = "/logf";

float g_cnt = 1;
HANDLE g_hEvent = NULL;


CLog::CLog(void):
m_logfile("Log.elog", std::ios::binary),
m_oarchive(m_logfile),
m_hObservr(NULL),
m_hWnd(NULL)
{
	/**��ʼ�����ļ����£�
	 * 1����ȡ��ǰ����ID
	 * 2�������洢�ļ� 
	 *
	 **/
	m_lPid = ::GetCurrentProcessId();
		
}


CLog::~CLog(void)
{	
	m_logfile.close();
}

LRESULT CALLBACK WindowProcc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	printf_s("Msg %d\n", uMsg);
#endif
	switch(uMsg)
	{
	case EASYLOG_SET_WND_MSG:
		{
			CLog::GetInstance().m_hObservr = (HWND)lParam;
			::SendMessage((HWND)wParam, EASYLOG_SET_WND_RESPONE, NULL, NULL);
		}
		break;	
	case EASYLOG_CLOSE_WND:
		{
			CLog::GetInstance().m_hObservr = NULL;
		}
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool CLog::CreateLogWindow()
{
	WNDCLASSEX wc = {sizeof(wc)};
	wc.hInstance = ::GetModuleHandle(0);
	//�õ�ǰ���̵�PID��Ϊ����
	TCHAR strClassName[100] = {0};
	wsprintf(strClassName, _T("EasyLog%d"), m_lPid);	
	wc.lpszClassName = strClassName;
	wc.lpfnWndProc   = &WindowProcc;
	RegisterClassEx(&wc);
	m_hWnd = CreateWindowEx(
		0, 
		wc.lpszClassName,
		_T("EasyLog Window"), 
		WS_OVERLAPPED, 
		0, 
		0, 
		0, 
		0, 
		NULL, 
		NULL, 
		wc.hInstance, 
		NULL);
	if(!m_hWnd) return false;
	ShowWindow(m_hWnd, SW_HIDE);
	return true;
}

DWORD __stdcall LogWindowThread(LPVOID pArg)
{
	//��������
	HWND hWnd = NULL;
	bool bCreate = CLog::GetInstance().CreateLogWindow();
	if(bCreate)
	{
		hWnd = CLog::GetInstance().m_hWnd;
	}
	SetEvent(g_hEvent);

	MSG msg;
	while(GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#ifdef _DEBUG
	printf_s("LogWindow MsgLoop End\n");
#endif
	

	return 0;
}

CLog& CLog::GetInstance()
{
	static CLog instance;
	return instance;
}

bool CLog::Init()
{
	m_nLevel = GetLevel();

	if(m_nLevel < LOG_FINAL)
	{
		//Log Debug ģʽ�²Żᴴ�����ڣ����ڸ�LogWatch�������ݣ�
		if(g_hEvent == NULL)
		{
			g_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		}
#ifdef _DEBUG
		printf_s("begin create logwindow\n");
#endif
		CreateThread(NULL, 0, &LogWindowThread, NULL, 0, NULL);
		WaitForSingleObject(g_hEvent, 5000);	
		
#ifdef _DEBUG
		printf_s("Event Handle %x  HWND %x\n", g_hEvent, m_hWnd);
		printf_s("create logwindow result %x\n", (DWORD)m_hWnd);
#endif
		return (m_hWnd != NULL);
	}
	return true;	
}

int CLog::GetLevel()
{
#ifdef _DEBUG	
	//�������debugģʽ�����LogDebug
	return LOG_DEBUG;
#endif
	char* p = ::GetCommandLineA();	
	
	if(p && strlen(p) <= MAX_COMMAND_LINE_SIZE)
	{
		char* q = strstr(p, strLogDebug);
		if(q) return LOG_DEBUG;//������Log Debug
	}
	return LOG_FINAL;//Ĭ�Ϸ���LogFinal
}

void CLog::Log(CLogData& data)
{
	g_cnt++;
	static time_t ts = 0;
	data.pid = CLog::GetInstance().m_lPid;
	time(&ts); data.ts = ts;

	//�������ļ�
	m_oarchive<<data;

	//�����Attach�ˣ��Ǿ�֪ͨ�°�
	if(m_hObservr)
	{		
		CSwapData d2(data.ts, data.line, data.pid, data.tid, data.level, data.file.c_str(), data.filter.c_str(), data.content.c_str(), data.function.c_str());
		COPYDATASTRUCT cd;
		cd.dwData = 0;
		cd.cbData = sizeof(d2);
		cd.lpData = (void*)&d2;
		LRESULT ret = SendMessage(m_hObservr, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cd);
#ifdef _DEBUG
		//�����ã��뿴�����m_hObserver�����ˣ���զ��
		printf_s("SendMessage ret = %x\n", ret);
#endif
	}
}
