/* ------------------------------------------------------------------------- */
// File       : menuFrameListener.cpp
// Project    : Long Forgotten Earth - Menu Principal
// Auteur     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "menuFrameListener.h"

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIBase.h> 
#include <CEGUI/CEGUISystem.h> 
#include <CEGUI/CEGUILogger.h> 
#include <CEGUI/CEGUIWindow.h> 
#include <CEGUI/CEGUIRenderer.h>
#include <OgreMemoryMacros.h>

#include "fmod.h"
#include "fmod_errors.h"
#include "macros.h"
#include "../LFE/KeyboardTranslator.h"


/* ---------------------------------------------------*/
/// Cette fonction est en dehors de toute classe pour des pb de compatibilité entre Ogre et CEGUI
/* ---------------------------------------------------*/
CEGUI::MouseButton convertOgreButtonToCegui(int ogre_button_id)
{
	switch (ogre_button_id)
	{
	case MouseEvent::BUTTON0_MASK:
		return CEGUI::LeftButton;
		break;

	case MouseEvent::BUTTON1_MASK:
		return CEGUI::RightButton;
		break;

	case MouseEvent::BUTTON2_MASK:
		return CEGUI::MiddleButton;
		break;

	case MouseEvent::BUTTON3_MASK:
		return CEGUI::X1Button;
		break;

	default:
		return CEGUI::LeftButton;
		break;
	}
}



/* ------------------------------------------------------------------------- */
// Constructor takes a RenderWindow to determine input context
/* ------------------------------------------------------------------------- */
menuFrameListener::menuFrameListener(RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer) : mGUIRenderer(renderer)
{
	// Autres initialisations
	mShutdown = false;
	mMenuQuitParameter = -1;
	// srand(1000);
	mTimeUntilMessageOff=-1;

	// Initialisation de la souris et du clavier
	mEventProcessor = new EventProcessor();
	mEventProcessor->initialise(win);
	mEventProcessor->startProcessingEvents();
		
	mEventProcessor->addKeyListener(this);
	mEventProcessor->addMouseMotionListener(this);	
	mEventProcessor->addMouseListener(this);		

	mInputDevice = mEventProcessor->getInputReader();  

	// Initialisation de l'affichage
	mCamera = cam;
	mWindow = win;
	mOverlayOn = true;
	mDebugOn = false;
	// On retrouve le scenemanager à partir de mCamera
	mSceneMgr = mCamera->getSceneManager();

	// Initialisation de la qualité du rendu
	mAniso = 1;
	mFiltering = TFO_BILINEAR;
	mTimeUntilNextToggle = 0;

	// Initialisation Camera: a l'arret
	mRotScale = 0.0;

	// On affiche la GUI du menu pricipal
	mMenuOverlay = new MenuOverlay(this, mSceneMgr, "../media/interface/mainmenu.xml");

	mTimeOutRunning=false;
	mInputDevice->setBufferedInput(true, true);	// ceci fait apparaitre le curseur
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
menuFrameListener::~menuFrameListener()
{
	delete mEventProcessor;

	if(mGUISystem)
	{
		delete mGUISystem;
		mGUISystem = NULL;
	}

	if(mGUIRenderer)
	{
		delete mGUIRenderer;
		mGUIRenderer = NULL;
	}

/*	if(mEditorGuiSheet)
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(mEditorGuiSheet);
	}
*/      
}

/* -------------------------------------------------------------------------- */
/// Initialise le type de clavier
/* -------------------------------------------------------------------------- */
void menuFrameListener::setKeyboardType(String LanguageCode)
{
	KeyboardTranslator::KeyMap LanguageMap = KeyboardTranslator::KEYMAP_ENGLISH;

	if (LanguageCode=="fre")
	{
		// Gestion des touches du clavier Français
		LanguageMap = KeyboardTranslator::KEYMAP_FRENCH;
	}

	new KeyboardTranslator(LanguageMap);
}



/* ------------------------------------------------------------------------- */
/// Gestion des touches
/* ------------------------------------------------------------------------- */
void menuFrameListener::processUnbufferedKeyInput(int keyID)
{
	TRACE(">> processUnbufferedKeyInput()");

	// ****************************************
	// Touches a appui impulsif
	// ****************************************
	// Touche ESC: Quitte le programme
	if (keyID == KC_ESCAPE)    
	{ 
		mShutdown=true;     
	} 

	// Touche F2: active ou non le GUI
    if (keyID == KC_F2) 
    {
       mOverlayOn = !mOverlayOn;
       mMenuOverlay->showOverlays(mOverlayOn);
    }

	// Touche F12: active ou non les infos de debug
    if (keyID == KC_F12) 
    {
       mDebugOn = !mDebugOn;
       mMenuOverlay->showDebugOverlay(mDebugOn);
    }


	// Touche F3: change de type de rendu 3D
    if (keyID == KC_F3) 
    {
		switch(mFiltering)
        {
        case TFO_BILINEAR:
                mFiltering = TFO_TRILINEAR;
				mWindow->setDebugText("Rendu Trilineaire");
                mAniso = 1;
                break;
        case TFO_TRILINEAR:
                mFiltering = TFO_ANISOTROPIC;
				mWindow->setDebugText("Rendu Anisotropique");
                mAniso = 8;
                break;
        case TFO_ANISOTROPIC:
                mFiltering = TFO_BILINEAR;
				mWindow->setDebugText("Rendu Bilineaire");
                mAniso = 1;
                break;
        default:
                break;
        }
        MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
        MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);
    }

	// Touche IMPRSCR: faire un screenshot
    if (keyID == KC_SYSRQ)
    {
        mWindow->writeContentsToTimestampedFile("screenshot_",".jpg");
		mWindow->setDebugText("Screenshot !");
    }
}


