
// LogWatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogWatch.h"
#include "LogWatchDlg.h"
#include "afxdialogex.h"
#include "DlgProc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogWatchDlg dialog


const wchar_t* szLevel[3] = {L"Unknown", L"Debug", L"Final"};

CLogWatchDlg::CLogWatchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogWatchDlg::IDD, pParent),
	m_bScrollToEnd(true),
	m_hAttachedWnd(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogWatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstLog);
	DDX_Control(pDX, IDC_EDIT1, m_edtContent);
}

BEGIN_MESSAGE_MAP(CLogWatchDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CLogWatchDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_WM_COPYDATA()
	ON_COMMAND(ID_FILE_OPENLOGFILE, &CLogWatchDlg::OnFileOpenlogfile)
	ON_BN_CLICKED(IDC_BUTTON6, &CLogWatchDlg::OnBnClickedButton6)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CLogWatchDlg::OnNMClickList1)
END_MESSAGE_MAP()


// CLogWatchDlg message handlers

BOOL CLogWatchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	CMenu* menu =new CMenu;
	menu->LoadMenu(MAKEINTRESOURCE(IDR_MENU1));
	this->SetMenu(menu);

	InitCtrl();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogWatchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogWatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLogWatchDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_lstLog.m_hWnd)
		m_lstLog.MoveWindow(175, 10, cx - 175, cy - 10);	
	if(m_edtContent.m_hWnd)
		m_edtContent.MoveWindow(10, 420, 160, cy - 425);
}

void CLogWatchDlg::InitCtrl()
{
	//ListView for Log
	m_lstLog.SetExtendedStyle(m_lstLog.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_lstLog.ModifyStyle(0, LVS_SINGLESEL);
	m_lstLog.InsertColumn(1, _T("Seq"), LVCFMT_LEFT, 50);
	m_lstLog.InsertColumn(2, _T("Level"), LVCFMT_LEFT, 60);
	m_lstLog.InsertColumn(3, _T("Time"), LVCFMT_LEFT, 130);
	m_lstLog.InsertColumn(4, _T("PID"), LVCFMT_LEFT, 50);	
	m_lstLog.InsertColumn(5, _T("TID"), LVCFMT_LEFT, 50);
	m_lstLog.InsertColumn(6, _T("Filter"), LVCFMT_LEFT, 80);
	m_lstLog.InsertColumn(7, _T("File"), LVCFMT_LEFT, 80);
	m_lstLog.InsertColumn(8, _T("Line"), LVCFMT_LEFT, 50);
	m_lstLog.InsertColumn(9, _T("Function"), LVCFMT_LEFT, 80);	
	m_lstLog.InsertColumn(10, _T("Text"), LVCFMT_LEFT, 350);	
}

VOID CLogWatchDlg::InsertLogItem(CSwapData& data)
{	
	//0 time|1 pid|2 tid|3 filter|4 file|5 line|6 function|7 text
	CTime ts(data.ts);
	CString strTime = ts.Format(_T("%Y-%m-%d %H:%M:%S"));
	CString strLine; strLine.Format(_T("%d"), data.line);
	CString strSeq; strSeq.Format(_T("%d"), m_lstLog.GetItemCount());
	int nRow = m_lstLog.InsertItem(m_lstLog.GetItemCount(), strSeq);

	CString strPid; strPid.Format(_T("%d"), data.pid);
	CString strTid; strTid.Format(_T("%d"), data.tid);
	
	m_lstLog.SetItemText(nRow, 1, szLevel[data.level < 3 ? data.level : 0]);
	m_lstLog.SetItemText(nRow, 2, strTime);
	m_lstLog.SetItemText(nRow, 3, strPid);
	m_lstLog.SetItemText(nRow, 4, strTid);
	m_lstLog.SetItemText(nRow, 5, data.filter);		
	m_lstLog.SetItemText(nRow, 6, data.file);
	m_lstLog.SetItemText(nRow, 7, strLine);
	m_lstLog.SetItemText(nRow, 8, data.function);
	m_lstLog.SetItemText(nRow, 9, data.content);
	
		
	//选中最后一个
	if(m_bScrollToEnd)
		m_lstLog.EnsureVisible(m_lstLog.GetItemCount() - 1, FALSE);	
}


void CLogWatchDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if(m_hAttachedWnd)
	{
		DisConnectWnd();
	}
	else
	{
		CDlgProc dlgProc(m_hWnd);
		dlgProc.DoModal();
		m_hAttachedWnd = dlgProc.GetAttachedHandle();		
	}
	//再次确认

	if(m_hAttachedWnd)
	{
		m_lstLog.DeleteAllItems();
		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("Diconnect!"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("Attach!"));
	}
}


void CLogWatchDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	DisConnectWnd();
	CDialogEx::OnClose();
}

void CLogWatchDlg::DisConnectWnd()
{
	if(m_hAttachedWnd)
	{
		::SendMessage(m_hAttachedWnd, EASYLOG_CLOSE_WND, NULL, NULL);
		m_hAttachedWnd = NULL;
	}
}


BOOL CLogWatchDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: Add your message handler code here and/or call default
	if(!pCopyDataStruct)
		return FALSE;
	CSwapData* pData = (CSwapData*)(pCopyDataStruct->lpData);
	if(pData)
	{		
		InsertLogItem(*pData);
	}
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CLogWatchDlg::OnFileOpenlogfile()
{
	// TODO: Add your command handler code here	
	CFileDialog dlgFile(TRUE, _T( "elog" ), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T( "EasyLog|*.elog" ));
	dlgFile.DoModal();
	CString strFile = dlgFile.GetPathName();
	//CString strFile = "F:\\Projects\\VisualStudio\\EasyLog\\EasyLog\\Release\\Log.elog";
	if(!strFile.IsEmpty())
	{
		ReadFile(strFile, m_vecLogData);
		LoadData(m_vecLogData);
	}
}

void CLogWatchDlg::ReadFile(CString& strFile, std::vector<CLogData>& vecLogData)
{
	vecLogData.clear();
	std::ifstream input(strFile);
	/*boost::archive::binary_iarchive*/CEasySerialize inputArchive(input);
	
	for(;;)
	{
		CLogData data;
		if(inputArchive>>data)
			vecLogData.push_back(data);
		else break;
	}
}

void CLogWatchDlg::LoadData(std::vector<CLogData>& vecLogData)
{
	m_lstLog.DeleteAllItems();
	int seq = 1;
	for(auto iter = vecLogData.begin(); iter != vecLogData.end(); ++iter)
	{
		CLogData& data = *iter;
		CTime ts(data.ts);
		CString strTime = ts.Format(_T("%Y-%m-%d %H:%M:%S"));
		CString strLine; strLine.Format(_T("%d"), data.line);
		CString strSeq; strSeq.Format(_T("%d"), seq++);
		int nRow = m_lstLog.InsertItem(m_lstLog.GetItemCount(), strSeq);

		CString strPid; strPid.Format(_T("%d"), data.pid);
		CString strTid; strTid.Format(_T("%d"), data.tid);
		
		m_lstLog.SetItemText(nRow, 1, szLevel[data.level < 3 ? data.level : 0]);
		m_lstLog.SetItemText(nRow, 2, strTime);
		m_lstLog.SetItemText(nRow, 3, strPid);
		m_lstLog.SetItemText(nRow, 4, strTid);
		m_lstLog.SetItemText(nRow, 5, data.filter.c_str());		
		m_lstLog.SetItemText(nRow, 6, data.file.c_str());
		m_lstLog.SetItemText(nRow, 7, strLine);
		m_lstLog.SetItemText(nRow, 8, data.function.c_str());
		m_lstLog.SetItemText(nRow, 9, data.content.c_str());		
	}
}


void CLogWatchDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	OnFileOpenlogfile();
}


void CLogWatchDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	int sel = pNMItemActivate->iItem;
	if(sel >= 0 && sel < m_lstLog.GetItemCount())
	{
		wchar_t szContent[MAX_CONTENT] = {0};
		m_lstLog.GetItemText(sel, 9, szContent, MAX_CONTENT);
		m_edtContent.SetWindowText(szContent);
	}
	
	*pResult = 0;
}
