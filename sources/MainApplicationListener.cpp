/* -------------------------------------------------------------------------- */
// File       : mainApplicationListener.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* -------------------------------------------------------------------------- */

#include "mainApplicationListener.h"

// Note: pour ne pas avoir de probleme de compilation avec CEGUI, 
// il faut mettre dans le header uniquement #include "CEGUIForwardRefs.h"
// et les autres includes de CEGUI dans le fichier cpp.
// Il faut aussi mettre les includes de CEGUI avant ceux de Ogre.
// Il faut aussi desactiver les Ogre Memory Macros pendant ces includes.

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIBase.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUIWindowManager.h> 
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIRenderer.h>
#include <OgreMemoryMacros.h>

#include <OgreOpcode.h>

#include "fmod.h"
#include "macros.h"
#include "Player.h"
#include "version.h"
#include "Clock.h"
#include "KeyboardTranslator.h"
#include "Simple_Interface.h"


// Objets cliquables
#include "Hibernation.h"
#include "Robot.h"
#include "Door.h"
#include "Computer.h"
#include "Button.h"

// Pour le debug
#include "TheShip.h"
#include "ShipLevel.h"
#include "GalaxyBox.h"
#include "NPC_Manager.h"

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


/* -------------------------------------------------------------------------- */
/// Constructor takes a RenderWindow to determine input context
/* -------------------------------------------------------------------------- */
mainFrameListener::mainFrameListener(RenderWindow* win, Camera* cam)
{
	DEBUG_LOG(">> mainFrameListener()");

	// Initialisation de l'affichage
	mCamera    = cam;
	mWindow    = win;
	mSceneMgr  = mCamera->getSceneManager();

	mOverlayOn = true;
	mOVL_help  = false;		// Affichage de l'ecran d'aide
	mOVL_debug = false;		// Affichage du debug/framerate
	mCptFrame  = 0;
	mRunNumber = 0;         // compte les passages dans le listener

	//debug
	focale           = mCamera->getFOVy();
	mTriplet         = ColourValue::White;
	mAttenuation     = 100;
	mSkyboxInProgress= false;
	mSkyboxStepNumber= 0;
	detec	         = true;
	mWPnumber        = 0;
	mShadowsOn       = false;
	
	mHelpInterface   = NULL;
	
	mLastFoundObject = NULL;
	mFoundObject     = NULL;
	mLastFoundID     = NULL_ID;
	mFoundID         = NULL_ID;
	m3Dmode          = true;
	mContinue        = true;

	mInputTypeSwitchingOn = true;   // Si true: gestion d'un buffer de touches. 
                                    // Si false: gestion des touches par snapshot
	mMouse_GestionAppli   = true;   // false = La souris est gérée par le Mouse Listener
	mKeys_GestionAppli    = true;   // true  = Les touches sont gérées par l'application
	// Autres initialisations
	srand(1000);


	if (mInputTypeSwitchingOn)
	{
		mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(win);
		mEventProcessor->startProcessingEvents();
		mEventProcessor->addKeyListener(this);
		mEventProcessor->addMouseListener(this);
		mEventProcessor->addMouseMotionListener(this);    // fait apparaitre le curseur.
		mInputDevice = mEventProcessor->getInputReader();
	}
    else
    {
		mInputDevice = PlatformManager::getSingleton().createInputReader();
		mInputDevice->initialise(win,true, true);
    }

	// Initialisation de la qualité du rendu
	mAniso = 1;
	mFiltering = TFO_BILINEAR;


	// Initialisation Camera: a l'arret
	mSpeed=60;
	mMoveScale = 0;
	mRotScale  = 0;
	mUpDownVector    = Vector3::ZERO;
	mTranslateVector = Vector3::ZERO;

	// Creation d'une sphere pour le Picker
	test_sphere = new Sphere(Vector3(0,0,0), 40);	// center & radius



	// Fenetre de statistiques
	this->showDebugOverlay(mOVL_debug);
	// Fenetre d'aide
	this->showHelpOverlay(mOVL_help);
	// Crosshair au centre de l'écran
	this->showCrosshairOverlay(true);
	
//	mInputDevice->setBufferedInput(!mKeys_GestionAppli, !mMouse_GestionAppli);
	this->initCEGUISystem();

	mVersion_ID = " Long Forgotten Earth \n version "+ String(VERSION_NB) + "\n compiled on " + __DATE__ ;

	DEBUG_LOG("<< mainFrameListener()");
	
}

/* -------------------------------------------------------------------------- */
/// Destructeur
/* -------------------------------------------------------------------------- */
mainFrameListener::~mainFrameListener()
{
	delete(test_sphere);

	if (mInputTypeSwitchingOn)
	{
		delete mEventProcessor;
	}
    else
	{
		PlatformManager::getSingleton().destroyInputReader( mInputDevice );
	}

	delete CEGUI::System::getSingletonPtr();
	if (mGUIRenderer)
	{
		delete mGUIRenderer;
		mGUIRenderer = NULL;
	}
}


/* -------------------------------------------------------------------------- */
/// Initialise le type de clavier
/* -------------------------------------------------------------------------- */
void mainFrameListener::setKeyboardType(String LanguageCode)
{
	KeyboardTranslator::KeyMap LanguageMap = KeyboardTranslator::KEYMAP_ENGLISH;

	if (LanguageCode=="fre")
	{
		// Gestion des touches du clavier Français
		LanguageMap = KeyboardTranslator::KEYMAP_FRENCH;
	}

	new KeyboardTranslator(LanguageMap);
}


/* -------------------------------------------------------------------------- */
/// Initialize the CE-GUI System
/* -------------------------------------------------------------------------- */
void mainFrameListener::initCEGUISystem()
{
	using namespace CEGUI;

	TRACE(">> initCEGUISystem()");

	// setup GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000); 
	new CEGUI::System(mGUIRenderer);

	try
	{
//		Logger::getSingleton().setLoggingLevel(Standard);
		Logger::getSingleton().setLoggingLevel(Informative);
//		Logger::getSingleton().setLoggingLevel(Insane);

		// load scheme and set up defaults
		SchemeManager::getSingleton().loadScheme((utf8*)"../media/gui/TaharezLook.scheme");
//		System::getSingleton().setDefaultMouseCursor((utf8*)"TaharezLook", (utf8*)"MouseArrow");
//		System::getSingleton().setDefaultFont("Tahoma-12");
//		System::getSingleton().setDefaultFont("Andromeda");
		System::getSingleton().setDefaultFont("SpaceGothic");
		
		// Creation de la fenetre ROOT
		Window* sheet = WindowManager::getSingleton().createWindow((utf8*)"DefaultWindow", "root_wnd");
		System::getSingleton().setGUISheet(sheet);

		// Creation des imageset du jeu
		Imageset*   giset = ImagesetManager::getSingleton().createImageset("../media/gui/ogregui.imageset");
	}

	// catch to prevent exit (errors will be logged).
	catch(CEGUI::Exception)
	{}

}

/* -------------------------------------------------------------------------- */
/// Gestion des touches qui peuvent etre appuyées de façon continue et.simultanée
/* -------------------------------------------------------------------------- */
void mainFrameListener::processKey_Arrows()
{
	if (mInputDevice->isKeyDown(KC_LEFT))	{mTranslateVector.x = -mMoveScale;} // Move camera LEFT
	if (mInputDevice->isKeyDown(KC_RIGHT))	{mTranslateVector.x =  mMoveScale;} // Move camera RIGHT
    if (mInputDevice->isKeyDown(KC_UP))		{mTranslateVector.z = -mMoveScale;} // Move camera FORWARD
	if (mInputDevice->isKeyDown(KC_DOWN))	{mTranslateVector.z =  mMoveScale;} // Move camera BACKWARD
	if (mInputDevice->isKeyDown(KC_RSHIFT))	{Player::getSingletonPtr()->setRunMode();} // Run Mode
	if (mInputDevice->isKeyDown(KC_LSHIFT))	{Player::getSingletonPtr()->setRunMode();} // Run Mode
	if (mInputDevice->isKeyDown(KC_PGUP))	{}    // unused
	if (mInputDevice->isKeyDown(KC_PGDOWN))	{}    // unused

}

