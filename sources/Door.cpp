/* ------------------------------------------------------------------------- */
/// @File       : Door.cpp
/// @Project    : Long Forgotten Earth
/// @Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Door.h"
#include "macros.h"
#include "classes.h"
#include "Clock.h"
#include "spares.h"
#include "TheShip.h"
#include "SoundManager.h"

/* ------------------------------------------------------------------------- */
/// Constructeur: Creation du Node de la Porte
/* ------------------------------------------------------------------------- */
Door::Door(String Name, String MeshFile, SceneManager* SceneMgr, SceneNode* ParentNode)
{
	mDoorName   = Name;
	mSceneMgr   = SceneMgr;
	mParentNode = ParentNode;

	mDoorEntity = mSceneMgr->createEntity(mDoorName, MeshFile);
	mDoorNode   = mParentNode->createChildSceneNode(mDoorName);
	mDoorEntity->setUserObject(this);

	// Les portes ne projettent pas d'ombres (receive only)
	mDoorEntity->setCastShadows(true);
	mDoorEntity->setQueryFlags(ACTIVABLE);

	// On recupère les sub-entities: panel1 et panel2 
	// pour pouvoir  changer leur texture en cours de programme.
	mFrontPanelMaterial = "";
	mBackPanelMaterial  = "";
	mScreen1Entity      = -1;		
	mScreen2Entity      = -1;		
	// Par défaut les panels sont le même mesh que la porte
	// mais il est possible de les avoir sur un mesh distinct.
	mPanelEntity        = mDoorEntity;  
	int NumSubEntities  = mPanelEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mDoorEntity->getSubEntity(i)->getMaterialName();
		if (MatName == "Door/screen1")  mScreen1Entity=i;
		if (MatName == "Door/screen2")  mScreen2Entity=i;
	}

	// Initialisation des membres
	mRoot          = Root::getSingletonPtr();
	mStatus        = CLOSED;
	mLocked        = false;
	mBroken        = false;
	mDepressurized = false;
	mDamage_ETR    = 0;
	mHasFocus      = false;
	mElevatorDoor  = false;
	mPanelNode     = NULL;
	mCollideObject = NULL;

	this->loadObject();
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Door::~Door()
{
}

/* ------------------------------------------------------------------------- */
/// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long Door::getTypeID (void) const
{
	return DOOR_ID;
}


/* ------------------------------------------------------------------------- */
/// Ajoute la porte à la scene courante
/* ------------------------------------------------------------------------- */
void Door::addToScene(Vector3 Pos)
{
	mDoorNode->setPosition(Pos);	// X=droite - Y=Haut - Z=devant
	mDoorNode->attachObject(mDoorEntity);
	if (mPanelNode) mPanelNode->attachObject(mPanelEntity);
	this->initCollisionShape();
}

/* ------------------------------------------------------------------------- */
/// Oriente la porte.
/** 
	@param Orientation Angle (en degrés) autour de l'axe vertical, dont doit etre orienté la porte.
	@param Qorient     Quaternion d'orientation de la porte.
	@param BlenderExport Si la porte a été décrite au format dotScene, il faut utiliser le quaternion  
*/
/* ------------------------------------------------------------------------- */
void Door::setOrientation(int Orientation, Quaternion Qorient, bool useQuaternion)
{
	if (useQuaternion) mDoorNode->setOrientation(Qorient);
	else mDoorNode->pitch(Degree(Orientation));
}


/* ------------------------------------------------------------------------- */
/// Memorise les materials des Panels de la porte.
/* ------------------------------------------------------------------------- */
void Door::setPanelMaterialNames(String FrontPanel, String BackPanel)
{
	DEBUG_LOG(mDoorName+"->setPanelMaterialNames("+FrontPanel+","+BackPanel+")");
	mFrontPanelMaterial = FrontPanel;
	mBackPanelMaterial  = BackPanel;
	if (!mBroken) this->setPanels();
}

