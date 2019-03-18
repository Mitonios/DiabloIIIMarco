
// DiabloIIIMarcoDlg.h : header file
//

#pragma once


// CDiabloIIIMarcoDlg dialog
class CDiabloIIIMarcoDlg : public CDialogEx
{
// Construction
public:
	CDiabloIIIMarcoDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIABLOIIIMARCO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	INT_PTR myTimerID;
public:
	afx_msg void OnTimer(UINT_PTR nIdEvent);
	afx_msg void SaveConfig();

	afx_msg void LoadConfig();

	afx_msg void OnKillFocusEditLeftMouseMs();
	afx_msg void OnKillFocusEditRightMouseMs();
	void OnKillFocusEditSkill1Ms();
	void OnKillFocusEditSkill2Ms();
	void OnKillFocusEditSkill3Ms();
	void OnKillFocusEditSkill4Ms();
	afx_msg void OnBnClickedCheckSkill1();
	void OnBnClickedCheckSkill2();
	void OnBnClickedCheckSkill3();
	void OnBnClickedCheckSkill4();
};