/* -------------------------------------------------------------------------- */
/// Gestion des touches qui peuvent etre appuyées de façon continue et simultanée. 
/** Touches AlphaNumeriques uniquement. Attention à la gestion des claviers QWERTY */
/* -------------------------------------------------------------------------- */
void mainFrameListener::processKey_Moves()
{
	if (mInputDevice->isKeyDown(KC_Q)) {mRotX =  mRotScale;  }             /// A: pivote à gauche 
	if (mInputDevice->isKeyDown(KC_E)) {mRotX = -mRotScale;  }             /// E: pivote a droite
	if (mInputDevice->isKeyDown(KC_A)) {mTranslateVector.x = -mMoveScale;} /// Q: Move camera LEFT 
	if (mInputDevice->isKeyDown(KC_D)) {mTranslateVector.x =  mMoveScale;} /// D: Move camera RIGHT
    if (mInputDevice->isKeyDown(KC_W)) {mTranslateVector.z = -mMoveScale;} /// Z: Move camera FORWARD
	if (mInputDevice->isKeyDown(KC_S)) {mTranslateVector.z =  mMoveScale;} /// S: Move camera BACKWARD
}


/* -------------------------------------------------------------------------- */
/// Gestion des touches a appui simple
/* -------------------------------------------------------------------------- */
void mainFrameListener::processKeyPressed(KeyEvent* e)
{
	// --------------------------------------
	// Touches 
	// --------------------------------------
	switch (e->getKey())
	{
	case KC_SYSRQ:               /// Touche IMPRSCR: faire un screenshot.
        mWindow->writeContentsToTimestampedFile("../screenshot_",".jpg");
		Clock::getSingletonPtr()->showMessageOverlay("Screenshot !");
		break;
	case KC_F1:                  /// Touche F1: Afficher le Help Screen.
		mOVL_help = !mOVL_help;
		this->showHelpOverlay(mOVL_help);
		break;
	case KC_F12:                 /// Touche F12: Afficher le Frame Rate.
		Clock::getSingletonPtr()->showMessageOverlay("F12=Show/Hide Framerate for window "+mWindow->getName());
		mOVL_debug = !mOVL_debug;
		showDebugOverlay(mOVL_debug);
		break;
	}

	// *****************************************
	// DEBUG
	// *****************************************
	switch (e->getKey())
	{
	// KC_A : reserved

	case KC_B:                   /// Touche B: change d'étage normalement
		{
			Clock::getSingletonPtr()->showMessageOverlay("Debug B: leaving level "+ITOA(Player::getSingletonPtr()->getCurrentLevel()));
			switch(Player::getSingletonPtr()->getCurrentLevel()) 
			{
			case 3:
				Player::getSingletonPtr()->goToLevel(4);
				break;
			case 4:
				Player::getSingletonPtr()->goToLevel(3);
				break;
			}
			break; 
		}
		
	case KC_C:                   /// Touche C: show all the rooms
		{
		Clock::getSingletonPtr()->showMessageOverlay("Debug C = Show all the rooms");
		int L = Player::getSingletonPtr()->getCurrentLevel();
		TheShip::getSingletonPtr()->getShipLevel(L)->showAllRooms();
		break;
		}

	// KC_D : reserved
	// KC_E : reserved

	case KC_F:                   /// Touche F: Affiche la position des lumières
		{
		TheShip::getSingletonPtr()->_showAllLightPosition();
		Clock::getSingletonPtr()->showMessageOverlay("Debug F = Show Light Positions");
		break;
		}

	case KC_G:                   /// Touche G: Affiche la position du robot
		{
		Robot* Wrobot = (Robot*)mSceneMgr->getEntity("ADdroid")->getUserObject();
		Vector3 P = Wrobot->getPosition();
		Clock::getSingletonPtr()->showMessageOverlay("Debug G = Position ADdroid Robot " + ITOA(int(P.x))+" "+ITOA(int(P.y))+" "+ITOA(int(P.z)) + StringConverter::toString(Wrobot->isVisible()));
		break;
		}

	case KC_H:                   /// Touche H: Retrécit la focale
		focale-=Degree(1);
		mCamera->setFOVy(focale);
		Clock::getSingletonPtr()->showMessageOverlay("Debug H = Reducing focal "+ITOA(focale));
		break;

	case KC_I:                  /// Touche I: Commute la vitesse de deplacement
		if      (mSpeed>10000) mSpeed=60;
		else if (mSpeed==60)   mSpeed=170;
		else if (mSpeed==170)  mSpeed=1000;
		else                   mSpeed=17000;
		Clock::getSingletonPtr()->showMessageOverlay ("Debug I = Changing camera speed: "+ITOA(mSpeed));
		break;

    case KC_J:                  /// Touche J: change d'étage brusquement
		{
			switch(Player::getSingletonPtr()->getCurrentLevel()) 
			{
			case 3:
				Clock::getSingletonPtr()->showMessageOverlay("Debug J = Jump to level 4");
				Player::getSingletonPtr()->jumpToLevel(4);
				break;
			case 4:
				Clock::getSingletonPtr()->showMessageOverlay("Debug J = Jump to level 3");
				Player::getSingletonPtr()->jumpToLevel(3);
				break;
			}
			break; 
		}

	case KC_K:                  /// Touche K: Active/Désactive les ombres
		mShadowsOn = !mShadowsOn;
		TheShip::getSingletonPtr()->mSpot1->setHilight(mShadowsOn);
		break;

	case KC_L:                  /// Touche L : rendu Solide
		Clock::getSingletonPtr()->showMessageOverlay("Debug L = Solid Rendering");
		mCamera->setDetailLevel (SDL_SOLID);
		break;

	case KC_M: 
		Clock::getSingletonPtr()->showMessageOverlay("Debug , = unused");
		break;

	case KC_N:                  /// Touche N : Vitesse Nulle
		Clock::getSingletonPtr()->showMessageOverlay("Debug N = Stop the Ship");
		TheShip::getSingletonPtr()->mSpeedVector=Vector3::ZERO;
		break;

	case KC_O:                  /// Touche O : rendu Wireframe
		Clock::getSingletonPtr()->showMessageOverlay("Debug O = Wireframe rendering");
		mCamera->setDetailLevel (SDL_WIREFRAME);
		break;

	case KC_P:                  /// Touche P: Consomme tout l'air
		Clock::getSingletonPtr()->showMessageOverlay("Debug: P=Collision Enable/Disable");
		TheShip::getSingletonPtr()->mAirProdSystem->setValue(0);
		TheShip::getSingletonPtr()->mWaterProdSystem->setValue(0);
		break;

	// KC_Q: reserved

	case KC_R:                   ///Touche R: Ajoute un Waypoint
		{
		Clock::getSingletonPtr()->showMessageOverlay("Debug R = Add a new Waypoint");
		String WP = "<waypoint ";
		WP += "x='"+ITOA(int(Player::getSingletonPtr()->getPosition().x))+"' ";
		WP += "y='"+ITOA(int(Player::getSingletonPtr()->getPosition().y))+"' ";
		WP += "z='"+ITOA(int(Player::getSingletonPtr()->getPosition().z))+"' ";
		WP += "id='wp9-"+ITOA(mWPnumber++)+"' />\n";
		FILE* WPfile;
		WPfile = fopen("../media/waypoints/new_WP.xml","a");
		fwrite(WP.c_str(), sizeof(char), WP.length(), WPfile);
		fclose(WPfile);
		break;
		}

	// KC_S: reserved

	case KC_T:                    /// Touche T: change de type de rendu 3D
		Clock::getSingletonPtr()->showMessageOverlay("Debug T = Change Rendering Algorithm");
		switch(mFiltering)
		{
		case TFO_BILINEAR:
			mFiltering = TFO_TRILINEAR;
			Clock::getSingletonPtr()->showMessageOverlay("Rendu Trilineaire");
			mAniso = 1;
			break;
		case TFO_TRILINEAR:
			mFiltering = TFO_ANISOTROPIC;
			Clock::getSingletonPtr()->showMessageOverlay("Rendu Anisotropique");
			mAniso = 8;
			break;
		case TFO_ANISOTROPIC:
			mFiltering = TFO_BILINEAR;
			Clock::getSingletonPtr()->showMessageOverlay("Rendu Bilineaire");
			mAniso = 1;
			break;
		}
		MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
		MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);
		break;

	case KC_U:                    /// Touche U: Affiche la Position p/r au Ship
		Clock::getSingletonPtr()->showMessageOverlay("Debug U = Position Player "+ITOA(Player::getSingletonPtr()->getPosition()));
		break;

	case KC_V:                    /// Touche V: Affiche la Position p/r au centre de la piece
		{
		Vector3 camrV = mCamera->getPosition();
		Vector3 roomV = TheShip::getSingletonPtr()->getActiveRoom()->getNode()->getPosition();
		Vector3 DistV = camrV - roomV;
		Clock::getSingletonPtr()->showMessageOverlay("Debug V = Position mCamera/Room "+ITOA(int(DistV.x))+" "+ITOA(int(DistV.y))+" "+ITOA(int(DistV.z)));
		break;
		}
		
	// KC_W: reserved

	case KC_X:                     /// Touche X: Déplace le Ship
		{
		Clock::getSingletonPtr()->showMessageOverlay("Debug X = Move the ship of 1000 units along Z");
		Vector3 ShipPos = GalaxyBox::getSingletonPtr()->getPosition();
		GalaxyBox::getSingletonPtr()->setShipLocalPosition(ShipPos.x, ShipPos.y, ShipPos.z+1000);
		break;
		}

	case KC_Y:	                  /// Touche Y: Elargit la focale
		// La focale est en radian, mais on lui ajoute 1 degree (avec conversion automatique).
		focale += Degree(1);
		mCamera->setFOVy(focale);
		Clock::getSingletonPtr()->showMessageOverlay("Debug Y = Enlarging focal "+ITOA(focale));
		break;

	/************************************************************************/
	/* Touches Numeriques                                                   */
	/************************************************************************/

	case KC_1:                   /// Touche 1: Se rend directement au Niveau 1
		{
		int     L = Player::getSingletonPtr()->getCurrentLevel();
		TheShip::getSingletonPtr()->getShipLevel(L)->hideAllRooms();
		Vector3 P = TheShip::getSingletonPtr()->_getCamPosition(1);
		Player::getSingletonPtr()->setPosition(P);
		Player::getSingletonPtr()->jumpToLevel(1);
		Clock::getSingletonPtr()->showMessageOverlay("Debug 1 = Jump to Level 1");
		break;
		}
		
	case KC_2:                  /// Touche 2: Se rend directement au Niveau 2
		{
		int     L = Player::getSingletonPtr()->getCurrentLevel();
		TheShip::getSingletonPtr()->getShipLevel(L)->hideAllRooms();
		GalaxyBox::getSingletonPtr()->hide();                           // test
		Vector3 P = TheShip::getSingletonPtr()->_getCamPosition(2);
		Player::getSingletonPtr()->setPosition(P);
		Player::getSingletonPtr()->jumpToLevel(2);
		Clock::getSingletonPtr()->showMessageOverlay("Debug 2 = Jump to Level 2");
		break;
		}
		
	case KC_3:                  /// Touche 3: Se rend directement au Niveau 3
		{
		int     L = Player::getSingletonPtr()->getCurrentLevel();
		TheShip::getSingletonPtr()->getShipLevel(L)->hideAllRooms();
		Vector3 P = TheShip::getSingletonPtr()->_getCamPosition(3);
		Player::getSingletonPtr()->setPosition(P);
		Player::getSingletonPtr()->jumpToLevel(3);
		Clock::getSingletonPtr()->showMessageOverlay("Debug 3 = Jump to Level 3");
		break;
		}

	case KC_4:                   /// Touche 4: Se rend directement au Niveau 4
		{
		int     L = Player::getSingletonPtr()->getCurrentLevel();
		TheShip::getSingletonPtr()->getShipLevel(L)->hideAllRooms();
		Vector3 P = TheShip::getSingletonPtr()->_getCamPosition(4);
		Player::getSingletonPtr()->setPosition(P);
		Player::getSingletonPtr()->jumpToLevel(4);
		Clock::getSingletonPtr()->showMessageOverlay("Debug 4 = Jump to Level 4");
		break;
		}

	case KC_5:                   /// Touche 5: Se rend directement au Niveau 5
		{
		Vector3 P = TheShip::getSingletonPtr()->_getCamPosition(5);
		Player::getSingletonPtr()->setPosition(P);
		Player::getSingletonPtr()->jumpToLevel(5);
		Clock::getSingletonPtr()->showMessageOverlay("Debug 5 = Jump to Level 5");
		break;
		}

	case KC_6:                   /// Touche 6: recule la lampe 
		{
		SceneNode* N = (SceneNode*)mSceneMgr->getSceneNode("room_nav/L1");
		N->translate(0,0,-5);
		Room* R = TheShip::getSingletonPtr()->getActiveRoom();
		if (R) R->_updateLightMarker(N);
		break;
		}

	case KC_7:                   /// Touche 7: Avance la lampe 
		{
		SceneNode* N = (SceneNode*)mSceneMgr->getSceneNode("room_nav/L1");
		N->translate(0,0,+5);
		Room* R = TheShip::getSingletonPtr()->getActiveRoom();
		if (R) R->_updateLightMarker(N);
		break;
		}

	case KC_0:                    /// Touche 0: Se rend directement au Niveau 0
		{
		Vector3 P = TheShip::getSingletonPtr()->_getCamPosition(0);
		Player::getSingletonPtr()->jumpToLevel(0);
		Clock::getSingletonPtr()->showMessageOverlay("Debug 0 = Jump to Level 0");
		break;
		}

	/************************************************************************/
	/* Pavé Numerique                                                       */
	/************************************************************************/

	case KC_NUMPAD7:             /// Touche KC_NUMPAD7: RGB: incremente R
		{
		mTriplet += ColourValue(0.1,0,0); 
		TheShip::getSingletonPtr()->getActiveRoom()->setLightColour(mTriplet);
		TheShip::getSingletonPtr()->getActiveRoom()->setLight(true);
		Clock::getSingletonPtr()->showMessageOverlay("Debug: Colour = "+ITOA(mTriplet.val[0])
		                                 +" "+ITOA(mTriplet.val[1])+" "+ITOA(mTriplet.val[2]));
		break;
		}
	case KC_NUMPAD8:            /// Touche KC_NUMPAD8: RGB: incremente G
		{
		mTriplet += ColourValue(0,0.1,0); 
		TheShip::getSingletonPtr()->getActiveRoom()->setLightColour(mTriplet);
		TheShip::getSingletonPtr()->getActiveRoom()->setLight(true);
		Clock::getSingletonPtr()->showMessageOverlay("Debug: Colour = "+ITOA(mTriplet.val[0])
		                                 +" "+ITOA(mTriplet.val[1])+" "+ITOA(mTriplet.val[2]));
		break;
		}
	case KC_NUMPAD9:            /// Touche KC_NUMPAD9: RGB: incremente B
		{
		mTriplet += ColourValue(0,0,0.1); 
		TheShip::getSingletonPtr()->getActiveRoom()->setLightColour(mTriplet);
		TheShip::getSingletonPtr()->getActiveRoom()->setLight(true);
		Clock::getSingletonPtr()->showMessageOverlay("Debug: Colour = "+ITOA(mTriplet.val[0])
		                                 +" "+ITOA(mTriplet.val[1])+" "+ITOA(mTriplet.val[2]));
		break;
		}
	case KC_NUMPAD1:            /// Touche KC_NUMPAD1: RGB: décremente R
		{
		mTriplet -= ColourValue(0.1,0,0); 
		TheShip::getSingletonPtr()->getActiveRoom()->setLightColour(mTriplet);
		TheShip::getSingletonPtr()->getActiveRoom()->setLight(true);
		Clock::getSingletonPtr()->showMessageOverlay("Debug: Colour = "+ITOA(mTriplet.val[0])
		                                 +" "+ITOA(mTriplet.val[1])+" "+ITOA(mTriplet.val[2]));
		break;
		}
	case KC_NUMPAD2:            /// Touche KC_NUMPAD2: RGB: décremente G
		{
		mTriplet -= ColourValue(0,0.1,0); 
		TheShip::getSingletonPtr()->getActiveRoom()->setLightColour(mTriplet);
		TheShip::getSingletonPtr()->getActiveRoom()->setLight(true);
		Clock::getSingletonPtr()->showMessageOverlay("Debug: Colour = "+ITOA(mTriplet.val[0])
		                                 +" "+ITOA(mTriplet.val[1])+" "+ITOA(mTriplet.val[2]));
		break;
		}
	case KC_NUMPAD3:            /// Touche KC_NUMPAD3: RGB: décremente B
		{
		mTriplet -= ColourValue(0,0,0.1); 
		TheShip::getSingletonPtr()->getActiveRoom()->setLightColour(mTriplet);
		TheShip::getSingletonPtr()->getActiveRoom()->setLight(true);
		Clock::getSingletonPtr()->showMessageOverlay("Debug: Colour = "+ITOA(mTriplet.val[0])
		                                 +" "+ITOA(mTriplet.val[1])+" "+ITOA(mTriplet.val[2]));
		break;
		}

	/************************************************************************/
	/* Touches de Fonction                                                  */
	/************************************************************************/

	// KC_F1: reserved

	case KC_F2:                   /// Touche F2: Change the Star Spectrum
		Clock::getSingletonPtr()->showMessageOverlay("Debug F2: Change the Star Spectrum");
		GalaxyBox::getSingletonPtr()->_changeStar();
		break;

	case KC_F3:                   /// Touche F3: Allume la lumiere Normale
		Clock::getSingletonPtr()->showMessageOverlay("Debug F3 = Normal Light On");
		TheShip::getSingletonPtr()->setRedLight(false);
		break;
		
	case KC_F4:                   /// Touche F4: Allume la lumiere Rouge
		Clock::getSingletonPtr()->showMessageOverlay("Debug F4 = Red Lights On");
		TheShip::getSingletonPtr()->setRedLight(true);
		break;
		
	case KC_F5:                   /// Touche F5: Lumières pleins pots
		Clock::getSingletonPtr()->showMessageOverlay("Debug F5 = Full Ambient Light On");
		mSceneMgr->setAmbientLight(ColourValue::White);
		break;
		
	case KC_F6:                   /// Touche F6: Infos sur la planete
		{
		String R1 = GalaxyBox::getSingletonPtr()->getSystemParameter(1); // catalog ID
		String R2 = GalaxyBox::getSingletonPtr()->getSystemParameter(2); // systeme name
		String R3 = GalaxyBox::getSingletonPtr()->getSystemParameter(3); // nb de planetes
		String R4 = GalaxyBox::getSingletonPtr()->getSystemParameter(4); // spectre
		Clock::getSingletonPtr()->showMessageOverlay("Debug F6 = Planet Info \n'Global Galactic Catalog' \n"+R1+"\n"+R2+"\n"+R4+"\n"+R3);
		break;
		}
		
	case KC_F7:                   /// Touche F7: Affichage du No de version 
		Clock::getSingletonPtr()->showMessageOverlay(mVersion_ID);
		break;

	case KC_F8:                   /// Touche F8: Diminue la portée du son
		Clock::getSingletonPtr()->showMessageOverlay ("Debug F8 = FMOD distance factor "+ITOA(mAttenuation));
		mAttenuation -= 10;
		FSOUND_3D_SetDistanceFactor(mAttenuation);
		break;

	case KC_F9:                   /// Touche F9: Augmente la portée du son
		Clock::getSingletonPtr()->showMessageOverlay ("Debug F9 = FMOD distance factor "+ITOA(mAttenuation));
		mAttenuation += 10;
		FSOUND_3D_SetDistanceFactor(mAttenuation);
		break;

	case KC_F10:                  /// Touche F10: Affichage des Waypoints
		Clock::getSingletonPtr()->showMessageOverlay ("Debug F10= showing waypoints");
		TheShip::getSingletonPtr()->_showWPnetwork();
		break;

	case KC_F11:                   /// Touche F11: Creation d'une skybox 
		mSkyboxInProgress=true;
		break;

	// KC_F12:                     /// Touche F12: Réservé : framerate
	}
}

