
// DiabloIIIMarcoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiabloIIIMarco.h"
#include "DiabloIIIMarcoDlg.h"
#include "afxdialogex.h"
#include "Trace.h"
#include <Windows.h>

//#ifdef _debug
//#define new debug_new
//#endif

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
bool					flagOnF4 = false;
bool					flagOnF5 = false;
int						leftMouseCooldown;
int						rightMouseCooldown;
int						skillSlot01Cooldown;
int						skillSlot02Cooldown;
int						skillSlot03Cooldown;
int						skillSlot04Cooldown;
wchar_t					keySKill01 = 0x31;
wchar_t					keySKill02 = 0x32;
wchar_t					keySKill03 = 0x33;
wchar_t					keySKill04 = 0x34;
HHOOK					hGlobalHook;

bool		IsD3WindowActive(void)
{
	HWND		currentHWD = GetForegroundWindow();
	if (currentHWD)
	{
		wchar_t		buffer[1024] = { 0 };
		GetWindowTextW(currentHWD, buffer, 1023);
		if (wcscmp(buffer, L"Diablo III") == 0)
		{
			return true;
		}
	}
	return false;
}
HWND		GetD3Windows(void)
{
	return FindWindowW(L"D3 Main Window Class", L"Diablo III");
	//return FindWindowW(L"Notepad", L"Untitled - Notepad");
}
void		SendD3Key(int keyCode)
{
	HWND d3Wnd = GetD3Windows();
	if (d3Wnd)
	{
		SendMessage(d3Wnd, WM_KEYDOWN, keyCode, 0);
		Sleep(5 + (rand() % 3));
		SendMessage(d3Wnd, WM_KEYUP, keyCode, 0);
		Sleep(5 + (rand() % 3));
	}
}
void		SetD3Mouse(int x, int y)
{
	HWND d3Wnd = GetD3Windows();
	if (d3Wnd)
	{
		RECT d3Rect = { 0 };
		GetWindowRect(d3Wnd, &d3Rect);
		SetCursorPos(d3Rect.left + x, d3Rect.top + y);
	}
}
COLORREF getColor(int x, int y) {
	HDC dc = GetDC(NULL);
	COLORREF color = GetPixel(dc, x, y);
	ReleaseDC(NULL, dc);
	return color;
}

