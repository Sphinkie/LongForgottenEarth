/* ------------------------------------------------------------------------- */
// File       : Computer_Navigation.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Computer_Navigation.h"

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
#include "CEGUI/elements/CEGUISlider.h"
#include "CEGUI/elements/CEGUIProgressBar.h"
#include "CEGUI/elements/CEGUIPushButton.h"
#include <OgreMemoryMacros.h>


#include "macros.h"
#include "Clock.h"
#include "TheShip.h"



/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Computer_Navigation::Computer_Navigation(String MeshFile, String DescriptionFile, SceneNode* ParentNode, SceneManager* SceneMgr) 
	: Computer(MeshFile, DescriptionFile, ParentNode, SceneMgr)
{
	// Le constructeur de la surclasse COMPUTER s'exécute automatiquement

	TRACE(">> Computer_Navigation()");
	mGUIname = "NavComputer";
	mSelectedOrder = -1;
	mSelectedMode  = -1; // -1= non affecte
	mRefreshTimer  = 0;
	mThrustSlider  = 0;	/// TODO : à initialiser avec une valeur mémorisée
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Computer_Navigation::~Computer_Navigation()
{
	// Le destructeur de la surclasse COMPUTER s'exécute automatiquement
}


/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Computer_Navigation::executeOrder(String Order)
{
	// On extrait les paramètres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");

	// *****************************************
	// Ordre de quitter le GUI
	// *****************************************
	if (FuncV.at(0)=="exitGUI")
	{
		mSelectedOrder = -1;
		// Arret du Listener
//		Root::getSingletonPtr()->removeFrameListener(this);
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
	// Ordre d'activer un moteur
	// *****************************************
	else if (FuncV.at(0)=="activate")
	{
		if (FuncV.at(1)=="Eng1")
			TheShip::getSingletonPtr()->mEngine1->switchActivity();
		if (FuncV.at(1)=="Eng2")
			TheShip::getSingletonPtr()->mEngine2->switchActivity();
		this->refreshTexts("mainWin");
	}
	// *****************************************
	// Ordre de prechauffer un moteur
	// *****************************************
	else if (FuncV.at(0)=="heat")
	{
		if (FuncV.at(1)=="Eng1")
			TheShip::getSingletonPtr()->mEngine1->switchHeating();
		if (FuncV.at(1)=="Eng2")
			TheShip::getSingletonPtr()->mEngine2->switchHeating();
		this->refreshTexts("mainWin");
	}
	// *****************************************
	// Ordre de monter un peu la puissance
	// *****************************************
	else if (FuncV.at(0)=="changeActivity")
	{
		Real value = mSlider[0]->getCurrentValue();
		Real delta = StringConverter::parseReal(FuncV.at(1));
		value += delta;
		mSlider[0]->setCurrentValue(value);
	}
	// *****************************************
	// Prise En Compte de la valeur de l'Activity.
	// *****************************************
	else if (FuncV.at(0)=="setActivity")
	{
		Real value = mSlider[0]->getCurrentValue();
		TheShip::getSingletonPtr()->mEngine1->setActivity(value);
		TheShip::getSingletonPtr()->mEngine2->setActivity(value);
	}
	// *****************************************
	// Sens de la poussée.
	// *****************************************
	else if (FuncV.at(0)=="setDirection")
	{
		Real value = StringConverter::parseInt(FuncV.at(1));
		TheShip::getSingletonPtr()->mEngine1->setPushDirection(value);
		TheShip::getSingletonPtr()->mEngine2->setPushDirection(value);
	}
	// *****************************************
	// Memorisation du mode selectionné dans la listbox
	// *****************************************
	else if (FuncV.at(0)=="changeMode")
	{
		ShipEngine::tMode mode;
		
		if      (FuncV.at(1)=="STOPPED_ENGINE")  mode = ShipEngine::STOPPED_ENGINE;
		else if (FuncV.at(1)=="IDLE_ENGINE")     mode = ShipEngine::IDLE_ENGINE;
		else if (FuncV.at(1)=="HEATING_ENGINE")  mode = ShipEngine::HEATING_ENGINE;
		else if (FuncV.at(1)=="ASTEROID_MODE")   mode = ShipEngine::ASTEROID_MODE;
		else if (FuncV.at(1)=="PLANETARY_MODE")  mode = ShipEngine::PLANETARY_MODE;
		else if (FuncV.at(1)=="SOLAR_MODE")      mode = ShipEngine::SOLAR_MODE;
		else if (FuncV.at(1)=="FULL_ENGINE")     mode = ShipEngine::FULL_ENGINE;

		// On mémorise la valeur du mode
		mSelectedMode = mode;
	}
	// *****************************************
	// Affection aux moteurs du Mode selectionné 
	// *****************************************
	else if (FuncV.at(0)=="setMode")
	{
		// Si mSelectedMode est négatif, ce n'est pas un vrai mode moteur
		if (mSelectedMode >0 )
		{
			ShipEngine::tMode mode = static_cast<ShipEngine::tMode>(mSelectedMode);
			// Si le moteur est STOPPED, il faut l'activer avant de pouvoir changer son mode.
			if (!TheShip::getSingletonPtr()->mEngine1->isEngineStopped()) 
				 TheShip::getSingletonPtr()->mEngine1->setMode(mode);
			if (!TheShip::getSingletonPtr()->mEngine2->isEngineStopped()) 
				 TheShip::getSingletonPtr()->mEngine2->setMode(mode);
			mModeText = "ENGINES RUNNING";
		}
	}
	// *****************************************
	// Gestion des Lampes Externes
	// *****************************************
	else if (FuncV.at(0)=="setLights")
	{
		TheShip::getSingletonPtr()->mSpot1->setHilight(!TheShip::getSingletonPtr()->mSpot1->getHilightStatus());
		TheShip::getSingletonPtr()->mSpot2->setHilight(!TheShip::getSingletonPtr()->mSpot2->getHilightStatus());
	}
	// *****************************************
	// DEBUG.....
	// *****************************************
	else if (FuncV.at(0)=="wakeup")
	{
		this->refreshTexts("mainWin");
		this->refreshProgressBar("mainWin");
		this->refreshPushButton("mainWin");
	}
	// *****************************************
	// Creation d'une fenetre annexe
	// *****************************************
	else if (FuncV.at(0)=="nextWindow")
	{
		String NewWindow;
		switch(mSelectedOrder) 
		{
		case -1:
			NewWindow = "DetailWin";
			mSelectedOrder=0;
			break;
		case 0:
			NewWindow = "ProgWin";
			mSelectedOrder=1;
			break;
		case 1:
			NewWindow = "CurveWin";
			mSelectedOrder=2;
			break;
		case 2:
			NewWindow = "CompassWin";
			mSelectedOrder=3;
			break;
		case 3:
			NewWindow = "BoostWin";
			mSelectedOrder=4;
			break;
		case 4:
			NewWindow = "DetailWin";
			mSelectedOrder=0;
			break;
		}
		// On cree la fenetre demandée
		this->removeAnnexWindow();
		this->addGUIWindow(NewWindow);
		// On met a jour les textes et images de la fenetre annexe
		this->refreshTexts(NewWindow);
		this->refreshImages(NewWindow);	
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
			mSelectedOrder = 2;
		}
	}
	// On met à jour la listbox (certains pushbuttons peuvent avoir un effet)
	this->refreshListboxes("mainWin");
}



/* ------------------------------------------------------------------------- */
/// Gestion des images dynamiques de la GUI
/* ------------------------------------------------------------------------- */
void Computer_Navigation::showDynamicImage(CEGUI::StaticImage* pImage, String Order)
{}



/* ------------------------------------------------------------------------- */
/// On affiche le texte demandé (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Computer_Navigation::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	// TRACE ("Computer_Navigation::showDynamicText("+Order+")");
	
	// *****************************************
	// Cas du Computer en panne
	// *****************************************
	if (this->isBroken())
	{
		pText->setHorizontalFormatting(CEGUI::StaticText::RightAligned);
		pText->setVerticalFormatting(CEGUI::StaticText::BottomAligned);
		pText->setTextColours(mRed);
		pText->setText("Malfunction");
		return;
	};

	
	// *****************************************
	// Gestion des textes dynamiques
	// *****************************************
	if (StringUtil::startsWith(Order,"@"))
	{
		// *****************************************
		// Voyant vert si le moteur 1 est allumé
		// *****************************************
		if (Order=="@activ(1)")
		{	
			if (TheShip::getSingletonPtr()->mEngine1->getMode() == ShipEngine::STOPPED_ENGINE)
				pText->setBackgroundColours(mBlack);
			else
				pText->setBackgroundColours(mGreen);
		}
		// *****************************************
		// Voyant vert si le moteur 2 est allumé
		// *****************************************
		else if (Order=="@activ(2)")
		{	
			if (TheShip::getSingletonPtr()->mEngine2->getMode() == ShipEngine::STOPPED_ENGINE)
				pText->setBackgroundColours(mBlack);
			else
				pText->setBackgroundColours(mGreen);
		}
		// *****************************************
		// Voyant coloré suivant l'état du moteur 1
		// *****************************************
		else if (Order=="@dmg(1)")
		{	
			if (TheShip::getSingletonPtr()->mEngine1->getDamage() < 0.1)
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mYellow);
		}
		// *****************************************
		// Voyant coloré suivant l'état du moteur 2
		// *****************************************
		else if (Order=="@dmg(2)")
		{	
			if (TheShip::getSingletonPtr()->mEngine2->getDamage() < 0.1)
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mYellow);
		}
		// *****************************************
		// Valeur des dommages du moteur 1
		// *****************************************
		else if (Order=="@dmgValue(1)")
		{	
			int D = (TheShip::getSingletonPtr()->mEngine1->getDamage() * 100);
			pText->setText("Dam = " + StringConverter::toString(D) + "%");
		}
		// *****************************************
		// Valeur des dommages du moteur 2
		// *****************************************
		else if (Order=="@dmgValue(2)")
		{	
			int D = (TheShip::getSingletonPtr()->mEngine2->getDamage() * 100);
			pText->setText("Dam = " + StringConverter::toString(D) + "%");
		}
		// *****************************************
		// Temperature du moteur 1
		// *****************************************
		else if (Order=="@temperature(1)")
		{	
			int T = TheShip::getSingletonPtr()->mEngine1->getTemperature();
			pText->setText("temp: "+StringConverter::toString(T)+"K");
			if (T < MIN_ENGINE_TEMP)
				pText->setBackgroundColours(mYellow);
			else if (T < MAX_ENGINE_TEMP)
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mRed);
		}
		// *****************************************
		// Temperature du moteur 2
		// *****************************************
		else if (Order=="@temperature(2)")
		{	
			int T = TheShip::getSingletonPtr()->mEngine2->getTemperature();
			pText->setText("temp: "+StringConverter::toString(T)+"K");
			if (T < MIN_ENGINE_TEMP)
				pText->setBackgroundColours(mYellow);
			else if (T < MAX_ENGINE_TEMP)
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mRed);
		}
		// *****************************************
		// Voyant coloré suivant la temperature du moteur 1
		// *****************************************
		else if (Order=="@temp(1)")
		{	
			pText->setText("temp: 54510K");
			if (TheShip::getSingletonPtr()->mEngine1->getTemperature() < MIN_ENGINE_TEMP)
				pText->setBackgroundColours(mYellow);
			else if (TheShip::getSingletonPtr()->mEngine1->getTemperature() < MAX_ENGINE_TEMP)
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mRed);
		}
		// *****************************************
		// Voyant coloré suivant la temperature du moteur 2
		// *****************************************
		else if (Order=="@temp(2)")
		{	
			if (TheShip::getSingletonPtr()->mEngine2->getTemperature() < MIN_ENGINE_TEMP)
				pText->setBackgroundColours(mYellow);
			else if (TheShip::getSingletonPtr()->mEngine2->getTemperature() < MAX_ENGINE_TEMP)
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mRed);
		}
		// *****************************************
		// Voyant coloré suivant le chauffage du moteur 1
		// *****************************************
		else if (Order=="@heat(1)")
		{	
			if (TheShip::getSingletonPtr()->mEngine1->isEngineHeating())
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mBlack);
		}
		// *****************************************
		// Voyant coloré suivant le chauffage du moteur 2
		// *****************************************
		else if (Order=="@heat(2)")
		{	
			if (TheShip::getSingletonPtr()->mEngine2->isEngineHeating())
				pText->setBackgroundColours(mGreen);
			else
				pText->setBackgroundColours(mBlack);
		}
		// *****************************************
		// On affiche le mode des moteurs 
		// *****************************************
		else if (Order=="@getModeName(1)")
		{	
			pText->setText(TheShip::getSingletonPtr()->mEngine1->getModeName());
		}
		else if (Order=="@getModeName(2)")
		{	
			pText->setText(TheShip::getSingletonPtr()->mEngine2->getModeName());
		}

		// *****************************************
		// On affiche le mode selectionné dans la listbox
		// *****************************************
		else if (Order=="@selectedmode")
		{	
			pText->setText(mModeText);
			if (mSelectedMode<0) pText->setBackgroundColours(mYellow);
			else pText->setBackgroundColours(mGreen);
		}	
		// *****************************************
		// On affiche la puissance demandée au moteur (selon la position du slider)
		// *****************************************
		else if (Order=="@activity")
		{	
			// On modifie le Range du slider de "puissance moteur" en fonction du mode
			ShipEngine::tMode mode = static_cast<ShipEngine::tMode>(mSelectedMode);
			Real max_thrust = TheShip::getSingletonPtr()->mEngine1->getModeCoeff(mode);
			pText->setText(StringConverter::toString(int(mSliderValue[0]*max_thrust)) +" %");
		}
		// *****************************************
		// On affiche la puissance reelle des moteurs
		// *****************************************
		else if (Order=="@realactivity")
		{	
			// On calcule la moyenne des activity des moteurs
			Real value = TheShip::getSingletonPtr()->mEngine1->getRealActivity() + TheShip::getSingletonPtr()->mEngine2->getRealActivity();
			value = value*50;     // note = /2 *100
			pText->setText("set: "+StringConverter::toString(int(value))+"%");
		}
		// *****************************************
		// On affiche la puissance reelle d'un moteur
		// *****************************************
		else if (Order=="@realactivity(1)")
		{	
			ShipEngine* Element = TheShip::getSingletonPtr()->mEngine1;
			int value = Element->getRealActivity()*100;
			int damge = Element->getDamage()*100;
			pText->setText("Thrust: "+StringConverter::toString(value)+"% \nof "+StringConverter::toString(100-damge)+"% max");
		}
		// *****************************************
		// On affiche la puissance reelle d'un moteur
		// *****************************************
		else if (Order=="@realactivity(2)")
		{	
			ShipEngine* Element = TheShip::getSingletonPtr()->mEngine2;
			int value = Element->getRealActivity()*100;
			int damge = Element->getDamage()*100;
			pText->setText("Thrust: "+StringConverter::toString(value)+"% \nof "+StringConverter::toString(100-damge)+"% max");
		}
		// *****************************************
		// On affiche la Poussée d'un moteur
		// *****************************************
		else if (Order=="@push(1)")
		{	
			ShipEngine* Element = TheShip::getSingletonPtr()->mEngine1;
			int value = Element->getValue();
			int damge = Element->getDamage()*100;
			pText->setText("Push: "+StringConverter::toString(value) + Element->getUnit()+" \nof "+StringConverter::toString(Element->getRealMaxValue())+ Element->getUnit()+" max");
	
		}
		// *****************************************
		// On affiche la Poussée d'un moteur
		// *****************************************
		else if (Order=="@push(2)")
		{	
			ShipEngine* Element = TheShip::getSingletonPtr()->mEngine2;
			int value = Element->getValue();
			int damge = Element->getDamage()*100;
			pText->setText("Push: "+StringConverter::toString(value) + Element->getUnit()+" \nof "+StringConverter::toString(Element->getRealMaxValue())+ Element->getUnit()+" max");
		}
		  
		  // *****************************************
		// On affiche la vitesse du vaisseau
		// *****************************************
		else if (Order=="@speed")
		{	
			Real unit = 1;
			int speed = TheShip::getSingletonPtr()->mSpeedVector.squaredLength();
			// On determine les unitées les plus adaptées à la vitesse du vaisseau
			if (speed < 1000)       unit = 1;
			else if (speed < 10000) unit = 10;
			if (TheShip::getSingletonPtr()->mSpeedVector.z<0) unit *= -1;
			pText->setText(StringConverter::toString(speed*unit));
		}
		// *****************************************
		// On affiche les unitées les plus adaptées à la vitesse du vaisseau
		// *****************************************
		else if (Order=="@speedunit")
		{	
			pText->setText("km/h");
			/// TODO    pText->setText("al");
		}
	}
	// *****************************************
	// Gestion des textes statiques (textes de la listbox, etc)
	// *****************************************
	else 
	{	
		// On considère que les seuls StaticText que l'on a dans cette GUI sont ceux du Mode.
		mModeText = Order;
		pText->setText(Order);
	}
}


