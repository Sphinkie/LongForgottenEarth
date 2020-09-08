/* ------------------------------------------------------------------------- */
// File       : Computer_Control.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _COMPUTER_CONTROL_H_
#define _COMPUTER_CONTROL_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Interface.h"
#include "Computer.h"
#include "ShipElement.h"
#include "ShipEngine.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère l'interface GUI du Computer de Controle, ainsi que les fonctionnalités et données spécifiques à cet ordinateur.
/* ------------------------------------------------------------------------- */
class Computer_Control : public Computer
{

public:
	Computer_Control(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Computer_Control();

protected:
	// fonctions specifiques du GUI
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order) {};
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order);
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order) {};

	void     executeOrder(String Order);
	void     showColouredLabels(int Order);

	// Fonctions de gestion des états du vaisseau
	void     loadShipStatus();
	String   getStatusInfo(int index);
	String   analyseStatus(String Status);
	String   getDamagedElements(String Status);
	String   formatElementData(ShipElement* Element, CEGUI::StaticText* pText);
	String   formatEngineData (ShipEngine*  Element, CEGUI::StaticText* pText);
	String   formatLogEntry(String LogEntry, String LogDate, CEGUI::StaticText* pText);


	ShipElement*       mCurrentElement;
	CEGUI::StaticText* mAnnexText;
};

#endif