/* ------------------------------------------------------------------------- */
/// Affecte des textures aux panneaux situés de chaque coté de la porte
/* ------------------------------------------------------------------------- */
void Door::setPanels()
{
	if ((mScreen1Entity>=0) && (mFrontPanelMaterial!=""))
		mPanelEntity->getSubEntity(mScreen1Entity)->setMaterialName(mFrontPanelMaterial);

	if ((mScreen2Entity>=0) && (mBackPanelMaterial !=""))
		mPanelEntity->getSubEntity(mScreen2Entity)->setMaterialName(mBackPanelMaterial);
}

/* ------------------------------------------------------------------------- */
/// Gestion du mouvement de la porte (ouverture et fermeture)
/* ------------------------------------------------------------------------- */
bool Door::frameStarted(const FrameEvent& evt)
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
			mWaitSeconds = 5; // delai d'attente avant fermeture de la porte.
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

	// Gestion des collisions avec les portes
	if (mCollideObject) 
	{
		mCollideObject->setDebug(false);
		mCollideObject->Refit(); // S'adapte aux animations (ouverture de porte)
	};

	return true;
}


/* ------------------------------------------------------------------------- */
/// On active le focus au moment ou la souris passe sur l'objet
/* ------------------------------------------------------------------------- */
void Door::setFocusOn()
{
	if (!mHasFocus)
	{
		/// S'il n'a pas deja le focus: on affiche un message
		Clock::getSingletonPtr()->showMessageOverlay("Open Door");
	}
	mHasFocus = true;
}


/* ------------------------------------------------------------------------- */
/// Quand la souris quitte l'objet, on perd le focus.
/* ------------------------------------------------------------------------- */
void Door::setFocusOff()
{
	mHasFocus = false;
}


/* ------------------------------------------------------------------------- */
/// Start animation for opening the door
/* ------------------------------------------------------------------------- */
bool Door::openDoor()
{
	bool DoingIt = true;	// Cette valeur est renvoyée en particulier si la porte est en cours de fermeture

	if ( !mLocked && !mBroken && !mDepressurized )
	{
		if ( mStatus==CLOSED )
		{
		TRACE (">> openDoor: "+mDoorName);
//		mOpenSoundChannel = SFX_playSound("../media/sounds/dooropen.wav",50,10000);
//		SFX_updateSoundSource(mOpenSoundChannel, mParentNode->getPosition()+mDoorNode->getPosition());
		mOpenSoundChannel = SoundManager::getSingletonPtr()->playSound("../media/sounds/dooropen.wav",50,10000);
		SoundManager::getSingletonPtr()->updateSoundSource(mOpenSoundChannel, mParentNode->getPosition()+mDoorNode->getPosition());
		// Il faut fournir la position relative au Ship

		Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);
		try{ mAnimState = mDoorEntity->getAnimationState(mAnimationName);} catch(...) {return false;}
		mAnimState->setEnabled(true);
		mAnimState->setLoop(false);
		mAnimationSpeed = 1; // durée de l'animation = 2.5 secondes
		mWaitSeconds = 0;
		mStatus = OPENING;
		mRoot->addFrameListener(this);
		DoingIt=true;
		}
	}
	else if (mBroken)
	{
		DEBUG_LOG (mDoorName+" is broken");
		mOpenSoundChannel = SoundManager::getSingletonPtr()->playSound("../media/sounds/doorlocked.wav",2 ,10000);
		Clock::getSingletonPtr()->showMessageOverlay("Door is broken");
		DoingIt=false;
	}
	else
	{
		DEBUG_LOG (mDoorName+" is locked");
		mOpenSoundChannel = SoundManager::getSingletonPtr()->playSound("../media/sounds/doorlocked.wav",2 ,10000);
		Clock::getSingletonPtr()->showMessageOverlay("Door is locked");
		DoingIt=false;
	}
	return DoingIt;

}


