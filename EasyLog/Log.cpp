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
	/**初始化做的几件事：
	 * 1）获取当前进程ID
	 * 2）创建存储文件 
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
	//用当前进程的PID作为类名
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
	//创建窗口
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
		//Log Debug 模式下才会创建窗口（用于跟LogWatch交互数据）
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
	//如果是在debug模式，则打开LogDebug
	return LOG_DEBUG;
#endif
	char* p = ::GetCommandLineA();	
	
	if(p && strlen(p) <= MAX_COMMAND_LINE_SIZE)
	{
		char* q = strstr(p, strLogDebug);
		if(q) return LOG_DEBUG;//开启了Log Debug
	}
	return LOG_FINAL;//默认返回LogFinal
}

void CLog::Log(CLogData& data)
{
	g_cnt++;
	static time_t ts = 0;
	data.pid = CLog::GetInstance().m_lPid;
	time(&ts); data.ts = ts;

	//保存至文件
	m_oarchive<<data;

	//如果被Attach了，那就通知下吧
	if(m_hObservr)
	{		
		CSwapData d2(data.ts, data.line, data.pid, data.tid, data.level, data.file.c_str(), data.filter.c_str(), data.content.c_str(), data.function.c_str());
		COPYDATASTRUCT cd;
		cd.dwData = 0;
		cd.cbData = sizeof(d2);
		cd.lpData = (void*)&d2;
		LRESULT ret = SendMessage(m_hObservr, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cd);
#ifdef _DEBUG
		//测试用，想看下如果m_hObserver不在了，会咋样
		printf_s("SendMessage ret = %x\n", ret);
#endif
	}
}
