/* ------------------------------------------------------------------------- */
// File       : mainApplication.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "fmod.h"
#include "fmod_errors.h"

#include <OgreOpcode.h>

#include "mainApplication.h"
#include "macros.h"
#include "version.h"
#include "GalaxyBox.h"
#include "NPC_Manager.h"
#include "TheShip.h"
#include "Player.h"
#include "SoundManager.h"
#include "Clock.h"


/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
mainApplication::mainApplication()   
{
	mRoot          = NULL;
	mFrameListener = NULL;
	mLanguageCode  = "eng";
}

/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
mainApplication::~mainApplication()   
{
	DEBUG_LOG("Destructing mainApplication");
	if (mFrameListener)  delete mFrameListener;
	if (mRoot)           delete mRoot;

	// TODO: On libere tous les channels audio
//	FSOUND_Sample_Free();

}


/* ------------------------------------------------------------------------- */
/// Override the Singleton Class method
/* ------------------------------------------------------------------------- */
template<> mainApplication* Singleton<mainApplication>::ms_Singleton = 0;

mainApplication* mainApplication::getSingletonPtr(void)
{
	return ms_Singleton;
}



/* -------------------------------------------------------------------------- */
/// Main function: Lanch the Game and the Ogre Engine
/* -------------------------------------------------------------------------- */
int mainApplication::go()
{
	if (!setup())  return (-1);
	mRoot->startRendering();
	/* ***** HERE THE GAME *** */
	TRACE("<< mainApplication.go()");
	return 0;
}


/* -------------------------------------------------------------------------- */
/// Sets up the application
/* -------------------------------------------------------------------------- */
bool mainApplication::setup()
{
	mRoot = new Root();

	TRACE("\n*-*-* Long Forgotten Earth: Version "+ String(VERSION_NB) + " compiled on " + __DATE__ +"\n");

	this->setupResources();
	
	// Si on n'arrive pas à charger la config, on quitte.
	/// Returns false if the config file is not found
	if (!loadConfig()) return false;

	this->chooseSceneManager();
	this->createCamera();
	this->createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Initialise and parse all scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	// Create the scene for the main menu
	this->createCollisionEngine();
	this->createScene();
//	this->playMusic("../media/music/ambiance1.mp3");
	this->createFrameListener();

	return true;
}


