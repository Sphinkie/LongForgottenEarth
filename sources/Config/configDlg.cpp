/* ------------------------------------------------------------------------ */
// File:        configDlg.cpp
// Project:		Long Forgotten Earth
// Description: Programme de configuration Audio
// Author:      David de Lorenzo
/* ------------------------------------------------------------------------ */

#include "stdafx.h"
#include "config.h"
#include "configDlg.h"

#include "fmod.h"
#include "fmod_errors.h"  


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/* ------------------------------------------------------------------------ */
/// Constructeur de la fenetre de dialogue.
/* ------------------------------------------------------------------------ */

CConfigDlg::CConfigDlg(CWnd* pParent) : CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	m_Output  = -1;
	m_Driver  = -1;
	m_Langue  = -1;
	m_Shadows = -1;
	m_AudioInfo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


/* ------------------------------------------------------------------------ */
/// Fonction d'echaneg de données entre les variables membres et les listbox.
/* ------------------------------------------------------------------------ */
void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	DDX_CBIndex(pDX, IDC_OUTPUT, m_Output);
	DDX_CBIndex(pDX, IDC_DRIVER, m_Driver);
	DDX_CBIndex(pDX, IDC_LANGUE, m_Langue);
	DDX_Text(pDX, IDC_EDIT1, m_AudioInfo);
	DDX_CBIndex(pDX, IDC_SHADOWS, m_Shadows);
	//}}AFX_DATA_MAP
}


/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_OUTPUT, OnSelchangeOutput)
	ON_CBN_SELCHANGE(IDC_DRIVER, OnSelchangeDriver)
	ON_CBN_SELCHANGE(IDC_LANGUE, OnSelchangeLangue)
	ON_CBN_SELCHANGE(IDC_SHADOWS, OnSelchangeShadows)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/* ------------------------------------------------------------------------ */
/// CConfigDlg message handlers
/* ------------------------------------------------------------------------ */
BOOL CConfigDlg::OnInitDialog()
{
	// Appel de la fonction de la surclasse.
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Add extra initialization here
	m_Output  = 0;		// On initialise l'audio a NO SOUND
	m_Langue  = 0; 		// On initialise la langue a ENGLISH
	m_Shadows = 1; 		// On initialise les ombres a TEXTURE SHADOWS
	UpdateData(FALSE);


	return TRUE;  // return TRUE  unless you set the focus to a control
}


/* ------------------------------------------------------------------------ */
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
/* ------------------------------------------------------------------------ */
void CConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

/* ------------------------------------------------------------------------ */
/// The system calls this to obtain the cursor to display while the user drags the minimized window.
/* ------------------------------------------------------------------------ */
HCURSOR CConfigDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/* ------------------------------------------------------------------------ */
/// ListBox du choix du type de sortie Audio.
/* ------------------------------------------------------------------------ */
void CConfigDlg::OnSelchangeOutput() 
{
	CString msg;
	CString driver;
	CComboBox* pCB;
	unsigned int caps = 0;

	UpdateData(TRUE); 

	// On affecte le choix de output
	switch(m_Output)
	{
	case 0:
		// No Sound
		m_OutputSetting = FSOUND_OUTPUT_NOSOUND;
		FSOUND_SetOutput(m_OutputSetting);	// config WinWM software only
		break;
	case 1:
		// config Direct Sound (hardware)
		m_OutputSetting = FSOUND_OUTPUT_DSOUND;
		FSOUND_SetOutput(m_OutputSetting); 
		break;
	case 2:
		//	Config Windows Media (Software)
		m_OutputSetting = FSOUND_OUTPUT_WINMM;
		FSOUND_SetOutput(m_OutputSetting); 
		break;
	case 3:
		//	Config ASIO (Low Latency)
		m_OutputSetting = FSOUND_OUTPUT_ASIO;
		FSOUND_SetOutput(m_OutputSetting); 
		break;
	default:
		m_OutputSetting = FSOUND_OUTPUT_NOSOUND;
		msg = "Undefined Output Type";
		break;
	}


	// On vérifie qu'il est bien pris en compte
    switch (FSOUND_GetOutput())
    {
        case FSOUND_OUTPUT_NOSOUND: msg = "Output = No Sound"; break;
        case FSOUND_OUTPUT_WINMM:   msg = "Output = Windows Multimedia Waveout (Software)"; break;
        case FSOUND_OUTPUT_DSOUND:  msg = "Output = Direct Sound (Hardware)"; break;
        case FSOUND_OUTPUT_ASIO:    msg = "Output = ASIO sound"; break;
    };

	// On demande la liste des drivers pour cet output
	pCB = (CComboBox*)GetDlgItem(IDC_DRIVER);
	pCB->ResetContent();
	for (int i=0; i < FSOUND_GetNumDrivers(); i++) 
    {
		// On remplit la combo IDC_DRIVER avec la liste des drivers.
        driver = FSOUND_GetDriverName(i);    // Driver names
		pCB->AddString(driver);
    }

	m_OutputInfo = msg;
	m_AudioInfo = m_OutputInfo; 
	UpdateData(FALSE); // IMPORTANT: otherwise the text doesn't show up.

	
}

