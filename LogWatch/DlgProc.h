#pragma once
#include "afxcmn.h"
#include <vector>

#define MAX_CLS_NAME 256
// CDlgProc dialog

class CDlgProc : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProc)

public:
	CDlgProc(HWND pParentWnd, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProc();
	HWND GetAttachedHandle();
	virtual BOOL OnInitDialog();
// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstProc;

private:
	struct tagProcInfo
	{
		CString strExeName;
		HWND    hWnd;
		DWORD   dwPid;
		tagProcInfo():hWnd(NULL), dwPid(0){}
	};
	HWND m_hParentWnd;
	HWND m_hAttachedWnd;	
	std::vector<tagProcInfo> m_vecProcInfo;
	VOID GetAllProcess(std::vector<tagProcInfo>& vecProcInfo);
	VOID DisplayList(std::vector<tagProcInfo>& vecProcInfo);	
	HWND GetWindowByPid(DWORD dwPid);

	LRESULT OnSetWindowRespone(WPARAM, LPARAM);
	VOID GetProcByFilter(std::vector<tagProcInfo>& vecProcInfo, LPCTSTR lpszFilter);
public:
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit1();
};