/* -------------------------------------------------------------------------- */
/// Gestion des mouvements de la souris
/* -------------------------------------------------------------------------- */
void mainFrameListener::processMouse(MouseEvent* e)
{
	mMouseButton0 = (e->getButtonID() == Ogre::MouseEvent::BUTTON0_MASK );
	mMouseButton1 = (e->getButtonID() == Ogre::MouseEvent::BUTTON1_MASK );

	// we rotate the camera
	mRotX = e->getRelX() * (-1.5);
	mRotY = e->getRelY() * (-1.5);

	// MouseWheel : Move camera 
//	if( mInputDevice->getMouseRelativeZ()>0) {mTranslateVector.z = -mMoveScale * 10;}
//	if( mInputDevice->getMouseRelativeZ()<0) {mTranslateVector.z =  mMoveScale * 10;}

}


/* -------------------------------------------------------------------------- */
/// Affiche une petite croix au centre de l'écran (crosshair)
/* -------------------------------------------------------------------------- */
void mainFrameListener::showCrosshairOverlay(bool show)
{
	TRACE(">> showCrosshairOverlay");

	Overlay* OverlayPage = (Overlay*)OverlayManager::getSingleton().getByName("sphinx/CrosshairOverlay");
	if (!OverlayPage)
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find sphinx/CrosshairOverlay ","showCrosshairOverlay" );

	if (show)  { OverlayPage->show(); }
	else       { OverlayPage->hide(); }
}


