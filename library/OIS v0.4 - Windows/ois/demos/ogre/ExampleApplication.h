#ifndef __ExampleApplication_H__
#define __ExampleApplication_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "ExampleFrameListener.h"

using namespace Ogre;

class ExampleApplication
{
public:
	/// Standard constructor
	ExampleApplication()
	{
		mFrameListener = 0;
		mRoot = 0;
	}
	/// Standard destructor
	virtual ~ExampleApplication()
	{
		delete mFrameListener;
		delete mRoot;
	}

	/// Start the example
	virtual void go(void)
	{
		if (!setup())
			return;

		mRoot->startRendering();

		// clean up
		destroyScene();
	}

protected:
	Root *mRoot;
	Camera* mCamera;
	SceneManager* mSceneMgr;
	ExampleFrameListener* mFrameListener;
	RenderWindow* mWindow;

	// These internal methods package up the stages in the startup process
	/** Sets up the application - returns false if the user chooses to abandon configuration. */
	virtual bool setup(void)
	{
		mRoot = new Root();

		setupResources();

		if (!configure())
			return false;

		chooseSceneManager();
		createCamera();
		createViewports();

		// Set default mipmap level (NB some APIs ignore this)
		TextureManager::getSingleton().setDefaultNumMipmaps(5);

		// Create any resource listeners (for loading screens)
		createResourceListener();
		// Load resources
		loadResources();
		// Create the scene
		createScene();
		createFrameListener();
		return true;
	}
	
	/** Configures the application - returns false if the user chooses to abandon configuration. */
	virtual bool configure(void)
	{
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->showConfigDialog())
		{
			// If returned true, user clicked OK so initialise
			// Here we choose to let the system create a default rendering window by passing 'true'
			mWindow = mRoot->initialise(true);
			return true;
		}

		return false;
	}

	virtual void chooseSceneManager(void)
	{
		// Get the SceneManager, in this case a generic one
		mSceneMgr = mRoot->getSceneManager(ST_GENERIC);
	}
	virtual void createCamera(void)
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		// Position it at 500 in Z direction
		mCamera->setPosition(Vector3(0,0,500));
		// Look back along -Z
		mCamera->lookAt(Vector3(0,0,-300));
		mCamera->setNearClipDistance(5);
	}
	virtual void createFrameListener(void)
	{
		mFrameListener= new ExampleFrameListener(mWindow, mCamera);
		mFrameListener->showDebugOverlay(true);
		mRoot->addFrameListener(mFrameListener);
	}

	// pure virtual - this has to be overridden
	virtual void createScene(void) = 0;
	
	// Optional to override this
	virtual void destroyScene(void){}

	virtual void createViewports(void)
	{
		// Create one viewport, entire window
		Viewport* vp = mWindow->addViewport(mCamera);
		vp->setBackgroundColour(ColourValue(0,0,0));

		// Alter the camera aspect ratio to match the viewport
		mCamera->setAspectRatio(
		Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
	}

	/// Method which will define the source of resources (other than current folder)
	virtual void setupResources(void)
	{
		// Load resource paths from config file
		ConfigFile cf;
		cf.load("resources.cfg");

		// Go through all sections & settings in the file
		ConfigFile::SectionIterator seci = cf.getSectionIterator();

		String secName, typeName, archName;
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			ConfigFile::SettingsMultiMap *settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
				ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
			}
		}
	}

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	virtual void createResourceListener(void) {}

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	virtual void loadResources(void)
	{
		// Initialise, parse scripts etc
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
};

#endif
