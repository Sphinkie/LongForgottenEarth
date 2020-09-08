/* ------------------------------------------------------------------------- */
// File       : Interface.cpp
// Project    : Long Forgotten Earth 
// Auteur     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Interface.h"
#include "macros.h"

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUIWindowManager.h> 
#include <CEGUI/CEGUIWindow.h> 
#include <CEGUI/CEGUIExceptions.h>
#include "CEGUI/elements/CEGUIStaticImage.h"
#include "CEGUI/elements/CEGUIStaticText.h"
#include "CEGUI/elements/CEGUIPushButton.h"
#include "CEGUI/elements/CEGUISlider.h"
#include "CEGUI/elements/CEGUIMultiLineEditbox.h"
#include "CEGUI/elements/CEGUIEditbox.h"
#include "CEGUI/elements/CEGUIListBox.h"
#include "CEGUI/elements/CEGUIListBoxItem.h"
#include "CEGUI/elements/CEGUIListBoxTextItem.h"
#include "CEGUI/elements/CEGUIProgressBar.h"
#include "CEGUI/elements/CEGUIScrollablePane.h"
#include <OgreMemoryMacros.h>


#undef  DEBUG_LOG
#define DEBUG_LOG TRACE


/* ------------------------------------------------------------------------- */
///	Constructor
/* ------------------------------------------------------------------------- */
Interface::Interface()		// : mGUIRenderer(NULL)
{
	DEBUG_LOG(">> Interface()");
	mExistGUI    = false;
	mAnnexWindow = NULL;
	mXMLinterface= NULL;
	for (int i=0; i<5; mSlider[i++]=NULL);
	for (int j=0; j<5; mSliderValue[j++]=0);
	DEBUG_LOG("<< Interface");
}


/* ------------------------------------------------------------------------- */
///	Destructor
/* ------------------------------------------------------------------------- */
Interface::~Interface()
{
	// On detruit le gestionnaire XML
	delete mXMLinterface ;

	// On masque le curseur de souris
	mWinMgr->getWindow(mGUIname+"/mainWin")->setMouseCursor(CEGUI::BlankMouseCursor);

	// On detruit le fenetre et tout le reste (y compris imageset)
	this->deleteInterface();
}


/* ------------------------------------------------------------------------- */
///	Chargement du fichier XML decrivant l'interface à afficher. Should be called FIRST.
/* ------------------------------------------------------------------------- */
void Interface::loadInterfaceFile(String InterfaceFile)
{
	DEBUG_LOG(">> loadInterfaceFile("+InterfaceFile+")");
	// On parse le fichier XML qui contient la description de l'interface
	mXMLinterface = new Interface_XMLParser(InterfaceFile);
}


/* ------------------------------------------------------------------------- */
///	Creation d'une fenetre par defaut
/* ------------------------------------------------------------------------- */
void Interface::createGUIWindows()
{
	using namespace CEGUI;

	if (!mXMLinterface) return;

	try
	{
		TRACE(">> createGUIWindows()");

		// On memorise un pointeur sur le CEGUI-Window-Manager
		mWinMgr = CEGUI::WindowManager::getSingletonPtr();
		// On memorise la fenetre ROOT de CEGUI 
		mRootWindow = mWinMgr->getWindow("root_wnd");
		
		// On crée une fenetre fixe
		mInterfaceWindow = (StaticText*)WindowManager::getSingleton().createWindow("TaharezLook/StaticText", mGUIname+"/mainWin");

		mRootWindow->addChildWindow(mInterfaceWindow);		

		// On extrait les élements concernant cette fenetre dans le fichier XML
		mXMLinterface->gotoElement("mainWin");

		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		mInterfaceWindow->setPosition(Point(posX, posY));

		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		mInterfaceWindow->setSize(Size(width, high));

		mInterfaceWindow->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		mInterfaceWindow->setInheritsAlpha(true);

		// On peut definir une image de fond
		Ogre::String Backgroundimage = mXMLinterface->getStringAttribute("backgroundimage");
		if (Backgroundimage!="") mInterfaceWindow->setBackgroundImage(mImageSetName, Backgroundimage);

		// Le fond de la fenetre doit-il apparaitre... (par defaut: oui)
		bool background = mXMLinterface->getBooleanAttribute("background",true);
		mInterfaceWindow->setBackgroundEnabled(background);

		// Le contour de la fenetre doit-il apparaitre... (par defaut: oui)
		bool frame = mXMLinterface->getBooleanAttribute("frame",true);
		mInterfaceWindow->setFrameEnabled(frame);

		// Affichage du titre
		Ogre::String Title = mXMLinterface->getTitle();
		mInterfaceWindow->setVerticalFormatting(StaticText::TopAligned);
		mInterfaceWindow->setHorizontalFormatting(StaticText::WordWrapCentred);
		//mInterfaceWindow->setTextColours(mBlack);
		if (StringUtil::startsWith(Title,"@")) showDynamicText(mInterfaceWindow, Title);
		else	mInterfaceWindow->setText(Title);

		// On définit les couleurs par defaut
		this->setDefaultColours();

		// On affiche le curseur de souris
		CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

		TRACE("<< createGUIWindows()");
	}
	catch(CEGUI::Exception)
	{
		// catch to prevent exit (errors will be logged).
		DEBUG_LOG ("CEGUI Exception in createGUIWindows()");
	}

}

