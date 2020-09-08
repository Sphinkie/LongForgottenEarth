/* ------------------------------------------------------------------------ */
// File:        config.h
// Project:		Long Forgotten Earth
// Description: Programme de configuration Audio
// Author:      David de Lorenzo
/* ------------------------------------------------------------------------ */

#if !defined(_AFX_CONFIG_H_)
#define _AFX_CONFIG_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/* ------------------------------------------------------------------------ */
// CConfigApp:
/* ------------------------------------------------------------------------ */
class CConfigApp : public CWinApp
{
public:
	CConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CConfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIG_H__5B7C0E2F_2F76_4A6D_A18B_2610C3BE4567__INCLUDED_)