/* ------------------------------------------------------------------------ */
/// ListBox du choix du driver Audio.
/* ------------------------------------------------------------------------ */
void CConfigDlg::OnSelchangeDriver() 
{
	CString msg="";
    unsigned int caps = 0;

	UpdateData(TRUE);

	// On affecte le driver selectionné
	FSOUND_SetDriver(m_Driver);

	// On affiche les capacité du driver
    FSOUND_GetDriverCaps(FSOUND_GetDriver(), &caps);
        
    if (!caps) 	msg="\r\nThis driver will support software mode only.\r\nIt does not properly support 3D sound hardware.";
    if (caps & FSOUND_CAPS_HARDWARE) msg+="\r\nThis driver supports hardware 3D sound.";
    if (caps & FSOUND_CAPS_EAX2)     msg+="\r\nThis driver supports EAX 2 reverb.";
    if (caps & FSOUND_CAPS_EAX3)     msg+="\r\nThis driver supports EAX 3 reverb.";

	// On met a jour les informations sur le driver audio
	m_AudioInfo = m_OutputInfo + msg;

	// On degrise le bouton OK
	GetDlgItem(IDOK)->EnableWindow(true);

	UpdateData(FALSE); // IMPORTANT: otherwise the value doesn't show up.

}

/* ------------------------------------------------------------------------ */
/// Clic sur le bouton OK
/* ------------------------------------------------------------------------ */
void CConfigDlg::OnOK() 
{

	char* ISO[3] = {"eng\0","fre\0",NULL};

	// On sauve le fichier de configuration 
	FILE* fh;
	
	fh = fopen("longFE.cfg","wt");  // create an empty file (text mode)

	fprintf(fh,"audio_output=%d \n",m_OutputSetting);
	fprintf(fh,"audio_driver=%d \n",m_Driver);
	fprintf(fh,"shadows=%d \n",m_Shadows);
	fprintf(fh,"language=%s",ISO[m_Langue]);
	fclose(fh);

	CDialog::OnOK();
}

/* ------------------------------------------------------------------------ */
/// ListBox du choix du clavier
/* ------------------------------------------------------------------------ */
void CConfigDlg::OnSelchangeLangue() 
{
	// On memorise le choix de type de clavier
	// La liste des langues connues est définie dans le tableau ISO[] de la fonction OnOK().
	UpdateData(TRUE);

}

/* ------------------------------------------------------------------------ */
/// ListBox du choix du type d'ombrages pour le rendu 3D du jeu
/* ------------------------------------------------------------------------ */
void CConfigDlg::OnSelchangeShadows() 
{
	// On memorise le choix de type de clavier
	UpdateData(TRUE);
}
