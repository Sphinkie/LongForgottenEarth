/* ------------------------------------------------------------------------- */
// File       : Computer_Maintenance.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _COMPUTER_MAINTENANCE_H_
#define _COMPUTER_MAINTENANCE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Interface.h"
#include "Computer.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère l'interface GUI du Computer de Maintenance, ainsi que les fonctionnalités et données spécifiques à cet ordinateur.
/* ------------------------------------------------------------------------- */
class Computer_Maintenance : public Computer	
{

public:
	Computer_Maintenance(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Computer_Maintenance();
	
protected:
	typedef struct Spare		/// Cette structure décrit un stock de spareparts
	{
		int    id;
		int    quantity;
		Spare* next;
	} ;

	// fonctions specifiques du GUI
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order) {};
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order) {};
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order) {};
	void     executeOrder(String Order);

	// Ecrans specifiques 
	void     setSpecificMaterial();

	// Fonctions de gestion des spares	
	void     loadStock();
	void     loadShipStatus();
	String   getStatusInfo(int index);
	void     addSpare(Spare* newSpare);
	void     addMissingSpare(int SpareID);
	bool     contains(int SpareID);

	String mShipStatus;
	String mAvailableSP;
	String mMissingSP;
	Spare* mSpareList;
	Spare* mLastSpare;
	Spare* mMissingList;
	Spare* mLastMissing;
	bool   mShipAnalyzed;
};

#endif
