/* ------------------------------------------------------------------------- */
// File       : Computer_Base.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Computer_Base.h"

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIBase.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include "CEGUI/elements/CEGUIStaticImage.h"
#include "CEGUI/elements/CEGUIStaticText.h"
#include "CEGUI/elements/CEGUIListbox.h"
#include <OgreMemoryMacros.h>


#include "macros.h"
#include "Clock.h"



/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Computer_Base::Computer_Base(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr) 
	: Computer(MeshFile, DescriptionFile, ParentNode, SceneMgr)
{
	// Le constructeur de la surclasse COMPUTER s'exécute automatiquement

	TRACE(">> Computer_Base()");
	mGUIname = "BasicComputer";
	mSelectedOrder = -1;
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Computer_Base::~Computer_Base()
{
	// Le destructeur de la surclasse COMPUTER s'exécute automatiquement
}


/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Computer_Base::executeOrder(String Order)
{
	// On extrait les paramètres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");

	// *****************************************
	// Ordre de quitter le GUI
	// *****************************************
	if (FuncV.at(0)=="exitGUI")
	{
		mSelectedOrder = -1;
		// On supprime l'interface.GUI
		this->requestExitInterface(0);
	}
	// *****************************************
	// Ordre de quitter la fenetre annexe
	// *****************************************
	if (FuncV.at(0)=="close")
	{
		mSelectedOrder = -1;
		this->removeAnnexWindow();
		this->refreshTexts("mainWin");
		this->refreshListboxes("mainWin");
	}
	// *****************************************
	// Ordre d'exécuter le test 0
	// *****************************************
	else if (FuncV.at(0)=="test0")
	{
		mSelectedOrder=0;
		this->refreshTexts("mainWin");
	}
	// *****************************************
	// Ordre d'exécuter le test 1
	// *****************************************
	else if (FuncV.at(0)=="test1")
	{
		mSelectedOrder=1;
		this->refreshTexts("mainWin");
	}
	// *****************************************
	// Creation d'une fenetre annexe
	// *****************************************
	else if (FuncV.at(0)=="addWindow")
	{
		// Conditions d'apparition:
		if (mAnnexWindow==NULL)
		{
			// On cree la fenetre demandée dans le premier parametre
			this->addGUIWindow(FuncV.at(1));
			// On met a jour les textes et images de la fenetre annexe
			this->refreshTexts(FuncV.at(1));
			this->refreshImages(FuncV.at(1));	
		}
	}
}



/* ------------------------------------------------------------------------- */
/// On affiche le texte demandé (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Computer_Base::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	// *****************************************
	// Cas du Computer en panne
	// *****************************************
	if (this->isBroken())
	{
		pText->setHorizontalFormatting(CEGUI::StaticText::RightAligned);
		pText->setVerticalFormatting(CEGUI::StaticText::BottomAligned);
		pText->setTextColours(mRed);
		pText->setText("Malfunction");
	}
	else
	{
		// *****************************************
		// Cas du Computer nominal
		// *****************************************
		if (Order=="@data")
		{	
			switch (mSelectedOrder) 
			{
			case 0: 
				pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
				pText->setText("Test de Normal Map #0"); 
				break;             
			case 1: 
				pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
				pText->setText(" Control Computer \nCurrent date \nday: "+ITOA(Clock::getSingletonPtr()->getCurrentDate()));
				break;  
			default:
				pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
				pText->setText("Ici on fait des tests de Normal Mapping..."); 
			}
		}
	}
}

