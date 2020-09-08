/* -------------------------------------------------------------------------- */
// File       : menuApplication.cpp
// Project    : Long Forgotten Earth - Menu Principal
// Auteur     : David de Lorenzo
/* -------------------------------------------------------------------------- */

#include "menuApplication.h"

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIBase.h> 
#include <CEGUI/CEGUISystem.h> 
#include <CEGUI/CEGUILogger.h> 
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUIWindowManager.h> 
#include <CEGUI/CEGUIWindow.h> 
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIRenderer.h>
#include <OgreMemoryMacros.h>

// Autres includes
#include "fmod.h"
#include "fmod_errors.h"
#include "macros.h"

/* -------------------------------------------------------------------------- */
/// Construction
/* -------------------------------------------------------------------------- */
menuApplication::menuApplication()    
{
	mRoot          = NULL;
	mFrameListener = NULL;
	mLanguageCode  = "eng";
}

/* -------------------------------------------------------------------------- */
/// Destruction
/* -------------------------------------------------------------------------- */
menuApplication::~menuApplication()   
{
	LogManager::getSingleton().logMessage("Destructing menuApplication");
	if (mFrameListener)  delete mFrameListener;
	if (mRoot)           delete mRoot;
}

/* -------------------------------------------------------------------------- */
/// Main function
/* -------------------------------------------------------------------------- */
int menuApplication::go()
{

	if (!setup())  return (-1);

	mRoot->startRendering();

	int mChoix = mFrameListener->getMenuQuitParameter();
	String mComment = "Back to menuApplication.go() with choice #" + StringConverter::toString(mChoix);
	LogManager::getSingleton().logMessage(mComment);
	return mChoix;
}


/* -------------------------------------------------------------------------- */
// Sets up the application
// Returns false if the config file is not found
/* -------------------------------------------------------------------------- */
bool menuApplication::setup()
{
	mRoot = new Root();
	Log* MenuLogFile = LogManager::getSingleton().createLog("menu.log");
	LogManager::getSingleton().setDefaultLog(MenuLogFile);

	setupResources();
	// Si on n'arrive pas à charger la config, on quitte.
	if (!loadConfig()) return false;

	chooseSceneManager();
	createCamera();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Initialise and parse all scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	

	// Create the scene for the main menu
	createScene();
	playMusic("../media/music/ruskivoz.mp3");
//	playMusic("../media/music/scifly.mp3");
	createFrameListener();

	return true;
}


/* -------------------------------------------------------------------------- */
/// Load the config File
/* -------------------------------------------------------------------------- */
bool menuApplication::loadConfig()
{
	// ---------------------------------------
	// Video Setup
	// ---------------------------------------

	if (mRoot->restoreConfig())
	{
		// If the ogre.cfg config file exist: we use it.
		// Let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true);
	}
	else
	{
		// If the ogre.cfg file doesnt exist: the user should run CONFIG.EXE
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find ogre.cfg.\nPLEASE RUN CONFIG.EXE","loadConfig" );
		return false;
    }

	// ---------------------------------------
	// Loading Setup File
	// ---------------------------------------
	int    fmod_output = FSOUND_OUTPUT_NOSOUND;
	int    fmod_driver = 0;

	// Loading config file longFE.cfg
	// We use the functions offered by OGRE
    ConfigFile setupfile;
    setupfile.load("longFE.cfg");
	fmod_output  = StringConverter::parseInt(setupfile.getSetting("audio_output"));
	fmod_driver  = StringConverter::parseInt(setupfile.getSetting("audio_driver"));
	mLanguageCode= setupfile.getSetting("language");

	// ---------------------------------------
	// Audio Setup
	// ---------------------------------------
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
	fmod_version =  "Audio DLL FMOD version " + StringConverter::toString(FSOUND_GetVersion());
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
	fmod_channelS = "Audio : " + StringConverter::toString(num2d) + " hardware 2D channels";
	DEBUG_LOG(fmod_channelS);
	fmod_channelS = "Audio : " + StringConverter::toString(num3d) + " hardware 3D channels";
	DEBUG_LOG(fmod_channelS);

	// ---------------------------------------
	// Language Setup
	// ---------------------------------------
	DEBUG_LOG("Keyboard Language is "+ mLanguageCode);

	return true;
}


/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
void menuApplication::chooseSceneManager()
{
	mSceneMgr = mRoot->getSceneManager(ST_EXTERIOR_CLOSE);
//	mSceneMgr = mRoot->getSceneManager(ST_GENERIC);
}


