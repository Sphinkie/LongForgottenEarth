/* -------------------------------------------------------------------------- */
// File       : mainFrameListener.h
// Project    : Long Forgotten Earth 
// Author     : David de Lorenzo
/* -------------------------------------------------------------------------- */

#ifndef _MAIN_FRAME_LISTENER_H_
#define _MAIN_FRAME_LISTENER_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#if defined( __WIN32__ ) || defined( _WIN32 )
  #ifdef OGRE_GUIRENDERER_EXPORTS
    #define OGRE_GUIRENDERER_API __declspec(dllexport)
  #else
   #define OGRE_GUIRENDERER_API __declspec(dllimport)
  #endif
#else
  #define OGRE_GUIRENDERER_API
#endif

// Pour CEGUI
#include "CEGUIForwardRefs.h"

// Pour Ogre
#include "Ogre.h"
#include "OgreEventListeners.h"
#include "OgreKeyEvent.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreOverlayManager.h"
#include "OgreConfigFile.h"
#include "OgreCEGUIRenderer.h"

using namespace Ogre;

#define DISTANCE_MAX 60	/// Distance max pour agir sur des objets (Picker)

/* -------------------------------------------------------------------------- */
/// Gere un peu tout ce qui est dynamique dans le jeu.
/* -------------------------------------------------------------------------- */
class mainFrameListener : public FrameListener, public KeyListener, public MouseListener, public MouseMotionListener
{

public:
	void setKeyboardType(String LanguageCode);
	mainFrameListener::mainFrameListener(RenderWindow* win, Camera* cam);
	~mainFrameListener();
	bool frameStarted      (const FrameEvent& evt);
	bool frameEnded        (const FrameEvent& evt);
	void showDebugOverlay  (bool show);
	void showHelpOverlay   (bool show);
	void showCrosshairOverlay(bool show);
	void _showMessageOverlay(String texte);
	void _hideMessageOverlay();
	bool makeSkybox();
	bool _makeSkybox();
	bool _worldToScreen(Vector3& worldPos, Vector3& screenPos);


protected:

	// variables d'essais pour debug
	bool        detec;
	Radian      focale;
	bool        mKeys_GestionAppli, mMouse_GestionAppli, mInputTypeSwitchingOn;
	bool        mShadowsOn;
	int         mWPnumber;    ///< pour la creation de Waypoints (touche R)
	int         mAttenuation; ///< pour tester l'atténuation sonore (touches F8-F9)
	float       mSpeed;       ///< pour le mode accéléré (touche I)
	ColourValue mTriplet;     ///< pour ajuster des couleurs en mode debug
	Sphere*     test_sphere;  ///< pour le Picker
	
	TextureFilterOptions mFiltering;
	EventProcessor*      mEventProcessor;
	InputReader*         mInputDevice;
	Camera*              mCamera;
	RenderWindow*        mWindow;
	SceneManager*        mSceneMgr;
	CEGUI::OgreCEGUIRenderer*   mGUIRenderer;
	bool                 mSkyboxInProgress;
	int                  mSkyboxStepNumber;
	String               mVersion_ID;  ///< No de la version du jeu

	bool      mContinue;               ///< Flag général pour continuer ou quitter le programme
	bool      m3Dmode;                 ///< Flag général: true=3D-mode  false=GUI-mode
	int       mRunNumber;              ///< Permet de faire des initialisations une seule fois.
	bool      mOverlayOn;
	bool      mOVL_debug;              ///< overlay: debug screen
	float     mMoveScale;
	float     mRotScale;
	float     mRotX, mRotY;
    Vector3   mTranslateVector;        ///< Pour les mouvements horizontaux de la camera
	Vector3   mUpDownVector;           ///< Pour les mouvements verticaux de la camera
	bool      mMouseButton0;
	bool      mMouseButton1;


	bool      mOVL_help;               ///< Help screen show/hide
	void*     mHelpInterface;          ///< The Help Screen

	// Obsolete
//	Real      mTimeUntilNextToggle ;   ///< to stop toggles flipping too fast
	Real      mTimeUntilMessageOff ;   ///< Timeout for the message line
	bool      mTimeOutRunning;

	int         mAniso;
	int         mCptFrame;
	long        mLastFoundID;
	long        mFoundID;
	SceneNode*         mLastFoundNode;
	UserDefinedObject* mLastFoundObject;
	UserDefinedObject* mFoundObject;

	/* -------------------------- */
	// Les touches et les deplacements
	/* -------------------------- */
	void processKeyPressed  (KeyEvent* e);
	void processKey_Arrows  ();
	void processKey_Moves   ();
	void processMouse(MouseEvent* e);

	/* -------------------------- */
	// Le Picker
	/* -------------------------- */
	UserDefinedObject* pickObject();
	SceneNode* _pickSubObject   (SceneNode* parent_node);
	bool       _hitSphericNode  (SceneNode* TestedNode, const Vector3 &origin, const Vector3 &direction);
	bool       _hitCubicNode    (SceneNode* TestedNode, const Vector3 &origin, const Vector3 &direction);

	/* -------------------------- */
	// Lise à jour des overlay
	/* -------------------------- */
	void updateMessageOverlay();
	void updateStats();
	
	/* -------------------------- */
	// Gestion de l'IHM
	/* -------------------------- */
	void initCEGUISystem();
	void keyClicked   (KeyEvent* e);
	void keyPressed   (KeyEvent* e);
	void keyReleased  (KeyEvent* e);
	void mouseClicked (MouseEvent* e);
	void mouseEntered (MouseEvent* e) {};
	void mouseExited  (MouseEvent* e) {};
	void mouseDragged (MouseEvent* e);
	void mousePressed (MouseEvent* e);
	void mouseReleased(MouseEvent* e);
	void mouseMoved   (MouseEvent* e);
//	CEGUI::MouseButton convertOgreButtonToCegui(int ogre_button_id);


};



#endif 
