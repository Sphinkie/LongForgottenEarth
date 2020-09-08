/* ------------------------------------------------------------------------- */
// File       : Computer_Logs.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _COMPUTER_LOGS_H_
#define _COMPUTER_LOGS_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Interface.h"
#include "Computer.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère l'interface GUI du Computer du capitaine, ainsi que les fonctionnalités et données spécifiques à cet ordinateur (log, etc).
/* ------------------------------------------------------------------------- */
class Computer_Logs : public Computer
{

public:
	Computer_Logs(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Computer_Logs();

protected:
	// fonctions specifiques du GUI
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order) {};
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order) {};
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order) {};
	void     executeOrder(String Order);
	void     createInterface();

	String   formatLogEntry(String LogEntry, String LogDate, CEGUI::StaticText* pText);
	void     refreshGUIWindow();

	String   mNewLogTitle;
	String   mNewLogText;
};

#endif
