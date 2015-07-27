#pragma once
#include <fstream>
#define CHAR_SIZE 2

/**
 ** @brief	�򵥵����л�����
 ** @author	gkpeng
 ** @date	2015/7
 **/
class CEasySerialize
{
public:
	/**
	 ** @brief ���л���ʼ��
	 **/
	CEasySerialize(std::ofstream& ofile):m_pfIn(NULL), m_pfOut(&ofile), m_nFileSize(0)
	{

	}
	/**
	 ** @brief �����л���ʼ��
	 **/
	CEasySerialize(std::ifstream& ifile):m_pfIn(&ifile), m_pfOut(NULL), m_nFileSize(0)
	{
		//�����ļ��Ĵ�С
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
	 ** @brief ����>>�����ڷ����л�
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
	 ** @brief ����<<���������л�
	 **/
	template<class T>
	void operator<<(T& rValue)
	{
		rValue.serialize(*this);
	}

	/**
	 ** @brief ����&�����������л����Ƿ����л�
	 **/
	template<class T>
	void operator&(T& rValue)
	{
		//����ͨ���ļ��ж������л����Ƿ����л�
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
	 ** @brief ����&��ƫ�ػ���string����(ֻ֧��Unicode)
	 **/
	template<>
	void operator&(std::wstring& rValue)
	{
		//������Ҫ�ж������л����Ƿ����л�
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