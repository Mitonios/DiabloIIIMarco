
// DiabloIIIMarcoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiabloIIIMarco.h"
#include "DiabloIIIMarcoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
struct MyConfig
{
	int		leftMouseTime;
	int		rightMouseTime;
	int		skillSlot01Time;
	int		skillSlot02Time;
	int		skillSlot03Time;
	int		skillSlot04Time;
	int		skill01Enable;
	int		skill02Enable;
	int		skill03Enable;
	int		skill04Enable;
};

/************************************************************************/
/* Variable                                                             */
/************************************************************************/
MyConfig myConfig;
wchar_t					configSavePath[3000] = { 0 };
const int				timerDelay = 50/*ms*/;
bool					flagOnF1 = false;
bool					flagOnF2 = false;
bool					flagOnF3 = false;
int						leftMouseCooldown;
int						rightMouseCooldown;
int						skillSlot01Cooldown;
int						skillSlot02Cooldown;
int						skillSlot03Cooldown;
int						skillSlot04Cooldown;
HHOOK					hGlobalHook;

void		ValidateConfig(void)
{
	TRACE("leftMouseTime %d\r\n", myConfig.leftMouseTime);
	myConfig.leftMouseTime = int(round(myConfig.leftMouseTime / 50.0) * 50);
	myConfig.rightMouseTime = int(round(myConfig.rightMouseTime / 50.0) * 50);
	myConfig.skillSlot01Time = int(round(myConfig.skillSlot01Time / 50.0) * 50);
	myConfig.skillSlot02Time = int(round(myConfig.skillSlot02Time / 50.0) * 50);
	myConfig.skillSlot03Time = int(round(myConfig.skillSlot03Time / 50.0) * 50);
	myConfig.skillSlot04Time = int(round(myConfig.skillSlot04Time / 50.0) * 50);
	if (myConfig.leftMouseTime < 50) myConfig.leftMouseTime = 50;
	if (myConfig.rightMouseTime < 50) myConfig.rightMouseTime = 50;
	if (myConfig.skillSlot01Time < 50) myConfig.skillSlot01Time = 50;
	if (myConfig.skillSlot02Time < 50) myConfig.skillSlot02Time = 50;
	if (myConfig.skillSlot03Time < 50) myConfig.skillSlot03Time = 50;
	if (myConfig.skillSlot04Time < 50) myConfig.skillSlot04Time = 50;
	if (myConfig.skill01Enable != 0) myConfig.skill01Enable = 1;
	if (myConfig.skill02Enable != 0) myConfig.skill02Enable = 1;
	if (myConfig.skill03Enable != 0) myConfig.skill03Enable = 1;
	if (myConfig.skill04Enable != 0) myConfig.skill04Enable = 1;
}

// CDiabloIIIMarcoDlg dialog


CDiabloIIIMarcoDlg::CDiabloIIIMarcoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIABLOIIIMARCO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDiabloIIIMarcoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDiabloIIIMarcoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_EN_KILLFOCUS(IDD_EDIT_LEFTMOUSE_MS, &CDiabloIIIMarcoDlg::OnKillFocusEditLeftMouseMs)
	ON_EN_KILLFOCUS(IDD_EDIT_RIGHTMOUSE_MS, &CDiabloIIIMarcoDlg::OnKillFocusEditRightMouseMs)

	ON_EN_KILLFOCUS(IDD_EDIT_SKILL1, &CDiabloIIIMarcoDlg::OnKillFocusEditSkill1Ms)
	ON_EN_KILLFOCUS(IDD_EDIT_SKILL2, &CDiabloIIIMarcoDlg::OnKillFocusEditSkill2Ms)
	ON_EN_KILLFOCUS(IDD_EDIT_SKILL3, &CDiabloIIIMarcoDlg::OnKillFocusEditSkill3Ms)
	ON_EN_KILLFOCUS(IDD_EDIT_SKILL4, &CDiabloIIIMarcoDlg::OnKillFocusEditSkill4Ms)

	ON_BN_CLICKED(IDC_CHECK_SKILL1, &CDiabloIIIMarcoDlg::OnBnClickedCheckSkill1)
	ON_BN_CLICKED(IDC_CHECK_SKILL2, &CDiabloIIIMarcoDlg::OnBnClickedCheckSkill2)
	ON_BN_CLICKED(IDC_CHECK_SKILL3, &CDiabloIIIMarcoDlg::OnBnClickedCheckSkill3)
	ON_BN_CLICKED(IDC_CHECK_SKILL4, &CDiabloIIIMarcoDlg::OnBnClickedCheckSkill4)