/* ------------------------------------------------------------------------- */
/// Make all the changes to the camera
/* ------------------------------------------------------------------------- */
void menuFrameListener::moveCamera()
{
	mCamera->pitch(Degree(mRotY));
    mCamera->yaw(Degree(mRotX));

	// Si la camera va trop haut ou trop bas, on corrige
	Vector3 camVect = mCamera->getDirection();
	Vector3 maxVect = Vector3( 1, 0.72, 1); // Le regard le plus haut possible = 45°
	Vector3 minVect = Vector3(-1,-0.62,-1);	// Le regard le plus bas possible  = 50°
	camVect.makeFloor(maxVect);
	camVect.makeCeil(minVect);
	mCamera->setDirection(camVect);

}


/* ------------------------------------------------------------------------- */
/// Override frameStarted event to process that (don't care about frameEnded)
/* ------------------------------------------------------------------------- */
bool menuFrameListener::frameStarted(const FrameEvent& evt)
{
	// ****************************************
	// Touches a appui continu
	// ****************************************
	if (mInputDevice->isKeyDown(KC_LEFT))	{mRotX = mRotScale;}	/// Move camera LEFT
	if (mInputDevice->isKeyDown(KC_RIGHT))	{mRotX = -mRotScale;}	/// Move camera RIGHT
    if (mInputDevice->isKeyDown(KC_UP))		{mRotY = mRotScale;}	/// Move camera UPWARD
	if (mInputDevice->isKeyDown(KC_DOWN))	{mRotY = -mRotScale;}	/// Move camera DOWNWARD

	// ****************************************
	// Gestion des timers
	// ****************************************
	if (mTimeUntilMessageOff >= 0) 	
	{
		mTimeUntilMessageOff -= evt.timeSinceLastFrame;
	}
	else	
	{
		// Quand le timeout est ecoulé, on efface le texte des memories
		// Pour mieux admirer le paysage 
		if (mMenuOverlay->isIntroRunning()) mMenuOverlay->hideInterface("memoriesWin");
		mTimeUntilMessageOff = -1;
	}

	// ****************************************
	// Gestion des mouvements
	// ****************************************
	// Scale movement units by time passed since last frame
	// Take about 20 seconds for full rotation (360/20=18)
	mRotScale = 18 * evt.timeSinceLastFrame;
	moveCamera();
	mRotX = 0;
	mRotY = 0;

	return true;
}

/* ------------------------------------------------------------------------- */
/// Fonction excécutée à la fin de chaque frame
/* ------------------------------------------------------------------------- */
bool menuFrameListener::frameEnded(const FrameEvent& evt)
{
    return !mShutdown;
}


/* ************************************************************************** */
/// Surcharge des fonctions de KeyListener
/* ************************************************************************** */

/* ---------------------------------------------------*/
/* ---------------------------------------------------*/
void menuFrameListener::keyClicked (KeyEvent* e) {}