/* ------------------------------------------------------------------------- */
/// Gestion des Listbox de la GUI: remplissage des items.
/* ------------------------------------------------------------------------- */
void Computer_Navigation::showDynamicListbox(CEGUI::Listbox* pListbox, String Order) 
{

	// TRACE (">> Computer_Navigation::showDynamicListbox("+Order+")" );

	ShipEngine::tMode mode1 = TheShip::getSingletonPtr()->mEngine1->getMode();
	ShipEngine::tMode mode2 = TheShip::getSingletonPtr()->mEngine2->getMode();

	// Si les deux moteurs sont à l'arret: on a le menu STOPPPED
	if ((mode1==ShipEngine::STOPPED_ENGINE) && (mode2==ShipEngine::STOPPED_ENGINE))
	{
		mSelectedMode = -2;
		mModeText     = "ENGINES OFF";
		fillListbox(pListbox, "menuModeStopped");
	}

	// Si l'un des deux moteurs chauffe, on a le menu HEATING
	else if ((mode1==ShipEngine::HEATING_ENGINE) || (mode2==ShipEngine::HEATING_ENGINE))
	{
		mSelectedMode = -3;
		mModeText     = "ENGINES WARMING UP";
		fillListbox(pListbox, "menuModeHeating");
	}

	// Si les deux moteurs sont froids: on a le menu COLD
	else if (   (TheShip::getSingletonPtr()->mEngine1->getTemperature()<MIN_ENGINE_TEMP)
	         && (TheShip::getSingletonPtr()->mEngine2->getTemperature()<MIN_ENGINE_TEMP) )
	{
		mSelectedMode = -4;
		mModeText     = "ENGINES NOT READY";
		fillListbox(pListbox, "menuModeCold");
	}

	// Sinon, si la listbox est encore vide, on met le menu normal de selection du mode Moteur.
	else if (pListbox->getItemCount()==0)
	{
		mModeText     = "ENGINE MODE: ";
		fillListbox(pListbox, "menuMode");
	}

	
	// Si le mode n'est pas encore choisi (valeurs positives) on se positionne sur un mode neutre
	// on a le menu normal de selection du mode Moteur.
	else if (mSelectedMode <0) 
	{
			mSelectedMode = 0;
			mModeText     = "SELECT ENGINE MODE:";
			fillListbox(pListbox, "menuMode");
	}
	// TRACE ("<< Computer_Navigation::showDynamicListbox()" );
}



