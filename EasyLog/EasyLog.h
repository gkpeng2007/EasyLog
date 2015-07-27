/**
 * @brief	EasyLog对外接口
 * @author	gkpeng
 * @date	2015/7
 * @todo	需要考虑unicode的兼容	
 **/
#pragma once
#include <WTypes.h>
#pragma comment(lib, "EasyLog")

#ifdef UNICODE
	#define __STR2WSTR(str) L##str
	#define _STR2WSTR(str) __STR2WSTR(str)
	#define __FUNCTIONT__ _STR2WSTR(__FUNCTION__)
	#define __FILENT__ _STR2WSTR(__FILE__)
#else
	#define __FUNCTIONT__ __FUNCTION__
	#define __FILENT__ __FILE__
#endif

/**
 * @brief		对外使用，通常建议使用宏版: LogDev LogFinal
 * @date		2015/7
 **/
void EasyLog(int level, LPCTSTR lpszFile, const int line, LPCTSTR pszFunction, LPCTSTR pszFilter, LPCTSTR pszFmt, ...);

#define LogDev(filter, fmt, ...) EasyLog(1, __FILENT__, __LINE__, __FUNCTIONT__, filter, fmt, ##__VA_ARGS__)
#define LogFinal(filter, fmt, ...) EasyLog(2, __FILENT__, __LINE__, __FUNCTIONT__, filter, fmt, ##__VA_ARGS__)