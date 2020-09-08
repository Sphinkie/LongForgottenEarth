/* ------------------------------------------------------------------------- */
// File       : Simple_Interface.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Simple_Interface.h"

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include "CEGUI/elements/CEGUIStaticImage.h"
#include "CEGUI/elements/CEGUIStaticText.h"
#include <OgreMemoryMacros.h>

#include "macros.h"
#include "specialFX.h"


/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Simple_Interface::Simple_Interface(String InterfaceFile) : Interface()
{
	// Le constructeur de la surclasse INTERFACE s'exécute automatiquement
	mGUIname = "simpleWin";
	// Chargement de la description de l'interface
	Interface::loadInterfaceFile(InterfaceFile);
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Simple_Interface::~Simple_Interface()
{
	// Le destructeur de la surclasse INTERFACE s'exécute automatiquement
}



/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Simple_Interface::executeOrder(String Order)
{
	// On extrait les paramètres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");


	if (FuncV.at(0)=="exitGUI")
	{}


}


/* ------------------------------------------------------------------------- */
/// On affiche le texte demandé (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Simple_Interface::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	if (StringUtil::startsWith(Order,"@"))
	{
		// Pour le HelpScreen
		if (Order == "@showHelp") 
		{
			pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
			pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapLeftAligned);
			pText->setVerticalScrollbarEnabled(true);
			pText->setText(readFile("../media/texts/help.txt"));
		}
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
void Simple_Interface::showDynamicImage(CEGUI::StaticImage* pImage, String Order)
{
	DEBUG_LOG(">> Simple_Interface::showDynamicImage()");
}


/* ------------------------------------------------------------------------- */
/// On remplit la Listbox
/* ------------------------------------------------------------------------- */
void Simple_Interface::showDynamicListbox(CEGUI::Listbox* pListbox, String Order)
{
	DEBUG_LOG(">> Simple_Interface::showDynamicListbox()");
}


