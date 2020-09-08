/* ------------------------------------------------------------------------- */
// File       : Computer_Maintenance.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Computer_Maintenance.h"

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
#include <OgreMemoryMacros.h>


#include "macros.h"
#include "spares.h"
// Les singletons utilis�s
#include "Clock.h"
#include "TheShip.h"
#include "NPC_Manager.h"
#include "XMLparser_T.h"



/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Computer_Maintenance::Computer_Maintenance(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr) 
	: Computer(MeshFile, DescriptionFile, ParentNode, SceneMgr)
{
	// Le constructeur de la surclasse COMPUTER s'ex�cute automatiquement

	TRACE(">> Computer_Maintenance()");
	mGUIname = "MaintenanceComputer";
	mSelectedOrder = -1;
	mShipStatus    = "Ship status is unknown.";
	mMissingSP     = "";
	mSpareList     = NULL;
	mLastSpare     = NULL;
	mMissingList   = NULL;
	mLastMissing   = NULL;
	mShipAnalyzed  = false;
	loadStock();
	// On change la texture de clavier, et de la visu
	if (mSubEntity_visu)     mSubEntity_visu->setMaterialName("computer/dyn/maintenance/menu");
	if (mSubEntity_keyboard) mSubEntity_keyboard->setMaterialName("computer/steel");
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Computer_Maintenance::~Computer_Maintenance()
{
	Spare* OldSpare;
	Spare* CurrentSpare;

	// On supprime la liste des SP du stock
	CurrentSpare = mSpareList;
	while (CurrentSpare)
	{
		OldSpare     = CurrentSpare; 
		CurrentSpare = CurrentSpare->next;
		delete OldSpare;
	}
	// On supprime la liste des SP manquantes
	CurrentSpare = mMissingList;
	while (CurrentSpare)
	{
		OldSpare     = CurrentSpare;
		CurrentSpare = CurrentSpare->next;
		delete OldSpare;
	}
	// Le destructeur de la surclasse COMPUTER s'ex�cute automatiquement
}


/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Computer_Maintenance::executeOrder(String Order)
{
	// On extrait les param�tres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");

	// *****************************************
	// Ordre de quitter le GUI
	// *****************************************
	if (FuncV.at(0)=="exitGUI")
	{
		// On vide les textes qui peuvent etre gros...
		mAvailableSP   = "";
		mMissingSP     = "";
		mShipStatus    = "";
		mSelectedOrder = -1;
		// On supprime l'interface.GUI
		mExitGui = true;
		// On change un peu les ecrans
		if (mSubEntity_screen)   mSubEntity_screen->setMaterialName("computer/dyn/idle");
		if (mSubEntity_visu)     mSubEntity_visu->setMaterialName("computer/dyn/maintenance/menu");
	}
	// *****************************************
	// Ordre d'afficher l'�tat du vaisseau
	// *****************************************
	else if (FuncV.at(0)=="getShipStatus")
	{
		mSelectedOrder=0;
		loadShipStatus();
		refreshTexts("mainWin");
		// On affiche les ecrans sp�cifiques "maintenance"
		this->setSpecificMaterial();
	}
	// *****************************************
	// Ordre de faire venir le MT droid
	// *****************************************
	else if (FuncV.at(0)=="callRobot")
	{
		mSelectedOrder=1;
		// On recup�re le WP le plus proche de l'ordinateur
		Vector3    CompAbsPosition = mNode->getPosition() + mParentNode->getPosition();
		SceneNode* WPnode = TheShip::getSingletonPtr()->getClosestWaypoint(CompAbsPosition);
		// On dit au MTdroid de venir.
		NPC_Manager::getSingletonPtr()->getRobot("MTdroid")->activateRobot();
		NPC_Manager::getSingletonPtr()->getRobot("MTdroid")->setActionString("awaiting orders");
		NPC_Manager::getSingletonPtr()->getRobot("MTdroid")->gotoNode(WPnode->getName());
		refreshTexts("mainWin");
	}
	// *****************************************
	// Ordre d'afficher la liste des SP du stock
	// *****************************************
	else if (FuncV.at(0)=="showAvailableSP")
	{
		mSelectedOrder=2;
		mAvailableSP = "Available Spare Parts: \n";
		Spare* CurrentSpare = mSpareList;
		if (!CurrentSpare) mAvailableSP += "- The Spareparts stock is empty. ";
		while (CurrentSpare)
		{
			mAvailableSP += " - " + SpareName[CurrentSpare->id] + " : " + ITOA(CurrentSpare->quantity) + "\n";
			CurrentSpare = CurrentSpare->next;
		}
		refreshTexts("mainWin");
	}
	// *****************************************
	// Ordre d'afficher la liste des SP manquantes
	// *****************************************
	else if (FuncV.at(0)=="showMissingSP")
	{
		mSelectedOrder=3;
		// Il faut analyser d'abord l'�tat du vaisseau, si cela n'a pas d�j� �t� fait.
		if (!mShipAnalyzed) loadShipStatus();

		mMissingSP = "Missing Spare Parts: \n";
		Spare* CurrentSpare = mMissingList;
		if (!CurrentSpare) mMissingSP +="- No Spareparts are missing. ";
		while (CurrentSpare)
		{
			mMissingSP += " - " + SpareName[CurrentSpare->id] + "\n";
			CurrentSpare = CurrentSpare->next;
		}
		refreshTexts("mainWin");
	}
	// *****************************************
	// Ordre d'afficher la liste des Commandes
	// *****************************************
	else if (FuncV.at(0)=="showMenu")
	{
		mSelectedOrder=4;
		refreshTexts("mainWin");
	}
}




/* ------------------------------------------------------------------------- */
/// On affiche le texte demand� (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Computer_Maintenance::showDynamicText(CEGUI::StaticText* pText, String Order)
{

	// *****************************************
	// Fenetre principale
	// *****************************************
	if (Order=="@data")
	{
		switch(mSelectedOrder)
		{
		case 0:
			pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapLeftAligned);
			pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
			pText->setVerticalScrollbarEnabled(true);
			pText->setText(mShipStatus);
			break;

		case 1:
			pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapCentred);
			pText->setVerticalFormatting(CEGUI::StaticText::VertCentred);
			pText->setText("Maintenance Robot is coming.");
			break;

		case 2:
			pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapLeftAligned);
			pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
			pText->setVerticalScrollbarEnabled(true);
			pText->setText(mAvailableSP);
			break;

		case 3:
			pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapLeftAligned);
			pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
			pText->setVerticalScrollbarEnabled(true);
			pText->setText(mMissingSP);
			break;

		case 4:
			pText->setHorizontalFormatting(CEGUI::StaticText::LeftAligned);
			pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
			pText->setVerticalScrollbarEnabled(false);
			pText->setText("\n___Menu \n\n\n___Analyse Ship Status \n\n\n\n____Call Maintenance Robot");
			break;

		default:
			pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapCentred);
			pText->setVerticalFormatting(CEGUI::StaticText::VertCentred);
			pText->setText(" Maintenance Computer \nCurrent date \nday: "+ITOA(Clock::getSingletonPtr()->getCurrentDate()));
			break;
		}
	}
	else if (Order=="@menu")
	{
		switch(mSelectedOrder)
		{
		case 4:
			pText->setHorizontalFormatting(CEGUI::StaticText::RightAligned);
			pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
			pText->setText("\nShow Avalaible Spareparts___\n\n\n\nShow Missing Spareparts___\n\n\nQuit___");
			break;
		default:
			pText->setText("");
			break;
		}
	}
}



