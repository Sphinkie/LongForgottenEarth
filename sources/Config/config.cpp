/* ------------------------------------------------------------------------ */
// File:        config.cpp
// Project:		Long Forgotten Earth
// Description: Programme de configuration Audio
// Author:      David de Lorenzo
/* ------------------------------------------------------------------------ */

#include "stdafx.h"
#include "config.h"
#include "configDlg.h"
#include "process.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------------ */
// CConfigApp
/* ------------------------------------------------------------------------ */
BEGIN_MESSAGE_MAP(CConfigApp, CWinApp)
	//{{AFX_MSG_MAP(CConfigApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


/* ------------------------------------------------------------------------ */
// CConfigApp construction
/* ------------------------------------------------------------------------ */
CConfigApp::CConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CConfigApp object
CConfigApp theApp;


/* ------------------------------------------------------------------------ */
// CConfigApp initialization
/* ------------------------------------------------------------------------ */
BOOL CConfigApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CConfigDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  When the dialog is dismissed with OK:
		//  we launch the setup-ogre application.
		execl("setupVideo.exe",NULL);
	}
	else if (nResponse == IDCANCEL)
	{
		// when the dialog is dismissed with Cancel:
		// Nothing to do
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}