/* -------------------------------------------------------------------------- */
/// Load the config File
/* -------------------------------------------------------------------------- */
bool mainApplication::loadConfig()
{
	/************************************************************************/
	/*  Video Setup                                                         */
	/************************************************************************/
	if (mRoot->restoreConfig())
	{
		// If the ogre.cfg config file exist: we use it.
		// Let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true);
	}
	else
	{
		// If the ogre.cfg file doesnt exist: the user should run CONFIG.EXE
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Could not find ogre.cfg.\nPLEASE RUN CONFIG.EXE","loadConfig" );
		return false;
    }

	/************************************************************************/
	/* Loading Setup File                                                   */
	/************************************************************************/
	int    fmod_output = FSOUND_OUTPUT_NOSOUND;
	int    fmod_driver = 0;

	// Loading config file longFE.cfg
	// We use the functions offered by OGRE
    ConfigFile setupfile;
    setupfile.load("longFE.cfg");
	fmod_output   = StringConverter::parseInt(setupfile.getSetting("audio_output"));
	fmod_driver   = StringConverter::parseInt(setupfile.getSetting("audio_driver"));
	mShadowsType  = StringConverter::parseInt(setupfile.getSetting("shadows"));
	mLanguageCode = setupfile.getSetting("language");

	/************************************************************************/
	/* Audio Setup                                                          */
	/************************************************************************/
	String fmod_version;
	String fmod_outputS;
	String fmod_driverS;
	String fmod_mixerS;
	String fmod_channelS;
	unsigned int caps = 0;
	int num2d, num3d;

	DEBUG_LOG("***********************");
	DEBUG_LOG("MENU: audio setup      ");
	DEBUG_LOG("***********************");

	// Check the FMOD audio library version
	fmod_version =  "Audio DLL FMOD version " + ITOA(FSOUND_GetVersion());
	DEBUG_LOG(fmod_version);

	// Set and check the audio output mode
	FSOUND_SetOutput(fmod_output);
	switch (FSOUND_GetOutput())
	{
		case FSOUND_OUTPUT_NOSOUND: 
			fmod_outputS = "Audio Output = NoSound"; 
			break;
		case FSOUND_OUTPUT_WINMM:   
			fmod_outputS = "Audio Output = Windows Multimedia Waveout"; 
			break;
		case FSOUND_OUTPUT_DSOUND:  
			fmod_outputS = "Audio Output = Direct Sound"; 
			break;
		case FSOUND_OUTPUT_ASIO:    
			fmod_outputS = "Audio Output = ASIO"; 
			break;
	};
	DEBUG_LOG(fmod_outputS);

	// Set and check the audio driver
	FSOUND_SetDriver(fmod_driver);
	fmod_driverS = FSOUND_GetDriverName(FSOUND_GetDriver());
	fmod_driverS = "Audio Driver = " + fmod_driverS;
	DEBUG_LOG(fmod_driverS);

	// Check the driver capabilities
	FSOUND_GetDriverCaps(FSOUND_GetDriver(), &caps);
	if (!caps)                   DEBUG_LOG("This driver supports software mode only.");
	if (caps & FSOUND_CAPS_HARDWARE) DEBUG_LOG("This driver supports hardware 3D sound.");
	if (caps & FSOUND_CAPS_EAX2) DEBUG_LOG("This driver supports EAX 2 reverb.");
	if (caps & FSOUND_CAPS_EAX3) DEBUG_LOG("This driver supports EAX 3 reverb.");

	// Activate the sound card
	FSOUND_SetMixer(FSOUND_MIXER_AUTODETECT);

	if (!FSOUND_Init(44100, 32, 0))
		DEBUG_LOG("FSOUND_init(): "+String(FMOD_ErrorString(FSOUND_GetError())));


	// 	Check the mixer type
	switch (FSOUND_GetMixer())
	{
		case FSOUND_MIXER_BLENDMODE:     
			fmod_mixerS = "Audio Mixer Mode = FSOUND_MIXER_BLENDMODE"; 
			break;
		case FSOUND_MIXER_MMXP5:         
			fmod_mixerS = "Audio Mixer Mode = FSOUND_MIXER_MMXP5"; 
			break;
		case FSOUND_MIXER_MMXP6:         
			fmod_mixerS = "Audio Mixer Mode = FSOUND_MIXER_MMXP6"; 
			break;
		case FSOUND_MIXER_QUALITY_FPU:   
			fmod_mixerS = "Audio Mixer Mode = FSOUND_MIXER_QUALITY_FPU"; 
			break;
		case FSOUND_MIXER_QUALITY_MMXP5: 
			fmod_mixerS = "Audio Mixer Mode = FSOUND_MIXER_QUALITY_MMXP5"; 
			break;
		case FSOUND_MIXER_QUALITY_MMXP6: 
			fmod_mixerS = "Audio Mixer Mode = FSOUND_MIXER_QUALITY_MMXP6"; 
			break;
	};
	DEBUG_LOG(fmod_mixerS);

	// Check the audio hardware channels
	FSOUND_GetNumHWChannels(&num2d, &num3d, NULL);
	fmod_channelS = "Audio : " + ITOA(num2d) + " hardware 2D channels";
	DEBUG_LOG(fmod_channelS);
	fmod_channelS = "Audio : " + ITOA(num3d) + " hardware 3D channels";
	DEBUG_LOG(fmod_channelS);

	/************************************************************************/
	/* Language Setup                                                       */
	/************************************************************************/
	TRACE("Keyboard Language is "+mLanguageCode);

	/************************************************************************/
	/* Shadows Type Setup                                                   */
	/************************************************************************/
	TRACE("Shadows Type is "+StringConverter::toString(mShadowsType));
	TRACE("0=none - 1=texture shadows - 2=stencil shadows ");

	return true;
}


/* ------------------------------------------------------------------------- */
/// Creation du SceneManeger
/* ------------------------------------------------------------------------- */
void mainApplication::chooseSceneManager()
{
	mSceneMgr = mRoot->getSceneManager(ST_GENERIC);
//	mSceneMgr = mRoot->getSceneManager(ST_EXTERIOR_CLOSE); 
}


/* ------------------------------------------------------------------------- */
// Create the camera
/* ------------------------------------------------------------------------- */
void mainApplication::createCamera()
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setQueryFlags(0);	// Pour ne pas etre "vu" par les rayons
	mCamera->setPosition(Vector3(0,0,0));
	// Look FRONT along Z
	mCamera->lookAt(Vector3(0,0,-100));
	// Configuration de la camera
	mCamera->setNearClipDistance(5); // 1
	mCamera->setFarClipDistance(0);  // 5000
	// On élargit un peu l'angle de vue pour bien voir les étoiles.
	mCamera->setFOVy(Degree(50));   // (58);		
}

/* ------------------------------------------------------------------------- */
/// Create one viewport
/* ------------------------------------------------------------------------- */
void mainApplication::createViewports()
{
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));	// new in 1.0
}