/* ------------------------------------------------------------------------- */
// Create the camera
/* ------------------------------------------------------------------------- */
void menuApplication::createCamera()
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");
	// Position it at center
	mCamera->setPosition(Vector3(0,0,0));
	// Look FRONT along Z
	mCamera->lookAt(Vector3(0,0,-100));
	// Configuration de la camera
	mCamera->setNearClipDistance(1); 
	//mCamera->setFarClipDistance(5000);
	// On élargit un peu l'angle de vue pour bien voir les étoiles.
	mCamera->setFOVy(Degree(65));		
}

/* ------------------------------------------------------------------------- */
// Create one viewport
/* ------------------------------------------------------------------------- */
void menuApplication::createViewports()
{
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));	// new in 1.0
}


/* ------------------------------------------------------------------------- */
// Method which will define the source of resources (other than current folder)
/* ------------------------------------------------------------------------- */
void menuApplication::setupResources()
{
	// Load resource paths from config file
	ConfigFile cf;
	String sectionName, typeName, archiveName;

	cf.load("resources_Menu.cfg");

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

	while (Section_iter.hasMoreElements())
	{
		sectionName = Section_iter.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = Section_iter.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName    = i->first;
			archiveName = i->second;
			// On ajoute toutes les sections du fichier resources.cfg
			ResourceGroupManager::getSingleton().addResourceLocation(archiveName, typeName, sectionName);
		}
	} 
}

/* ------------------------------------------------------------------------- */
// Joue un fichier MP3 en fond musical
/* ------------------------------------------------------------------------- */
void menuApplication::playMusic(String filename)
{
	// On joue la musique
	FSOUND_STREAM* Musique;
	Musique=FSOUND_Stream_Open(filename.c_str(),FSOUND_LOOP_NORMAL,0,0);
	if (!Musique)
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Could not find "+filename,"playMusic");

	FSOUND_Stream_Play(0, Musique);
}


/* ------------------------------------------------------------------------- */
// Cree les nodes communs a toutes les scenes
/* ------------------------------------------------------------------------- */
void menuApplication::createScene()
{
	DEBUG_LOG(">> createScene()");

	/* On cree les elements principaux et indispensables à la scene */
	Entity*	   SphereEnt;
	SceneNode* CameraNode;
	SceneNode* SphereNode;
  	SceneNode* ParticleSourceNode;
  	Light*     lum;

	// On cree les nodes, qi seront ensuite utilisés par les memories
	CameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	CameraNode->attachObject(mCamera);

	SphereEnt  = mSceneMgr->createEntity("sphere1", "sun.mesh");;
	SphereNode = CameraNode->createChildSceneNode();
	SphereNode->attachObject(SphereEnt);

	ParticleSourceNode  = mSceneMgr->getRootSceneNode()->createChildSceneNode("ParticleSourceNode"); 

	lum= mSceneMgr->createLight("MainLight");

	initCEGUISystem();

	DEBUG_LOG("<< createScene()");
}

/* ------------------------------------------------------------------------- */
// Call the frame Listener
/* ------------------------------------------------------------------------- */
void menuApplication::createFrameListener()
{
	mFrameListener = new menuFrameListener(mWindow, mCamera, mGUIRenderer);
	mFrameListener->setKeyboardType(mLanguageCode);
	mRoot->addFrameListener(mFrameListener);
}


/* -------------------------------------------------------------------------- */
/// Initialize the CE-GUI System
/* -------------------------------------------------------------------------- */
void menuApplication::initCEGUISystem()
{
	using namespace CEGUI;

	//TRACE(">> initCEGUISystem()");

	// setup GUI system
//	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000);
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 0, Ogre::ST_EXTERIOR_CLOSE);
	mGUISystem   = new CEGUI::System(mGUIRenderer);

	try
	{
		Logger::getSingleton().setLoggingLevel(Informative);	// Standard // Informative

		// load scheme and set up defaults
		SchemeManager::getSingleton().loadScheme((utf8*)"Mainmenu.scheme");
		mGUISystem->setDefaultMouseCursor((utf8*)"TaharezLook", (utf8*)"MouseArrow");
		mGUISystem->setDefaultFont("SpaceGothic");
		
		// create root window
		Window* sheet = WindowManager::getSingleton().createWindow((utf8*)"DefaultWindow", "root_wnd");
		mGUISystem->setGUISheet(sheet);

		// Creation des imageset par defaut de Ogre
		Imageset*   giset = ImagesetManager::getSingleton().createImageset("../media/gui/ogregui.imageset");
	}

	// catch to prevent exit (errors will be logged).
	catch(CEGUI::Exception)
	{}

	//TRACE("<< initCEGUISystem");
}

