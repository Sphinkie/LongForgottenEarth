/* ------------------------------------------------------------------------- */
// File       : Computer_Logs.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Computer_Logs.h"

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
#include "CEGUI/elements/CEGUIEditbox.h"
#include "CEGUI/elements/CEGUIListbox.h"
#include <OgreMemoryMacros.h>


#include "macros.h"
#include "Clock.h"
#include "TheShip.h"



/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Computer_Logs::Computer_Logs(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr) 
	: Computer(MeshFile, DescriptionFile, ParentNode, SceneMgr)
{
	// Le constructeur de la surclasse COMPUTER s'exécute automatiquement

	TRACE(">> Computer_Logs()");
	mGUIname = "LogsComputer";
	mSelectedOrder = -1;
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Computer_Logs::~Computer_Logs()
{
	// Le destructeur de la surclasse COMPUTER s'exécute automatiquement
}


/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Computer_Logs::executeOrder(String Order)
{
	TRACE(">> executeOrder("+Order+")");
	// On extrait les paramètres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");

	// *****************************************
	// Ordre de quitter le GUI
	// *****************************************
	if (FuncV.at(0)=="exitGUI")
	{
		mSelectedOrder = -1;
		// On supprime l'interface GUI
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
	}
	// *****************************************
	// Login: verification du mot de passe et accès à la fenetre principale
	// *****************************************
	else if (FuncV.at(0)=="login")
	{
		mSelectedOrder=0;

		CEGUI::Editbox* EditBox_0 = (CEGUI::Editbox*)mWinMgr->getWindow("LogsComputer/loginWin/edit/0");
		CEGUI::Editbox* EditBox_1 = (CEGUI::Editbox*)mWinMgr->getWindow("LogsComputer/loginWin/edit/1");
		// On recupère les textes saisis
		String login = EditBox_0->getText().c_str();
		String passw = EditBox_1->getText().c_str();

		// On vérifie qu'ils sont exacts
		if ((login=="assimiev") && (passw=="c"))
		{
			// On efface la fenetre
			this->removeAnnexWindow();
			this->refreshTexts("mainWin");
			mInterfaceWindow->setEnabled(true);
			EditBox_1->setText("");	// on efface le mot de passe
		}
	}
	// *****************************************
	// Affiche le journal de bord
	// *****************************************
	else if (FuncV.at(0)=="showShipLog")
	{
		mSelectedOrder=1;
		TheShip::getSingletonPtr()->mLogBook->gotoFirstItem();
		this->removeAnnexWindow();
		this->addGUIWindow("RWdocWin");
		this->refreshGUIWindow();
	}
	// *****************************************
	// Ordre d'afficher le Log des alertes
	// *****************************************
	else if (FuncV.at(0)=="showAlertLog")
	{
		mSelectedOrder=2;
		TheShip::getSingletonPtr()->mAlertBook->gotoFirstItem();
		// On cree la fenetre 
		this->removeAnnexWindow();
		this->addGUIWindow("ROdocWin");
		this->refreshGUIWindow();
	}
	// *****************************************
	// Affiche le journal des quètes et missions
	// *****************************************
	else if (FuncV.at(0)=="showMissions")
	{
		mSelectedOrder=3;
		TheShip::getSingletonPtr()->mMissionBook->gotoFirstItem();
		this->removeAnnexWindow();
		this->addGUIWindow("ROdocWin");
		this->refreshGUIWindow();
	}
	// *****************************************
	// Affiche les données sur l'équipage et les passsagers
	// *****************************************
	else if (FuncV.at(0)=="showCrewData")
	{
		mSelectedOrder=4;
		TheShip::getSingletonPtr()->mCrewBook->gotoFirstItem();
		this->removeAnnexWindow();
		this->addGUIWindow("ROdocWin");
		this->refreshGUIWindow();
	}
	// *****************************************
	// On se positionne sur le premier element du book
	// *****************************************
	else if (FuncV.at(0)=="showFirst")
	{	
		TheShip::getSingletonPtr()->mAlertBook->gotoFirstItem();
		TheShip::getSingletonPtr()->mLogBook->gotoFirstItem();
		TheShip::getSingletonPtr()->mCrewBook->gotoFirstItem();
		TheShip::getSingletonPtr()->mMissionBook->gotoFirstItem();
		this->refreshGUIWindow();
	}
	// *****************************************
	// *****************************************
	else if (FuncV.at(0)=="showLast")
	{	
		TheShip::getSingletonPtr()->mAlertBook->gotoLastItem();
		TheShip::getSingletonPtr()->mLogBook->gotoLastItem();
		TheShip::getSingletonPtr()->mCrewBook->gotoLastItem();
		TheShip::getSingletonPtr()->mMissionBook->gotoLastItem();
		this->refreshGUIWindow();
	}
	// *****************************************
	// *****************************************
	else if (FuncV.at(0)=="showNext")
	{	
		TheShip::getSingletonPtr()->mAlertBook->gotoNextItem();
		TheShip::getSingletonPtr()->mLogBook->gotoNextItem();
		TheShip::getSingletonPtr()->mCrewBook->gotoNextItem();
		TheShip::getSingletonPtr()->mMissionBook->gotoNextItem();
		this->refreshGUIWindow();
	}
	// *****************************************
	// *****************************************
	else if (FuncV.at(0)=="showPrev")
	{	
		TheShip::getSingletonPtr()->mAlertBook->gotoPreviousItem();
		TheShip::getSingletonPtr()->mLogBook->gotoPreviousItem();
		TheShip::getSingletonPtr()->mCrewBook->gotoPreviousItem();
		TheShip::getSingletonPtr()->mMissionBook->gotoPreviousItem();
		this->refreshGUIWindow();
	}
	// *****************************************
	// On ajoute un nouveau record 
	// *****************************************
	if (FuncV.at(0)=="add")
	{
		mSelectedOrder = 5;
		// On cree la fenetre 
		this->removeAnnexWindow();
		this->addGUIWindow("EditWin");
		mWinMgr->getWindow("LogsComputer/EditWin/edit/0")->setText(" Date: day "+StringConverter::toString(Clock::getSingletonPtr()->getCurrentDate())+" ");
		mWinMgr->getWindow("LogsComputer/EditWin/edit/1")->setText("");
		this->refreshGUIWindow();
	}
	// *****************************************
	// On abandonne le record en cours d'édition
	// *****************************************
	if (FuncV.at(0)=="discard")
	{
		// On revient à la fenetre précédente
		this->removeAnnexWindow();
		mSelectedOrder = 1;
		this->addGUIWindow("RWdocWin");
		this->refreshGUIWindow();
	}
	// *****************************************
	// On enregistre le record en cours
	// *****************************************
	if (FuncV.at(0)=="save")
	{
		CEGUI::Editbox* eBox_0 = (CEGUI::Editbox*)mWinMgr->getWindow("LogsComputer/EditWin/edit/0");
		CEGUI::Editbox* eBox_1 = (CEGUI::Editbox*)mWinMgr->getWindow("LogsComputer/EditWin/edit/1");

		// On recupère les textes saisis
		mNewLogTitle= eBox_0->getText().c_str();
		mNewLogText = eBox_1->getText().c_str();
		// On les stocke dans le journal
		TheShip::getSingletonPtr()->mLogBook->addItem(mNewLogTitle,mNewLogText);
		TheShip::getSingletonPtr()->mLogBook->saveObject();
		// On revient à la fenetre précédente
		this->removeAnnexWindow();
		mSelectedOrder = 1;
		this->addGUIWindow("RWdocWin");
		this->refreshGUIWindow();
	}
	// *****************************************
	// On efface le record en cours
	// *****************************************
	if (FuncV.at(0)=="erase")
	{
		// Suivant l'action en cours, ce n'est pas le même type de document qu'il faut traiter
		switch(mSelectedOrder) 
		{
		case 1:	TheShip::getSingletonPtr()->mLogBook->removeCurrentItem();     break;
		case 2:	TheShip::getSingletonPtr()->mAlertBook->removeCurrentItem();   break;
		case 3:	TheShip::getSingletonPtr()->mMissionBook->removeCurrentItem(); break;
		case 4:	TheShip::getSingletonPtr()->mCrewBook->removeCurrentItem();    break;
		}		
		this->refreshGUIWindow();
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
void Computer_Logs::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	TRACE (">> showDynamicText");
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
		// Creation de la fenetre de login
		// *****************************************
		if (Order=="@login")
		{	
			pText->setText("Sphinx SpaceShip Captain Computer"); 
		}
		// *****************************************
		// Edition d'un nouveau log: titre
		// *****************************************
		else if (Order=="@newtitle")
		{	
			pText->setFormatting(CEGUI::StaticText::WordWrapLeftAligned, CEGUI::StaticText::TopAligned);
			mNewLogTitle = " Date: day ";
			mNewLogTitle += StringConverter::toString(Clock::getSingletonPtr()->getCurrentDate());
			pText->setText(mNewLogTitle); 
		}
		// *****************************************
		// Edition d'un nouveau log: texte
		// *****************************************
		else if (Order=="@newtext")
		{	
			pText->setFormatting(CEGUI::StaticText::WordWrapLeftAligned, CEGUI::StaticText::TopAligned);
			mNewLogText= "";
			pText->setText(mNewLogText); 
		}
		// *****************************************
		// Affichage du document demandé
		// *****************************************
		else if (Order=="@doc")
		{	
			String text;
			switch (mSelectedOrder) 
			{
			case 0: 
				pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
				pText->setText(""); 
				break;             
			case 1:  // LogBook
				{
				String LogEntry = TheShip::getSingletonPtr()->mLogBook->readCurrentItemText();
				String LogDate  = TheShip::getSingletonPtr()->mLogBook->readCurrentItemTitle();
				text = TheShip::getSingletonPtr()->mLogBook->readDocumentName();
				text += this->formatLogEntry(LogEntry, LogDate, pText);
				pText->setText(text);
				break;
				}
			case 2:     // Alertbook
				{
				String LogEntry = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemText();
				String LogDate  = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemTitle();
				text = TheShip::getSingletonPtr()->mAlertBook->readDocumentName();
				text += this->formatLogEntry(LogEntry, LogDate, pText);
				pText->setText(text);
				break;
				}
			case 3:     // Missions
				{
				String LogEntry = TheShip::getSingletonPtr()->mMissionBook->readCurrentItemText();
				String LogDate  = TheShip::getSingletonPtr()->mMissionBook->readCurrentItemTitle();
				text = TheShip::getSingletonPtr()->mMissionBook->readDocumentName();
				text += this->formatLogEntry(LogEntry, LogDate, pText);
				pText->setText(text);
				break;
				}
			case 4:     // CrewBook
				{
				String LogEntry = TheShip::getSingletonPtr()->mCrewBook->readCurrentItemText();
				String LogDate  = TheShip::getSingletonPtr()->mCrewBook->readCurrentItemTitle();
				text = TheShip::getSingletonPtr()->mCrewBook->readDocumentName();
				text += this->formatLogEntry(LogEntry, LogDate, pText);
				pText->setText(text);
				break;
				}
			case 5:     // Edition du Logbook
				{
				pText->setFormatting(CEGUI::StaticText::WordWrapLeftAligned, CEGUI::StaticText::TopAligned);
				text = TheShip::getSingletonPtr()->mCrewBook->readDocumentName();
				pText->setText(text);
				break;
				}
			default:
				pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
				pText->setText("empty document");
			}
		}
	}
}





/* ------------------------------------------------------------------------- */
/// Polymorphe de la fonction de creation d'interface, avec en plus, l'apparition d'une fenetre de login.
/* ------------------------------------------------------------------------- */
void Computer_Logs::createInterface()
{
	// Appel de la fonction (virtuelle) de base
	Interface::createInterface ();

	// Ajout de la fenetre de login
	this->addGUIWindow("loginWin");
	this->refreshTexts("loginWin");
	// On désactive le clic sur la fenetre de fond.
	mInterfaceWindow->setEnabled(false);
}



/* ------------------------------------------------------------------------- */
/// Affiche les informations pour un element de type "LogBook"
/* ------------------------------------------------------------------------- */
String Computer_Logs::formatLogEntry(String LogEntry, String LogDate, CEGUI::StaticText* pText)
{
	pText->setFormatting(CEGUI::StaticText::WordWrapLeftAligned, CEGUI::StaticText::TopAligned);
	pText->setVerticalScrollbarEnabled(true);
	pText->setTextColours(mWhite);

	String text = "\n--------------------------------\n\n";

	if (LogDate!="")
	{
		text += LogDate;
		text += "\n--------------------------------\n";
		text += LogEntry;
	}
	return text;
}


/* ------------------------------------------------------------------------- */
/// Rafraichit les textes de la fenetre GUI
/* ------------------------------------------------------------------------- */
void Computer_Logs::refreshGUIWindow()
{
	// Suivant l'action en cours, ce n'est pas le même type de document qu'il faut refraichir...
	switch(mSelectedOrder) 
	{
	case 1:
		this->refreshTexts("RWdocWin"); 
		break;
	case 2:
		this->refreshTexts("ROdocWin"); 
		break;
	case 3:
		this->refreshTexts("ROdocWin"); 
		break;
	case 4:
		this->refreshTexts("ROdocWin"); 
		break;
	case 5:
		this->refreshTexts("EditWin"); 
		break;
	}
}