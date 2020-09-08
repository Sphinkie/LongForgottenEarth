/* ------------------------------------------------------------------------- */
// File       : Computer.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "fmod.h"

#include "Computer.h"
#include "macros.h"
#include "SoundManager.h"
#include "TheShip.h"
#include "Clock.h"
#include "XMLparser_T.h"


/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
Computer::Computer(String MeshFile, String DescFile, SceneNode* ParentNode, SceneManager* SceneMgr): Interface()
{
	// Le constructeur de la surclasse INTERFACE s'exécute automatiquement

	// Stockage des paramètres
	// ---------------------------------------------
	mSceneMgr  = SceneMgr;
	mRoot      = Root::getSingletonPtr();
	mParentNode= ParentNode;
	mMeshFile  = MeshFile;

	// Initialisation des variables membres
	// ---------------------------------------------
	mOperatingSoundChannel= NULL_CHANNEL;
	mHelloSoundChannel    = NULL_CHANNEL;
	mExitGuiDelay         = 5;
	mExitGui              = false;
	mBroken               = false;
	mHasFocus             = false;
	mSubEntity_screen     = NULL;
	mSubEntity_visu       = NULL;
	mSubEntity_keyboard   = NULL;
	
	// ---------------------------------------------
	// Lecture des parametres du fichier de config
	// et Chargement de la configuration initiale de l'objet
	// ---------------------------------------------
	this->loadDescription(DescFile);
	this->loadObject();

	// ---------------------------------------------
	// Creation de l'entité
	// ---------------------------------------------
	mEntity = SceneMgr->createEntity(mName, mMeshFile);
	mNode   = ParentNode->createChildSceneNode(mName);
	mEntity->setUserObject(this);
	mEntity->setCastShadows(true);
	mEntity->setQueryFlags(ACTIVABLE);

	// ---------------------------------------------
	// Creation de l'objet Collision
	// ---------------------------------------------
	using namespace OgreOpcode;
	CollisionContext* CollideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();
	CollisionShape*   CollideShape   = CollisionManager::getSingletonPtr()->NewShape(mName.c_str());
	CollideShape->Load(mEntity);
	CollisionObject* CollideObject = CollideContext->NewObject();
	CollideObject->SetCollClass("object");
	CollideObject->SetShape(CollideShape);
//	CollideObject->setDebug(true);              // fait planter ????
	CollideContext->AddObject(CollideObject);

	// ---------------------------------------------
	// Creation d'un node supplémentaire pour les particules
	// ---------------------------------------------
	mSmoke          = NULL;
	mParticulesNode = mNode->createChildSceneNode();
	mParticulesNode ->setPosition(0,24,0);


	// ---------------------------------------------
	// Chargement de l'interface GUI
	// ---------------------------------------------
	Interface::loadInterfaceFile(mInterfaceFile);

	// ---------------------------------------------
	// On recupère la sub-entity: "screen" 
	// pour pouvoir changer sa texture en cours de programme.
	// ---------------------------------------------
	int NumSubEntities = mEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mEntity->getSubEntity(i)->getMaterialName();
		if      (StringUtil::startsWith(MatName,"computer/screen"))   mSubEntity_screen  = mEntity->getSubEntity(i);
		else if (StringUtil::startsWith(MatName,"computer/visu"))     mSubEntity_visu    = mEntity->getSubEntity(i);
		else if (StringUtil::startsWith(MatName,"computer/keyboard")) mSubEntity_keyboard= mEntity->getSubEntity(i);
	}
	// Initialise la texture du computer screen (idle ou broken)
	this->setComputerMaterial();	
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Computer::~Computer()
{
	// Arreter la gestion
	mRoot->removeFrameListener(this);

	// Detacher les particules et l'Entity
	mNode->detachAllObjects();
	
	// TODO : faut-t-il destroyer l'Entity???

}


/* ------------------------------------------------------------------------- */
/// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long Computer::getTypeID (void) const
{
	return COMPUTER_ID;
}


/* ------------------------------------------------------------------------- */
/// Charge les parametres initiaux du Computer
/* ------------------------------------------------------------------------- */
void Computer::loadDescription (String DescFile)
{
    String filename = "../media/objects/"+DescFile;
	TRACE(">> loadDescription("+filename+")");

	try
	{
		/// Lecture des parametres du fichier de config
	    ConfigFile DescriptionFile;
	    DescriptionFile.load(filename);
		mName               = DescriptionFile.getSetting("Name");
		mHelloSoundFile     = DescriptionFile.getSetting("HelloSoundFile");
		mInterfaceFile      = DescriptionFile.getSetting("InterfaceFile");
		mOperatingSoundFile = DescriptionFile.getSetting("OperatingSoundFile");
		mStatusFile         = DescriptionFile.getSetting("StatusFile");

	}
	catch (...) 
	{
		TRACE("WARNING: Computer config file not found");
	}
}


