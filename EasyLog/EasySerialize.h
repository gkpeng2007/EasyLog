#pragma once
#include <fstream>
#define CHAR_SIZE 2

/**
 ** @brief	简单的序列化工具
 ** @author	gkpeng
 ** @date	2015/7
 **/
class CEasySerialize
{
public:
	/**
	 ** @brief 序列化初始化
	 **/
	CEasySerialize(std::ofstream& ofile):m_pfIn(NULL), m_pfOut(&ofile), m_nFileSize(0)
	{

	}
	/**
	 ** @brief 反序列化初始化
	 **/
	CEasySerialize(std::ifstream& ifile):m_pfIn(&ifile), m_pfOut(NULL), m_nFileSize(0)
	{
		//计算文件的大小
		m_pfIn->seekg(0, std::ios::end);
		m_nFileSize = m_pfIn->tellg();
		m_pfIn->seekg(0, std::ios::beg);
	}

	~CEasySerialize(void)
	{
		if(m_pfIn) m_pfIn->close();
		if(m_pfOut) m_pfOut->close();
	}

	/**
	 ** @brief 重载>>，用于反序列化
	 **/
	template<class T>
	bool operator>>(T& rValue)
	{
		if(m_nFileSize <= 0)
		{
#ifdef _DEBUG
			std::cout<<"File End!"<<std::endl;	
#endif
			return false;
		}
		rValue.serialize(*this);
		return true;
	}

	/**
	 ** @brief 重载<<，用于序列化
	 **/
	template<class T>
	void operator<<(T& rValue)
	{
		rValue.serialize(*this);
	}

	/**
	 ** @brief 重载&，能区分序列化还是反序列化
	 **/
	template<class T>
	void operator&(T& rValue)
	{
		//这里通过文件判断是序列化还是反序列化
		if(m_pfIn)
		{
			m_pfIn->read((char*)&rValue, sizeof T);
			m_nFileSize -= (sizeof T);
		}
		else if(m_pfOut)
		{
			int writeSize = sizeof T;
			const char* writeBuf = (char*)&rValue;
			m_pfOut->write(writeBuf, writeSize);
		}
	}
	
	/**
	 ** @brief 重载&的偏特化：string类型(只支持Unicode)
	 **/
	template<>
	void operator&(std::wstring& rValue)
	{
		//这里需要判断是序列化还是反序列化
		if(m_pfIn)
		{
			int hdrSize = 0;
			m_pfIn->read((char*)&hdrSize, (sizeof hdrSize));
			m_nFileSize -= (sizeof hdrSize);

			wchar_t* readBuf = new wchar_t[hdrSize + 1];
			readBuf[hdrSize] = 0;
			m_pfIn->read((char*)readBuf, hdrSize * CHAR_SIZE);
			m_nFileSize -= (hdrSize * CHAR_SIZE);
			rValue = readBuf;

			delete[] readBuf;
		}
		else if(m_pfOut)
		{
			int hdrSize = rValue.length();
			const char* hdrBuf = (char*)&hdrSize;
			m_pfOut->write(hdrBuf, sizeof hdrSize);

			int writeSize = rValue.length() * CHAR_SIZE;
			const wchar_t* writeBuf = rValue.c_str();
			m_pfOut->write((char*)writeBuf, writeSize);
		}
	}

private:
	std::ofstream* m_pfOut;
	std::ifstream* m_pfIn;
	int m_nFileSize;
};