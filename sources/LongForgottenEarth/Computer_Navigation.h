/* ------------------------------------------------------------------------- */
// File       : Computer_Navigation.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _COMPUTER_NAVIGATION_H_
#define _COMPUTER_NAVIGATION_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Interface.h"
#include "Computer.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère l'interface GUI du Computer de Navigation, qui commande les deux moteurs du vaisseau spatial.
/* ------------------------------------------------------------------------- */
class Computer_Navigation : public Computer
{

public:
	Computer_Navigation(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Computer_Navigation();

protected:
	// fonctions specifiques du GUI
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order);
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order);
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order);
	void     executeOrder(String Order);
	bool     frameStarted(const FrameEvent& evt);
	void     createInterface();
	void     deleteInterface();

	int      mSelectedMode;        /// Mémorisation du mode sélectionné, avant qu'il soit affecté aux moteurs.
	String   mModeText;            /// Description du mode sélectionné
	Real     mRefreshTimer;        /// Variable pour refrachir certains éléments de la GUI de temps en temps.
	Real     mThrustSlider;        /// Position du curseur du slider de puissance	
};

#endif