/* ------------------------------------------------------------------------- */
/// Initialize the Collision Engine (OgreOpcode)
/* ------------------------------------------------------------------------- */
void mainApplication::createCollisionEngine()
{
using namespace OgreOpcode;

	// The collision manager must be created prior to using the collision detection system.
	new CollisionManager(mSceneMgr);

	// A collision class doesn't need to correspond to an EntityType (or game object type). 
	// The class is really like a category type for determining collision relationships. 
	CollisionManager::getSingletonPtr()->BeginCollClasses();
	CollisionManager::getSingletonPtr()->AddCollClass("room");
	CollisionManager::getSingletonPtr()->AddCollClass("door");
	CollisionManager::getSingletonPtr()->AddCollClass("object");
	CollisionManager::getSingletonPtr()->AddCollClass("npc");
	CollisionManager::getSingletonPtr()->EndCollClasses();
   
	// Collision types define what happens when two classes collide. 
	// They tell the collision system whether or not it should report a collision and the amount 
	// of work that it should do in determining whether or not a collision occurred.
	// Collision types are one of:
	// - COLLTYPE_IGNORE: Don't report collisions.
	// - COLLTYPE_QUICK: Do fast sphere vs. sphere checks.
	// - COLLTYPE_CONTACT: Report the first contact.
	// - COLLTYPE_EXACT: Report all contacts.
	// Example : 
	// - vehicles could bounce off each other and requires exact hit, 
	// - powerups can be picked up by being around the right place, 
	// - Bullets require exact. 
	// - Powerups ignore other powerups and bullets. 
	// - Bullets ignore other bullets.
	CollisionManager::getSingletonPtr()->BeginCollTypes();

	CollisionManager::getSingletonPtr()->AddCollType("room",  "npc",   COLLTYPE_EXACT);
	CollisionManager::getSingletonPtr()->AddCollType("door",  "npc",   COLLTYPE_EXACT);
	CollisionManager::getSingletonPtr()->AddCollType("object","npc",   COLLTYPE_QUICK);
	CollisionManager::getSingletonPtr()->AddCollType("npc",   "npc",   COLLTYPE_EXACT);

	CollisionManager::getSingletonPtr()->AddCollType("room", "room",   COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->AddCollType("room", "door",   COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->AddCollType("room", "object", COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->AddCollType("door", "door",   COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->AddCollType("door", "object", COLLTYPE_IGNORE);

	CollisionManager::getSingletonPtr()->AddCollType("object","object",COLLTYPE_IGNORE);

	CollisionManager::getSingletonPtr()->EndCollTypes();
	
}

/* ------------------------------------------------------------------------- */
/// Method which will define the source of resources (other than current folder)
/* ------------------------------------------------------------------------- */
void mainApplication::setupResources()
{
	// Load resource paths from config file
	ConfigFile cf;
	cf.load("resources.cfg");

	// Exemple de contenu
	// [General]
	// FileSystem=media
	// Zip=media/packs/cubemap.zip

	//   General....... sectionName
	//   FileSystem.... typeName
	//   media......... archiveName
	//   Zip........... typeName
	//   cubemap.zip... archiveName

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator Section_iter = cf.getSectionIterator();

	String sectionName, typeName, archiveName;
	while (Section_iter.hasMoreElements())
	{
		sectionName = Section_iter.peekNextKey();
		if (sectionName=="Menu") continue;        // On ne charge pas les resources necéssaires au menu-LFE.
		ConfigFile::SettingsMultiMap *settings = Section_iter.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName    = i->first;
			archiveName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archiveName, typeName, sectionName);
		}
	} 
}


/* ------------------------------------------------------------------------- */
/// Joue un fichier MP3 en fond musical
/* ------------------------------------------------------------------------- */
void mainApplication::playMusic(String MusicFile)
{
	// On joue la musique
	FSOUND_STREAM* Musique;
	
	Musique=FSOUND_Stream_Open(MusicFile.c_str(),FSOUND_LOOP_NORMAL,0,0);
	if (!Musique)
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Could not find "+MusicFile ,"playMusic");

	FSOUND_Stream_Play(0, Musique);
}


/* ------------------------------------------------------------------------- */
/// Création de la scene principale (racine)
/* ------------------------------------------------------------------------- */
void mainApplication::createScene()
{

	DEBUG_LOG("Creating Scene");

	// On initialise le systeme audio.
	new SoundManager();

	// Activation du type d'ombres demandé dans config.exe
	switch(mShadowsType) 
	{
	case 1:
	//	Activation d'ombres de type TEXTURE
		mSceneMgr->setShadowTechnique (SHADOWTYPE_TEXTURE_MODULATIVE);	
		mSceneMgr->setShadowColour(ColourValue(0.4, 0.4, 0.4));
		TRACE ("Shadows technique = TEXTURE MODULATIVE shadows.");
		break;
	case 2:
		//	Activation d'ombres de type STENCIL
		mSceneMgr->setShadowTechnique (SHADOWTYPE_STENCIL_MODULATIVE);	
//		mSceneMgr->setShadowTechnique (SHADOWTYPE_STENCIL_ADDITIVE);	 // joli, mais demande trop de cpu
		mSceneMgr->setShadowFarDistance(1000);
		//Les ombres de Room sont desactivées par mRoomEntity->setCastShadows(false);
		TRACE ("Shadows technique = STENCIL shadows.");
		break;
	default:
		// Autres cas: pas d'ombres
		TRACE ("Shadows technique = No shadows.");
	}

	// On cree les objets stellaires
	TRACE ("Creating Galaxy.");
	GalaxyBox* Quadran= new GalaxyBox(mSceneMgr);
	// Quadran->addToScene();

	// On démarre l'horloge du jeu
	TRACE ("Creating Time.");
	new Clock(mWindow);

	// On cree le vaisseau spatial
	TRACE ("Creating SpaceShip.");
	TheShip* mShip = new TheShip(mSceneMgr);
	mShip->addToScene(Vector3(0, 0, 0));
	mShip->getNode()->setPosition(0,0,0);

	// On cree les NPC
	TRACE ("Creating NPC's.");
	NPC_Manager* NPCmanager = new NPC_Manager(mSceneMgr);

	// NPC qui ne changeront pas
	NPCmanager->addBody("Cpt H.G. Assimiev",      "../media/objects/deadsoldier.xml");
	NPCmanager->addBody("Lt Condoleance Noodle",  "../media/objects/lowsoldier.xml");
	NPCmanager->addBody("Sldt Alfredo Martens",   "../media/objects/lowsoldier.xml");
	NPCmanager->addBody("Sldt Mark Staedler",     "../media/objects/lowsoldier.xml");
	NPCmanager->addBody("Sldt Ahmed Fazal",       "../media/objects/deadsoldier.xml");
	NPCmanager->addBody("Sldt Yves Nguyen",       "../media/objects/deadsoldier.xml");

	// NPC dynamiques
	NPCmanager->addBody("Lt Norman Cano",         "../savedgames/lieutenant.xml");
	
	// On cree le droid de MAINTENANCE
	Robot* newRobot = NPCmanager->addRobot("MTdroid", "robot_MT.cfg");
	newRobot->attachToNode(mShip->getNode());
//	newRobot->addToScene();	

	// On cree le droid MAIA
	Robot* MAIArobot = NPCmanager->addRobot("ADdroid", "robot_AD.cfg");
	MAIArobot->attachToNode(mShip->getNode());
//	MAIArobot->addToScene();

	/// On cree un robot de TEST
	Robot* TESTrobot = NPCmanager->addRobot("TESTrobot", "robot_TEST.cfg");
	TESTrobot->attachToNode(mShip->getNode());
//	TESTrobot->addToScene();

	// On ajoute le joueur dans le vaisseau
	TRACE ("Creating Player.");
	Player* player = new Player(mCamera, mShip->getNode());
	NPCmanager->addBody(player->getName(), "../savedgames/playerbody.xml",true);

	// A collision context is a space within which collisions occur. 
	// It is used to minimize the set of objects that any given object can collide with 
	// (since it can only collide with those in the same context). 
//	OgreOpcode::CollisionManager::getSingletonPtr()->GetDefaultContext()->Reset();

	DEBUG_LOG("<< createScene");

}

/* ------------------------------------------------------------------------- */
/// Cree le Frame Listener principal
/* ------------------------------------------------------------------------- */
void mainApplication::createFrameListener()
{
	TRACE(">> createFrameListener()");
	// On cree le Listener 
	// On passe en paramètre les objets que l'on veut controler
	mFrameListener = new mainFrameListener(mWindow, mCamera);
	// On gère les claviers particuliers
	mFrameListener->setKeyboardType(mLanguageCode);
	// On le ratache au Root de la scene
	mRoot->addFrameListener(mFrameListener);
	TRACE("<< createFrameListener");
}



/* ------------------------------------------------------------------------- */
/// Renvoie le SceneManager pour toutes les classes qui ont besoin de créer des entities
/* ------------------------------------------------------------------------- */
SceneManager*  mainApplication::getSceneMgr()
{
	return mSceneMgr;
}



