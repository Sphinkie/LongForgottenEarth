/* ------------------------------------------------------------------------- */
// File       : Interface.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#if _MSC_VER > 1000
#pragma once
#endif 


// Pour CEGUI
#include "CEGUIForwardRefs.h"
#include <CEGUI/CEGUIcolour.h>


// Pour LFE
#include "../LFE/Interface_XMLparser.h"


using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Surclasse générique contenant les fonctions de base, utilisables pour toutes les interfaces GUI
/* ------------------------------------------------------------------------- */
class Interface  
{
public:
	Interface();
	virtual ~Interface();
	void     loadInterfaceFile(String InterfaceFile);
	virtual void createInterface();
	virtual void deleteInterface();

	void     showInterface(String name = "mainWin");
	void     hideInterface(String name = "mainWin");
	void	 setAlpha(float AlphaValue);
	void     hideMouse();

	void	 addGUIWindow(String guiWindow);
	void     removeAnnexWindow();

	void     refreshTexts(String guiWindow);
	void     refreshImages(String guiWindow);
	void     refreshListboxes(String guiWindow);
	void     refreshEditboxes(String guiWindow);
	void     refreshProgressBar(String guiWindow);
	void     refreshPushButton(String guiWindow);

	CEGUI::colour getColour(String name);


protected:

	void	 createGUIWindows();
	void	 createGUIWidgets(String guiWindow);

	bool	 handleListItem   (const CEGUI::EventArgs& e);
	bool	 handleSlider     (const CEGUI::EventArgs& e);
	bool	 handlePushButton (const CEGUI::EventArgs& e);
	bool     handleListbox    (const CEGUI::EventArgs& e);
	void     fillListbox      (CEGUI::Listbox* pListbox,String MenuName);
	void     setDefaultColours();

// Fonctions à implémenter dans les classes derivées:
	virtual void showDynamicListbox    (CEGUI::Listbox* pListbox,          String Order) =0;
	virtual void showDynamicText       (CEGUI::StaticText* pText,          String Order) =0;
	virtual void showDynamicPushButton (CEGUI::PushButton* pPushButton,    String Order) =0;
	virtual void showDynamicImage      (CEGUI::StaticImage* pImage,        String Order) =0;
	virtual void showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar,  String Order) =0;

	virtual void executeOrder(String Order) =0;

	// variables membres
	Interface_XMLParser*  mXMLinterface;
	RenderWindow*         mWindow;
	String                mGUIname;
	bool                  mExistGUI;
	
	
	String                mImageSetName;
	CEGUI::Renderer*      mGUIRenderer;
	CEGUI::WindowManager* mWinMgr;
	CEGUI::Window*        mRootWindow;
	CEGUI::StaticText*    mInterfaceWindow;
	CEGUI::StaticText*    mAnnexWindow;

	// Valeurs de différents controles
	CEGUI::Slider*       mSlider[5];	    // Pointeurs vers les curseurs de la fenetre (5 sliders max)
	int                  mSliderValue[5];	// La valeur des curseurs de la fenetre (5 sliders max)
	
	// Les couleurs de l'interface (la palette de base)
	CEGUI::colour        mRed;
	CEGUI::colour        mGreen;
	CEGUI::colour        mBlue;
	CEGUI::colour        mBlack;
	CEGUI::colour        mWhite;
	CEGUI::colour        mGray;
	CEGUI::colour        mYellow;
};

#endif 