/* ------------------------------------------------------------------------- */
/// Analyse un �lement XML de \<status\>, et le met sous forme affichable
/// @param index L'index est le num�ro de l'tem
/* ------------------------------------------------------------------------- */
String Computer_Maintenance::getStatusInfo(int index)
{
	String ElementStatus = "";

	// Nom de l'�l�ment
	ElementStatus = mXMLinterface->getTextContent();
	if (ElementStatus =="Nothing...") return "";

	// Dur�e n�c�ssaire � la r�paration
	int ETR = mXMLinterface->getIntAttribute("ETR");
	switch (ETR)
	{
	case -1: 	ElementStatus += " (unknown) "; break;
	case 0: 	ElementStatus += " "; break;
	default: 	ElementStatus += " ("+ITOA(ETR)+" min) "; break;
	}
	
	// Spareparts n�c�ssaires � la r�paration
	// Note: un seul element � la fois
	int spareID = mXMLinterface->getIntAttribute("spares");
	ElementStatus +=  ": " + SpareName[spareID];
	
	// V�rifie si ce spare est en stock ou non
	// S'il n'est pas en stock, on le rajoute � la liste des "missing spares"
	if (!this->contains(spareID)) 
	{
		this->addMissingSpare(spareID);
	}

	// Pr�sentation sous forme de liste
	ElementStatus = " - "+ElementStatus+"\n";
	return (ElementStatus);
}