/* ------------------------------------------------------------------------- */
/// Charge l'etat initial du Computer
/* ------------------------------------------------------------------------- */
void Computer::loadObject()
{
    String     filename = "../savedgames/"+mStatusFile;
	TRACE(">> loadGame: "+filename);

	/// Lecture des parametres du fichier de status
	XMLParser_T* StatusFile = new XMLParser_T(filename);

	StatusFile->gotoElement("computer","status");
	this->setBroken(StatusFile->getBooleanAttribute("broken"));

	delete (StatusFile);
}


/* ------------------------------------------------------------------------- */
/// Positionne le Computer
/* ------------------------------------------------------------------------- */
void Computer::setPosition(Vector3 Position)
{
	mNode->setPosition(Position);
}


/* ------------------------------------------------------------------------- */
/// Positionne le Computer
/* ------------------------------------------------------------------------- */
void Computer::setOrientation(int Orientation)
{
	mNode->yaw(Degree(Orientation));
}


/* ------------------------------------------------------------------------- */
/// Positionne le Computer
/* ------------------------------------------------------------------------- */
void Computer::setOrientation(Quaternion Orientation)
{
	mNode->setOrientation(Orientation);
}


/* ------------------------------------------------------------------------- */
/// Ajoute le Computer à la scene courante
/* ------------------------------------------------------------------------- */
void Computer::addToScene()
{
	mNode->attachObject(mEntity);
}


/* ------------------------------------------------------------------------- */
/// Retire le Computer de la scene courante
/* ------------------------------------------------------------------------- */
void Computer::removeFromScene()
{
	mRoot->removeFrameListener(this);
	mNode->detachObject(mEntity);
	if (mSmoke)  mNode->detachObject(mSmoke);
}


/* ------------------------------------------------------------------------- */
/// Gere les pannes du Computer
/* ------------------------------------------------------------------------- */
void Computer::setBroken(bool broken)
{
	mBroken = broken;
}


/* ------------------------------------------------------------------------- */
/// On active le focus au moment ou la souris passe sur l'objet
/* ------------------------------------------------------------------------- */
void Computer::setFocusOn()
{
	if (!mHasFocus)
	{
		/// S'il n'a pas deja le focus: on émet un son.
		mHelloSoundChannel = SoundManager::getSingletonPtr()->playSound(mHelloSoundFile,5,100);
		/// Et on affiche un message
		Clock::getSingletonPtr()->showMessageOverlay("Activate Computer");
	}
	mHasFocus = true;
	mExitGui  = false;
}


/* ------------------------------------------------------------------------- */
/// Quand on perd le focus, on detruit le GUI
/* ------------------------------------------------------------------------- */
void Computer::setFocusOff()
{
	TRACE (">> Computer::setFocusOff()");
	if (mHasFocus)
	{
		// On annule les ordres
		mSelectedOrder = -1;
		/// On efface l'interface
		this->deleteInterface();
	}
	mHasFocus = false;
	mExitGui  = true;
	TRACE ("<< Computer::setFocusOff()");
}



/* ------------------------------------------------------------------------- */
/// Mémorise une demande de fin de la GUI
/* ------------------------------------------------------------------------- */
void Computer::requestExitInterface(Real seconds)
{
	mExitGui = true;
	mExitGuiDelay=seconds;
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si une demande de sortie est en cours.
/* ------------------------------------------------------------------------- */
bool Computer::isExitInterfaceRequiered()
{
	/// Renvoie FALSE si aucune demande de sortie n'est en cours.
	return mExitGui;
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le GUI est inactif  (cad non affiché)
/* ------------------------------------------------------------------------- */
bool Computer::isInterfaceDisabled()
{
	return (!mExistGUI);
}


/* ------------------------------------------------------------------------- */
/// Renvois TRUE si le Computer est endommagé.
/* ------------------------------------------------------------------------- */
bool Computer::isBroken()
{
	return mBroken;
}



/* ------------------------------------------------------------------------- */
/// Gère les elements dynamiques du Computer
/* ------------------------------------------------------------------------- */
bool Computer::frameStarted (const FrameEvent& evt)
{

	if (mExitGui)           {mExitGuiDelay-=evt.timeSinceLastFrame;}
	if (mExitGuiDelay<0)	{this->setFocusOff(); mExitGui=false; mExitGuiDelay=5;}

	return true;
}


/* ------------------------------------------------------------------------- */
/// Gère le lancement du FrameListener
/* ------------------------------------------------------------------------- */
void Computer::addFrameListener(short User)
{
	TRACE(">> Computer::addFrameListener()");
}


/* ------------------------------------------------------------------------- */
/// Met à jour l'écran du Computer
/* ------------------------------------------------------------------------- */
void Computer::setComputerMaterial()
{
	TRACE(">> Computer::setComputerMaterial()");


	if (mBroken)
	{
		if (mSubEntity_screen) mSubEntity_screen->setMaterialName("computer/dyn/broken");
		if (mSubEntity_visu)   mSubEntity_visu->setMaterialName("computer/dyn/broken");
	}
	else
	{
		if (mSubEntity_screen) mSubEntity_screen->setMaterialName("computer/dyn/idle");
		if (mSubEntity_visu)   mSubEntity_visu->setMaterialName("computer/dyn/idle");
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie le nom du Computer
/* ------------------------------------------------------------------------- */
String Computer::getName()
{
	return mName;
}