void		SendD3LeftMouseClick()
{
	HWND d3Wnd = GetD3Windows();
	if (d3Wnd)
	{
		POINT point = { 0 };
		GetCursorPos(&point);

		RECT d3Rect = { 0 };
		GetWindowRect(d3Wnd, &d3Rect);

		LPARAM lParam = (point.x - d3Rect.left) | ((point.y - d3Rect.top) << 16);

		SendMessage(d3Wnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
		Sleep(5 + (rand() % 3));
		SendMessage(d3Wnd, WM_LBUTTONUP, 0, lParam);
		Sleep(5 + (rand() % 3));
	}
}
void		SendD3RightMouseClick()
{
	HWND d3Wnd = GetD3Windows();
	if (d3Wnd)
	{
		POINT point = { 0 };
		GetCursorPos(&point);

		RECT d3Rect = { 0 };
		GetWindowRect(d3Wnd, &d3Rect);

		LPARAM lParam = (point.x - d3Rect.left) | ((point.y - d3Rect.top) << 16);

		SendMessage(d3Wnd, WM_RBUTTONDOWN, MK_RBUTTON, lParam);
		Sleep(5 + (rand() % 3));
		SendMessage(d3Wnd, WM_RBUTTONUP, 0, lParam);
		Sleep(5 + (rand() % 3));
	}
}
bool		PointInRect(POINT point, int rLeft, int rRight, int rTop, int rBottom)
{
	if (rLeft >= rRight || rTop >= rBottom)
	{
		MessageBox(0, L"D3 Engine Error!!", L"(rLeft <= rRight || rTop <= rBottom)", MB_OK);
	}
	return (point.x > rLeft && point.x < rRight && point.y > rTop && point.y < rBottom);
}

bool		ValidToSendD3Click(void)
{
	if (IsD3WindowActive())
	{
		HWND d3Wnd = GetD3Windows();
		if (d3Wnd)
		{
			RECT d3Rect = { 0 };
			GetWindowRect(d3Wnd, &d3Rect);
			if (d3Rect.right > d3Rect.left && d3Rect.bottom > d3Rect.top)
			{
				POINT point = { 0 };
				GetCursorPos(&point);
				if (PointInRect(point, d3Rect.left, d3Rect.right, d3Rect.top, d3Rect.bottom))
				{
					if (d3Rect.right - d3Rect.left == 1920 && d3Rect.bottom - d3Rect.top == 1080)
					{
						//chat button
						if (PointInRect(point, d3Rect.left + 10, d3Rect.left + 80, d3Rect.top + 980, d3Rect.top + 1044))
						{
							return false;
						}

						//Skill, Inventory,...
						if (PointInRect(point, d3Rect.left + 1089, d3Rect.left + 1284, d3Rect.top + 995, d3Rect.top + 1062))
						{
							return false;
						}

						//Friend Button
						if (PointInRect(point, d3Rect.left + 1764, d3Rect.left + 1904, d3Rect.top + 979, d3Rect.top + 1044))
						{
							return false;
						}

						//Small chat box
						if (PointInRect(point, d3Rect.left + 32, d3Rect.left + 346, d3Rect.top + 738, d3Rect.top + 877))
						{
							return false;
						}

						//Objectives Object
						if (PointInRect(point, d3Rect.left + 1862, d3Rect.left + 1893, d3Rect.top + 367, d3Rect.top + 391))
						{
							return false;
						}

						//Main-Player
						if (PointInRect(point, d3Rect.left + 28, d3Rect.left + 93, d3Rect.top + 47, d3Rect.top + 149))
						{
							return false;
						}
					}


					return true;
				}
			}
		}
	}
	return false;
}

bool CheckNullInventorySlot(COLORREF color) {
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);
	if (r >= 12 && r <= 17) {
		if (g >= 12 && g <= 17) {
			if (b >= 6 && b <= 13) {
				return true;
			}
		}
	}
	return false;
}

void ClickOkButton(double d3Scale) {
	int x = (int)(754.0*d3Scale);
	int y = (int)(373.0*d3Scale);
	COLORREF color = getColor(x, y);
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);
	TRACE(_T("Button Color R:%d,G:%d,B:%d \r\n"), r, g, b);
	if (r >= 35 && r <= 48) {
		if (g >= 5 && g <= 10) {
			if (b >= 0 && b <= 5) {
				//SetD3Mouse(x, y);
				//SendD3LeftMouseClick();
				SendD3Key(VK_RETURN);
				Sleep(50 + (rand() % 5));
				SendD3Key(VK_RETURN);
				Sleep(50 + (rand() % 5));
			}
		}
	}
}

