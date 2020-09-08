/* ------------------------------------------------------------------------- */
// MenuOverlay.h: interface for the MenuOverlay class.
/* ------------------------------------------------------------------------- */

#ifndef _MENU_OVERLAY_H_
#define _MENU_OVERLAY_H_

#if _MSC_VER > 1000
#pragma once
#endif 


#include "CEGUIForwardRefs.h"
#include "Ogre.h"

// On reutilise la gestion de CEGUI implémentée dans le jeu
#include "../LFE/Interface.h"
#include "memories.h"



using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion de la GUI du menu principal
/* ------------------------------------------------------------------------- */
class MenuOverlay : public Interface
{
public:
	MenuOverlay(void* pFriend, SceneManager* SceneMgr, String InterfaceFile);
	virtual ~MenuOverlay();
	
	void createMenuOverlay();
	void createMemoriesOverlay();
	void createCreditsOverlay();
	void createInfosOverlay();

	void showOverlays(bool show);

	void showDebugOverlay(bool show);
	void updateMessageOverlay();
	bool isIntroRunning();


protected:
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order) {};
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order) {};
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order) {};

	void     executeOrder(String Order);
	String   readFile(String filename);
	void     writeFile(String filename, String data, bool append=true);
	

	SceneManager* mSceneMgr;
	int           mSelectedCredit;
	Memories*     mCurrentMem;
	int           mMemoriesNumber;
	void*         mMainListener; 
	bool          mIntroOnly;
	bool          mPleaseWait;
	String        mLangCode;
};

#endif 