/* -------------------------------------------------------------------------- */
/// Affiche des informations de debug sur l'overlay DEBUG (framerate, etc)
/* -------------------------------------------------------------------------- */
void mainFrameListener::showDebugOverlay(bool show)
{
	Overlay* OverlayPage = (Overlay*)OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	if (!OverlayPage)
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find Core/DebugOverlay ","showDebugOverlay" );

	if (show)  { OverlayPage->show(); }
	else       { OverlayPage->hide(); }
}


/* -------------------------------------------------------------------------- */
/// Affichage de l'ihm "HELP SCREEN"
/* -------------------------------------------------------------------------- */
void mainFrameListener::showHelpOverlay(bool show)
{
	if (show)
	{
		// On cree une interface avec l'ecran d'aide
		mHelpInterface = new Simple_Interface("../media/interface/help.xml");
		static_cast<Simple_Interface*>(mHelpInterface)->createInterface();
		// On passe en mode GUI
		this->showCrosshairOverlay(false);
		m3Dmode = false;
	}
	else 
	{
		// On detruit l'interface (si elle existe)
		if (mHelpInterface)
			delete (static_cast<Simple_Interface*>(mHelpInterface));
		mHelpInterface=NULL;
		// On retourne en mode 3D
		this->showCrosshairOverlay(true);
		m3Dmode = true;
	}
}