/* ------------------------------------------------------------------------- */
/// Start animation for closing the door
/* ------------------------------------------------------------------------- */
void Door::closeDoor()
{
	mOpenSoundChannel = SoundManager::getSingletonPtr()->playSound("../media/sounds/dooropen.wav",50,10000);
	SoundManager::getSingletonPtr()->updateSoundSource(mOpenSoundChannel, mParentNode->getPosition()+mDoorNode->getPosition());

	Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);
	mAnimState = mDoorEntity->getAnimationState(mAnimationName);
	mAnimState->setEnabled(true);
	mAnimState->setLoop(false);
	mAnimationSpeed = -1;
}


/* ------------------------------------------------------------------------- */
/// The door is locked
/* ------------------------------------------------------------------------- */
void Door::setDoorLocked()
{
	mLocked = true;
	if(mScreen1Entity>=0)
		mPanelEntity->getSubEntity(mScreen1Entity)->setMaterialName("Doorpanel/locked");
	if(mScreen2Entity>=0)
		mPanelEntity->getSubEntity(mScreen2Entity)->setMaterialName("Doorpanel/locked");
}

/* ------------------------------------------------------------------------- */
/// The door is locked (une des pieces est dépressurisée)
/* ------------------------------------------------------------------------- */
void Door::setDoorDepressurized()
{
	mDepressurized = true;
	if(mScreen1Entity>=0)
		mPanelEntity->getSubEntity(mScreen1Entity)->setMaterialName("Doorpanel/pressure");
	if(mScreen2Entity>=0)
		mPanelEntity->getSubEntity(mScreen2Entity)->setMaterialName("Doorpanel/pressure");
}

