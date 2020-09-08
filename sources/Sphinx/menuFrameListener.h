/* ------------------------------------------------------------------------- */
// File       : menuFrameListener.h
// Project    : Long Forgotten Earth - Menu Principal
/* ------------------------------------------------------------------------- */

#ifndef _MENU_FRAMELISTENER_H_
#define _MENU_FRAMELISTENER_H_

#if _MSC_VER > 1000
#pragma once
#endif


// Pour CEGUI
#include "CEGUIForwardRefs.h"

// Pour Ogre
#include "Ogre.h"
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
//#include "OgreStringConverter.h"
#include "OgreException.h"
//#include "OgreConfigFile.h"
//#include "OgreCEGUIRenderer.h"

// Pour LFE
#include "MenuOverlay.h"
//#include "Memories.h"


using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Menu principal: gestion des touches
/* ------------------------------------------------------------------------- */
class menuFrameListener: public FrameListener, public KeyListener, public MouseListener, public MouseMotionListener
{
friend class MenuOverlay;

protected:
	Camera*          mCamera;
	RenderWindow*    mWindow;
	SceneManager*    mSceneMgr;
	EventProcessor*  mEventProcessor;           /// Ogre: Gestion des clavier/souris
	InputReader*     mInputDevice;              /// Ogre: Gestion des clavier/souris
	bool             mOverlayOn;
	bool             mDebugOn;
	Real             mTimeUntilNextToggle ;	    // Gestion de l'antirebond
	Real             mTimeUntilMessageOff ;	    // Timeout for the message line
	Real             mTimeUntilMemoriesOff;		// Timeout for memories overlay
	bool             mTimeOutRunning;
	float            mRotScale;                 /// Vitesse de rotation de la camera
	float            mRotX, mRotY;              /// Mouvements de la camera
	TextureFilterOptions mFiltering;            /// Parametre pour la qualité de l'affichage
	int              mAniso;                    /// Parametre pour la qualité de l'affichage
	bool             mShutdown;                 /// Memorise l'ordre de quitter le programme
	int	             mMenuQuitParameter;		/// -1:Exit 0:NewGame 1:LoadGame#1, etc
	MenuOverlay*     mMenuOverlay;              /// Objet gerant toute la partie GUI
    CEGUI::Renderer* mGUIRenderer;              /// Renderer pour CE-GUI
    CEGUI::System*   mGUISystem;                /// System pour CE-GUI

public:
	menuFrameListener(RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer);
	~menuFrameListener() ;
	void processUnbufferedKeyInput(int keyID);
	void setKeyboardType(String LanguageCode);
	void moveCamera();
	bool frameStarted(const FrameEvent& evt);
	bool frameEnded(const FrameEvent& evt);
/**********************************************************************/
	int  getMenuQuitParameter();
	void requestMenuExit(int parameter);
/**********************************************************************/
	void keyClicked(KeyEvent* e);
	void keyPressed(KeyEvent* e);
	void keyReleased(KeyEvent* e);
/**********************************************************************/
	void mouseClicked(MouseEvent* e);
	void mouseEntered(MouseEvent* e);
	void mouseExited(MouseEvent* e);
	void mousePressed(MouseEvent* e);
	void mouseReleased(MouseEvent* e);
	void mouseMoved(MouseEvent* e);
	void mouseDragged(MouseEvent* e);
};

#endif
