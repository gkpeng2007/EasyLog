#include "EasyLog.h"
#include "Log.h"
#include <atlstr.h>

static bool g_bInit = false;
/**
 * @brief 从完整的路径中，取出文件名 
 **/
static LPCWSTR GetFileName(LPCWSTR lpszSrc)
{
	if(!lpszSrc) return NULL;
	
	wchar_t* p = _tcsrchr(const_cast<wchar_t*>(lpszSrc), L'\\');
		
	return p ? ++p : lpszSrc;
}

static bool PrevWork(int level)
{
	if(!g_bInit)
	{
		g_bInit = CLog::GetInstance().Init();
		if(!g_bInit) return false;
	}

	if(level < CLog::GetInstance().m_nLevel) return false;

	return true;
}

/**
 ** @brief 提供Unicode跟多字节两套
 **/
void EasyLog(int level, LPCWSTR lpszFile, const int line, LPCWSTR lpszFunction, LPCWSTR lpszFilter, LPCWSTR lpszFmt, ...)
{
	if(!PrevWork(level)) return;

	va_list va;
	va_start(va, lpszFmt);
	CString strContent;
	strContent.FormatV(lpszFmt, va);

	CLogData data(0, line, 0, ::GetCurrentThreadId(), level, GetFileName(lpszFile), lpszFilter, strContent.GetString(), lpszFunction);
	CLog::GetInstance().Log(data);		
}

/**
 ** @brief	如果上层是多字节调用，也支持一下呗
 ** @author gkpeng
 **/
void EasyLog(int level, LPCSTR lpszFile, const int line, LPCSTR lpszFunction, LPCSTR lpszFilter, LPCSTR lpszFmt, ...)
{
	if(!PrevWork(level)) return;

	wchar_t file[MAX_FILE_FILTER_FUNCTION] = {0};
	wchar_t filter[MAX_FILE_FILTER_FUNCTION] = {0};
	wchar_t function[MAX_FILE_FILTER_FUNCTION] = {0};
	wchar_t content[MAX_CONTENT] = {0};

	MultiByteToWideChar(CP_ACP, 0, lpszFile, strlen(lpszFile), file, MAX_FILE_FILTER_FUNCTION);
	MultiByteToWideChar(CP_ACP, 0, lpszFilter, strlen(lpszFilter), filter, MAX_FILE_FILTER_FUNCTION);	
	MultiByteToWideChar(CP_ACP, 0, lpszFunction, strlen(lpszFunction), function, MAX_CONTENT);

	va_list va;
	va_start(va, lpszFmt);
	CStringA strContent;
	strContent.FormatV(lpszFmt, va);

	MultiByteToWideChar(CP_ACP, 0, (char*)strContent.GetBuffer(), strContent.GetLength(), content, MAX_CONTENT);

	//多字节转unicode
	CLogData data(0, line, 0, ::GetCurrentThreadId(), level, GetFileName(file), filter, content, function);
	CLog::GetInstance().Log(data);

}