/**
 ** @brief	跟Log相关的操作封装类，singleton
 ** @author	gkpeng
 ** @date	2015/7
 **/
#pragma once
#include <windows.h>
#include <fstream>
#include <tchar.h>
//#include <boost/archive/binary_oarchive.hpp>
#include "EasySerialize.h"//自己写了一个序列化工具，boost的lib太大了 :(

#define BOOST_ALL_NO_LIB 
#define EASYLOG_SET_WND_MSG     WM_USER + 1000
#define EASYLOG_SET_WND_RESPONE WM_USER + 1001
#define EASYLOG_RESET_WND_MSG   WM_USER + 1002
#define EASYLOG_LOG_MSG			WM_USER + 1003
#define EASYLOG_CLOSE_WND		WM_USER + 1020

#define MAX_COMMAND_LINE_SIZE			256
#define MAX_FILE_FILTER_FUNCTION		100
#define MAX_CONTENT						256

enum LOGLEVEL
{
	LOG_DEBUG = 1,
	LOG_FINAL = 2,
};

/**
 ** @brief	WM_COPYDATA 貌似对可变长的字符串支持的不是很好，于是写了一个字符串数组版本的CLogData ^_^
 ** @todo	对内容的size，目前的256是偏小
 ** @date	2015/7
 **/
class CSwapData
{
public:
	CSwapData():
	  ts(0), line(0), pid(0), tid(0), level(0)		  
	  {}
	  CSwapData(long _ts, int _line, int _pid, int _tid, byte _level, LPCTSTR _file, LPCTSTR _filter, LPCTSTR _content, LPCTSTR _function):
	  ts(_ts), line(_line), pid(_pid), tid(_tid), level(_level)
	  {
#ifdef UNICODE
		  wcsncpy_s(file, _file, MAX_FILE_FILTER_FUNCTION);
		  wcsncpy_s(filter, _filter, MAX_FILE_FILTER_FUNCTION);
		  wcsncpy_s(content, _content, MAX_CONTENT);
		  wcsncpy_s(function, _function, MAX_FILE_FILTER_FUNCTION);
#else
		  memset(file, 0, MAX_FILE_FILTER_FUNCTION);
		  memset(filter, 0, MAX_FILE_FILTER_FUNCTION);
		  memset(function, 0, MAX_FILE_FILTER_FUNCTION);
		  memset(content, 0, MAX_CONTENT);

		  MultiByteToWideChar(CP_ACP, 0, _file, strlen(_file), file, MAX_FILE_FILTER_FUNCTION);
		  MultiByteToWideChar(CP_ACP, 0, _filter, strlen(_filter), filter, MAX_FILE_FILTER_FUNCTION);
		  MultiByteToWideChar(CP_ACP, 0, _content, strlen(_content), content, MAX_CONTENT);
		  MultiByteToWideChar(CP_ACP, 0, _function, strlen(_function), function, MAX_CONTENT);
#endif
	  }

public:
	long	ts;
	int		line;
	int		pid;
	int		tid;
	byte	level;
	wchar_t	file[MAX_FILE_FILTER_FUNCTION];
	wchar_t	filter[MAX_FILE_FILTER_FUNCTION];
	wchar_t	content[MAX_CONTENT];
	wchar_t	function[MAX_FILE_FILTER_FUNCTION];
};

class CLogData
{
public:
	CLogData():
	ts(0), line(0), pid(0), tid(0), level(0),
	file(L""), filter(L""), content(L""), function(L"")
	{}
	CLogData(long _ts, int _line, int _pid, int _tid, byte _level, std::wstring _file, std::wstring _filter, std::wstring _content, std::wstring _function):
	ts(_ts), line(_line), pid(_pid), tid(_tid), level(_level), file(_file), filter(_filter), content(_content), function(_function)
	{}

private:
	friend class CEasySerialize;

	template<class Archive>
	void serialize(Archive& ar)
	{
		ar & ts;
		ar & line;
		ar & pid;
		ar & tid;
		ar & level;
		ar & file;
		ar & filter;
		ar & content;
		ar & function;
	}			

public:
	long  ts;
	int   line;
	int	  pid;
	int   tid;
	byte  level;
	std::wstring file;
	std::wstring filter;
	std::wstring content;
	std::wstring function;
};



class CLog
{
	typedef CLog this_class;
public:	
	static CLog& GetInstance();
	~CLog(void);		
	bool Init();
	void Log(CLogData& data);
	bool CreateLogWindow();
private:
	CLog(void);	
	int GetLevel();	
public:
	HWND m_hWnd;//当前window
	HWND m_hObservr;//attach当前窗口的window
	long m_lPid;
	int	 m_nLevel;
	std::ofstream m_logfile;
	CEasySerialize m_oarchive;
};