/* ------------------------------------------------------------------------- */
/// Gestion des PushButtons de la GUI
/* ------------------------------------------------------------------------- */
void Computer_Navigation::showDynamicPushButton(CEGUI::PushButton* pPushButton, String Order) 
{
	// *****************************************
	// On affiche des dommages des moteurs 
	// *****************************************
	if (Order=="@dmgValue(1)")
	{	
		int D = (TheShip::getSingletonPtr()->mEngine1->getDamage() * 100);
		pPushButton->setText("Dmg = " + StringConverter::toString(D) + "%");
	}
	else if (Order=="@dmgValue(2)")
	{	
		int D = (TheShip::getSingletonPtr()->mEngine2->getDamage() * 100);
		pPushButton->setText("Dmg = " + StringConverter::toString(D) + "%");
	}

};



/* ------------------------------------------------------------------------- */
/// Gestion des ProgressBar de la GUI
/* ------------------------------------------------------------------------- */
void Computer_Navigation::showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order)
{

	// *****************************************
	// On affiche la puissance des moteurs 
	// *****************************************
	if (Order=="@power(1)")
	{	
		pProgressBar->setProgress(TheShip::getSingletonPtr()->mEngine1->getRealActivity());
	}
	else if (Order=="@power(2)")
	{	
		pProgressBar->setProgress(TheShip::getSingletonPtr()->mEngine2->getRealActivity());
	}
	// *****************************************
	// On affiche la temperature des moteurs 
	// *****************************************
	else if (Order=="@temp(1)")
	{	
		pProgressBar->setProgress(TheShip::getSingletonPtr()->mEngine1->getTemperature()/MAX_ENGINE_TEMP);
	}
	else if (Order=="@temp(2)")
	{	
		pProgressBar->setProgress(TheShip::getSingletonPtr()->mEngine2->getTemperature()/MAX_ENGINE_TEMP);
	}
	// *****************************************
	// On affiche des dommages des moteurs 
	// *****************************************
	else if (Order=="@dmg(1)")
	{	
		pProgressBar->setProgress(1-TheShip::getSingletonPtr()->mEngine1->getDamage());
	}
	else if (Order=="@dmg(2)")
	{	
		pProgressBar->setProgress(1-TheShip::getSingletonPtr()->mEngine2->getDamage());
	}
}