/* ------------------------------------------------------------------------- */
/// Creation des éléments (widgets) de la fenetre
/* ------------------------------------------------------------------------- */
void Interface::createGUIWidgets(String guiWindow)
{
	using namespace CEGUI;
	int   index;

	TRACE(">> createGUIWidgets("+guiWindow+")");
	CEGUI::StaticText* InterfaceWindow = (StaticText*)mWinMgr->getWindow(mGUIname+"/"+guiWindow);
	
	// cas particulier de la fenetre principale
	if (guiWindow=="mainWin") InterfaceWindow =mInterfaceWindow;
	// Cas particulier du pointeur NULL
	if (!InterfaceWindow) return;
	// Cas particulier d'une mauvaise initialisation
	if (!mXMLinterface) return;

	mXMLinterface->gotoElement(guiWindow);

	/* ------------------------------ */
	/* CREATION DES STATIC-IMAGES     */
	/* ------------------------------ */
	// On cree l'image en premier, sinon elle masque les autres widgets
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <image>
	index = 0;
	while (mXMLinterface->gotoSubElement("image",index))
	{
		// On définit un ID pour ce widget
		Ogre::String WidgetID = mGUIname+"/"+guiWindow+"/image/"+ITOA(index);
		// On cree le StaticImage
		StaticImage* widget = (StaticImage*)mWinMgr->createWindow("TaharezLook/StaticImage", WidgetID);

		/* Gestion des images avec scroll */
		if (mXMLinterface->getBooleanAttribute("scroll"))
		{
			// On définit un ID pour le pane
			Ogre::String PaneID  = mGUIname+"/"+guiWindow+"/scrollpane/"+ITOA(index);
			ScrollablePane* pane = (ScrollablePane*)mWinMgr->createWindow("TaharezLook/ScrollablePane", PaneID);
			// Le scrollPane a la dimension voulue sur l'écran.
			Real posX  = mXMLinterface->getRealAttribute("x");
			Real posY  = mXMLinterface->getRealAttribute("y");
			Real width = mXMLinterface->getRealAttribute("w");
			Real high  = mXMLinterface->getRealAttribute("h");
			pane->setMetricsMode(CEGUI::Absolute); /* ? */
			pane->setPosition(Point(posX,posY));
			pane->setSize(Size(width, high));
			pane->setVisible(true); 
			pane->setEnabled(false);	// par defaut, on ne peut pas cliquer dessus
			pane->setContentPaneAutoSized(false); 
			pane->setContentPaneArea(CEGUI::Rect(0, 0, width*2, high)); /* left-top-right-bottom */
			pane->addChildWindow(widget);
		}
		else
			InterfaceWindow->addChildWindow(widget);

		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setMaximumSize(Size(1,1));
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		widget->setFrameEnabled(false);
		widget->setBackgroundEnabled(false);
		widget->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		// Par defaut les images sont opaques. Il faut préciser leur transparence individuellement.
		widget->setInheritsAlpha(false);
		widget->setEnabled(false);	// par defaut, on ne peut pas cliquer dessus
		index++;

	}

	/* --------------------------------- */
	/* CREATION DES SCROLLING-IMAGES     */
	/* --------------------------------- */
	// On cree l'image en premier, sinon elle masque les autres widgets
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <scrollimage>
	index = 0;
	while (mXMLinterface->gotoSubElement("scrollimage",index))
	{
		// On définit un ID pour ce widget
		Ogre::String WidgetID = mGUIname+"/"+guiWindow+"/scrollpane/"+ITOA(index);
		// On cree le ScrollablePane
		ScrollablePane* widget = (ScrollablePane*)mWinMgr->createWindow("TaharezLook/ScrollablePane", WidgetID);
		// Le scrollPane a la dimension voulue sur l'écran.
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setMetricsMode(CEGUI::Absolute); /* ? */
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		widget->setVisible(true); 
		widget->setEnabled(false);	// par defaut, on ne peut pas cliquer dessus
		widget->setContentPaneAutoSized(false); 

		// On cree une image statique pour ce Pane
		Ogre::String imageID = mGUIname+"/"+guiWindow+"/image/"+ITOA(index);
		// On cree le StaticImage, attaché au ScrollablePane
		StaticImage* image = (StaticImage*)mWinMgr->createWindow("TaharezLook/StaticImage", imageID);
		widget->addChildWindow(image);
		image->setMaximumSize(Size(1,1));
		image->setFrameEnabled(false);
		image->setBackgroundEnabled(false);
		image->setInheritsAlpha(false);
		image->setEnabled(false);	// par defaut, on ne peut pas cliquer dessus
		
		// On defini un contenu pour le scrollPane (plus grand que ce dernier)
		// widget->setContentPaneArea(CEGUI::Rect(Point(10.0,0), Size(width-10, high))); 
		widget->setContentPaneArea(CEGUI::Rect(0, 0, width*3, high)); /* left-top-right-bottom */
}

	/* ------------------------------ */
	/* CREATION DES STATIC-TEXTS      */
	/* ------------------------------ */
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <text>
	index = 0;
	while (mXMLinterface->gotoSubElement("text",index))
	{
		// On définit un ID pour ce texte
		Ogre::String ButtonID = mGUIname+"/"+guiWindow+"/text/"+ITOA(index);
		
		// On cree le static text
		StaticText* widget = (StaticText*)mWinMgr->createWindow("TaharezLook/StaticText", ButtonID);
		InterfaceWindow->addChildWindow(widget);
		// On positionne les parametres standards
		widget->setMaximumSize(Size(1,1));
		widget->setFrameEnabled(mXMLinterface->getBooleanAttribute("frame", false));
		widget->setBackgroundEnabled(mXMLinterface->getBooleanAttribute("background", true));
		if (mXMLinterface->getBooleanAttribute("centered", true))
			widget->setHorizontalFormatting(StaticText::WordWrapCentred);
		else
			widget->setHorizontalFormatting(StaticText::WordWrapLeftAligned);
	
		widget->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		// On positionne le texte
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setFont(mXMLinterface->getStringAttribute("font"));
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		// On met les couleurs
		widget->setBackgroundColours(this->getColour(mXMLinterface->getStringAttribute("colour")));	
		widget->setTextColours(this->getColour(mXMLinterface->getStringAttribute("pen")));
		index++;
	}


	/* ------------------------------ */
	/* CREATION DES EDIT-BOX          */
	/* ------------------------------ */
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <edit>
	index = 0;
	while (mXMLinterface->gotoSubElement("edit",index))
	{
		// On définit un ID pour ce texte
		Ogre::String ButtonID = mGUIname+"/"+guiWindow+"/edit/"+ITOA(index);
		
		// On cree la Edit-Box
		Editbox* widget = (Editbox*)mWinMgr->createWindow("TaharezLook/Editbox", ButtonID);
		InterfaceWindow->addChildWindow(widget);
		// On positionne les parametres standards
		widget->setMaximumSize(Size(1,1));
		widget->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		// On positionne la box
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setFont(mXMLinterface->getStringAttribute("font"));
		widget->setNormalTextColour(getColour(mXMLinterface->getStringAttribute("pen")));
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		index++;
	}

	/* --------------------------------- */
	/* CREATION DES EDIT-BOX MULTILIGNES */
	/* --------------------------------- */
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <medit>
	int mindex = 0; 
	while (mXMLinterface->gotoSubElement("medit",mindex))
	{
		// On définit un ID pour ce texte
		Ogre::String ButtonID = mGUIname+"/"+guiWindow+"/edit/"+ITOA(index); //On utilise les index à la suite des Editbox
		
		// On cree la Edit-Box
		MultiLineEditbox* widget = (MultiLineEditbox*)mWinMgr->createWindow("TaharezLook/MultiLineEditbox", ButtonID);
		InterfaceWindow->addChildWindow(widget);
		// On positionne les parametres standards
		widget->setMaximumSize(Size(1,1));
		widget->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		// On positionne la box
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setFont(mXMLinterface->getStringAttribute("font"));
		widget->setNormalTextColour(getColour(mXMLinterface->getStringAttribute("pen")));
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		widget->setWordWrapping(true);
		index++; mindex++;
	}


	/* ------------------------------ */
	/* CREATION DES PUSH-BUTTONS      */
	/* ------------------------------ */
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <pushbutton>
	index = 0;
	while (mXMLinterface->gotoSubElement("pushbutton",index))
	{
		// On définit un ID pour ce bouton
		Ogre::String ButtonID = mGUIname+"/"+guiWindow+"/push/"+ITOA(index);
		// On cree le Push bouton
		PushButton* widget = (PushButton*)mWinMgr->createWindow("TaharezLook/Button", ButtonID);
		InterfaceWindow->addChildWindow(widget);
		// On positionne les parametres standards
		widget->setMaximumSize(Size(1,1));
		widget->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		// On positionne le bouton
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setFont(mXMLinterface->getStringAttribute("font"));
		widget->setNormalTextColour(getColour(mXMLinterface->getStringAttribute("pen")));
		widget->setText(mXMLinterface->getTextContent());
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		// On cree son wiring
		widget->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&Interface::handlePushButton, this));
		index++;
	}

	/* ------------------------------ */
	/* CREATION DES SLIDERS (CURSEURS)*/
	/* ------------------------------ */
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <slider>
	index = 0;
	while (mXMLinterface->gotoSubElement("slider",index))
	{
		// On définit un ID pour ce widget
		Ogre::String ButtonID = mGUIname+"/"+guiWindow+"/slider/"+ITOA(index);
		// On cree le slider
		Slider* widget = (Slider*)mWinMgr->createWindow("TaharezLook/Slider", ButtonID);
		InterfaceWindow->addChildWindow(widget);
		// On positionne les parametres standards
		widget->setMaximumSize(Size(1,1));
		widget->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		// On positionne le slider
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		// On initialise le curseur
		widget->setCurrentValue(0);
		mSliderValue[index] = 0;
		// On memorise le pointeur
		mSlider[index] = widget;
		// On cree son wiring
		widget->subscribeEvent(Slider::EventValueChanged, Event::Subscriber(&Interface::handleSlider, this));
		index++;
	}

	
	/* ------------------------------ */
	/* CREATION DES LISTBOX           */
	/* ------------------------------ */
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <listbox>
	index = 0;
	while (mXMLinterface->gotoSubElement("listbox",index))
	{
		// On définit un ID pour ce widget
		Ogre::String ButtonID = mGUIname+"/"+guiWindow+"/listbox/"+ITOA(index);
		// On cree le slider
		Listbox* widget = (Listbox*)mWinMgr->createWindow("TaharezLook/Listbox", ButtonID);
		InterfaceWindow->addChildWindow(widget);
		// On positionne les parametres standards
		widget->setMaximumSize(Size(1,1));
		// On positionne le bouton
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
//		widget->setBackgroundEnabled(mXMLinterface->getBooleanAttribute("background", true));
		// On cree son wiring
		widget->subscribeEvent(Listbox::EventSelectionChanged, Event::Subscriber(&Interface::handleListbox, this));
		index++;
	}


	/* ---------------------------------- */
	/* CREATION DES BARRES DE PROGRESSION */
	/* ---------------------------------- */
	// On parcourt la section <guiWindow> du fichier XML en traitant tous les <ProgressBar>
	index = 0;
	while (mXMLinterface->gotoSubElement("ProgressBar",index))
	{
		// On définit un ID pour ce widget
		Ogre::String ButtonID = mGUIname+"/"+guiWindow+"/ProgressBar/"+ITOA(index);
		// On cree le slider
		//ProgressBar* widget = (ProgressBar*)mWinMgr->createWindow("TaharezLook/AlternateProgressBar", ButtonID);
		ProgressBar* widget = (ProgressBar*)mWinMgr->createWindow("TaharezLook/ProgressBar", ButtonID);
		InterfaceWindow->addChildWindow(widget);
		// On positionne les parametres standards
		widget->setMaximumSize(Size(1,1));
		// On positionne le widget
		Real posX  = mXMLinterface->getRealAttribute("x");
		Real posY  = mXMLinterface->getRealAttribute("y");
		Real width = mXMLinterface->getRealAttribute("w");
		Real high  = mXMLinterface->getRealAttribute("h");
		widget->setPosition(Point(posX,posY));
		widget->setSize(Size(width, high));
		widget->setProgress(0.0);	// valeur par defaut: zero
		widget->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
		index++;
	}


	/* ------------------------------------------------------------------------- */
	// On met à jour les widgets affichés à l'écran
	/* ------------------------------------------------------------------------- */
	this->refreshTexts(guiWindow);
	this->refreshImages(guiWindow);
	this->refreshListboxes(guiWindow);
	this->refreshEditboxes(guiWindow);

	TRACE("<< createGUIWidgets()");
}




