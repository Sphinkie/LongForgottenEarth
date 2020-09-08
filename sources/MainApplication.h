/* ------------------------------------------------------------------------- */
// File       : mainApplication.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _MAIN_APPLICATION_H_
#define _MAIN_APPLICATION_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Ogre.h"
#include "mainApplicationListener.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Application Principale. Création de la Scene 3D. C'est un singleton.
/* ------------------------------------------------------------------------- */
class mainApplication   : public Singleton <mainApplication>	
{
public:
	mainApplication();
	virtual        ~mainApplication();
	int            go();
    SceneManager*  getSceneMgr();
	void           playMusic(String MusicFile);
	static mainApplication* getSingletonPtr(void);

protected:
	bool setup();
	bool loadConfig();
	void createScene();
	void createFrameListener();
	void chooseSceneManager();
	void createCamera();
	void createViewports();
	void createCollisionEngine();
	void setupResources();

    Root*              mRoot;
    Camera*            mCamera;
    RenderWindow*      mWindow;
    SceneManager*      mSceneMgr;
    mainFrameListener* mFrameListener;
	String             mLanguageCode;
	int                mShadowsType;
};

#endif 