END_MESSAGE_MAP()


// CDiabloIIIMarcoDlg message handlers

BOOL CDiabloIIIMarcoDlg::OnInitDialog()
{
	// Create mutex
	HANDLE hMutex = CreateMutex(NULL, TRUE, L"Diablo III Marco");
	switch (GetLastError())
	{
	case ERROR_SUCCESS:
		// Mutex created successfully. There is 
		// no instances running
		break;

	case ERROR_ALREADY_EXISTS:
		// Mutex already exists so there is a 
		// running instance of our app.
		hMutex = NULL;
		exit(EXIT_SUCCESS);
		break;

	default:
		// Failed to create mutex by unknown reason
		break;
	}

	srand((unsigned int)time((time_t*)0));

	CDialogEx::OnInitDialog();
	((CListBox*)GetDlgItem(IDC_LIST_LOG))->InsertString(0, L"Init Program");


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	myTimerID = CDialogEx::SetTimer(1, timerDelay, NULL);

	if (configSavePath[0] == 0)
	{
		wchar_t appdataPath[2000] = { 0 };
		GetEnvironmentVariable(L"AppData", appdataPath, 1999);
		wchar_t bufferDir[2000] = { 0 };
		swprintf_s(bufferDir, L"%ls\\DialoIIIMarco\\", appdataPath);
		CreateDirectoryW(bufferDir, 0);
		swprintf_s(configSavePath, L"%ls\\DialoIIIMarco\\DialoIIIMarco.dat", appdataPath);
	}
	LoadConfig();
	ValidateConfig();
	wchar_t buffer[1000] = { 0 };

	swprintf_s(buffer, L"%d", myConfig.leftMouseTime);
	GetDlgItem(IDD_EDIT_LEFTMOUSE_MS)->SetWindowText(buffer);
	swprintf_s(buffer, L"%d", myConfig.rightMouseTime);
	GetDlgItem(IDD_EDIT_RIGHTMOUSE_MS)->SetWindowText(buffer);

	swprintf_s(buffer, L"%d", myConfig.skill01Enable);
	((CButton*)GetDlgItem(IDC_CHECK_SKILL1))->SetCheck(myConfig.skill01Enable);
	GetDlgItem(IDD_EDIT_SKILL1)->EnableWindow(myConfig.skill01Enable);
	swprintf_s(buffer, L"%d", myConfig.skillSlot01Time);
	GetDlgItem(IDD_EDIT_SKILL1)->SetWindowText(buffer);

	swprintf_s(buffer, L"%d", myConfig.skill02Enable);
	((CButton*)GetDlgItem(IDC_CHECK_SKILL2))->SetCheck(myConfig.skill02Enable);
	GetDlgItem(IDD_EDIT_SKILL2)->EnableWindow(myConfig.skill02Enable);
	swprintf_s(buffer, L"%d", myConfig.skillSlot02Time);
	GetDlgItem(IDD_EDIT_SKILL2)->SetWindowText(buffer);

	swprintf_s(buffer, L"%d", myConfig.skill03Enable);
	((CButton*)GetDlgItem(IDC_CHECK_SKILL3))->SetCheck(myConfig.skill03Enable);
	GetDlgItem(IDD_EDIT_SKILL3)->EnableWindow(myConfig.skill03Enable);
	swprintf_s(buffer, L"%d", myConfig.skillSlot03Time);
	GetDlgItem(IDD_EDIT_SKILL3)->SetWindowText(buffer);

	swprintf_s(buffer, L"%d", myConfig.skill04Enable);
	((CButton*)GetDlgItem(IDC_CHECK_SKILL4))->SetCheck(myConfig.skill04Enable);
	GetDlgItem(IDD_EDIT_SKILL4)->EnableWindow(myConfig.skill04Enable);
	swprintf_s(buffer, L"%d", myConfig.skillSlot04Time);
	GetDlgItem(IDD_EDIT_SKILL4)->SetWindowText(buffer);

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDiabloIIIMarcoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialogEx::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDiabloIIIMarcoDlg::OnPaint()
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
HCURSOR CDiabloIIIMarcoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDiabloIIIMarcoDlg::OnTimer(UINT_PTR nIdEvent)
{
	//TRACE("Test\r\n");
	// TODO: Add your implementation code here.
}


void CDiabloIIIMarcoDlg::SaveConfig()
{
	((CListBox*)GetDlgItem(IDC_LIST_LOG))->InsertString(0, L"Save Config");
	CFile saveFile;
	if (saveFile.Open(configSavePath, CFile::modeCreate | CFile::modeWrite))
	{
		saveFile.Write(&myConfig, sizeof(myConfig));
		saveFile.Close();
	}
}

void CDiabloIIIMarcoDlg::LoadConfig()
{
	((CListBox*)GetDlgItem(IDC_LIST_LOG))->InsertString(0, L"Load Config");
	CFile saveFile;
	if (saveFile.Open(configSavePath, CFile::modeRead))
	{
		saveFile.Read(&myConfig, sizeof(myConfig));
		saveFile.Close();
	}
}


void CDiabloIIIMarcoDlg::OnKillFocusEditLeftMouseMs()
{
	wchar_t bufferText[1000] = { 0 };
	GetDlgItem(IDD_EDIT_LEFTMOUSE_MS)->GetWindowTextW(bufferText, 999);
	int newValue = 0;
	swscanf(bufferText, L"%d", &newValue);
	newValue = int(round(newValue / 50.0) * 50);
	if (newValue < 50) newValue = 50;
	swprintf(bufferText, L"%d", newValue);
	GetDlgItem(IDD_EDIT_LEFTMOUSE_MS)->SetWindowTextW(bufferText);
	if (newValue != myConfig.leftMouseTime)
	{
		myConfig.leftMouseTime = newValue;
		TRACE("Left Mouse ms %d\r\n", myConfig.leftMouseTime);
		SaveConfig();
	}

}
void CDiabloIIIMarcoDlg::OnKillFocusEditRightMouseMs()
{
	wchar_t bufferText[1000] = { 0 };
	GetDlgItem(IDD_EDIT_RIGHTMOUSE_MS)->GetWindowTextW(bufferText, 999);
	int newValue = 0;
	swscanf(bufferText, L"%d", &newValue);
	newValue = int(round(newValue / 50.0) * 50);
	if (newValue < 50) newValue = 50;
	swprintf(bufferText, L"%d", newValue);
	GetDlgItem(IDD_EDIT_RIGHTMOUSE_MS)->SetWindowTextW(bufferText);
	if (newValue != myConfig.rightMouseTime)
	{
		myConfig.rightMouseTime = newValue;
		TRACE("Right Mouse ms %d\r\n", myConfig.rightMouseTime);
		SaveConfig();
	}
}

void CDiabloIIIMarcoDlg::OnKillFocusEditSkill1Ms()
{
	wchar_t bufferText[1000] = { 0 };
	GetDlgItem(IDD_EDIT_SKILL1)->GetWindowTextW(bufferText, 999);
	int newValue = 0;
	swscanf(bufferText, L"%d", &newValue);
	newValue = int(round(newValue / 50.0) * 50);
	if (newValue < 50) newValue = 50;
	swprintf(bufferText, L"%d", newValue);
	GetDlgItem(IDD_EDIT_SKILL1)->SetWindowTextW(bufferText);
	if (newValue != myConfig.skillSlot01Time)
	{
		myConfig.skillSlot01Time = newValue;
		TRACE("Right Mouse ms %d\r\n", myConfig.skillSlot01Time);
		SaveConfig();
	}
}


void CDiabloIIIMarcoDlg::OnKillFocusEditSkill2Ms()
{
	wchar_t bufferText[1000] = { 0 };
	GetDlgItem(IDD_EDIT_SKILL2)->GetWindowTextW(bufferText, 999);
	int newValue = 0;
	swscanf(bufferText, L"%d", &newValue);
	newValue = int(round(newValue / 50.0) * 50);
	if (newValue < 50) newValue = 50;
	swprintf(bufferText, L"%d", newValue);
	GetDlgItem(IDD_EDIT_SKILL2)->SetWindowTextW(bufferText);
	if (newValue != myConfig.skillSlot02Time)
	{
		myConfig.skillSlot02Time = newValue;
		TRACE("Right Mouse ms %d\r\n", myConfig.skillSlot02Time);
		SaveConfig();
	}
}
void CDiabloIIIMarcoDlg::OnKillFocusEditSkill3Ms()
{
	wchar_t bufferText[1000] = { 0 };
	GetDlgItem(IDD_EDIT_SKILL3)->GetWindowTextW(bufferText, 999);
	int newValue = 0;
	swscanf(bufferText, L"%d", &newValue);
	newValue = int(round(newValue / 50.0) * 50);
	if (newValue < 50) newValue = 50;
	swprintf(bufferText, L"%d", newValue);
	GetDlgItem(IDD_EDIT_SKILL3)->SetWindowTextW(bufferText);
	if (newValue != myConfig.skillSlot03Time)
	{
		myConfig.skillSlot03Time = newValue;
		TRACE("Right Mouse ms %d\r\n", myConfig.skillSlot03Time);
		SaveConfig();
	}
}

void CDiabloIIIMarcoDlg::OnKillFocusEditSkill4Ms()
{
	wchar_t bufferText[1000] = { 0 };
	GetDlgItem(IDD_EDIT_SKILL4)->GetWindowTextW(bufferText, 999);
	int newValue = 0;
	swscanf(bufferText, L"%d", &newValue);
	newValue = int(round(newValue / 50.0) * 50);
	if (newValue < 50) newValue = 50;
	swprintf(bufferText, L"%d", newValue);
	GetDlgItem(IDD_EDIT_SKILL4)->SetWindowTextW(bufferText);
	if (newValue != myConfig.skillSlot04Time)
	{
		myConfig.skillSlot04Time = newValue;
		TRACE("Right Mouse ms %d\r\n", myConfig.skillSlot04Time);
		SaveConfig();
	}
}

void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill1()
{
	myConfig.skill01Enable = !myConfig.skill01Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL1))->SetCheck(myConfig.skill01Enable);
	GetDlgItem(IDD_EDIT_SKILL1)->EnableWindow(myConfig.skill01Enable);
	SaveConfig();
}
void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill2()
{
	myConfig.skill02Enable = !myConfig.skill02Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL2))->SetCheck(myConfig.skill02Enable);
	GetDlgItem(IDD_EDIT_SKILL2)->EnableWindow(myConfig.skill02Enable);
	SaveConfig();
}

void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill3()
{
	myConfig.skill03Enable = !myConfig.skill03Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL3))->SetCheck(myConfig.skill03Enable);
	GetDlgItem(IDD_EDIT_SKILL3)->EnableWindow(myConfig.skill03Enable);
	SaveConfig();
}
void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill4()
{
	myConfig.skill04Enable = !myConfig.skill04Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL4))->SetCheck(myConfig.skill04Enable);
	GetDlgItem(IDD_EDIT_SKILL4)->EnableWindow(myConfig.skill04Enable);
	SaveConfig();
}