/* ------------------------------------------------------------------------- */
/// Handle: Gestion des PushButtons
/* ------------------------------------------------------------------------- */
bool Interface::handlePushButton(const CEGUI::EventArgs& e)
{
	TRACE(">> handlePushButton()");

	CEGUI::PushButton*   HandledWidget = (CEGUI::PushButton*)((CEGUI::WindowEventArgs&)e).window;

	// Le nom de l'objet est du type "bed-G2/mainWin/push/12". 
	String ButtonName  = HandledWidget->getName().c_str();
	StringVector WidgetName  = StringUtil::split(ButtonName,"/");
	int index = StringUtil::toReal(WidgetName.at(3));

	// On lance les actions associées au PushButton (tag call=)
	mXMLinterface->gotoElement(WidgetName.at(1));		// par ex: "mainWin"
	mXMLinterface->gotoSubElement("pushbutton",index);
	String CallInstruction = mXMLinterface->getStringAttribute("call");
	this->executeOrder(CallInstruction);
	TRACE("<< handlePushButton()");

	return true;
}


/* ------------------------------------------------------------------------- */
/// Handle: Gestion des Sliders
/* ------------------------------------------------------------------------- */
bool Interface::handleSlider(const CEGUI::EventArgs& e)
{
	// TRACE(">> handleSlider()");

	// On recupère la valeur du curseur (entre 0.0 et 1.0)
	CEGUI::Slider* HandledWidget= (CEGUI::Slider*)((CEGUI::WindowEventArgs&)e).window;
	float   val = HandledWidget->getCurrentValue();

	// Le nom de l'objet est du type "InterfaceName/WindowName/slider/1". 
	String SliderName  = HandledWidget->getName().c_str();
	StringVector WidgetName  = StringUtil::split(SliderName,"/");
	int index = StringUtil::toReal(WidgetName.at(3));

	// On récupère le range= associées à l'item dans le fichier XML
	mXMLinterface->gotoElement(WidgetName.at(1));		    // = Nom de la fenetre
	mXMLinterface->gotoSubElement(WidgetName.at(2),index);  // ="slider"

	// On transforme la valeur [0..1] en une valeur [0..range]
	Real range = mXMLinterface->getRealAttribute("range");
	if (mXMLinterface->getBooleanAttribute("linear",false))
		mSliderValue[index]= range*val;	        // loi linéaire
	else
		mSliderValue[index]=range*val*val;	   // loi quadratique

	// On rafraichit les textes dynamiques de la fenetre 
	// (En general, il y en a un qui affiche la valeur du slider)
	this->refreshTexts(WidgetName.at(1));		// = Nom de la fenetre

	return true;
}