/* -------------------------------------------------------------------------- */
/// Actions en debut de frame
/* -------------------------------------------------------------------------- */
bool mainFrameListener::frameStarted(const FrameEvent& evt)
{
	if (!mInputTypeSwitchingOn)
   	{
		mInputDevice->capture();
    }


	// -------------------------------------------------------
	// On décremente le timer d'antirebond
	// -------------------------------------------------------
//	if (mTimeUntilNextToggle >= 0) mTimeUntilNextToggle -= evt.timeSinceLastFrame;


	switch (mRunNumber)
	{
	// -------------------------------------------------------
	// Lors du premier passage: on effectue quelques initialisations
	// -------------------------------------------------------
	case 0:
		{
		// On charge les paramètre initiaux concernant le vaisseau, et tout son contenu
		TheShip::getSingletonPtr()->loadInitialSituation();
		// Affichage de la piece où se trouve le joueur
		TheShip::getSingletonPtr()->showActiveRoom(Player::getSingletonPtr()->getInitialRoom());
		// On affiche le No de version
		Clock::getSingletonPtr()->showMessageOverlay(mVersion_ID);
		// On commence la gestion dynamique de l'univers
		TRACE("loading stars.");
		GalaxyBox::getSingletonPtr()->loadAllStars();
		GalaxyBox::getSingletonPtr()->createStars();
		Root::getSingletonPtr()->addFrameListener(GalaxyBox::getSingletonPtr());
		TheShip::getSingletonPtr()->initOISSystem();
		OgreOpcode::CollisionManager::getSingletonPtr()->GetDefaultContext()->Reset();

		mRunNumber++;
		break;
		}
	
	// -------------------------------------------------------
	// Lors du second passage: on ajoute les NPC
	// -------------------------------------------------------
	case 1:
		{
		// Les robots ont déjà été créés, mais il ne sont pas encore dans la scene.
		static_cast<Robot*>(mSceneMgr->getEntity("ADdroid")->getUserObject())->addToScene();
		static_cast<Robot*>(mSceneMgr->getEntity("MTdroid")->getUserObject())->addToScene();
		static_cast<Robot*>(mSceneMgr->getEntity("TESTrobot")->getUserObject())->addToScene();
		mRunNumber++;
		break;
		}

	// -------------------------------------------------------
	// Autres passages
	// -------------------------------------------------------
	default:
		{
		// We scale the movement units by time passed since last frame
		mMoveScale = mSpeed * evt.timeSinceLastFrame;
		// Take about 20 seconds for full rotation (6.28/20=0.3)
		mRotScale = 0.3 * evt.timeSinceLastFrame;
		}
	}

	OgreOpcode::CollisionManager::getSingletonPtr()->GetDefaultContext()->Collide();

	
	//	mUpDownVector    = Vector3::ZERO;
	mTranslateVector = Vector3::ZERO;
//	mRun = 1;

	// -------------------------------------------------------
	// Gestion des touches par defaut. 
	// -------------------------------------------------------
	// On traite ici les touches qui ne sont pas traitées lors du KeyPressed
	// cad (en général) les touches à appui continu (avancer, etc...)
	this->processKey_Arrows();
	if (m3Dmode) this->processKey_Moves();	
	// Les mouvements commandés par les touches alphas ne sont pas actifs en mode GUI

	// -------------------------------------------------------
	// Gestion de la GUI
	// -------------------------------------------------------
	// On gere ici la sortie du mode GUI et le retour au mode 3D.
	if (!m3Dmode)
	{
		switch(mFoundID) 
		{
		case ROBOT_ID:
			m3Dmode = ((Robot*)mFoundObject)->isInterfaceDisabled(); 
			// Astuce pour faire disparaitre la souris
			CEGUI::System::getSingleton().injectMouseMove(0, 0);
			break;

		case HIBERNATION_ID:
			m3Dmode = ((Hibernation*)mFoundObject)->isInterfaceDisabled(); 
			// Astuce pour faire disparaitre la souris
			CEGUI::System::getSingleton().injectMouseMove(0, 0);
			break;

		case COMPUTER_ID:
			if (((Computer*)mFoundObject)->isExitInterfaceRequiered()) 
			{
				m3Dmode=true;
				// On fait disparaitre le GUI
				// Note: ici le GUI est une sous-classe de Computer, pas un objet créé
				static_cast<Computer*>(mFoundObject)->deleteInterface();
				// Astuce pour faire disparaitre la souris
				CEGUI::System::getSingleton().injectMouseMove(0, 0);
			}
			break;
		default:
			{
				// On est en mode GUI, mais aucun objet n'est actif
				// (c'est le cas par exemple, du help screen)
				// TRACE("   GUI for unidentified mFoundID");
			}
		}
	}

	// -------------------------------------------------------
	// Certaines actions ne sont pas executées à chaque passsage
	// -------------------------------------------------------
	switch (mCptFrame)
	{
	case 5:
	case 15:
		// ---------------------------------------------------
		// Gestion de l'objet sous le curseur
		// ---------------------------------------------------
		mFoundID     = NULL_ID;
		mFoundObject = this->pickObject(); 

		if (mFoundObject)     mFoundID=mFoundObject->getTypeID();

		if (mLastFoundObject!=mFoundObject)
		{	
			// *************************************************
			// Le focus a quitté l'objet.
			// *************************************************
			if (mLastFoundID == DOOR_ID)        ((Door*)mLastFoundObject)->setFocusOff();
			if (mLastFoundID == ROBOT_ID)       ((Robot*)mLastFoundObject)->setFocusOff();
			if (mLastFoundID == HIBERNATION_ID) ((Hibernation*)mLastFoundObject)->setFocusOff();
			if (mLastFoundID == COMPUTER_ID)    ((Computer*)mLastFoundObject)->setFocusOff();
			if (mLastFoundID == BUTTON_ID)      ((Button*)mLastFoundObject)->setFocusOff();
			mLastFoundObject=mFoundObject;
			mLastFoundID=mFoundID;
			m3Dmode = true;
		}

		if (mFoundObject) 
		{
			// *************************************************
			// Si on pointe sur un objet, on lui passe le focus.
			// *************************************************
			// Si le curseur passe sur certains objets, cela peut déclencher une action, un bruit, etc 
			// Si ensuite on clique sur l'objet: c'est géré dans mousePressed()
			switch (mFoundID)
			{
			case DOOR_ID:	     ((Door*)mFoundObject)->setFocusOn() ;       break;
			case ROBOT_ID:	     ((Robot*)mFoundObject)->setFocusOn();       break;
			case HIBERNATION_ID: ((Hibernation*)mFoundObject)->setFocusOn(); break;
			case BUTTON_ID:      ((Button*)mFoundObject)->setFocusOn();      break;
			case COMPUTER_ID:    ((Computer*)mFoundObject)->setFocusOn();    break;
			}
			// On memorise l'objet avec focus
			mLastFoundObject = mFoundObject ;
			mLastFoundID     = mFoundID;
		}
		break;
	}
	mCptFrame++;
	if (mCptFrame>30) mCptFrame=0;

	// -------------------------------------------------------
	// Met a jour la position du joueur
	// -------------------------------------------------------
	Player::getSingletonPtr()->moveHead(mRotX, mRotY);
	Player::getSingletonPtr()->moveBody(mTranslateVector);
//	mCamera->move(mUpDownVector);				/// TODO pas ici ....
	Player::getSingletonPtr()->moveEars();
	mRotX = 0;
	mRotY = 0;

	// -------------------------------------------------------
	// quelques traitements particuliers
	// -------------------------------------------------------
	if (mSkyboxInProgress) this->makeSkybox();
	
	return true; 
}


/* -------------------------------------------------------------------------- */
/// Actions en fin de frame:
/* -------------------------------------------------------------------------- */
bool mainFrameListener::frameEnded(const FrameEvent& evt)
{
	/// Update des statistiques d'affichage.
	this->updateStats();
	return (mContinue);
}



/* ---------------------------------------------------*/
/// Handle sur les déplacements de la souris
/* ---------------------------------------------------*/
void 	mainFrameListener::mouseMoved (MouseEvent* e)
{
	if (m3Dmode)
	{
		// En mode 3D, on bouge la camera
		this->processMouse(e);
	}
	else
	{
		/// On peut bouger la souris en maintenant le clic droit
		if (e->getButtonID() == Ogre::MouseEvent::BUTTON1_MASK)
		{
			TRACE(">> mouseMoved with Button1");
			this->processMouse(e);					// TODO: NE MARCHE PAS !
		}
		else
		{
			CEGUI::Renderer* rend = CEGUI::System::getSingleton().getRenderer();
			CEGUI::System::getSingleton().injectMouseMove(e->getRelX() * rend->getWidth(), e->getRelY() * rend->getHeight());
		}
	}
	e->consume();
}

/* ---------------------------------------------------*/
/// Handle sur les déplacements de souris avec bouton 0 appuyé.
/* ---------------------------------------------------*/
void 	mainFrameListener::mouseDragged (MouseEvent* e)
{
	this->mouseMoved(e);
}


/* ---------------------------------------------------*/
/// Gestion du Clic sur des objets.
/* ---------------------------------------------------*/
void 	mainFrameListener::mousePressed (MouseEvent *e)
{
	// TRACE(">> mousePressed");
	if (m3Dmode)
	{
		/// En mode 3D, la souris fait bouger la caméra
		processMouse(e);
		/// En mode 3D, le clic peut activer des objets
		switch (mFoundID)
		{
		case ROBOT_ID:
			/// Activation de l'Interface Robot
			TRACE("Clic on "+((Robot*)mFoundObject)->getName());
			static_cast<Robot*>(mFoundObject)->createInterface(); 
			m3Dmode = false;
			break;
		case HIBERNATION_ID:
			/// Activation de l'Interface des capsules d'hibernation
			TRACE("Clic on "+((Hibernation*)mFoundObject)->getName());
			static_cast<Hibernation*>(mFoundObject)->createInterface(); 
			m3Dmode = false;
			break;
		case COMPUTER_ID:
			{
			/// Activation de l'Interface des Computers
			TRACE("Clic on "+((Computer*)mFoundObject)->getName());
			static_cast<Computer*>(mFoundObject)->createInterface();		
			m3Dmode = false;
			break;
			}
		case BUTTON_ID:
			{
			/// Activation d'un bouton
			static_cast<Button*>(mFoundObject)->actionButton();		
			break;
			}
		case DOOR_ID:
			/// Si on clique sur une porte, on demande l'ouverture de la porte
			TheShip::getSingletonPtr()->openDoor((Door*)mFoundObject);
			break;
		}
	}
	else
	{
		// En mode GUI, on transmet le clic à CEGUI
		CEGUI::System::getSingleton().injectMouseButtonDown(convertOgreButtonToCegui(e->getButtonID()));
	}
	e->consume();
}

