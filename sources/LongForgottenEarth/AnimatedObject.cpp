/* ------------------------------------------------------------------------- */
// File       : AnimatedObject.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "AnimatedObject.h"
#include "macros.h"
#include "classes.h"
#include "Clock.h"
#include "SoundManager.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
AnimatedObject::AnimatedObject(String Name, String MeshFile, SceneManager* SceneMgr, SceneNode* ParentNode)
{
	mName   = Name;
	mEntity = SceneMgr->createEntity(Name, MeshFile);
	mNode   = ParentNode->createChildSceneNode(Name);
	mEntity->setUserObject(this);

	// Les Objets peuvent projeter des ombres, si on leur demande.
	mEntity->setCastShadows(false);
	mEntity->setQueryFlags(ACTIVABLE);

	// Initialisation des membres
	mSceneMgr      = SceneMgr;
	mRoot          = Root::getSingletonPtr();
	mParentNode    = ParentNode;
	mStatus        = CLOSED;
	mHasFocus      = false;
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
AnimatedObject::~AnimatedObject()
{
}

/* ------------------------------------------------------------------------- */
/// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long AnimatedObject::getTypeID (void) const
{
	return ANIMOBJECT_ID;
}


/* ------------------------------------------------------------------------- */
/// Ajoute l'objet à la scene courante
/**
	@param Pos La position de l'objet.
*/
/* ------------------------------------------------------------------------- */
void AnimatedObject::addToScene(Vector3 Pos)
{
	mNode->attachObject(mEntity);
	mNode->setPosition(Pos);	// X=droite - Y=Haut - Z=devant
}

/* ------------------------------------------------------------------------- */
/// Oriente l'objet.
/**
	@param Orientation Angle (en degrés) autour de l'axe vertical, dont doit etre orienté l'objet.
	@param BlenderExport Si l'objet a été obtenu par l'Exporter blender, il faut un contournement
	d'un bug de gestion des axes.
*/
/* ------------------------------------------------------------------------- */
void AnimatedObject::setOrientation(int Orientation, bool BlenderExport)
{
	if (BlenderExport) mNode->pitch(Degree(Orientation));
	else mNode->yaw(Degree(Orientation));	// cas normal
}


/* ------------------------------------------------------------------------- */
/// Gestion du mouvement de la porte (ouverture et fermeture)
/* ------------------------------------------------------------------------- */
bool AnimatedObject::frameStarted(const FrameEvent& evt)
{
	//TRACE(">> Door::frameStarted()");
	mAnimState->addTime(evt.timeSinceLastFrame * mAnimationSpeed);
	// DEBUG_LOG("Length = "+ITOA(mAnimState->getTimePosition())+" sur "+ITOA(mAnimState->getLength()));

	switch (mStatus)
	{
	case OPENING:
		// Si la porte est en train de s'ouvrir,
		// On attend que l'animation soit finie.
		if (mAnimState->getTimePosition() >= mAnimState->getLength())
		{
			mStatus = WAITING;
			mWaitSeconds = 5;
		}
		break;

	case WAITING:
		// Si la porte est ouverte,
		// On attend que la temporisation soit finie.
		// On décremente les timers
		if (mWaitSeconds > 0)
			mWaitSeconds -= evt.timeSinceLastFrame;
		else
		{
			// DEBUG_LOG ("closing door");
			mStatus = CLOSING;
			closeDoor();
		}
		break;
	case CLOSING:
		// Si la porte est en train de se fermer,
		// On attend que l'animation soit finie.
		if (mAnimState->getTimePosition() == 0)
		{
			mStatus = CLOSED;
			DEBUG_LOG ("removing Door Listener");
			mRoot->removeFrameListener(this);
		}
		break;
	default:
		mStatus = CLOSED;
	}

	return true;
}


/* ------------------------------------------------------------------------- */
/// On active le focus au moment ou la souris passe sur l'objet
/* ------------------------------------------------------------------------- */
void AnimatedObject::setFocusOn()
{
	if (!mHasFocus)
	{
		/// S'il n'a pas deja le focus: on affiche un message
		Clock::getSingletonPtr()->showMessageOverlay("Activate");
	}
	mHasFocus = true;
}


/* ------------------------------------------------------------------------- */
/// Quand la souris quitte l'objet, on perd le focus.
/* ------------------------------------------------------------------------- */
void AnimatedObject::setFocusOff()
{
	mHasFocus = false;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction lance l'animation.
/* ------------------------------------------------------------------------- */
bool AnimatedObject::openDoor()
{
	bool DoingIt = true;	// Cette valeur est renvoyée en particulier si la porte est en cours de fermeture

	{
		if ( mStatus==CLOSED )
		{
		TRACE (">> openDoor: "+mName);
//		mOpenSoundChannel = SFX_playSound("../media/sounds/dooropen.wav",50,10000);
//		SFX_updateSoundSource(mOpenSoundChannel, mParentNode->getPosition()+mDoorNode->getPosition());
		mOpenSoundChannel = SoundManager::getSingletonPtr()->playSound("../media/sounds/dooropen.wav",50,10000);
		SoundManager::getSingletonPtr()->updateSoundSource(mOpenSoundChannel, mParentNode->getPosition()+mNode->getPosition());
		// Il faut fournir la position relative au Ship

		Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);
		try{ mAnimState = mEntity->getAnimationState(mAnimationName);} catch(...) {return false;}
		mAnimState->setEnabled(true);
		mAnimState->setLoop(false);
		mAnimationSpeed = 1; // durée de l'animation = 2.5 secondes
		mWaitSeconds = 0;
		mStatus = OPENING;
		mRoot->addFrameListener(this);
		DoingIt=true;
		}
	}
	return DoingIt;

}


/* ------------------------------------------------------------------------- */
/// Cette fonction joue l'animation à l'envers.
/* ------------------------------------------------------------------------- */
void AnimatedObject::closeDoor()
{
	mOpenSoundChannel = SoundManager::getSingletonPtr()->playSound("../media/sounds/dooropen.wav",50,10000);
	SoundManager::getSingletonPtr()->updateSoundSource(mOpenSoundChannel, mParentNode->getPosition()+mNode->getPosition());

	Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);
	mAnimState = mEntity->getAnimationState(mAnimationName);
	mAnimState->setEnabled(true);
	mAnimState->setLoop(false);
	mAnimationSpeed = -1;
}




/* ------------------------------------------------------------------------- */
/// Renvoie le nom de l'objet
/* ------------------------------------------------------------------------- */
String AnimatedObject::getName()
{
	return mName;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie le nom de l'animation de l'objet.
/* ------------------------------------------------------------------------- */
void AnimatedObject::setAnimation(String AnimationName)
{
	mAnimationName = AnimationName;
}


/* ------------------------------------------------------------------------- */
/// Déplace le Node de l'objet.
/* ------------------------------------------------------------------------- */
void AnimatedObject::translate(Vector3 offsetVector)
{
	mNode->translate(offsetVector);
}

