#include <iostream>
#include <windows.h>
#include <tchar.h>
#include "../easylog/easyLog.h"
#include "../EasyLog/Log.h" 
#include "../EasyLog/EasySerialize.h"
#include <atlstr.h>
int main()
{
#if 1	
	//Log写入测试
	int i = 0;
	for(; i < 200; i++)
	{		
		Sleep(500);
		LogDev(_T("Debug Filter"), _T("I am yielding at %d"), i);
		Sleep(200);
		LogFinal(_T("Final Filter"), _T("Final Log %f"), i * 0.23f);
		//EasyLog(1, L"Hello", 1000, L"Main", L"Filter", L"Hello, World! %d", 100);
	}
#else 
	//Log读取测试，可以在LogWatch工程验证
	std::ifstream inf(_T("D:\\Log.elog"), std::ios::binary);
	CEasySerialize ia(inf);
	for(;;)
	{
		CLogData data;
		if(ia>>data)
		{
			CString str = data.content.c_str();
			std::cout<<str<<std::endl;
		}
		else
		{
			break;
		}
	}
#endif	
	return 0;
}