/* ------------------------------------------------------------------------- */
/// Handle: Gestion des Listbox
/* ------------------------------------------------------------------------- */
bool Interface::handleListbox(const CEGUI::EventArgs& e)
{
	TRACE(">> handleListbox()");

	CEGUI::uint       id = -1;
	CEGUI::Listbox*   HandledWidget = (CEGUI::Listbox*)((CEGUI::WindowEventArgs&)e).window;

	// On vérifie si un item est sélectionné, et on récupère son ID
	if (HandledWidget->getSelectedCount()>0) id = HandledWidget->getFirstSelectedItem()->getID();
	// DEBUG_LOG ("Handling listBox selected item with ID "+ITOA(id));

	// Le nom de la Listbox est du type "MTdroid/mainWin/listbox/0". 
	String ListboxName = HandledWidget->getName().c_str();

	// L'item cliqué peut contenir un attribut "text". Dans ce cas, 
	// On met a jour le texte associé à cet item de la listbox
	String newText            = mXMLinterface->getItemStringAttribute(id,"text");
	String AssociatedTextName = mXMLinterface->getAssociatedTextName(ListboxName);
	if (AssociatedTextName!="")
	{
		CEGUI::StaticText* AssociatedText = (CEGUI::StaticText*)mWinMgr->getWindow(AssociatedTextName);
		showDynamicText(AssociatedText, newText);
	}

	// L'item cliqué peut contenir un attribut "submenu". Dans ce cas, 
	// On met a jour le menu, avec le sous-menu précisé dans cet item.
	Ogre::String newMenu = mXMLinterface->getItemStringAttribute(id,"submenu");
	if (newMenu!="")	fillListbox(HandledWidget, newMenu);

	// L'item cliqué peut contenir une action a réaliser (attribut "call").
	// Dans ce cas, on lance les actions associées à l'item
	Ogre::String newOrder = mXMLinterface->getItemStringAttribute(id,"call");
	executeOrder(newOrder);

	return true;
}