void		ValidateConfig(void)
{
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
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
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

extern "C" __declspec(dllexport) LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	bool		flagNeedMoreHook = true;
	if (nCode >= 0 && nCode == HC_ACTION)
	{
		LPKBDLLHOOKSTRUCT keyParam = (LPKBDLLHOOKSTRUCT)(void*)lParam;
		if (wParam == WM_KEYUP)
		{
			switch (keyParam->vkCode)
			{
			case VK_F1:
				if (IsD3WindowActive()) {
					flagNeedMoreHook = false;
				}
				break;
			}
		}
		else if (wParam == WM_KEYDOWN)
		{
			switch (keyParam->vkCode)
			{
			case VK_F1:
				flagNeedMoreHook = false;
				flagOnF1 = !flagOnF1;
				leftMouseCooldown = 99999;
				break;
			case VK_F2:
				flagOnF2 = !flagOnF2;
				skillSlot01Cooldown = 99999;
				skillSlot02Cooldown = 99999;
				skillSlot03Cooldown = 99999;
				skillSlot04Cooldown = 99999;
				break;
			case VK_F3:
				flagOnF3 = !flagOnF3;
				rightMouseCooldown = 99999;
				break;
			case VK_F4:
				TRACE(_T("Salvage\r\n"));
				flagOnF4 = !flagOnF4;
				break;
			case VK_F5:
				TRACE(_T("Check Mouse xy\r\n"));
				flagOnF5 = !flagOnF5;
				break;
			}
		}
	}
	if (flagNeedMoreHook == false)
	{
		return 1;
	}
	return CallNextHookEx(hGlobalHook, nCode, wParam, lParam);
}
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

	hGlobalHook = SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, GetModuleHandle(NULL), 0);
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
	if (myTimerID == nIdEvent)
	{
		HWND d3Wnd = GetD3Windows();
		RECT d3Rect = { 0 };
		if (d3Wnd == 0)
		{
			flagOnF1 = false;
			flagOnF2 = false;
			flagOnF3 = false;
		}
		else {
			::GetWindowRect(d3Wnd, &d3Rect);
		}
		POINT point = { 0 };

		GetDlgItem(IDD_EDIT_LEFTMOUSE_MS)->EnableWindow(!flagOnF1);
		GetDlgItem(IDC_STATIC_LEFTMOUSE)->ShowWindow(flagOnF1);

		GetDlgItem(IDC_STATIC_AUTOBUTTON)->ShowWindow(flagOnF2);
		GetDlgItem(IDC_CHECK_SKILL1)->EnableWindow(!flagOnF2);
		GetDlgItem(IDC_CHECK_SKILL2)->EnableWindow(!flagOnF2);
		GetDlgItem(IDC_CHECK_SKILL3)->EnableWindow(!flagOnF2);
		GetDlgItem(IDC_CHECK_SKILL4)->EnableWindow(!flagOnF2);
		GetDlgItem(IDD_EDIT_SKILL1)->EnableWindow(!flagOnF2&&myConfig.skill01Enable);
		GetDlgItem(IDD_EDIT_SKILL2)->EnableWindow(!flagOnF2&&myConfig.skill02Enable);
		GetDlgItem(IDD_EDIT_SKILL3)->EnableWindow(!flagOnF2&&myConfig.skill03Enable);
		GetDlgItem(IDD_EDIT_SKILL4)->EnableWindow(!flagOnF2&&myConfig.skill04Enable);

		GetDlgItem(IDD_EDIT_RIGHTMOUSE_MS)->EnableWindow(!flagOnF3);
		GetDlgItem(IDC_STATIC_RIGHTMOUSE)->ShowWindow(flagOnF3);

		if (flagOnF1) {
			leftMouseCooldown += timerDelay;
			if (leftMouseCooldown >= myConfig.leftMouseTime)
			{
				if (ValidToSendD3Click()) SendD3LeftMouseClick();
				leftMouseCooldown = 0;
			}
		}

		if (flagOnF3) {
			rightMouseCooldown += timerDelay;
			if (rightMouseCooldown >= myConfig.rightMouseTime)
			{
				if (ValidToSendD3Click()) SendD3RightMouseClick();
				rightMouseCooldown = 0;
			}
		}
		if (flagOnF2) {
			if (myConfig.skill01Enable) {
				skillSlot01Cooldown += timerDelay;
				if (skillSlot01Cooldown >= myConfig.skillSlot01Time)
				{
					SendD3Key(keySKill01);
					skillSlot01Cooldown = 0;
				}
			}
			if (myConfig.skill02Enable) {
				skillSlot02Cooldown += timerDelay;
				if (skillSlot02Cooldown >= myConfig.skillSlot02Time)
				{
					SendD3Key(keySKill02);
					skillSlot02Cooldown = 0;
				}
			}
			if (myConfig.skill03Enable) {
				skillSlot03Cooldown += timerDelay;
				if (skillSlot03Cooldown >= myConfig.skillSlot03Time)
				{
					SendD3Key(keySKill03);
					skillSlot03Cooldown = 0;
				}
			}
			if (myConfig.skill04Enable) {
				skillSlot04Cooldown += timerDelay;
				if (skillSlot04Cooldown >= myConfig.skillSlot04Time)
				{
					SendD3Key(keySKill04);
					skillSlot04Cooldown = 0;
				}
			}
		}
		int		d3Width = d3Rect.right - d3Rect.left;
		int		d3Height = d3Rect.bottom - d3Rect.top;
		double		d3Scale = d3Height / 1080.0;
		int xCenter = int(d3Width / 2);
		int yCenter = int(d3Height / 2);
		//Khởi tạo tọa độ hòm đồ
		int			xIventoryArray[60] = { 0 };
		int			yIventoryArray[60] = { 0 };
		double		xCubeLeftPage = 583 * d3Scale;
		double		yCubeLeftPage = 840 * d3Scale;
		double		xCubeRightPage = 852 * d3Scale;
		double		yCubeRightPage = 840 * d3Scale;
		if (xIventoryArray[0] == 0 && yIventoryArray[0] == 0)
		{
			double		xInventory = (d3Width - (1920.0 - 1423.0) * d3Scale);
			double		yInventory = 583 * d3Scale;
			double		wIventory = 500.0 * d3Scale;
			double		hIventory = 296.0 * d3Scale;
			double		wSlot = wIventory / 10.0;
			double		hSlot = hIventory / 6.0;
			for (int icolumn = 0; icolumn < 10; icolumn++)
			{
				int currentX = (int)round(xInventory + wSlot * icolumn);
				for (int irow = 0; irow < 6; irow++)
				{
					xIventoryArray[irow * 10 + icolumn] = currentX;
					yIventoryArray[irow * 10 + icolumn] = (int)round(yInventory + hSlot * irow);
				}
			}
		}
		//END-Khởi tạo tọa độ hòm đồ
		if (flagOnF4) {
			int xTabSalvage = (int)round(518.0 * d3Scale);
			int yTabSalvage = (int)round(486.0 * d3Scale);


			//Stash
			//TRACE(_T("d3Width: %d, d3Height: %d, d3Scale: %d \n"), d3Width, d3Height, d3Scale);

			//Chuyển qua Tab Salvage
			SetD3Mouse(xTabSalvage, yTabSalvage);
			SendD3LeftMouseClick();

			GetCursorPos(&point);
			COLORREF cl = getColor(point.x, point.y);
			//TRACE(_T("Color x: %d, y: %d, R: %d, G: %d, B: %d \n"), point.x, point.y,  GetRValue(cl), GetGValue(cl), GetBValue(cl));

			if (GetRValue(cl) == 252 && GetGValue(cl) == 195 && GetBValue(cl) == 21) {
				//Đúng màu thì làm tiếp
				int			xIventoryProcess[60] = { 0 };
				int			yIventoryProcess[60] = { 0 };
				bool		process = false;
				int xSalvage = (int)round(170.0 * d3Scale);
				int ySalvage = (int)round(290.0 * d3Scale);
				//Sal các loại vàng xanh xám
				int xSalvageGold = (int)round(381.0 * d3Scale);
				if (flagOnF4) SetD3Mouse(xSalvageGold, ySalvage);
				if (flagOnF4) SendD3LeftMouseClick();
				if (flagOnF4) Sleep(50 + (rand() % 5));
				if (flagOnF4) ClickOkButton(d3Scale);

				int xSalvageBlue = (int)round(318.0 * d3Scale);
				if (flagOnF4) SetD3Mouse(xSalvageBlue, ySalvage);
				if (flagOnF4) SendD3LeftMouseClick();
				if (flagOnF4) Sleep(50 + (rand() % 5));
				if (flagOnF4) ClickOkButton(d3Scale);

				int xSalvageGray = (int)round(294.0 * d3Scale);
				if (flagOnF4) SetD3Mouse(xSalvageGray, ySalvage);
				if (flagOnF4) SendD3LeftMouseClick();
				if (flagOnF4) Sleep(50 + (rand() % 5));
				if (flagOnF4) ClickOkButton(d3Scale);
				//Kiểm tra những ô có item
				for (int iitem = 0; iitem < 60; iitem++)
				{
					//Kiểm tra những ô có item
					cl = getColor(xIventoryArray[iitem], yIventoryArray[iitem]);
					//SetD3Mouse(xIventoryArray[iitem], yIventoryArray[iitem]);
					if (!CheckNullInventorySlot(cl)) {
						xIventoryProcess[iitem] = xIventoryArray[iitem];
						yIventoryProcess[iitem] = yIventoryArray[iitem];
						process = true;
						//TRACE(_T("Color x: %d, y: %d, R: %d, G: %d, B: %d \n"), xIventoryArray[iitem], yIventoryArray[iitem], GetRValue(cl), GetGValue(cl), GetBValue(cl));
					}
				}
				//Xử lý những ô có item
				if (process == true) {
					//Sal các loại Legend
					if (flagOnF4) SetD3Mouse(xSalvage, ySalvage);
					if (flagOnF4) SendD3LeftMouseClick();
					if (flagOnF4) Sleep(50 + (rand() % 5));
					for (int iitem = 0; iitem < 60; iitem++)
					{
						if (xIventoryProcess[iitem] != 0 && xIventoryProcess[iitem] != 0) {
							if (flagOnF4) SetD3Mouse(xIventoryArray[iitem], yIventoryArray[iitem]);
							if (flagOnF4) SendD3LeftMouseClick();
							if (flagOnF4) Sleep(50 + (rand() % 5));
							if (flagOnF4) ClickOkButton(d3Scale);
						}
					}
				}
				//Xử lý xong sửa đồ
				int xRepairTable = 517 * d3Scale;
				int yRepairTable = 620 * d3Scale;

				int xRepairButton = 264 * d3Scale;
				int yRepairButton = 594 * d3Scale;
				if (flagOnF4) SetD3Mouse(xRepairTable, yRepairTable);
				if (flagOnF4) SendD3LeftMouseClick();
				if (flagOnF4) SetD3Mouse(xRepairButton, yRepairButton);
				if (flagOnF4) SendD3LeftMouseClick();
			}
			flagOnF4 = false;
		}
		if (flagOnF5) {
			int x = (int)(754.0*d3Scale);
			int y = (int)(373.0*d3Scale);
			GetCursorPos(&point);
			COLORREF cl = getColor(x, y);
			TRACE(_T("Color x: %d, y: %d, R: %d, G: %d, B: %d \n"), point.x, point.y, GetRValue(cl), GetGValue(cl), GetBValue(cl));
			flagOnF5 = false;
		}
	}
}



