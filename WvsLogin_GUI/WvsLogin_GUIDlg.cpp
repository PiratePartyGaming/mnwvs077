
// WvsLogin_GUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WvsLogin_GUI.h"
#include "WvsLogin_GUIDlg.h"
#include "afxdialogex.h"
#include "..\WvsLogin\LoginApp.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include <thread>
#include <string>
#include <locale>
#include <codecvt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CWvsLogin_GUIDlg* g_pConsoleDialog = nullptr;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWvsLogin_GUIDlg dialog



CWvsLogin_GUIDlg::CWvsLogin_GUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WvsLogin_GUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWvsLogin_GUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_ConsoleOutput);
	DDX_Control(pDX, IDC_EDIT1, m_CommandInput);
	DDX_Control(pDX, IDC_CAPTION1, m_InputBoxCaption);
}

BEGIN_MESSAGE_MAP(CWvsLogin_GUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CWvsLogin_GUIDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


void ForwardConsoleMessage(int nLogLevel, const std::string& sMessage)
{
	g_pConsoleDialog->AppendMessage(nLogLevel, sMessage);
}

// CWvsLogin_GUIDlg message handlers

BOOL CWvsLogin_GUIDlg::OnInitDialog()
{
	/*
	Forward all messages passed to WvsLogger to this GUI.
	*/
	g_pConsoleDialog = this;
	WvsLogger::SetForwardFunc(ForwardConsoleMessage);

	/*
	Convert utf8 args to ascii.
	*/
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	const std::wstring wide_string = __targv[1];
	std::string utf8_string = converter.to_bytes(wide_string);
	char* pArg[] = { "", (char*)utf8_string.c_str() };


	CDialogEx::OnInitDialog();
	m_InputBoxCaption.SetWindowTextW(L"Command:");

	std::thread(LoginApp::InitializeService, __argc, pArg).detach();
	m_ConsoleOutput.SetBackgroundColor(
		0, RGB(0xD0, 0xD0, 0xD0)
	);

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWvsLogin_GUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWvsLogin_GUIDlg::OnPaint()
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
HCURSOR CWvsLogin_GUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWvsLogin_GUIDlg::OnEnChangeEdit1()
{
}

/*
Capture "Enter" event on m_CommandInput.
*/
BOOL CWvsLogin_GUIDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT1))
		{
			UpdateData(TRUE);
			CString sCommandInput;
			m_CommandInput.GetWindowTextW(sCommandInput);

			/*
			Convert to std::string and pass to ConsolePromptManager.
			*/
			std::string s;
			s = CT2A(sCommandInput);
			if (s == "")
				return false;

			ForwardConsoleMessage(WvsLogger::LEVEL_NORMAL, ">>> ");
			ForwardConsoleMessage(WvsLogger::LEVEL_NORMAL, s + "\n");
			LoginApp::OnCommandPromptInput(s);
			m_CommandInput.SetWindowTextW(L"");

			UpdateData(FALSE);
		}
		return true;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CWvsLogin_GUIDlg::AppendMessage(int nLogLevel, const std::string & sMsg)
{

	CHARFORMAT cf = { sizeof(cf) };
	cf.dwEffects = CFM_BOLD;
	cf.dwMask = CFM_BOLD | CFM_COLOR;

	cf.crTextColor = RGB(0, 0, 0);
	if (nLogLevel == WvsLogger::LEVEL_ERROR)
		cf.crTextColor = RGB(0xFF, 0x00, 0x00);
	else if (nLogLevel == WvsLogger::LEVEL_INFO)
		cf.crTextColor = RGB(0x00, 0x00, 0xE3);
	else if (nLogLevel == WvsLogger::LEVEL_WARNING)
		cf.crTextColor = RGB(0xFF, 0x58, 0x09);

	if (m_ConsoleOutput.GetLineCount() >= 256)
		m_ConsoleOutput.SetWindowTextW(L""); //Clear all.
	int nOriginalLine = m_ConsoleOutput.GetLineCount();

	m_ConsoleOutput.SetSelectionCharFormat(cf);
	int nLength = m_ConsoleOutput.GetTextLength();
	m_ConsoleOutput.SetSel(nLength, nLength);
	m_ConsoleOutput.ReplaceSel(CString(sMsg.c_str()));

	int nWrittenLine = m_ConsoleOutput.GetLineCount();
	m_ConsoleOutput.LineScroll(nWrittenLine - nOriginalLine);
}