/* ------------------------------------------------------------------------- */
/// Crée un objet d'Interface  (GUI)
/* ------------------------------------------------------------------------- */
void Interface::createInterface() 
{
	TRACE(">> createInterface()");
	// Si l'interface existe deja, on ne la recrée pas
	if (!mExistGUI)
	{
		// On extrait les élements generaux dans le fichier XML
		mXMLinterface->gotoElement("imageset");

		DEBUG_LOG("loading "+mXMLinterface->getStringAttribute("file"));

		// Creation de l'imageset indiqué dans le fichier XML
		CEGUI::ImagesetManager::getSingleton().createImageset(mXMLinterface->getStringAttribute("file"));
		mImageSetName = mXMLinterface->getStringAttribute("name");
			
		//	Creation de la fenetre et des widgets
		this->createGUIWindows();
		this->createGUIWidgets("mainWin");
		mExistGUI=true;
	}
}

/* ------------------------------------------------------------------------- */
/// Détruit un objet d'Interface  (GUI)
/* ------------------------------------------------------------------------- */
void Interface::deleteInterface() 
{
	TRACE (">> Interface::deleteInterface()");
	if (mExistGUI)
	{
		TRACE ("   mExistGUI = true");
		// On efface la souris
		this->hideMouse();
		// On détruit l'éventuelle fenetre Annexe
		if (mAnnexWindow) mWinMgr->destroyWindow(mAnnexWindow);
		mAnnexWindow=NULL;
		
		// On detruit la fenetre principale
		mWinMgr->destroyWindow(mGUIname+"/mainWin");

		// le controle est inclus dans la fonction
		// if (CEGUI::ImagesetManager::getSingletonPtr()->isImagesetPresent(mImageSetName)): 
		// Destruction de l'imageset 
		CEGUI::ImagesetManager::getSingletonPtr()->destroyImageset(mImageSetName);
	}

	mExistGUI=false;
	TRACE ("<< Interface::deleteInterface()");
}