/* ------------------------------------------------------------------------- */
/// Charge le contenu du stock de spareparts (depuis le fichier XML de status)
/* ------------------------------------------------------------------------- */
void Computer_Maintenance::loadStock()
{
	Spare* ThisSpare;
    String filename = "../savedgames/"+mStatusFile;
	TRACE(">> loadStock: "+filename);

	/// Lecture des parametres du fichier de status
	XMLParser_T* StatusFile = new XMLParser_T(filename);

	StatusFile->gotoElement("computer","spareparts");

	bool SpareNames = StatusFile->getBooleanAttribute("listedByName");
	int  index = 0;

	if (SpareNames)
	{
		/// TODO: gestion des spareparts par leurs noms
	}
	/// Gestion des spareparts par leurs "item number"
	else while (StatusFile->gotoSubElement("spare",index++))
	{
		// On cr�e une nouvelle sparepart
		ThisSpare = new Spare;
		ThisSpare->next= NULL;
		// On m�morise le spare
		ThisSpare->id=StatusFile->getIntAttribute("item");
		// On m�morise sa quantit�
		ThisSpare->quantity=StatusFile->getIntAttribute("qty");
		addSpare(ThisSpare);
	}

	delete (StatusFile);
}

/* ------------------------------------------------------------------------- */
/// Ajoute un element � la liste des spareparts du stock
/* ------------------------------------------------------------------------- */
void Computer_Maintenance::addSpare(Spare* newSpare)
{
	// Si c'est du vide, on ne le rajoute pas � la liste.
	if (newSpare->id==0) return;

	// DEBUG_LOG("Add "+ITOA(newSpare->quantity)+" items of "+ITOA(newSpare->id));

	if (mSpareList==NULL)		
	{
		// On met � jour le pointeur vers la premiere ligne
		mSpareList = newSpare;	
	    mLastSpare = newSpare;
	}
	else
	{	
		// Sinon, il y a des lignes pr�c�dentes, et on s'y rattache
		mLastSpare->next = newSpare;
		mLastSpare       = newSpare;
	}
}

/* ------------------------------------------------------------------------- */
/// Cr�e et ajoute un element � la liste des spareparts manquantes
/* ------------------------------------------------------------------------- */
void Computer_Maintenance::addMissingSpare(int SpareID)
{
	// Si c'est du vide, on ne le rajoute pas � la liste.
	if (SpareID==0) return;

	// On cr�e une nouvelle spare
	// On m�morise le spare
	// On m�morise sa quantiti�
	Spare* ThisSpare = new Spare;
	ThisSpare->id=SpareID;
	ThisSpare->quantity=1;
	ThisSpare->next= NULL;

	// Si c'est la premi�re Spare: on m�morise 
	if (mMissingList==NULL)		
	{
		// On met � jour le pointeur vers la premiere ligne
		mMissingList = ThisSpare;	
	    mLastMissing = ThisSpare;
	}
	else
	{	
		// Sinon, il y a des lignes pr�c�dentes, et on s'y rattache
		mLastMissing->next = ThisSpare;
		mLastMissing       = ThisSpare;
	}
}


/* ------------------------------------------------------------------------- */
/// Charge et analyse l'�tat du vaisseau
/* ------------------------------------------------------------------------- */
void Computer_Maintenance::loadShipStatus()
{
	// Charge l'�tat du vaisseau
	String ShipStatus = TheShip::getSingletonPtr()->getStatus();
	mXMLinterface->addXmlData(ShipStatus ,"status");

	// Met les information sur l'�tat du vaisseau sous une forme affichable
	mShipStatus = "Ship Status Analyzed.\n";
	mShipStatus = "Damaged Elements & Needed Spareparts: \n";
	mXMLinterface->gotoElement("status");
	int index=0;
	while (mXMLinterface->gotoSubElement("item",index))
	{
		mShipStatus += getStatusInfo(index);
		index++;
	}
	// On m�morise que cette action a �t� faite.
	mShipAnalyzed = true;
}


/* ------------------------------------------------------------------------- */
/// Indique si le stock contient ce sparepart
/* ------------------------------------------------------------------------- */
bool Computer_Maintenance::contains(int SpareID)
{
	bool result = false;

	// On parcourt la liste des spare parts en stock
	Spare* CurrentSpare = mSpareList;
	while (CurrentSpare)
	{
		// D�s qu'on en trouve, on sort
		if (SpareID == CurrentSpare->id) return true;
		CurrentSpare = CurrentSpare->next;
	}
	return result;
}


/* ------------------------------------------------------------------------- */
/// Met � jour l'�cran du Computer de Maintenance
/* ------------------------------------------------------------------------- */
void Computer_Maintenance::setSpecificMaterial()
{
	TRACE(">> Computer::setSpecificMaterial()");

	// On change un peu les �crans
	if (mSubEntity_screen)   mSubEntity_screen->setMaterialName("computer/dyn/maintenance");
	if (mSubEntity_visu)     mSubEntity_visu->setMaterialName("computer/dyn/maintenance/menu");
}

