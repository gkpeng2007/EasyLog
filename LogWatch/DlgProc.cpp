// DlgProc.cpp : implementation file
//

#include "stdafx.h"
#include "LogWatch.h"
#include "DlgProc.h"
#include "afxdialogex.h"
#include <TlHelp32.h>

// CDlgProc dialog

IMPLEMENT_DYNAMIC(CDlgProc, CDialogEx)

#define EASYLOG_SET_WND_MSG		WM_USER + 1000
#define EASYLOG_SET_WND_RESPONE WM_USER + 1001

CDlgProc::CDlgProc(HWND pParentWnd, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgProc::IDD, pParent),
	m_hAttachedWnd(NULL),
	m_hParentWnd(pParentWnd)
{

}

CDlgProc::~CDlgProc()
{
}

void CDlgProc::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstProc);
}


BEGIN_MESSAGE_MAP(CDlgProc, CDialogEx)
	ON_MESSAGE(EASYLOG_SET_WND_RESPONE, &OnSetWindowRespone)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDlgProc::OnNMDblclkList1)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgProc::OnEnChangeEdit1)
END_MESSAGE_MAP()

HWND CDlgProc::GetAttachedHandle()
{
	return m_hAttachedWnd;
}

BOOL CDlgProc::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//初始化ListView
	m_lstProc.SetExtendedStyle(m_lstProc.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_lstProc.ModifyStyle(0, LVS_SINGLESEL);
	m_lstProc.InsertColumn(0, _T("PID"), LVCFMT_LEFT, 80);
	m_lstProc.InsertColumn(1, _T("Process"), LVCFMT_LEFT, 200);		

	GetAllProcess(m_vecProcInfo);
	DisplayList(m_vecProcInfo);

	GetDlgItem(IDC_EDIT1)->SetFocus();
	return TRUE;
}

VOID CDlgProc::GetAllProcess(std::vector<tagProcInfo>& vecProcInfo)
{
	vecProcInfo.clear();

	PROCESSENTRY32 pe32;
	//在使用这个结构前，先设置它的大小
	pe32.dwSize = sizeof(pe32);
	//给系统内所有的进程拍个快照
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	//遍历进程快照，轮流显示每个进程的信息
	BOOL bMore = ::Process32First(hProcessSnap,&pe32);
	while (bMore)
	{
		tagProcInfo info;
		info.dwPid = pe32.th32ProcessID;
		info.strExeName = pe32.szExeFile;
		vecProcInfo.push_back(info);
		bMore = ::Process32Next(hProcessSnap,&pe32);
	}
	::CloseHandle(hProcessSnap);
}

VOID CDlgProc::DisplayList(std::vector<tagProcInfo>& vecProcInfo)
{
	m_lstProc.DeleteAllItems();

	for(auto iter = vecProcInfo.rbegin(); iter != vecProcInfo.rend(); ++iter)
	{       
		CString strPid; strPid.Format(_T("%d"), iter->dwPid);
		CString strProc = iter->strExeName;				
		int nRow = m_lstProc.InsertItem(m_lstProc.GetItemCount(), strPid);
		m_lstProc.SetItemText(nRow, 1, strProc);
	}
}
// CDlgProc message handlers

HWND CDlgProc::GetWindowByPid(DWORD dwPid)
{
	CString strClsName;
	strClsName.Format(_T("EasyLog%d"), dwPid);
	return ::FindWindow(strClsName, NULL);
}

LRESULT CDlgProc::OnSetWindowRespone(WPARAM wParam, LPARAM lParam)
{
	OnOK();
	return S_OK;
}


void CDlgProc::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int sel = pNMItemActivate->iItem;
	// TODO: Add your control notification handler code here.
	if(sel >= 0 && sel < m_lstProc.GetItemCount())
	{
		TCHAR szPid[50] = {0};
		m_lstProc.GetItemText(sel, 0, szPid, 50);		
		char mbPid[50] = {0};
		WideCharToMultiByte(CP_ACP, 0, szPid, wcslen(szPid), mbPid, 50, NULL, NULL);   
		DWORD dwPid = atol(mbPid);

		m_hAttachedWnd = GetWindowByPid(dwPid);
		if(m_hAttachedWnd)
		{
			::SendMessage(m_hAttachedWnd, EASYLOG_SET_WND_MSG, (WPARAM)m_hWnd, (LPARAM)m_hParentWnd);
			//Wait for repsone
		}
		else
		{
			MessageBox(L"Attach Failed!", L"Error", MB_OK);			
		}
	}
	*pResult = 0;	
}

#define MAX_FILTER_SIZE 256
void CDlgProc::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	TCHAR lpszFilter[MAX_FILTER_SIZE] = {0};
	GetDlgItem(IDC_EDIT1)->GetWindowText(lpszFilter, MAX_FILTER_SIZE);
	std::vector<tagProcInfo> vecProcInfo;
	GetProcByFilter(vecProcInfo, lpszFilter);
	DisplayList(vecProcInfo);
}

VOID CDlgProc::GetProcByFilter(std::vector<tagProcInfo>& vecProcInfo, LPCTSTR lpszFilter)
{
	vecProcInfo.clear();
	CString strFilter(lpszFilter);
	strFilter.MakeLower();
	for(auto iter = m_vecProcInfo.begin(); iter != m_vecProcInfo.end(); ++iter)
	{
		if(iter->strExeName.MakeLower().Find(strFilter) >= 0)
		{
			vecProcInfo.push_back(*iter);
		}
	}
}