/* ------------------------------------------------------------------------- */
///	Affiche la fenetre
/* ------------------------------------------------------------------------- */
void Interface::showInterface(String name)
{
	try
	{
		mWinMgr->getWindow(mGUIname+"/"+name)->show();
	}
	catch (...) 
	{
		DEBUG_LOG ("showInterface(): This windows does not exist: "+mGUIname+"/"+name);
	}
}


/* ------------------------------------------------------------------------- */
///	Cache la fenetre
/* ------------------------------------------------------------------------- */
void Interface::hideInterface(String name)
{
	try
	{
		mWinMgr->getWindow(mGUIname+"/"+name)->hide();
	}
	catch (...) 
	{
		DEBUG_LOG ("hideInterface(): This windows does not exist: "+mGUIname+"/"+name);
	}
}


/* ------------------------------------------------------------------------- */
///	Cache le curseur de souris
/* ------------------------------------------------------------------------- */
void Interface::hideMouse()
{
	// On masque le curseur de souris
	CEGUI::System::getSingleton().setDefaultMouseCursor(CEGUI::BlankMouseCursor);
}


/* ------------------------------------------------------------------------- */
///	Ajuste la transparence de toutes les fenetres
/* ------------------------------------------------------------------------- */
void Interface::setAlpha(float AlphaValue)
{
	mRootWindow->setAlpha(AlphaValue);
}



/* ------------------------------------------------------------------------- */
///	Définit des couleurs pour l'interface
/* ------------------------------------------------------------------------- */
void Interface::setDefaultColours()
{
	mRed    = CEGUI::colour(1.0, 0.0, 0.0);
	mGreen  = CEGUI::colour(0.0, 1.0, 0.0);
	mBlue   = CEGUI::colour(0.0, 0.0, 1.0);
	mBlack  = CEGUI::colour(0.0, 0.0, 0.0);
	mWhite  = CEGUI::colour(1.0, 1.0, 1.0);
	mGray   = CEGUI::colour(0.8, 0.8, 0.8);
	mYellow = CEGUI::colour(1.0, 1.0, 0.0);
}



/* ------------------------------------------------------------------------- */
/// On rafraichit le contenu des StaticText de la fenetre
/* ------------------------------------------------------------------------- */
void Interface::refreshTexts(String guiWindow)
{
//	TRACE(">> refreshTexts("+guiWindow+")");
	
	int index    = 0;
	String WidgetTag = "text";
	mXMLinterface->gotoElement(guiWindow);

	// On parcourt les statictext du fichier XML, pour les rafraichir
	while (mXMLinterface->gotoSubElement(WidgetTag,index))
	{
		// On recupere le widget
		String TextID = mGUIname+"/"+guiWindow+"/"+WidgetTag+"/"+ITOA(index);
		CEGUI::StaticText* widget = (CEGUI::StaticText*)mWinMgr->getWindow(TextID);
		String TextContent = mXMLinterface->getTextContent();
		// On ecrit le texte
		if (StringUtil::startsWith(TextContent,"@"))
			showDynamicText(widget, TextContent);
		else
			widget->setText(TextContent);
		// On passe au texte suivant
		index++;
	}
}

/* ------------------------------------------------------------------------- */
/// On rafraichit le contenu des EditBox de la fenetre
/* ------------------------------------------------------------------------- */
void Interface::refreshEditboxes(String guiWindow)
{
	TRACE(">> refreshEditboxes("+guiWindow+")");
	
	int index    = 0;
	String WidgetTag = "edit";
	mXMLinterface->gotoElement(guiWindow);

	// On parcourt les editbox du fichier XML, pour les rafraichir
	while (mXMLinterface->gotoSubElement(WidgetTag,index))
	{
		// On recupere le widget
		String TextID = mGUIname+"/"+guiWindow+"/"+WidgetTag+"/"+ITOA(index);
		CEGUI::Window* widget = (CEGUI::Window*)mWinMgr->getWindow(TextID);
		String TextContent = mXMLinterface->getTextContent();
		// On ecrit le texte
		if (StringUtil::startsWith(TextContent,"@"))
			showDynamicText((CEGUI::StaticText*)widget, TextContent);
		else
			widget->setText(TextContent);
		// On passe au texte suivant
		index++;
	}
}