void CDiabloIIIMarcoDlg::SaveConfig()
{
	CFile saveFile;
	if (saveFile.Open(configSavePath, CFile::modeCreate | CFile::modeWrite))
	{
		saveFile.Write(&myConfig, sizeof(myConfig));
		saveFile.Close();
	}
}

void CDiabloIIIMarcoDlg::LoadConfig()
{
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
		SaveConfig();
	}
}

void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill1()
{
	myConfig.skill01Enable = !myConfig.skill01Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL1))->SetCheck(myConfig.skill01Enable);
	GetDlgItem(IDD_EDIT_SKILL1)->EnableWindow(myConfig.skill01Enable);
	GetDlgItem(IDD_EDIT_SKILL1)->SetFocus();
	SaveConfig();
}
void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill2()
{
	myConfig.skill02Enable = !myConfig.skill02Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL2))->SetCheck(myConfig.skill02Enable);
	GetDlgItem(IDD_EDIT_SKILL2)->EnableWindow(myConfig.skill02Enable);
	GetDlgItem(IDD_EDIT_SKILL2)->SetFocus();
	SaveConfig();
}

void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill3()
{
	myConfig.skill03Enable = !myConfig.skill03Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL3))->SetCheck(myConfig.skill03Enable);
	GetDlgItem(IDD_EDIT_SKILL3)->EnableWindow(myConfig.skill03Enable);
	GetDlgItem(IDD_EDIT_SKILL3)->SetFocus();
	SaveConfig();
}
void CDiabloIIIMarcoDlg::OnBnClickedCheckSkill4()
{
	myConfig.skill04Enable = !myConfig.skill04Enable;
	((CButton*)GetDlgItem(IDC_CHECK_SKILL4))->SetCheck(myConfig.skill04Enable);
	GetDlgItem(IDD_EDIT_SKILL4)->EnableWindow(myConfig.skill04Enable);
	GetDlgItem(IDD_EDIT_SKILL4)->SetFocus();
	SaveConfig();
}
