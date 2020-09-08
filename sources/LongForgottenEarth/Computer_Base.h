/* ------------------------------------------------------------------------- */
// File       : Computer_Base.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _COMPUTER_BASE_H_
#define _COMPUTER_BASE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Interface.h"
#include "Computer.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère l'interface GUI du Computer de Base, ainsi que les fonctionnalités et données spécifiques à cet ordinateur.
/* ------------------------------------------------------------------------- */
class Computer_Base : public Computer
{

public:
	Computer_Base(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Computer_Base();

protected:
	// fonctions specifiques du GUI
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order) {};
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order) {};
	void     executeOrder(String Order);

};

#endif