/* ------------------------------------------------------------------------- */
/// On rafraichit le contenu des StaticText de la fenetre
/* ------------------------------------------------------------------------- */
void Interface::refreshListboxes(String guiWindow)
{	
	TRACE(">> refreshListboxes("+guiWindow+")");

	int    index     = 0;
	String WidgetTag = "listbox";
	mXMLinterface->gotoElement(guiWindow);

	// On parcourt les Listboxes du fichier XML, pour les rafraichir
	while (mXMLinterface->gotoSubElement(WidgetTag,index))
	{
		// On charge le nom de la listbox à rafraichir
		String WidgetID = mGUIname+"/"+guiWindow+"/"+WidgetTag+"/"+ITOA(index);
		String ListboxName = mXMLinterface->getTextContent();

		// On recupere le widget
		CEGUI::Listbox* widget = (CEGUI::Listbox*)mWinMgr->getWindow(WidgetID);

		/// Si le contenu commence par @, on appelle la fonction locale de gestion de listbox, sinon on charge les items demandés
		if (StringUtil::startsWith(ListboxName,"@"))
			showDynamicListbox(widget, ListboxName);
		else
			fillListbox(widget, ListboxName);
		// On passe à la listbox suivante
		index++;
	}
	TRACE("<< refreshListboxes()");
}


/* ------------------------------------------------------------------------- */
/// On rafraichit le contenu des StaticImages de la fenetre
/* ------------------------------------------------------------------------- */
void Interface::refreshImages(String guiWindow)
{
	TRACE(">> refreshImages("+guiWindow+")");

	int    index     = 0;
	String WidgetTag = "image";
	mXMLinterface->gotoElement(guiWindow);

	// On parcourt les StaticImage du fichier XML, pour les rafraichir
	while (mXMLinterface->gotoSubElement(WidgetTag,index))
	{
		// On recupere le widget
		String WidgetID = mGUIname+"/"+guiWindow+"/"+WidgetTag+"/"+ITOA(index);
		CEGUI::StaticImage* widget = (CEGUI::StaticImage*)mWinMgr->getWindow(WidgetID);
		// On charge le nom de l'image à afficher
		String ImageName = mXMLinterface->getTextContent();
		if (StringUtil::startsWith(ImageName,"@"))
			showDynamicImage(widget, ImageName);
		else
			widget->setImage(mImageSetName, ImageName);
		// On passe à l'image suivante
		index++;
	}
}


/* ------------------------------------------------------------------------- */
/// On rafraichit toutes les ProgressBar de la fenetre
/* ------------------------------------------------------------------------- */
void Interface::refreshProgressBar(String guiWindow)
{
//	TRACE(">> refreshProgressBar("+guiWindow+")");

	int    index     = 0;
	String WidgetTag = "ProgressBar";
	mXMLinterface->gotoElement(guiWindow);

	// On parcourt les ProgressBar du fichier XML, pour les rafraichir
	while (mXMLinterface->gotoSubElement(WidgetTag,index))
	{
		// On recupere le widget
		String WidgetID = mGUIname+"/"+guiWindow+"/"+WidgetTag+"/"+ITOA(index);
		CEGUI::ProgressBar* widget = (CEGUI::ProgressBar*)mWinMgr->getWindow(WidgetID);
		// On charge le nom de la variable à afficher
		String VariableName = mXMLinterface->getTextContent();
		if (StringUtil::startsWith(VariableName,"@"))
			showDynamicProgressBar(widget, VariableName);
		else
			widget->setProgress(StringConverter::parseReal(VariableName));
		// On passe à la ProgressBar suivante
		index++;
	}
}



/* ------------------------------------------------------------------------- */
/// On rafraichit tous les PushButton de la fenetre
/* ------------------------------------------------------------------------- */
void Interface::refreshPushButton(String guiWindow)
{
//	TRACE(">> refreshPushButton("+guiWindow+")");

	int    index     = 0;
	mXMLinterface->gotoElement(guiWindow);

	// On parcourt les PushButton du fichier XML, pour les rafraichir
	while (mXMLinterface->gotoSubElement("pushbutton",index))
	{
		// On recupere le widget
		String WidgetID = mGUIname+"/"+guiWindow+"/push/"+ITOA(index);
		CEGUI::PushButton* widget = (CEGUI::PushButton*)mWinMgr->getWindow(WidgetID);
		// On charge le texte à afficher
		String ButtonName = mXMLinterface->getTextContent();
		if (StringUtil::startsWith(ButtonName,"@"))
			showDynamicPushButton(widget, ButtonName);
		else
			widget->setText(ButtonName);
		// On passe au PushButton suivant
		index++;
	}
}




