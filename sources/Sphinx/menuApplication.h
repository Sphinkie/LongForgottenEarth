/* ------------------------------------------------------------------------- */
// File       : menuApplication.h
// Project    : Long Forgotten Earth - Menu Principal
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _MENU_APPLICATION_H_
#define _MENU_APPLICATION_H_

#if _MSC_VER > 1000
#pragma once
#endif


// Pour CEGUI
#include "CEGUIForwardRefs.h"

// Pour Ogre
#include "Ogre.h"
#include "OgreCEGUIRenderer.h"

// Pour LFE
#include "memories.h"
#include "menuFrameListener.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Menu Principal. Creation de la scene.
/* ------------------------------------------------------------------------- */
class menuApplication
{
public:
	menuApplication();
	virtual ~menuApplication();
	int go();

protected:
	void   createScene();
	void   createFrameListener();
	bool   setup();
	bool   loadConfig();
	void   chooseSceneManager();
	void   createCamera();
	void   createViewports();
	void   setupResources();
	void   playMusic(String filename);
	void   initCEGUISystem();


    Root*              mRoot;
    Camera*            mCamera;
	Memories*          mCurrentMem;
    RenderWindow*      mWindow;
    SceneManager*      mSceneMgr;
    menuFrameListener* mFrameListener;
	String             mLanguageCode;
    CEGUI::OgreCEGUIRenderer* mGUIRenderer;
    CEGUI::Window*            mEditorGuiSheet;
    CEGUI::System*            mGUISystem;
};

#endif