/* ---------------------------------------------------*/
/// Handle sur la relache du bouton de souris
/* ---------------------------------------------------*/
void 	mainFrameListener::mouseReleased (MouseEvent *e)
{
	// TRACE(">> mouseReleased()");
	if (!m3Dmode) 
	{
		CEGUI::System::getSingleton().injectMouseButtonUp(convertOgreButtonToCegui(e->getButtonID()));
	}
	e->consume();
}

/* ---------------------------------------------------*/
/// Clicked = Pressed puis Released
/* ---------------------------------------------------*/
void 	mainFrameListener::mouseClicked (MouseEvent* e)
{
	// DEBUG_LOG(">> mouseClicked");
}

/* ---------------------------------------------------*/
/// Gestion evenementielle du clavier
/* ---------------------------------------------------*/
void 	mainFrameListener::keyPressed (KeyEvent* e)
{
	if (m3Dmode)
	{
		// On gere les touches du clavier
		this->processKeyPressed(e);
		// Si mode 3D: la touche ESC fait quitter le jeu
		if (e->getKey()==KC_ESCAPE) mContinue = false; 
	}
	else
	{
		// On gere les differents claviers (QWERTY, AZERTY, etc)
		KeyboardTranslator::getSingleton().translate(e->getKeyChar());
		unsigned char translatedKey = KeyboardTranslator::getSingleton().getKey();

		// do Key injection for CEGUI
		CEGUI::System& cegui = CEGUI::System::getSingleton();
		cegui.injectKeyDown(e->getKey());
		cegui.injectChar(translatedKey);	// inject translated chararacter 
		// cegui.injectChar(e->getKeyChar());	// inject chararacter directly

		// En Mode GUI: la touche ESC fait quitter le GUI
		if ((e->getKey()==KC_ESCAPE) && (mFoundID==ROBOT_ID))
		{
			((Robot*)mFoundObject)->setFocusOff(); 
//			m3Dmode = true;
		}

		// En Mode GUI: la touche ESC fait quitter le GUI
		if (e->getKey()==KC_ESCAPE)		m3Dmode = true;

		// En Mode GUI: la touche F1 est gérée
		if (e->getKey()==KC_F1)	 this->processKeyPressed(e);
		if (e->getKey()==KC_F12) this->processKeyPressed(e);
		
}
	e->consume();
}


/* ---------------------------------------------------*/
/// Une touche du clavier est relachée
/* ---------------------------------------------------*/
void 	mainFrameListener::keyReleased (KeyEvent* e)
{
	// TRACE(">> keyReleased");
	if (!m3Dmode)
	{
		// Ceci permet la recuperation des touches dans les editbox de CEGUI
		CEGUI::System::getSingleton().injectKeyUp(e->getKey());
	}
}

/* -------------------------------------------------------------------------- */
/// Instanciation des fonctions virtuelles de KeyListener
/* -------------------------------------------------------------------------- */
void mainFrameListener::keyClicked (KeyEvent* e) 
{
	// TRACE(">> keyClicked");
}



/* -------------------------------------------------------------------------- */
/// Met a jour les informations de frame rate
/* -------------------------------------------------------------------------- */
void mainFrameListener::updateStats()
{
	static String currFps  = "Current FPS: ";
	static String avgFps   = "Average FPS: ";
	static String bestFps  = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris     = "Triangle Count: ";

	
	OverlayElement* guiAvg   = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
	OverlayElement* guiCurr  = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
	OverlayElement* guiBest  = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");
	OverlayElement* guiTris  = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");

	const RenderTarget::FrameStats& stats = mWindow->getStatistics();

	guiAvg  ->setCaption (avgFps  + ITOA(stats.avgFPS));
	guiCurr ->setCaption (currFps + ITOA(stats.lastFPS));
	guiBest ->setCaption (bestFps + ITOA(stats.bestFPS) +" ("+ITOA(stats.bestFrameTime) +" ms)");
	guiWorst->setCaption (worstFps+ ITOA(stats.worstFPS)+" ("+ITOA(stats.worstFrameTime)+" ms)");
	guiTris ->setCaption (tris    + ITOA(stats.triangleCount));

	// MAJ de debugText
	// OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
	// guiDbg->setCaption(mWindow->getDebugText());
}





/* -------------------------------------------------------------------------- */
/// Regarde si le curseur survole un objet de la scene.
/* -------------------------------------------------------------------------- */
UserDefinedObject* mainFrameListener::pickObject()
{
	// Cree un rayon partant du centre de la camera vers l'avant
  	Ray PickingRay = mCamera->getCameraToViewportRay(0.5, 0.5);
  	// Lancement du rayon
  	RaySceneQuery* PickingRayQuery = mSceneMgr->createRayQuery(PickingRay, ACTIVABLE);
  	PickingRayQuery->execute();

  	// Recuperation des objets traversés par le rayon
  	RaySceneQueryResult Result = PickingRayQuery->getLastResults();

	UserDefinedObject* closestObject = NULL;
	UserDefinedObject* ListedObject  = NULL;
	Real closestDistance = DISTANCE_MAX;	// distance maximum pour agir sur des objets

	std::list< Ogre::RaySceneQueryResultEntry >::iterator rayIterator;

	// On parcourt la liste des objets trouvés
	for (rayIterator=Result.begin();  rayIterator!=Result.end(); rayIterator++ )
	{
		ListedObject = rayIterator->movable->getUserObject();
		if (ListedObject != NULL)
	 	{
	 		// On a trouvé un objet
			// On ignore les objets de type ROOM
			// TODO : utiliser les masques
        	if (   (ListedObject->getTypeID() != ROOM_ID) 
				&& (rayIterator->distance < closestDistance))
			{
				// C'est le plus proche: On memorise l'objet et sa distance
				closestObject   = ListedObject;
				closestDistance = rayIterator->distance;
			}
		}
	}

	// On detruit le rayon
	PickingRayQuery->clearResults();
	mSceneMgr->destroyQuery(PickingRayQuery);

   	return (closestObject);

}


