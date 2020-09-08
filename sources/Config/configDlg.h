/* ------------------------------------------------------------------------ */
// File:        configDlg.h
// Project:		Long Forgotten Earth
// Description: Programme de configuration Audio
// Author:      David de Lorenzo
/* ------------------------------------------------------------------------ */

#if !defined(AFX_CONFIGDLG_H__13905470_B2D6_4A5E_869E_DE40FBB10163__INCLUDED_)
#define AFX_CONFIGDLG_H__13905470_B2D6_4A5E_869E_DE40FBB10163__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 




/* ------------------------------------------------------------------------ */
// CConfigDlg dialog
/* ------------------------------------------------------------------------ */

class CConfigDlg : public CDialog
{
public:
	CConfigDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_CONFIG_DIALOG };
	int		m_Output;
	int		m_Driver;
	int		m_Langue;
	CString	m_AudioInfo;
	int		m_Shadows;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	HICON   m_hIcon;
	CString	m_OutputInfo;
	int		m_OutputSetting;

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeOutput();
	afx_msg void OnSelchangeDriver();
	virtual void OnOK();
	afx_msg void OnSelchangeLangue();
	afx_msg void OnSelchangeShadows();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__13905470_B2D6_4A5E_869E_DE40FBB10163__INCLUDED_)
