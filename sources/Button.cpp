/* ------------------------------------------------------------------------- */
// File       : Button.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "macros.h"
#include "classes.h"
#include "Button.h"
#include "Player.h"
#include "Clock.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
Button::Button(String Name, String MeshFile, String DescFile, SceneNode* ParentNode, SceneManager* SceneMgr)
{
	// Stockage des paramètres
	// ---------------------------------------------
	mSceneMgr   = SceneMgr;
	mRoot       = Root::getSingletonPtr();
	mParentNode = ParentNode;
	mName       = Name;

	// Initialisation des variables membres
	// ---------------------------------------------
	mBroken            = false;
	mHasFocus          = false;
	mSubEntity_screen  = NULL;
	
	// Lecture des parametres du fichier de config
	// et Chargement de la configuration initiale de l'objet
	// ---------------------------------------------
	this->loadObject (DescFile);

	// Creation de l'entité
	// ---------------------------------------------
	mEntity = SceneMgr->createEntity(mName, MeshFile);
	mNode   = ParentNode->createChildSceneNode(mName);
	mEntity->setUserObject(this);
	mEntity->setCastShadows(false);
	mEntity->setQueryFlags(ACTIVABLE);

	// ---------------------------------------------
	// On recupère la sub-entity: "elev_button/button" 
	// ou: On recupère la sub-entity: "2/button" 
	// pour pouvoir changer sa texture en cours de programme.
	// ---------------------------------------------
	int NumSubEntities = mEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mEntity->getSubEntity(i)->getMaterialName();
		if (MatName=="elev_button/button")   mSubEntity_screen  = mEntity->getSubEntity(i);
		if (MatName=="2/button")             mSubEntity_screen  = mEntity->getSubEntity(i);
	}
	this->setMaterial(mIdleMaterial);
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Button::~Button()
{
}

/* ------------------------------------------------------------------------- */
/// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long Button::getTypeID (void) const
{
	return BUTTON_ID;
}


/* ------------------------------------------------------------------------- */
/// Charge les parametres initiaux du Button
/* ------------------------------------------------------------------------- */
void Button::loadObject (String DescFile)
{
    String     filename = "../media/objects/"+DescFile;
	TRACE(">> loadObject("+filename+")");

	try
	{
		/// Lecture des parametres du fichier de config
	    ConfigFile DescriptionFile;
	    DescriptionFile.load(filename);
		mIdleMaterial       = DescriptionFile.getSetting("IdleMaterial");
		mActiveMaterial     = DescriptionFile.getSetting("ActiveMaterial");
//		mMeshFile           = DescriptionFile.getSetting("MeshFile");
		mAction             = DescriptionFile.getSetting("Action");
		mBroken             =(DescriptionFile.getSetting("Broken")=="true");
	}
	catch (Ogre::Exception& e) 
	{
		// On affiche le number juste pour ne pas avoir de warning à la compilation sur la variable e
		DEBUG_LOG("WARNING: saved object not found - code "+ITOA(e.getNumber()));
	}
}

/* ------------------------------------------------------------------------- */
/// Met à jour la facade du Button
/* ------------------------------------------------------------------------- */
void Button::setMaterial(String MaterialName)
{
	if (mBroken)
	{
	}
	else
	{
		if (mSubEntity_screen) mSubEntity_screen->setMaterialName(MaterialName);
	}
}


/* ------------------------------------------------------------------------- */
/// On active le focus au moment ou la souris passe sur l'objet
/* ------------------------------------------------------------------------- */
void Button::setFocusOn()
{
	mHasFocus = true;
	this->setMaterial(mActiveMaterial);
	/// Et on affiche un message
	Clock::getSingletonPtr()->showMessageOverlay("Push Button");
}


/* ------------------------------------------------------------------------- */
/// Quand on perd le focus, on change la façade
/* ------------------------------------------------------------------------- */
void Button::setFocusOff()
{
	mHasFocus = false;
	this->setMaterial(mIdleMaterial);
}


/* ------------------------------------------------------------------------- */
/// Positionne le Button
/* ------------------------------------------------------------------------- */
void Button::setPosition(Vector3 Position)
{
	mNode->setPosition(Position);
}


/* ------------------------------------------------------------------------- */
/// Positionne le Button
/* ------------------------------------------------------------------------- */
void Button::setOrientation(int Orientation)
{
	mNode->yaw(Degree(Orientation));
}


/* ------------------------------------------------------------------------- */
/// Positionne le Button
/* ------------------------------------------------------------------------- */
void Button::setOrientation(Quaternion Orientation)
{
	mNode->setOrientation(Orientation);
}


/* ------------------------------------------------------------------------- */
/// Ajoute le Button à la scene courante
/* ------------------------------------------------------------------------- */
void Button::addToScene()
{
	mNode->attachObject(mEntity);
}


/* ------------------------------------------------------------------------- */
/// Retire le Button de la scene courante
/* ------------------------------------------------------------------------- */
void Button::removeFromScene()
{
	mNode->detachObject(mEntity);
}


/* ------------------------------------------------------------------------- */
/// Action du bouton: Declenche l'action qui a été demandée pour ce bouton
/* ------------------------------------------------------------------------- */
void Button::actionButton()
{
	if (mAction=="playerUP")
		this->actionPlayerUp();
	else if (mAction=="playerDOWN")
		this->actionPlayerDown();

}

/* ------------------------------------------------------------------------- */
/// Action du bouton: fait monter le joueur d'un niveau dans le vaisseau
/* ------------------------------------------------------------------------- */
void Button::actionPlayerUp()
{
	int CurrLevel = Player::getSingletonPtr()->getCurrentLevel();
	if (CurrLevel<5) Player::getSingletonPtr()->goToLevel(CurrLevel+1);
}

/* ------------------------------------------------------------------------- */
/// Action du bouton: fait descendre le joueur d'un niveau dans le vaisseau
/* ------------------------------------------------------------------------- */
void Button::actionPlayerDown()
{
	int CurrLevel = Player::getSingletonPtr()->getCurrentLevel();
	if (CurrLevel>0) Player::getSingletonPtr()->goToLevel(CurrLevel-1);
}