/* -------------------------------------------------------------------------- */
/// Regarde si le curseur survole un objet fils de l'objet passe en parametre.
/* -------------------------------------------------------------------------- */
SceneNode* mainFrameListener::_pickSubObject(SceneNode* parent_node)
{
	Vector3 rayDirection, rayOrigin;

	// Starting point of the ray :
	// here: the center of the screen

	// Example for a ray not starting from the center of the screen
	// Real screenX = 0.5;
	// Real screenY = 0.5;
	// Calculate the near distance
	// Real nearDist = ( 2 * mCamera->getNearClipDistance() ); 
	// nearDist *= Math::Tan( Math::DegreesToRadians( mCamera->getFOVy()/2 ) ); 
	// Calculate the ray direction 
	// rayDirection.x = ( screenX - 0.5 ) * nearDist * mCamera->getAspectRatio(); 
	// rayDirection.y = ( 0.5 - screenY ) * nearDist; 
	// rayDirection.z = -mCamera->getNearClipDistance(); 

	//Originate the ray at camera position 
	rayOrigin = mCamera->getDerivedPosition(); 

	// Define the ray direction 
	rayDirection.x = 0; 
	rayDirection.y = 0; 
	rayDirection.z = -1; 
	//Adjust ray direction by camera orientation 
	rayDirection = mCamera->getDerivedOrientation() * rayDirection; 
	//Normalise the ray direction 
	rayDirection.normalise(); 

	SceneNode* TestedNode = NULL;
	SceneNode* FoundNode  = NULL; 
	Vector3    TestedNodeV;
	Vector3    FoundNodeV;
	Real       TestedNodeDistance;
	Real       FoundNodeDistance = 50;	// Initialisé avec la distance max d'action.

	// We will iterate all the children of the parent node
	SceneNode::ChildNodeIterator ObjectIterator = parent_node->getChildIterator(); 

	// Iterate all nodes under the parent node: the tested nodes
	while ( ObjectIterator.hasMoreElements() ) 
	{ 
		// Get the next node 
		TestedNode = (SceneNode*)ObjectIterator.getNext(); 

		if (TestedNode!=NULL)
		{
			// Test if the ray intersect with the Object
			if (_hitSphericNode(TestedNode, rayOrigin, rayDirection)) 
//			if (_hitCubicNode(TestedNode, rayOrigin, rayDirection)) 
			{ 
				// A Node was hit
				mWindow->setDebugText("The ray hit "+TestedNode ->getName());
				if ( !FoundNode ) 
				{ 
					// If it is the first tested node: it is the closest.
					FoundNode  = TestedNode; 
					FoundNodeV = FoundNode->getPosition() - mCamera->getPosition(); 
					FoundNodeDistance = FoundNodeV.squaredLength();
				} 
				else 
				{ 
					// Check if the TestedNode  is closer than the already FoundNode
					TestedNodeV = TestedNode->getPosition() - mCamera->getPosition(); 
					TestedNodeDistance = TestedNodeV.squaredLength();
					if ( TestedNodeDistance < FoundNodeDistance ) 
					{ 
						// If so, we store it
						FoundNode  = TestedNode; 
						FoundNodeV = TestedNodeV; 
						FoundNodeDistance = TestedNodeDistance;
					} 
				}
			}
		}
	}
	return FoundNode;
}