/* ---------------------------------------------------*/
/// Fonction appelée lors de l'appui sur une touche
/* ---------------------------------------------------*/
void menuFrameListener::keyPressed (KeyEvent* e)
{
	// On gere les touches du clavier

	// Traitement prioritaire de certaines touches (F1...F12)
	processUnbufferedKeyInput(e->getKey());

	// do event injection (Key Down)
	KeyboardTranslator::getSingleton().translate(e->getKeyChar());
	unsigned char translatedKey = KeyboardTranslator::getSingleton().getKey();
	
	CEGUI::System::getSingleton().injectKeyDown(e->getKey());
	CEGUI::System::getSingleton().injectChar(translatedKey); // inject translated chararacter
	e->consume();
}
/* ---------------------------------------------------*/
/// Cette fonction permet la recuperation des touches dans les editbox de CEGUI
/* ---------------------------------------------------*/
void menuFrameListener::keyReleased(KeyEvent* e)
{
//	TRACE(">> keyReleased");
	CEGUI::System::getSingleton().injectKeyUp(e->getKey());
}

/* ************************************************************************** */
/// Surcharge des fonctions de MouseListener
/* ************************************************************************** */

/* ---------------------------------------------------*/
/// Handle sur les déplacements de la souris
/* ---------------------------------------------------*/
void menuFrameListener::mouseMoved (MouseEvent* e)
{
//	CEGUI::Renderer* mGUIRenderer = CEGUI::System::getSingleton().getRenderer();
	CEGUI::System::getSingleton().injectMouseMove(e->getRelX() * mGUIRenderer->getWidth(), e->getRelY() * mGUIRenderer->getHeight());
	e->consume();
}

/* ---------------------------------------------------*/
/// Quand on clique sur la souris: cela fait un petit "click"
/* ---------------------------------------------------*/
void menuFrameListener::mousePressed (MouseEvent *e)
{
	// On joue un petit click
	FSOUND_SAMPLE* Sound;
	int SoundChannel ;
	String SoundFile = "../media/sounds/click.wav";

	// Loads and decodes a static soundfile into memory.
	Sound=FSOUND_Sample_Load(FSOUND_FREE,SoundFile.c_str(),FSOUND_LOOP_OFF,0,0);

	if (!Sound)
	{
		DEBUG_LOG ("Error Loading SoundFile:"+SoundFile);
		DEBUG_LOG (FMOD_ErrorString(FSOUND_GetError()) );
		FSOUND_Sample_Free(Sound);
	}

	// play the sound
	SoundChannel = FSOUND_PlaySound(FSOUND_FREE, Sound);
	// set the volume 
	FSOUND_SetVolume(SoundChannel, 255);	

	// On rearme l'affichage des textes pour 30 secondes
	mTimeUntilMessageOff=30.0;

	// On transmet le clic à CEGUI
	CEGUI::System::getSingleton().injectMouseButtonDown(convertOgreButtonToCegui(e->getButtonID()));
	e->consume();
}

/* ---------------------------------------------------*/
/// unused.
/* ---------------------------------------------------*/
void menuFrameListener::mouseEntered (MouseEvent* e) {}

/* ---------------------------------------------------*/
/// unused
/* ---------------------------------------------------*/
void menuFrameListener::mouseClicked( MouseEvent* e) {}

/* ---------------------------------------------------*/
/// unused.
/* ---------------------------------------------------*/
void menuFrameListener::mouseExited  (MouseEvent* e) {}

/* ---------------------------------------------------*/
/// Identique à mouseMoved().
/* ---------------------------------------------------*/
void menuFrameListener::mouseDragged (MouseEvent* e) 
{
	mouseMoved(e);
}

/* ---------------------------------------------------*/
/// Handle sur la relache du bouton de souris
/* ---------------------------------------------------*/
void menuFrameListener::mouseReleased (MouseEvent *e)
{
	TRACE(">> mouseReleased");
	CEGUI::System::getSingleton().injectMouseButtonUp(convertOgreButtonToCegui(e->getButtonID()));
	e->consume();

	if (mMenuOverlay->isIntroRunning()) mMenuOverlay->showInterface("memoriesWin");
}

/* ------------------------------------------------------------------------- */
/// Renvoie la façon dont est sorti du menu principal
/* ------------------------------------------------------------------------- */
int menuFrameListener::getMenuQuitParameter()
{
	return mMenuQuitParameter;
}

/* ------------------------------------------------------------------------- */
/// Donne l'ordre de sortir du menu principal
/* ------------------------------------------------------------------------- */
void menuFrameListener::requestMenuExit(int parameter)
{
	mMenuQuitParameter=parameter;
	mShutdown = true;
}
