/* ------------------------------------------------------------------------- */
// File       : Robot_Interface.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Robot_Interface.h"

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include "CEGUI/elements/CEGUIStaticImage.h"
#include "CEGUI/elements/CEGUIStaticText.h"
#include <OgreMemoryMacros.h>

#include "Robot.h"
#include "theShip.h"
#include "macros.h"

/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Robot_Interface::Robot_Interface(void* pFriend, String InterfaceFile) : Interface()
{
	// Le constructeur de la surclasse INTERFACE s'exécute automatiquement
	mRobot   = pFriend;
	mGUIname = static_cast<Robot*>(pFriend)->mRobotName;
	// Chargement de la description de l'interface
	Interface::loadInterfaceFile(InterfaceFile);
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Robot_Interface::~Robot_Interface()
{
	// Le destructeur de la surclasse INTERFACE s'exécute automatiquement
}


/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Robot_Interface::executeOrder(String Order)
{
	// On extrait les paramètres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");

	
	if (FuncV.at(0)=="exitGUI")
	{
		// On masque l'interface.GUI
		((Robot*)mRobot)->requestExitInterface(0.7);
	}

	else if (FuncV.at(0)=="work")
	{
		((Robot*)mRobot)->work(true);
	}

	else if (FuncV.at(0)=="alarm")
	{
		SubEntity* su = ((Robot*)mRobot)->getSubEntity("adRobotInside");
		su->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName("adRobotColorInRed.jpg");
	}
	/************************************************************************/
	/* Joue une animation.                                                  */
	/************************************************************************/
	else if (FuncV.at(0)=="playAnim")
	{
		((Robot*)mRobot)->startAnimation(FuncV.at(1), false);
	}
	/************************************************************************/
	/* Joue une animation en boucle.                                        */
	/************************************************************************/
	else if (FuncV.at(0)=="loopAnim")
	{
		((Robot*)mRobot)->startAnimation(FuncV.at(1), true);
	}
	/************************************************************************/
	/* Démarre l'animation.                                        */
	/************************************************************************/
	else if (FuncV.at(0)=="startAnim")
	{
		((Robot*)mRobot)->mAnimationSpeed = 2;
	}
	/************************************************************************/
	/* Va rejoindre un Node nommé.                                          */
	/************************************************************************/
	else if (FuncV.at(0)=="gotoNode")
	{
		((Robot*)mRobot)->gotoNode(FuncV.at(1));
		((Robot*)mRobot)->requestExitInterface(1.5);
		((Robot*)mRobot)->setActionString("going to "+FuncV.at(1));
	}

	else if (FuncV.at(0)=="shutdown")
	{
		if (FuncV.at(1)=="false") 
		{
			((Robot*)mRobot)->activateRobot();
			((Robot*)mRobot)->requestExitInterface(0.7);
		}
		if (FuncV.at(1)=="true" ) 
		{
			((Robot*)mRobot)->deactivateRobot();
			((Robot*)mRobot)->requestExitInterface(0.7);
		}
	}

	else if (FuncV.at(0)=="setBroken")
	{
		if (FuncV.at(1)=="true" ) ((Robot*)mRobot)->setBroken(true);
		if (FuncV.at(1)=="false") ((Robot*)mRobot)->setBroken(false);
		((Robot*)mRobot)->setActionString("malfunction.");
	}

	else if (FuncV.at(0)=="autoRepair")
	{
		((Robot*)mRobot)->setBroken(false);
		((Robot*)mRobot)->requestExitInterface(1.5);
	}

	else if (FuncV.at(0)=="cancelOrders")
	{
		((Robot*)mRobot)->setEngineOff();
		((Robot*)mRobot)->setActionString("Orders where cancelled. Awaiting new orders.");
	}

	else if (FuncV.at(0)=="getShipStatus")
	{
		((Robot*)mRobot)->playSound(Robot::ANALYSE);
		mXMLinterface->addXmlData(TheShip::getSingletonPtr()->getStatus(), "status");
	}

	else if (FuncV.at(0)=="repair")
	{
		// En parametre de repair, on trouve l'ID de l'item qui permet de retrouver les 
		// autres informations pour cette réparation dans les données XML
		int itemID = StringConverter::parseInt(FuncV.at(1));
		String location = mXMLinterface->getItemStringAttribute(itemID ,"location");
		String objet = mXMLinterface->getItemTextContent(itemID);
		if (location != "")
		{
			((Robot*)mRobot)->gotoNode(location);	
			((Robot*)mRobot)->requestExitInterface(5);
			((Robot*)mRobot)->setActionString("going to repair " + objet);
		}
	}

	else if (FuncV.at(0)=="save")
	{
		mXMLinterface->saveToFile("E:/mytest-doc.xml");	// TODO: pour les tests
	}

}


/* ------------------------------------------------------------------------- */
/// On affiche le texte demandé (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Robot_Interface::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	if (StringUtil::startsWith(Order,"@"))
	{
		pText->setTextColours(mYellow);
		if (Order == "@ActionString") pText->setText(((Robot*)mRobot)->getActionString());
	}
	else
	{
		// sinon, on peut afficher le texte tel quel
		pText->setTextColours(mWhite);
		pText->setText(Order);
	}
}


/* ------------------------------------------------------------------------- */
/// On affiche l'image demandée (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Robot_Interface::showDynamicImage(CEGUI::StaticImage* pImage, String Order)
{
		pImage->setImage("RobotImagery","empty");
}


/* ------------------------------------------------------------------------- */
/// On remplit la Listbox
/* ------------------------------------------------------------------------- */
void Robot_Interface::showDynamicListbox(CEGUI::Listbox* pListbox, String Order)
{
	TRACE(">> showDynamicListbox()");

	if      (((Robot*)mRobot)->isDown())     fillListbox(pListbox, "menuDown");
	else if (((Robot*)mRobot)->isBroken())   fillListbox(pListbox, "menuBroken");
	else                                     fillListbox(pListbox, "menu");

}