/* ------------------------------------------------------------------------- */
///	Enleve la fenetre annexe, créée avec addGUIWindow().
/* ------------------------------------------------------------------------- */
void Interface::removeAnnexWindow()
{
	// On efface la fenetre annexe (mais on ne la détruit pas)
	if (mAnnexWindow) mRootWindow->removeChildWindow(mAnnexWindow);
	mAnnexWindow=NULL;
}


/* ------------------------------------------------------------------------- */
///	Creation d'une seconde fenetre annexe. 
/* ------------------------------------------------------------------------- */
void Interface::addGUIWindow(String guiWindow)
{
using namespace CEGUI;

	TRACE(">> addGUIWindow("+guiWindow+")");

	///	Il ne peut exister qu'une Fenetre Annexe a la fois.
	/// Avant d'en ajouter une nouvelle, s'assurer que la précédente a ete retirée avec removeAnnexWindow()
	if (mAnnexWindow!=NULL) return;

	try
	{
		// vérifie si la fenetre existe dejà
		if (WindowManager::getSingletonPtr()->isWindowPresent(mGUIname+"/"+guiWindow))
		{
			// La fenetre existe dejà: on ne la crée pas, mais on la rattache.
			mAnnexWindow = (StaticText*)WindowManager::getSingletonPtr()->getWindow(mGUIname+"/"+guiWindow);
			mRootWindow->addChildWindow(mAnnexWindow);		
		}
		else
		{
			// Cree une fenetre fixe
			mAnnexWindow = (StaticText*)WindowManager::getSingletonPtr()->createWindow("TaharezLook/StaticText", mGUIname+"/"+guiWindow);
			mRootWindow->addChildWindow(mAnnexWindow);		
			// On extrait les élements concernant cette fenetre dans le fichier XML
			mXMLinterface->gotoElement(guiWindow);

			Real posX  = mXMLinterface->getRealAttribute("x");
			Real posY  = mXMLinterface->getRealAttribute("y");
			mAnnexWindow->setPosition(Point(posX, posY));

			Real width = mXMLinterface->getRealAttribute("w");
			Real high = mXMLinterface->getRealAttribute("h");
			mAnnexWindow->setSize(Size(width, high));
		
			// Le fond de la fenetre doit-il apparaitre... (par defaut: oui)
			bool background = mXMLinterface->getBooleanAttribute("background",true);
			mAnnexWindow->setBackgroundEnabled(background);

			// Le contour de la fenetre doit-il apparaitre... (par defaut: oui)
			bool frame = mXMLinterface->getBooleanAttribute("frame",true);
			mAnnexWindow->setFrameEnabled(frame);

			mAnnexWindow->setAlpha(mXMLinterface->getRealAttribute("alpha",1));
			// Sa transparence dépend aussi de rootwindow
			mAnnexWindow->setInheritsAlpha(true);	
			createGUIWidgets(guiWindow);
		}
	}
	catch(CEGUI::Exception)
	{
		// catch to prevent exit (errors will be logged).
		DEBUG_LOG ("CEGUI Exception in Interface::addGUIWindows()");
	}

	TRACE("<< addGUIWindow("+guiWindow+")");
}


/* ------------------------------------------------------------------------- */
/// Remplit la Listbox de la GUI avec les items d'un <menu ... > du fichier XML
/* ------------------------------------------------------------------------- */
void Interface::fillListbox(CEGUI::Listbox* pListbox, String MenuName)
{
	int    index;
	int    ItemID;
	String ItemText;

	TRACE (">> fillListbox("+MenuName+")");
	
	if (MenuName==StringUtil::BLANK) return;
	// On vide la listbox
	pListbox->resetList();

	// on peut afficher le sous-menu du fichier XML
	// On parcourt la section <menu> du fichier XML en traitant tous les <item>
	mXMLinterface->gotoElement(MenuName);
	CEGUI::colour ItemColour = this->getColour(mXMLinterface->getStringAttribute("colour"));

	index = 0;
	while (mXMLinterface->gotoSubElement("item",index))
	{
		ItemID   = mXMLinterface->getRealAttribute("id");
		ItemText = mXMLinterface->getTextContent();
		CEGUI::ListboxTextItem* Item = new CEGUI::ListboxTextItem(ItemText,ItemID);
		Item->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");
		Item->setSelectionColours(ItemColour);
		pListbox->addItem(Item);
		// On charge l'item suivant
		index++;
	}
	pListbox->activate();
	TRACE ("<< fillListbox()");
}



/* ------------------------------------------------------------------------- */
/// Transforme le nom de la couleur en ses trois composantes au format CEGUI
/* ------------------------------------------------------------------------- */
CEGUI::colour Interface::getColour(String name)
{
	if (name=="black")   	return (mBlack);	
	else if (name=="red") 	return (mRed);	
	else if (name=="green") return (mGreen);	
	else if (name=="blue")	return (mBlue);	
	else if (name=="yellow")return (mYellow);	
	else if (name=="white") return (mWhite);	
	else                    return (mGray);	

}