/* ------------------------------------------------------------------------- */
/// Le mecanisme de la porte est en panne (donc verrouillée).
/* ------------------------------------------------------------------------- */
void Door::setDoorMalfunction(bool broken)
{
	mBroken = broken;

	if (mBroken)
	{
		if(mScreen1Entity>=0)
			mPanelEntity->getSubEntity(mScreen1Entity)->setMaterialName("Doorpanel/malfunction");
		if(mScreen2Entity>=0)
			mPanelEntity->getSubEntity(mScreen2Entity)->setMaterialName("Doorpanel/malfunction");
	}
	else
	{
		// Si la porte est OK ou réparée, on affiche les bonnes textures.
		this->setPanels();
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie le nom de la porte 
/* ------------------------------------------------------------------------- */
String Door::getName()
{
	return mDoorName;
}

/* ------------------------------------------------------------------------- */
/// Fournit le nom de l'animation d'ouverture de la porte
/* ------------------------------------------------------------------------- */
void Door::setAnimation(String AnimationName)
{
	mAnimationName = AnimationName;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le status de la porte sous forme de chaine XML
/* ------------------------------------------------------------------------- */
String Door::getStatus()
{

	String Xstatus = "";

	// call=repair(id)     Uniquement s'il y des réparations à faire
	//                     Auquel cas, 'location' doit etre renseigné

	if (mBroken)
	{
		// On recupère le WayPoint le plus proche de la porte.
		SceneNode* WpNode = TheShip::getSingletonPtr()->getClosestWaypoint(mDoorNode->getPosition());
		Xstatus  = "<item";
		Xstatus += " id='"+ITOA((int)mDoorNode)   +"' ";
		Xstatus += " location='"+WpNode->getName()+"' ";
		Xstatus += " spares='"  +ITOA(CABLES)     +"' ";
		Xstatus += " ETR='"     +ITOA(mDamage_ETR)+"' ";
		Xstatus += " text='I just need five minutes to fix this...' ";
		Xstatus += " call = 'repair("+ITOA((int)mDoorNode)+")' ";	
		Xstatus += ">";
		Xstatus += mDoorName;
		Xstatus += "</item> \n";
	}

	if (mDepressurized)
	{
		Xstatus  = "<item";
		Xstatus += " id='"+ITOA((int)mDoorNode)   +"' ";
//		Xstatus += " location='none' ";
		Xstatus += " spares='"  +ITOA(NOTHING)    +"' ";
		Xstatus += " ETR='-1' ";		// "Estimated Time for Repair" inconnu
		Xstatus += " text='Negative.\nThis door is locked for security reasons. (Pressure differential too high)' ";
		Xstatus += ">";
		Xstatus += mDoorName;
		Xstatus += "</item> \n";
	}

	return Xstatus;
}



/* ------------------------------------------------------------------------- */
/// Déplace le Node de la Porte. La porte suit ainsi la position de la scene à laquelle elle appartient.
/* ------------------------------------------------------------------------- */
void Door::translate(Vector3 offsetVector)
{
	mDoorNode->translate(offsetVector);
}


/* ------------------------------------------------------------------------- */
/// Ajout du panel de la porte, en tant que mesh distinct.
/* ------------------------------------------------------------------------- */
void Door::addPanel(String MeshFile, Vector3 Pos, Quaternion Ori)
{
	if (MeshFile=="") return;

	mPanelEntity = mSceneMgr->createEntity(mDoorName+"/panel", MeshFile);
	mPanelNode   = mDoorNode->createChildSceneNode();
	mPanelNode->setPosition(Pos);
	mPanelNode->setOrientation(Ori);
	mPanelEntity->setQueryFlags(IMMOBILE);

	// On recupère les sub-entities: panel1 et panel2 
	// pour pouvoir  changer leur texture en cours de programme.
	mScreen1Entity=-1;		
	mScreen2Entity=-1;		
	int NumSubEntities = mPanelEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mPanelEntity->getSubEntity(i)->getMaterialName();
		if (MatName == "2/door/inside")   mScreen1Entity=i;
		if (MatName == "2/door/outside")  mScreen2Entity=i;
	}
	DEBUG_LOG("  mScreen1Entity "+ITOA(mScreen1Entity));
	DEBUG_LOG("  mScreen2Entity "+ITOA(mScreen2Entity));
}

/* ------------------------------------------------------------------------- */
/// Cette fonction affecte (ou non) à la porte le status de porte d'ascenseur.
/* ------------------------------------------------------------------------- */
void Door::setElevatorDoor(bool elevator)
{
	mElevatorDoor = elevator;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction indique si la porte donne sur un ascenseur.
/* ------------------------------------------------------------------------- */
bool Door::isElevatorDoor()
{
	return mElevatorDoor;
}



/* ------------------------------------------------------------------------- */
/// Charge les paramètres initiaux de la Porte.
/* ------------------------------------------------------------------------- */
void Door::loadObject()
{
    String filename = "../savedgames/doors.xml";
	TRACE("Loading "+filename);

	XMLParser_T* SavedGame = new XMLParser_T(filename);

	if (!SavedGame) return;
	if (!SavedGame->gotoRootElement("doors")) return;
	if (!SavedGame->gotoSubNamedElement("status",mDoorName)) return;

	mLocked     = SavedGame->getBooleanAttribute("locked");
	mBroken     = SavedGame->getBooleanAttribute("broken");
	mDamage_ETR = SavedGame->getIntAttribute("etr");

	this->setDoorMalfunction(mBroken);

	delete(SavedGame);
}



/* ------------------------------------------------------------------------- */
/// Gere les collisions de la porte.
/* ------------------------------------------------------------------------- */
void Door::initCollisionShape()
{
	using namespace OgreOpcode;
	
	CollisionContext* collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();

	// Creation de la forme englobante
	CollisionShape *collideShape = CollisionManager::getSingletonPtr()->NewShape(mDoorName.c_str());
	// Chargement du mesh associé à cette forme
	collideShape->Load(mDoorEntity);
	// Creation de l'objet
	mCollideObject = collideContext->NewObject();
	mCollideObject->SetCollClass("door");
	mCollideObject->SetShape(collideShape);

	// Ajout de cet objet dans le contexte
	collideContext->AddObject(mCollideObject);
	TRACE("Collision System initialized for "+mDoorName);
}