/* -------------------------------------------------------------------------- */
/// Renvoie vrai si le rayon croise la sphere située au centre de l'objet
/* -------------------------------------------------------------------------- */
bool mainFrameListener::_hitSphericNode(SceneNode* TestedNode, const Vector3 &origin, const Vector3 &direction) 
{ 
	/// Attention, le rayon de la sphere doit etre dimensionné de façon adequate...
	/// Note: Spheric est plus rapide, mais moins précis que AABB.

	// Put the sphere at the Node position
	test_sphere->setCenter(TestedNode->getPosition());			
	if (Ray(origin, direction).intersects(*test_sphere).first==true 
	   && Ray(origin, direction).intersects(*test_sphere).second>0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}


/* -------------------------------------------------------------------------- */
/// Renvoie vrai si le rayon croise le cube englobant l'objet.
/* -------------------------------------------------------------------------- */
bool mainFrameListener::_hitCubicNode(SceneNode* TestedNode, const Vector3 &origin, const Vector3 &direction) 
{
	/// Note: AABB est plus précis, mais moins rapide que la sphère.
	// Get the corners
	AxisAlignedBox AABB = TestedNode->_getWorldAABB();
	Vector3* corners    = (Vector3*)AABB.getAllCorners();

	// Used to test if a hit was made
	bool hit = false;

	// Test values used in the evaluation
	Real t, lx, ly, lz;

	if( direction.dotProduct( -Vector3::UNIT_Z ) > 0 ) 
	{
		t = ( corners[ 0 ].z - origin.z ) / direction.z;
		lx = origin.x + ( direction.x * t );
		ly = origin.y + ( direction.y * t );

		if( lx > corners[ 1 ].x &&
			lx < corners[ 2 ].x &&
			ly > corners[ 0 ].y &&
			ly < corners[ 1 ].y) 
		{
		// Hit made
		return true;
		}
	}

  if( !hit && direction.dotProduct( Vector3::UNIT_Z ) > 0 ) 
  {
    t = ( corners[ 4 ].z - origin.z ) / direction.z;
    lx = origin.x + ( direction.x * t );
    ly = origin.y + ( direction.y * t );

    if( lx > corners[ 5 ].x &&
        lx < corners[ 4 ].x &&
        ly > corners[ 7 ].y &&
        ly < corners[ 4 ].y ) 
	{
      // Hit made
      return true;
    }
  }

  if( !hit && direction.dotProduct( -Vector3::UNIT_Y ) > 0 ) 
  {
    t = ( corners[ 0 ].y - origin.y ) / direction.y;
    lx = origin.x + ( direction.x * t );
    lz = origin.z + ( direction.z * t );

    if( lx > corners[ 0 ].x &&
        lx < corners[ 3 ].x &&
        lz > corners[ 0 ].z &&
        lz < corners[ 6 ].z ) 
	{
      // Hit made
      return true;
    }
  }

  if( !hit && direction.dotProduct( Vector3::UNIT_Y ) > 0 ) 
  {
    t = ( corners[ 1 ].y - origin.y ) / direction.y;
    lx = origin.x + ( direction.x * t );
    lz = origin.z + ( direction.z * t  );

    if( lx > corners[ 1 ].x &&
        lx < corners[ 2 ].x &&
        lz > corners[ 1 ].z &&
        lz < corners[ 5 ].z ) 
	{
      // Hit made
      return true;
    }
  }

  if( !hit && direction.dotProduct( -Vector3::UNIT_X ) > 0 ) 
  {
    t = ( corners[ 0 ].x - origin.x ) / direction.x;
    ly = origin.y + ( direction.y * t );
    lz = origin.z + ( direction.z * t );

    if( ly > corners[ 0 ].y &&
        ly < corners[ 1 ].y &&
        lz > corners[ 0 ].z &&
        lz < corners[ 6 ].z ) 
	{
      // Hit made
      return true;
    }
  }

  if( !hit && direction.dotProduct( Vector3::UNIT_X ) > 0 ) 
  {
    t = ( corners[ 2 ].x - origin.x ) / direction.x;
    ly = origin.y + ( direction.y * t );
    lz = origin.z + ( direction.z * t );

    if( ly > corners[ 3 ].y &&
        ly < corners[ 2 ].y &&
        lz > corners[ 2 ].z &&
        lz < corners[ 4 ].z ) 
	{
      // Hit made
      return true;
    }
  }

  // No hit was made
  return false;   
}


/* -------------------------------------------------------------------------- */
/// Genere les 6 images d'une Skybox. Renvoie FALSE quand l'automate est terminé.
/* -------------------------------------------------------------------------- */
bool mainFrameListener::_makeSkybox()
{
	Camera*        NewCam;
	Viewport*      NewVP;
	RenderWindow*  NewWin ;
	RenderTexture* NewTex;

	/// Cette methode cree une seconde camera, viewport et renderwindow

	if (!mSkyboxInProgress) return false;

	switch(mSkyboxStepNumber)
	{
	case 0:			/// Step 0: Initialisations

		{
		// Create a new camera
		NewCam = mSceneMgr->createCamera("NewCam");
		NewCam->setWindow(512,512,512,512);
		NewCam->setAspectRatio(Real(mWindow->getViewport(0)->getActualWidth()) / Real(mWindow->getViewport(0)->getActualHeight()));

		// ???
		NewTex = Root::getSingletonPtr()->getRenderSystem()->createRenderTexture( "NewTex", 512, 512, TEX_TYPE_2D, PF_R8G8B8 );

		// Create a new viewport
		NewVP = NewTex->addViewport(NewCam);
		NewVP->setBackgroundColour(ColourValue::Black);


		NewCam->setPosition(mCamera->getPosition());
		NewCam->setFOVy(Degree(90));
		NewCam->setWindow(512,512,512,512);
		NewCam->setAspectRatio(1);	
		// NewCam->setAspectRatio(Real(mWindow->getViewport(0)->getActualWidth()) / Real(mWindow->getViewport(0)->getActualHeight()));	


		mSkyboxStepNumber++;
TRACE ("step 0 done");
		break;
		}


	case 1:		/// Step 1: Creation d'une skybox (FRONT)
		{
		NewCam = mSceneMgr->getCamera("NewCam");
		NewVP  = NewCam->getViewport();
		// Make the Screenshot
		NewWin = (RenderWindow*)NewVP->getTarget();
//		NewTex= (RenderWindow*)Root::getSingleton().getRenderTarget("NewTex");
        NewWin->writeContentsToFile("../CubeSkyBox_FR.jpg");	

		NewCam->yaw(Degree(90));
		mSkyboxStepNumber++;
		mSkyboxInProgress=false;
TRACE ("step 1 done");
		break;
		}

	case 2:     /// Step 2: Creation d'une skybox (LEFT)
		NewCam = mSceneMgr->getCamera("skyboxcam");
		NewWin = (RenderWindow*)Root::getSingleton().getRenderTarget("skyboxwin");
        NewWin->writeContentsToFile("../CubeSkyBox_LF.jpg");
		NewCam ->yaw(Degree(90));
		mSkyboxStepNumber++;
TRACE ("step 2 done");
		break;

	case 3:     /// Step 3: Creation d'une skybox (BACK)
		NewCam = mSceneMgr->getCamera("skyboxcam");
		NewWin = (RenderWindow*)Root::getSingleton().getRenderTarget("skyboxwin");
        NewWin->writeContentsToFile("../CubeSkyBox_BK.jpg");
		NewCam ->yaw(Degree(90));
		mSkyboxStepNumber++;
TRACE ("step 3 done");
		break;

	case 4:     /// Step 4: Creation d'une skybox (RIGHT)
		NewCam = mSceneMgr->getCamera("skyboxcam");
		NewWin = (RenderWindow*)Root::getSingleton().getRenderTarget("skyboxwin");
        NewWin->writeContentsToFile("../CubeSkyBox_RT.jpg");
		NewCam->yaw(Degree(90));	// On revient vers l'avant
		NewCam ->pitch(Degree(90)); // On regarde vers le haut
		mSkyboxStepNumber++;
TRACE ("step 4 done");
		break;

	case 5:     /// Step 5: Creation d'une skybox (UP)
		NewCam = mSceneMgr->getCamera("skyboxcam");
		NewWin = (RenderWindow*)Root::getSingleton().getRenderTarget("skyboxwin");
        NewWin->writeContentsToFile("../CubeSkyBox_UP.jpg");
		NewCam ->pitch(Degree(-180)); // On regarde vers le bas
		mSkyboxStepNumber++;
TRACE ("step 5 done");
        break;

	case 6:     /// Step 6: Creation d'une skybox (DOWN)
		NewCam = mSceneMgr->getCamera("skyboxcam");
		NewWin = (RenderWindow*)Root::getSingleton().getRenderTarget("skyboxwin");
		NewWin->writeContentsToFile("../CubeSkyBox_DN.jpg");
		mSkyboxStepNumber++;
TRACE ("step 6 done");
		break;
		
	case 7:     /// Step 7: Terminate
		mSceneMgr->removeCamera("skyboxcam");
		Root::getSingletonPtr()->detachRenderTarget("skyboxwin");
		Root::getSingletonPtr()->detachRenderTarget("skyboxtex");
	/// TODO: detruitre le viewport
		DEBUG_WIN("Debug: F11= Skybox done");
		mSkyboxStepNumber=0;
		mSkyboxInProgress=false;
TRACE ("step 7 done");
		break;
		
	}

	return (mSkyboxInProgress);
}



/* -------------------------------------------------------------------------- */
/// Genere les 6 images d'une Skybox. Renvoie FALSE quand l'automate est terminé.
/* -------------------------------------------------------------------------- */
bool mainFrameListener::makeSkybox()
{
	Camera*        Skyboxcam;
	RenderWindow*  Skyboxwin;
	Viewport*      Skyboxvp;

	/// Cette methode utilise la camera, viewport et renderwindow du joueur
	if (!mSkyboxInProgress) return false;

	switch(mSkyboxStepNumber)
	{
	case 0:			/// Step 0: Initialisations
		showDebugOverlay(false);
		Clock::getSingletonPtr()->showMessageOverlay(false);
		Skyboxcam = mCamera;
		focale = mCamera->getFOVy();
		// On la place au même endroit que le joueur
		Skyboxcam->setFOVy(Degree(90));
		// On cree la fenetre de rendu pour cette camera
		Skyboxcam->setWindow(512,512,512,512);
		// Alter the camera aspect ratio to match the viewport
		Skyboxcam->setAspectRatio(1);	
		// Recuperation du viewport
		Skyboxvp  = Skyboxcam->getViewport();
		mSkyboxStepNumber++;
		break;


	case 1:		/// Step 1: Creation d'une skybox (FRONT)
		Skyboxcam = mCamera;
		Skyboxvp  = Skyboxcam->getViewport();
		Skyboxwin = (RenderWindow*)Skyboxvp->getTarget();
        Skyboxwin->writeContentsToFile("../CubeSkyBox_FR.jpg");	// ../media/materials/textures/
		Skyboxcam->yaw(Degree(90));
		mSkyboxStepNumber++;
		break;

	case 2:     /// Step 2: Creation d'une skybox (LEFT)
		Skyboxcam = mCamera;
		Skyboxvp  = Skyboxcam->getViewport();
		Skyboxwin = (RenderWindow*)Skyboxvp->getTarget();
        Skyboxwin->writeContentsToFile("../CubeSkyBox_LF.jpg");
		Skyboxcam->yaw(Degree(90));
		mSkyboxStepNumber++;
		break;

	case 3:     /// Step 3: Creation d'une skybox (BACK)
		Skyboxcam = mCamera;
		Skyboxvp  = Skyboxcam->getViewport();
		Skyboxwin = (RenderWindow*)Skyboxvp->getTarget();
        Skyboxwin->writeContentsToFile("../CubeSkyBox_BK.jpg");
		Skyboxcam->yaw(Degree(90));
		mSkyboxStepNumber++;
		break;

	case 4:     /// Step 4: Creation d'une skybox (RIGHT)
		Skyboxcam = mCamera;
		Skyboxvp  = Skyboxcam->getViewport();
		Skyboxwin = (RenderWindow*)Skyboxvp->getTarget();
        Skyboxwin->writeContentsToFile("../CubeSkyBox_RT.jpg");
		Skyboxcam->yaw(Degree(90));	// On revient vers l'avant
		Skyboxcam->pitch(Degree(90)); // On regarde vers le haut
		mSkyboxStepNumber++;
		break;

	case 5:     /// Step 5: Creation d'une skybox (UP)
		Skyboxcam = mCamera;
		Skyboxvp  = Skyboxcam->getViewport();
		Skyboxwin = (RenderWindow*)Skyboxvp->getTarget();
        Skyboxwin->writeContentsToFile("../CubeSkyBox_UP.jpg");
		Skyboxcam->pitch(Degree(-180)); // On regarde vers le bas
		mSkyboxStepNumber++;
        break;

	case 6:     /// Step 6: Creation d'une skybox (DOWN)
		Skyboxcam = mCamera;
		Skyboxvp  = Skyboxcam->getViewport();
		Skyboxwin = (RenderWindow*)Skyboxvp->getTarget();
		Skyboxwin->writeContentsToFile("../CubeSkyBox_DN.jpg");
		mSkyboxStepNumber++;
		break;
		
	case 7:     /// Step 7: Terminate
		Skyboxcam = mCamera;
		// On retablit le contexte
		Skyboxcam->pitch(Degree(90));
		Skyboxcam->setFOVy(focale);
		mSkyboxStepNumber=0;
		mSkyboxInProgress=false;
		break;
		
	}

	return (mSkyboxInProgress);
}





/* -------------------------------------------------------------------------- */
/// This function convert a 3D world coordinates into a 2D screen coordinates.
/** 
@param worldPos  IN:  The object world coordinates
@param screenPos OUT: The object screen coordinates (X and Y only) 
@returns TRUE if the worldPos is on-screen, and FALSE if the worldPos is off-screen.
*/ 
/* -------------------------------------------------------------------------- */
bool mainFrameListener::_worldToScreen(Vector3& worldPos, Vector3& screenPos) 
{ 
   Vector3 hcsPosition = mCamera->getProjectionMatrix() * (mCamera->getViewMatrix() * worldPos); 

   // Teste si l'objet est dans le champ de la caméra.
   if ((hcsPosition.x < -1.0f) || 
       (hcsPosition.x > 1.0f)  || 
       (hcsPosition.y < -1.0f) || 
       (hcsPosition.y > 1.0f)) 
      return false; 

   int nCWidth  = (mWindow->getWidth()/2); 
   int nCHeight = (mWindow->getHeight()/2); 

   screenPos.x = nCWidth + (nCWidth * -hcsPosition.x); 
   screenPos.y = nCHeight + (nCHeight * hcsPosition.y); 

   return true; 
}
