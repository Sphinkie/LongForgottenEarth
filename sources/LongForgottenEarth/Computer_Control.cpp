/* ------------------------------------------------------------------------- */
// File       : Computer_Control.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Computer_Control.h"

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



// Les singletons utilisés
#include "macros.h"
#include "Clock.h"
#include "TheShip.h"
#include "NPC_Manager.h"


#include "ShipEngine.h"


/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Computer_Control::Computer_Control(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr) 
	: Computer(MeshFile, DescriptionFile, ParentNode, SceneMgr)
{
	// Le constructeur de la surclasse COMPUTER s'exécute automatiquement

	TRACE(">> Computer_Control()");
	mGUIname = "ControlComputer";
	mSelectedOrder  = -1;
	mCurrentElement = NULL;
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Computer_Control::~Computer_Control()
{
	// Le destructeur de la surclasse COMPUTER s'exécute automatiquement
}


/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Computer_Control::executeOrder(String Order)
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
	// Ordre d'afficher le Status des elements vitaux
	// *****************************************
	else if (FuncV.at(0)=="LifeStatus")
	{
		mSelectedOrder=0;
		// On cree la fenetre 
		this->removeAnnexWindow();
		this->addGUIWindow("statusWin");
		// On met a jour les textes et images de la fenetre annexe
		this->refreshTexts("statusWin");
		this->refreshTexts("mainWin");
		this->refreshListboxes("mainWin");
	}
	// *****************************************
	// Ordre d'afficher l'état du vaisseau
	// *****************************************
	else if (FuncV.at(0)=="ShipStatus")
	{
		mSelectedOrder=1;
		// On cree la fenetre 
		this->removeAnnexWindow();
		this->addGUIWindow("statusWin");
		// On met a jour les textes et images de la fenetre annexe
		this->refreshTexts("statusWin");
		this->refreshTexts("mainWin");
		this->refreshListboxes("mainWin");
	}
	// *****************************************
	// Ordre d'afficher l'état des Moteurs
	// *****************************************
	else if (FuncV.at(0)=="EngStatus")
	{
		mSelectedOrder=2;
		// On cree la fenetre 
		this->removeAnnexWindow();
		this->addGUIWindow("statusWin");
		// On met a jour les textes et images de la fenetre annexe
		this->refreshTexts("statusWin");
		this->refreshTexts("mainWin");
		this->refreshListboxes("mainWin");
	}
	// *****************************************
	// Ordre d'afficher l'état des Controles
	// *****************************************
	else if (FuncV.at(0)=="CtrlStatus")
	{
		mSelectedOrder=3;
		// On cree la fenetre 
		this->removeAnnexWindow();
		this->addGUIWindow("statusWin");
		// On met a jour les textes et images de la fenetre annexe
		this->refreshTexts("statusWin");
		this->refreshTexts("mainWin");
		this->refreshListboxes("mainWin");
	}
	// *****************************************
	// Ordre d'afficher l'état des Alertes
	// *****************************************
	else if (FuncV.at(0)=="AlertStatus")
	{
		mSelectedOrder=4;
		// On cree la fenetre 
		this->removeAnnexWindow();
		this->addGUIWindow("statusWin");
		// On met a jour les textes et images de la fenetre annexe
		this->refreshTexts("statusWin");
		this->refreshTexts("mainWin");
		this->refreshListboxes("mainWin");
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
	// *****************************************
	// Execute l'action liée au bouton 1
	// *****************************************
	else if (FuncV.at(0)=="doButton1")
	{
		// Cette action depend du contexte
		switch (mSelectedOrder) 
		{
		case 0: TheShip::getSingletonPtr()->setRedLight(false); break; // Life: Internal Lights ON
		case 1: break;			/// TODO: Ship : "Navigation Shield"
		case 2: /// Engine: Show Damage
			if (mCurrentElement)
			{
				Real   dmge = mCurrentElement->getDamage();
				String text = "Plasma Engine Damage Status:";
				text += "\n---------------------------------------\n\n";
				text += "\n Identification =  "+ mCurrentElement->getName();
				text += "\n Damage Level = "+ StringConverter::toString(mCurrentElement->getDamage()*100)+" %";
				if      (dmge==0  ) text += "\n Not Damaged. \n This Engine is fully functionnal.";
				else if (dmge<0.40) text += "\n Minor damage.\n\nThis Engine can be repaired in any concessionnal Shipyard.";
				else if (dmge<0.70) text += "\n Major damage.\n\nThis Engine can only be repaired in authorized Renault Shipyards.";
				else if (dmge<0.95) text += "\n Critical damage.\n\nThis Engine can only be repaired in specialized Renault Technical Shipyards.";
				else text += "\n Out of function \n\nThis Engine cannot be repaired.  Please contact our commercial Dpt for replacing.";
				mAnnexText->setText(text);
			}
			break;
		case 3: break;			/// TODO : Control TBD
		case 4:	if (mAnnexText) this->showDynamicText(mAnnexText, "@prev");	break; // Goto Previous incident
		}
	}
	// *****************************************
	// Execute l'action liée au bouton 2
	// *****************************************
	else if (FuncV.at(0)=="doButton2")
	{
		// Cette action depend du contexte
		switch (mSelectedOrder) 
		{
		case 0: TheShip::getSingletonPtr()->setRedLight(true); break; // Life: Internal Lights OFF
		case 1: break;			/// TODO : Ship : "Combat Shield"
		case 2: this->showDynamicText(mAnnexText, "@engTemp"); break; // Engine: Show Temperature
		case 3: break;			/// TODO : Control : TBD
		case 4:	this->showDynamicText(mAnnexText, "@next");	break; // Goto Next incident
		}
	}
	// *****************************************
	// Execute l'action liée au bouton 3
	// *****************************************
	else if (FuncV.at(0)=="doButton3")
	{
		// Cette action depend du contexte
		switch (mSelectedOrder) 
		{
		case 0:  // TODO : Life: Glass Polarization
			break;
		case 1:          // Ship : External Lights
			TheShip::getSingletonPtr()->mSpot1->setHilight(!TheShip::getSingletonPtr()->mSpot1->getHilightStatus());
			TheShip::getSingletonPtr()->mSpot2->setHilight(!TheShip::getSingletonPtr()->mSpot2->getHilightStatus());
		    this->showDynamicText(mAnnexText, "@extlights");	
			break;
		case 2:          // Engine: Show Power
			if (mCurrentElement)
			{
				String text = "Plasma Engine Power: ";
				text += "  \n---------------------------------------\n\n";
				text += "\n Identification = "      + mCurrentElement->getName();
				text += "\n Engine Mode = "         + TheShip::getSingletonPtr()->mEngine1->getModeName();  // Les deux moteurs ont toujours le même Mode
				text += "\n Requested Power = "     + StringConverter::toString(mCurrentElement->getActivity()*100)+" % \n";
				text += "\n Engine Capacity Used = "+ StringConverter::toString(mCurrentElement->getPourcentageValue()*100)+" % ";
				text += "\n Actual Thrust = "       + StringConverter::toString(mCurrentElement->getPourcentageValue() * mCurrentElement->getMaxValue()) + mCurrentElement->getUnit();
				text += "\n Actual Thrust = "       + StringConverter::toString(mCurrentElement->getValue())+ mCurrentElement->getUnit();
				text += "\n Plasma Consom' = "      + StringConverter::toString(0)+ mCurrentElement->getUnit() + " . . . TODO";
				text += (mCurrentElement->getActivity()==0) ? "\n Status = Stopped" : "\n Status = On"; // operateur ternaire: (cond)?result_vrai:result_faux;
				mAnnexText->setText(text);
			}
			break;
		case 3:  break;			/// TODO : Control : TBD
		case 4:	        // Alertes: Clear incident
			TheShip::getSingletonPtr()->mAlertBook->removeCurrentItem();
			this->showDynamicText(mAnnexText, "@curr");	
			break;			
		}
	}
	// *****************************************
	// Execute l'action liée au bouton 4
	// *****************************************
	else if (FuncV.at(0)=="doButton4")
	{
		// Cette action depend du contexte
		switch (mSelectedOrder) 
		{
		case 0:          // Life: Reset RadMeter
			TheShip::getSingletonPtr()->mMaxRadiation=0; 
			this->showDynamicText(mAnnexText,"@radLevel");
			break;
		case 1:  break;			/// TODO : Ship : Eco Mode
		case 2:          // Engine: Show Maximum Power
			if (mCurrentElement)
			{
				int dmge = (int)(mCurrentElement->getDamage()*100);
				String text = "Plasma Engine Maximum Power:";
				text += "\n---------------------------------------\n\n";
				text += "\n Identification =  "+ mCurrentElement->getName();
				if (dmge == 0) text += "\n This Engine is not damaged.\n It is fully operationnal.";
				else text += "\n This Engine is damaged.\n It is not fully operationnal.";
				text += "\n Max Power is "+StringConverter::toString(100-dmge)+"% of nominal power.";
				Real RealMaxPower = mCurrentElement->getRealMaxValue();
				text += "\n Maximum Power is "+StringConverter::toString(RealMaxPower)+ mCurrentElement->getUnit();
				mAnnexText->setText(text);
			}
			break;
		case 3:  break;			/// TODO : Control: Change Music
		case 4:          // Alertes: Captain Alarm 
			TheShip::getSingletonPtr()->setAlertLevel(4);
			this->showDynamicText(mAnnexText, "@setAlarm");	
			break; 
		}
	}
	// On met a jour la couleur des labels des pushbuttons
	this->showColouredLabels(mSelectedOrder);
}




/* ------------------------------------------------------------------------- */
/// On affiche le texte demandé (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Computer_Control::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	// Un pointeur sur cette zone texte peut être nécessaire par ailleurs
	if (!pText) return;
	mAnnexText  = pText;	// Les textes dynamiques sont tous dans la fenetre annexe

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
		// Alignement par défaut (sauf precision contraire)
		pText->setFormatting(CEGUI::StaticText::WordWrapLeftAligned, CEGUI::StaticText::TopAligned);

		// TODO: Debug, tous les textes sont en blanc. On les passe en vert au fur et à mesure qu'il sont gérés.
		pText->setTextColours(mWhite);

		// *****************************************
		// Cas du Computer nominal
		// *****************************************
		if (Order=="@label1")
		// *****************************************
		{	// Description du premier bouton
			pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
			switch (mSelectedOrder) 
			{
			case 0: pText->setText("Lights ON"); break;          // Life Status
			case 1: pText->setText("Navigation Shield"); break;  // Ship Status
			case 2: pText->setText("Show Damage"); break;        // Engine Status
			case 3: pText->setText("TBD"); break;			     // Control Status
			case 4: pText->setText("Prev. Incident"); break;     // AlertsStatus
			default:pText->setVisible(false);
			}
		}
		// *****************************************
		else if (Order=="@label2")
		// *****************************************
		{	// Description du deuxieme bouton
			pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
			switch (mSelectedOrder) 
			{
			case 0: pText->setText("Lights OFF"); break;           // Life Status
			case 1: pText->setText("Combat Shield"); break;        // Ship Status
			case 2: pText->setText("Show Temperature"); break;     // Engine Status
			case 3: pText->setText("TBD"); break;			       // Control Status
			case 4: pText->setText("Next Incident"); break;              // AlertsStatus
			default:pText->setVisible(false);
			}
		}
		// *****************************************
		// Description du troisième bouton
		else if (Order=="@label3")
		// *****************************************
		{
			pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
			switch (mSelectedOrder) 
			{
			case 0: pText->setText("Glass Polarization"); break;    // Life Status
			case 1: pText->setText("External Lights"); break;       // Ship Status
			case 2: pText->setText("Show Power"); break;            // Engine Status
			case 3: pText->setText("TBD"); break;			        // Control Status
			case 4: pText->setText("Clear Incident"); break;        // AlertsStatus
			}
		}
		// *****************************************
		// Description du quatrieme bouton
		else if (Order=="@label4")
		// *****************************************
		{	
			pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
			switch (mSelectedOrder) 
			{
			case 0: pText->setText("Reset RadMeter"); break;        // Life Status
			case 1: pText->setText("Eco. Mode");      break;        // Ship Status
			case 2: pText->setText("Show Max Power"); break;        // Engine Status
			case 3: pText->setText("Change Music");   break;        // Control Status
			case 4: pText->setText("ALARM !");        break;        // AlertsStatus
			}
		}
		// *****************************************
		else if (Order=="@time")
		// *****************************************
		{
			pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
			pText->setText(" Control Computer \nCurrent date \nday: "+ITOA(Clock::getSingletonPtr()->getCurrentDate()));
		}
		// *****************************************
		else if (Order=="@foodLevel")
		// *****************************************
		{	
			String text = this->formatElementData(TheShip::getSingletonPtr()->mFoodProdSystem, pText);
			if (!TheShip::getSingletonPtr()->mWaterProdSystem->isBroken())
				text += " (one dose for one meal).\n";
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@airPressure")
		// *****************************************
		{	
			String valeur = TheShip::getSingletonPtr()->getStatus();
			String text = "Internal Air Pressure:";
			text += "\n---------------------------------------\n\n";
			String diag = analyseStatus(valeur);
			if (diag.empty())
			{
				text += " Situation is nominal. ";
				pText->setTextColours(mGreen);
			}
			else
			{
				text += " Warning. Warning. \n ";
				text += diag;
				text += ".\n All access have been locked.";
				pText->setTextColours(mYellow);
			}
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@ambTemperature")
		// *****************************************
		{	
			pText->setTextColours(mGreen);
			String text = "Internal Ambiant Temperature:";
			text += "\n---------------------------------------\n\n";
			short valeur = TheShip::getSingletonPtr()->getAmbiantTemperature();
			if (valeur <270)
			{
				text += " Warning: Temperature is TOO LOW. \n";
				pText->setTextColours(mYellow);
			}
			else if (valeur>320)
			{
				text += " Warning: Temperature is TOO HIGH. \n";
				pText->setTextColours(mYellow);
			}
			else
			{
				text += " Temperature is within acceptable range. \n";
			}
			text += " Temperature is: ";
			// Conversion = °F = °C / (5/9)) + 32
			text += "\n - Celsius    = "+StringConverter::toString(valeur-273) + " degrees";
			text += "\n - Kelvin      = "+StringConverter::toString(valeur) + " degrees";
			text += "\n - Farenheit = "+StringConverter::toString(((valeur-273)*9/5)+32) + " degrees";
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@waterLevel")
		// *****************************************
		{	
			String text = this->formatElementData(TheShip::getSingletonPtr()->mWaterProdSystem, pText);
			if (!TheShip::getSingletonPtr()->mWaterProdSystem->isBroken())
			{
				text += " Water distribution is nominal.\n";
				text += " ( 5 litres /person /day )\n";
			}
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@extlights")
		// *****************************************
		{	
			String text = " External Lights:";
			text += "\n--------------------------------\n\n";
			text += "\n Left External Light is ";
			text += (TheShip::getSingletonPtr()->mSpot1->getHilightStatus()) ? "ON" : "OFF"; // operateur ternaire: (cond)?result_vrai:result_faux;
			text += "\n Right External Light is ";
			text += (TheShip::getSingletonPtr()->mSpot2->getHilightStatus()) ? "ON" : "OFF"; // operateur ternaire: (cond)?result_vrai:result_faux;
			pText->setFormatting(CEGUI::StaticText::WordWrapLeftAligned, CEGUI::StaticText::TopAligned);
			pText->setTextColours(mGreen);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@radLevel")
		// *****************************************
		{	
			pText->setTextColours(mGreen);
			String text = "Radiation Level:";
			text += "\n---------------------------------------\n\n";
			text += "This radiation rate is measured inside the Ship (after Hull protection).\n";
			text += "If the cumuled value exceed 100 rads, you should make a medical checkup.\n\n";
			text += " Current rate of radiation = "+ StringConverter::toString(TheShip::getSingletonPtr()->getCurRadiation()) + " rad/day\n";
			text += " Maximum rate detected = "+ StringConverter::toString(TheShip::getSingletonPtr()->getMaxRadiation()) + " rad/day\n";
			text += "\n";
			if (TheShip::getSingletonPtr()->getMaxRadiation() > 100)
			{
				text += "Radiation Level is OVER Security Level. \n";
				text += "Take immediate protective and decontamination actions. \n";
				pText->setTextColours(mYellow);
			}
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@airProdLevel")
		// *****************************************
		{	
			String text = this->formatElementData(TheShip::getSingletonPtr()->mAirProdSystem, pText);
			if (!TheShip::getSingletonPtr()->mAirProdSystem->isBroken())
			{
				text += " Air distribution is nominal.\n";
				text += " ( 7 cubic-meters /person /day )\n";
			}
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@infos")
		// *****************************************
		{	
			String text = "Ship General Information:";
			text += "\n---------------------------------------\n\n";
			text += "\n Name: "   + TheShip::getSingletonPtr()->mName;
			text += "\n Type: "   + TheShip::getSingletonPtr()->mType;
			text += "\n Class: "   + TheShip::getSingletonPtr()->mClass;
			text += "\n Origin: " + TheShip::getSingletonPtr()->mOrigin;
			text += "\n Owner: "  + TheShip::getSingletonPtr()->mOwner;
			text += "\n Crew: "   + StringConverter::toString(TheShip::getSingletonPtr()->mCrew) + " men";
			text += "\n Mass: "   + StringConverter::toString(TheShip::getSingletonPtr()->mMass) + " tons";
			pText->setTextColours(mGreen);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@sstat")
		// *****************************************
		{	
			String valeur = TheShip::getSingletonPtr()->getStatus();
			String text = "Ship General Status:";
			text += "\n---------------------------------------\n\n";
			String diag = this->getDamagedElements(valeur);
			if (diag.empty())
			{
				text += "\n Situation is nominal. ";
				pText->setTextColours(mGreen);
			}
			else
			{
				text += "Warning. Some elements are damaged: \n";
				text += diag;
				text += "\nSee Maintenance Computer and Maintenance Robot for repair.";
				pText->setTextColours(mYellow);
			}
			pText->setText(text);
			pText->setVerticalScrollbarEnabled(true);
		}
		// *****************************************
		else if (Order=="@exits")
		// *****************************************
		{	
			pText->setText("Exit Access: \n All exits are closed, locked and secured.");
		}
		// *****************************************
		else if (Order=="@hull")
		// *****************************************
		{	
			String text = "Ship Hull Status:";
			text += "\n---------------------------------------\n\n";
			text += "\n Name: "   + TheShip::getSingletonPtr()->mName;
			text += "\n Hull Integrity is " + StringConverter::toString(TheShip::getSingletonPtr()->mHull) + " %";
			pText->setTextColours(mGreen);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@shields")
		// *****************************************
		{	
			pText->setText("Shield Status: \n CAUTION \n Shield are disabled.");
		}
		// *****************************************
		else if (Order=="@energy")
		// *****************************************
		{	
			String text = formatElementData(TheShip::getSingletonPtr()->mEnergyProdSystem, pText);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@weapons")
		// *****************************************
		{	
			pText->setText("Weapons: \n All weapon systems are disabled and secured\n Countermeasure systems are operationnal");
		}
		// *****************************************
		else if (Order=="@tractor")
		// *****************************************
		{	
			pText->setText("Tractor: \n Tractor module is operationnal");
		}
		// *****************************************
		else if (Order=="@eng1")
		// *****************************************
		{	
			// On stocke quelques élements utilisés par les pushbuttons associés
			mAnnexText      = pText;
			mCurrentElement = TheShip::getSingletonPtr()->mEngine1;
			// On affiche le texte
			String text = this->formatEngineData((ShipEngine*)mCurrentElement, pText);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@eng2")
		// *****************************************
		{	
			// On stocke quelques élements utilisés par les pushbuttons associés
			mAnnexText      = pText;
			mCurrentElement = TheShip::getSingletonPtr()->mEngine2;
			String text = this->formatEngineData((ShipEngine*)mCurrentElement, pText);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@engTemp")
		// *****************************************
		{	
			int    T1 = TheShip::getSingletonPtr()->mEngine1->getTemperature();
			int    T2 = TheShip::getSingletonPtr()->mEngine2->getTemperature();
			String text = "Dual Plasma Engine Temperature:";
			text += "\n---------------------------------------\n\n";
			text += "\n Engine I  Temperature = "+ StringConverter::toString(T1)+" Kelvin";
			text += "\n Engine II Temperature = "+ StringConverter::toString(T2)+" Kelvin";
			text += "\n\nRecommandations:";
			text += "\n---------------------";
			text += "\n-Under 800 Kelvin, Plasma Engine cannot operate. Use 'Heating System' to set the engine at the right temperature.";
			text += "\n-Over 10.000 Kelvin, Plasma Engine can be damaged. Do not exceed this threshold, and let the Engine rest after each long use.";
			pText->setFormatting(CEGUI::StaticText::WordWrapLeftAligned, CEGUI::StaticText::TopAligned);
			pText->setVerticalScrollbarEnabled(true);
			pText->setTextColours(mGreen);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@jump")
		// *****************************************
		{	
			pText->setText("Not available yet");
		}
		// *****************************************
		else if (Order=="@first")
		// *****************************************
		{	
			String text;
			TheShip::getSingletonPtr()->mAlertBook->gotoFirstItem();
			String LogEntry = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemText();
			String LogDate  = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemTitle();
			text += this->formatLogEntry(LogEntry, LogDate, pText);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@last")
		// *****************************************
		{	
			String text;
			TheShip::getSingletonPtr()->mAlertBook->gotoLastItem();
			String LogEntry = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemText();
			String LogDate  = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemTitle();
			text += this->formatLogEntry(LogEntry, LogDate, pText);
			pText->setTextColours(mYellow);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@prev")
		// *****************************************
		{	
			TheShip::getSingletonPtr()->mAlertBook->gotoPreviousItem();
			String LogEntry = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemText();
			String LogDate  = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemTitle();
			if (LogDate!="")
			{
				String text = this->formatLogEntry(LogEntry, LogDate, pText);
				pText->setText(text);
			}
		}
		// *****************************************
		else if (Order=="@next")
		// *****************************************
		{	
			TheShip::getSingletonPtr()->mAlertBook->gotoNextItem();
			String LogEntry = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemText();
			String LogDate  = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemTitle();
			if (LogDate!="")
			{
				String text = this->formatLogEntry(LogEntry, LogDate, pText);
				pText->setText(text);
			}
		}
		// *****************************************
		else if (Order=="@curr")
		// *****************************************
		{	
			String LogEntry = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemText();
			String LogDate  = TheShip::getSingletonPtr()->mAlertBook->readCurrentItemTitle();
			if (LogDate!="")
			{
				String text = this->formatLogEntry(LogEntry, LogDate, pText);
				pText->setText(text);
			}
		}
		// *****************************************
		else if (Order=="@ackAlarm")
		// *****************************************
		{	
			String text = "ALARM :";
			text += "\n---------------------------------------\n\n";
			text += "\n Set Alarm Level to Normal";
			text += "\n---------------------------------------\n\n";
			text += "\nThe current Alarm Status is acknowleged and cleared.";
			text += "\nShip Status is set back to Nominal Level.";
			TheShip::getSingletonPtr()->setAlertLevel(0); // Alertes: Acquittement Alarm
			pText->setTextColours(mGreen);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@setAlarm")
		// *****************************************
		{	
			String text = "ALARM :";
			text += "\n---------------------------------------\n\n";
			text += "\n Set Alarm Level to '4' ";
			text += "\n---------------------------------------\n\n";
			text += "\nCaptain is speaking.";
			text += "\nAll the crew is requested to join his post.";
			pText->setTextColours(mGreen);
			pText->setText(text);
		}
		// *****************************************
		else if (Order=="@clrall")
		// *****************************************
		{
			String CurDate = StringConverter::toString(Clock::getSingletonPtr()->getCurrentDate());
			TheShip::getSingletonPtr()->mAlertBook->removeAllItems();
			String text = formatLogEntry("Alert Log Book had been cleared", "Day "+CurDate, pText);
			pText->setTextColours(mYellow);
			pText->setText(text);
		}
		// *****************************************
		else
		// *****************************************
		{
			pText->setFormatting(CEGUI::StaticText::WordWrapCentred, CEGUI::StaticText::VertCentred);
			pText->setText(" Control Computer \nCurrent date \nday: "+ITOA(Clock::getSingletonPtr()->getCurrentDate()));
		}
	}
}


/* ------------------------------------------------------------------------- */
/// Gestion des Listbox de la GUI
/* ------------------------------------------------------------------------- */
void Computer_Control::showDynamicListbox(CEGUI::Listbox* pListbox, String Order) 
{
	// Le parametre Order contient le nom du Menu à utiliser pour remplir la Listbox.
	// en general, il contient juste "@chooseMenu"
	
	TRACE(">> "+mName+"::showDynamicListbox("+Order+")");
	
	if (this->isBroken()) mSelectedOrder = -1;

	switch(mSelectedOrder) 
	{
	case 0:
		fillListbox(pListbox, "menuLife");
		pListbox->setVisible(true);
		break;
	case 1:
		fillListbox(pListbox, "menuShip");
		pListbox->setVisible(true);
		break;
	case 2:
		fillListbox(pListbox, "menuEng");
		pListbox->setVisible(true);
		break;
	case 3:
		fillListbox(pListbox, "menuCtrl");
		pListbox->setVisible(true);
		break;
	case 4:
		fillListbox(pListbox, "menuWarning");
		pListbox->setVisible(true);
		break;
	default:
		pListbox->setVisible(false);
	}

}


/* ------------------------------------------------------------------------- */
/// Analyse le ShipStatus pour trouver les Room dépressurisées
/* ------------------------------------------------------------------------- */
String Computer_Control::analyseStatus(String Status)
{
	// On recherche la chaine: "The room is depressurized"
	String Pattern = "The room is depressurized";
	String Result  = "";
	String Temp;
	int Pos_Stat   = 0;

	// On cherche la premiere occurence du pattern
	Pos_Stat = Status.find(Pattern);

	while (Pos_Stat != -1) 
	{
		// Si on a trouvé le pattern, on extrait le nom de la pièce
		int Pos_Room1 = Status.find(">",Pos_Stat);
		int Pos_Room2 = Status.find("<",Pos_Stat);
		Temp = Status.substr(Pos_Room1+1, Pos_Room2-Pos_Room1-1);
		Result += "\n Low Pressure detected in " + Temp;

		// On cherche l'occurence suivante du pattern
		Pos_Stat = Status.find(Pattern,Pos_Room2);
	}

	return Result;
}

/* ------------------------------------------------------------------------- */
/// Analyse le ShipStatus pour trouver la liste des elements endommagés
/* ------------------------------------------------------------------------- */
String Computer_Control::getDamagedElements(String Status)
{
	// On extrait le nom de l'élément de la chaine: <item atributs="" >Nom de l'élément</item> 
	String Pattern = "<item";
	String Result  = "";
	String Text_Content;
	int Pos_Status = 0;

	// On cherche la premiere occurence du pattern
	Pos_Status = Status.find(Pattern);

	DEBUG_LOG(Status);

	while (Pos_Status != -1) 
	{
		// Si on a trouvé le pattern, on extrait le nom de la pièce
		int Pos_debut_content = Status.find(">",Pos_Status+Pattern.length());
		int Pos_fin_content   = Status.find("<",Pos_Status+Pattern.length());

		Text_Content = Status.substr(Pos_debut_content+1, Pos_fin_content-Pos_debut_content-1);
		
		if (Text_Content != "Nothing...") 
		{
			Result += " - " + Text_Content + " damaged\n";
			TRACE("   !startsWith(Text_Content, 'Nothing') =" + Text_Content);
		}

		// On cherche l'occurence suivante du pattern
		Pos_Status = Status.find(Pattern, Pos_fin_content);
	}

	return Result;
}



/* ------------------------------------------------------------------------- */
/// Prepare au bon format, le contenu d'un Ship Element
/* ------------------------------------------------------------------------- */
String Computer_Control::formatElementData(ShipElement* Element, CEGUI::StaticText* pText)
{
	String text  = Element->getName()+":";
	Real  level  = int (Element->getValue());
	short valeur = Element->getPourcentageValue() * 100;
	pText->setTextColours(mGreen);
	text += "\n---------------------------------------\n\n";
	text += " "+Element->getName()+ " contains "+StringConverter::toString(level)+Element->getUnit()+"  \n";
	text += " "+Element->getName()+ " is "+StringConverter::toString(valeur)+"% full. \n";

	if (valeur <10)
	{
		text += " Warning: Level is LOW. \n";
		pText->setTextColours(mYellow);
	}
	else 
	{
		text += " Level is nominal. \n";
	}

	if (Element->isBroken())
	{
		text += " Warning: "+Element->getName()+" is damaged.\n";
		pText->setTextColours(mYellow);
	}
	else 
	{
		text += " "+Element->getName()+" is operational.\n";
	}
	// On n'affiche pas ici, au cas où la fonction appelante veut ajouter quelque-chose
//	pText->setText(text);
	return text;
}


/* ------------------------------------------------------------------------- */
/// Affiche les informations pour un element de type "Engine"
/* ------------------------------------------------------------------------- */
String Computer_Control::formatEngineData(ShipEngine* Element, CEGUI::StaticText* pText)
{
	pText->setTextColours(mGreen);
	pText->setVerticalScrollbarEnabled(true);

	String text = "Plasma Engine Status:";
	text += "\n--------------------------------\n\n";
	text += "\n Identification = "  + Element->getName();
	text += "\n Engine Mode = "     + Element->getModeName();  
	text += "\n Requested Thrust = "+ StringConverter::toString(Element->getActivity()*100)+" %";
	text += "\n Actual Thrust = "   + StringConverter::toString(Element->getPourcentageValue() * Element->getModeCoeff() * 100,1)+" % \n";
	text += "\n Manufacturer = Renault";
	text += "\n Model = Renault SunBurn H2-GTI";
	text += "\n Construction Date = 2094";
	text += "\n Maximal Thrust (empty load) = " + StringConverter::toString(Element->getMaxValue()) + Element->getUnit();
	text += "\n Serial Number = "+ StringConverter::toString((long)Element)+"-ARFC \n";
	text += "\nTechnical Data: ";
	text += "\n-------------------- ";
	text += "\nThis Engine is based on the most recent technics of Hydrogen Plasma recycling. ";
	text += "\nNo refuel will ever be needed, as Hydrogen can be found almost everywhere, even in Deep Space.";
	text += "\nBeware not to exceed the maximal authorized temperature for this model of Engine.";
	text += " Therefore, an 'Engine Stopped' period is recommanded after every use.";

	return text;
}


/* ------------------------------------------------------------------------- */
/// Affiche les informations pour un element de type "LogBook"
/* ------------------------------------------------------------------------- */
String Computer_Control::formatLogEntry(String LogEntry, String LogDate, CEGUI::StaticText* pText)
{
	pText->setVerticalScrollbarEnabled(true);
	pText->setTextColours(mGreen);

	String text = " Alert Log:";
	text += "\n--------------------------------\n\n";

	if (LogDate!="")
	{
		text += LogDate ;
		text += "\n--------------------------------\n";
		text += LogEntry;
	}
	return text;
}


/* ------------------------------------------------------------------------- */
/// Gestion des Labels dynamiques de la GUI
/* ------------------------------------------------------------------------- */
void Computer_Control::showColouredLabels(int Order)
{
	TRACE (">> showColouredLabels()");
	
	// On gere les labels des pushbutton de la Fenetre Annexe
	mAnnexText = static_cast<CEGUI::StaticText*>(mWinMgr->getWindow(mGUIname+"/statusWin/text/0"));
	CEGUI::StaticText* bouton1 = static_cast<CEGUI::StaticText*>(mWinMgr->getWindow(mGUIname+"/statusWin/text/1"));
	CEGUI::StaticText* bouton2 = static_cast<CEGUI::StaticText*>(mWinMgr->getWindow(mGUIname+"/statusWin/text/2"));
	CEGUI::StaticText* bouton3 = static_cast<CEGUI::StaticText*>(mWinMgr->getWindow(mGUIname+"/statusWin/text/3"));
	CEGUI::StaticText* bouton4 = static_cast<CEGUI::StaticText*>(mWinMgr->getWindow(mGUIname+"/statusWin/text/4"));

	// Par defaut, tous les labels sont en vert 
	// (je ne sais pas pourquoi l'instruction mise dans le XML ne fonctionne pas: alors on force
	bouton1->setTextColours(mGreen);
	bouton2->setTextColours(mGreen);
	bouton3->setTextColours(mGreen);
	bouton4->setTextColours(mGreen);

	switch (mSelectedOrder) 
	{
	case 0:		// Life 
		break; 
	case 1:     // Ship
		// Si les projecteurs exterieurs sont allumés, le label passe en rouge
		if (TheShip::getSingletonPtr()->mSpot1->getHilightStatus())  bouton3->setTextColours(mRed);
		if (!TheShip::getSingletonPtr()->mSpot1->getHilightStatus()) bouton3->setTextColours(mGreen);
		break;			
	case 2:     // Engine
		break;
	case 3:     // Control
		break;
	case 4:     // Alerts
		// Si une alarme est en cours, le label passe en rouge
		if (TheShip::getSingletonPtr()->getAlertLevel()>0)  bouton4->setTextColours(mRed);
		if (TheShip::getSingletonPtr()->getAlertLevel()==0) bouton4->setTextColours(mGreen);
		break;
	}
}

