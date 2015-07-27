
// LogWatchDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include <fstream>
#include <iostream>
//#include <boost/archive/binary_iarchive.hpp>
#include "EasySerialize.h"
#include "Log.h"
#include <vector>
#include "afxwin.h"
// CLogWatchDlg dialog
class CLogWatchDlg : public CDialogEx
{
// Construction
public:
	CLogWatchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LOGWATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CListCtrl m_lstLog;
	bool m_bScrollToEnd;
	HWND m_hAttachedWnd;
	
	void InitCtrl();
	VOID InsertLogItem(CSwapData& data);
	void DisConnectWnd();
	void LoadData(std::vector<CLogData>& vecLogData);
	void ReadFile(CString& strFile, std::vector<CLogData>& vecLogData);
	std::vector<CLogData> m_vecLogData;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnClose();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnFileOpenlogfile();
	afx_msg void OnBnClickedButton6();
	CEdit m_edtContent;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