/* ------------------------------------------------------------------------- */
/// Mise a jour régulière de l'interface du Computer
/* ------------------------------------------------------------------------- */
bool Computer_Navigation::frameStarted(const FrameEvent& evt)
{
	// On rafraichit le contenu de la fenetre principale
	this->refreshTexts("mainWin");
	this->refreshPushButton("mainWin");
	this->refreshProgressBar("mainWin");

	mRefreshTimer += evt.timeSinceLastFrame;
	if (mRefreshTimer>2) 
	{
		// Toutes les deux secondes, on rafraichit la listbox des modes moteur
		if (mSelectedMode < 0) this->refreshListboxes("mainWin");
		mRefreshTimer=0;
	}

	// On rafraichit le contenu de la fenetre Annexe
	switch(mSelectedOrder) 
	{
	case 0:
		this->refreshTexts("DetailWin");
		break;
	case 1:
		this->refreshTexts("ProgWin");
		break;
	case 2:
		this->refreshTexts("CurveWin");
		break;
	case 3:
		this->refreshTexts("CompassWin");
		break;
	case 4:
		this->refreshTexts("BoostWin");
		break;
	}

	return true;
}




/* ------------------------------------------------------------------------- */
/// Polymorphe de la fonction de creation d'interface, avec en plus le lancement d'un listener.
/* ------------------------------------------------------------------------- */
void Computer_Navigation::createInterface()
{
	// Appel de la fonction (virtuelle) de base
	Interface::createInterface();

	// Lancement d'un Listener
	Root::getSingletonPtr()->addFrameListener(this);

	// On initialise la position du slider
	mSlider[0]->setCurrentValue(mThrustSlider);	
}


/* ------------------------------------------------------------------------- */
/// Polymorphe de la fonction de destruction d'interface, avec en premier l'arret du listener.
/* ------------------------------------------------------------------------- */
void Computer_Navigation::deleteInterface()
{
	TRACE(">> Computer_Navigation::deleteInterface()");
	// On memorise la position actuelle du curseur
	if (mSlider[0]) mThrustSlider = mSlider[0]->getCurrentValue();	

	// Arret du Listener
	Root::getSingletonPtr()->removeFrameListener(this);
	// Appel de la fonction (virtuelle) de base
	Interface::deleteInterface();
}
