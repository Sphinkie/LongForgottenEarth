/* ------------------------------------------------------------------------- */
// File       : Hibernation.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "fmod.h"

#include "Hibernation.h"
#include "macros.h"
#include "classes.h"
#include "spares.h"
#include "TheShip.h"
#include "Clock.h"
#include "Player.h"
#include "NPC_manager.h"
#include "SoundManager.h"
#include "XMLparser_T.h"

// Les differentes actions qui ont besoin du FrameListener
#define FOR_NOBODY    0
#define FOR_ANIMATION 1
#define FOR_WORK      2
#define FOR_GUI       4


/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
Hibernation::Hibernation(String Name, String MeshFile, SceneNode* ParentNode, SceneManager* SceneMgr)
{
	// Stockage des paramètres
	// ---------------------------------------------
	mHibernationName= Name;
	mSceneMgr       = SceneMgr;
	mRoot           = Root::getSingletonPtr();
	mMeshFile       = MeshFile;
	mParentNode     = ParentNode;

	// Lecture des parametres du fichier de config
	// ---------------------------------------------
    ConfigFile DescriptionFile;
    DescriptionFile.load("../media/objects/hibernation.cfg");
	TRACE("Loading ../media/objects/hibernation.cfg");

	mOpenSoundFile      =DescriptionFile.getSetting("OpenSoundFile");
	mStartSoundFile     =DescriptionFile.getSetting("StartSoundFile");
	mEndSoundFile       =DescriptionFile.getSetting("EndSoundFile");
	mHelloSoundFile     =DescriptionFile.getSetting("HelloSoundFile");
	mInterfaceFile      =DescriptionFile.getSetting("InterfaceFile");
	mOperatingSoundFile =DescriptionFile.getSetting("OperatingSoundFile");

	// TODO: Quid en cas de fichier inexistant ???

	// Initialisation des variables membres
	// ---------------------------------------------
	mHibernation_GUI = NULL;
	mExitGui         = false;
	mExitGuiDelay    = 5;
	mCptFrame        = 0;
	mListenerNeeded  = FOR_NOBODY;

	
	mOpenSoundChannel     = NULL_CHANNEL;
	mStartSoundChannel    = NULL_CHANNEL;
	mEndSoundChannel      = NULL_CHANNEL;
	mOperatingSoundChannel= NULL_CHANNEL;
	mHelloSoundChannel    = NULL_CHANNEL;


	// Creation de l'entité
	// ---------------------------------------------
	mEntity = SceneMgr->createEntity(Name, mMeshFile);
	mHibernationNode   = mParentNode->createChildSceneNode(Name);
	mEntity->setUserObject(this);
	mEntity->setCastShadows(true);
	mEntity->setQueryFlags(ACTIVABLE);

	// ---------------------------------------------
	// Creation de l'objet Collision
	// ---------------------------------------------
	using namespace OgreOpcode;
	CollisionContext* CollideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();
	CollisionShape*   CollideShape   = CollisionManager::getSingletonPtr()->NewShape(Name.c_str());
	CollideShape->Load(mEntity);
	CollisionObject* CollideObject = CollideContext->NewObject();
	CollideObject->SetCollClass("object");
	CollideObject->SetShape(CollideShape);
	CollideContext->AddObject(CollideObject);


	// Creation d'un node supplémentaire pour les particules
	// ---------------------------------------------
	mParticulesNode= mHibernationNode->createChildSceneNode();
	mParticulesNode->setPosition(0,24,30);
	mSmoke       = NULL;
	mSparks      = NULL;
	mGivre       = NULL;

	// On recupère la sub-entity: "screen" 
	// pour pouvoir changer sa texture en cours de programme.
	// ---------------------------------------------
	mScreenEntity=0;		// valeur par defaut
	int NumSubEntities = mEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mEntity->getSubEntity(i)->getMaterialName();
		if (MatName == "hibernation/screen")  mScreenEntity=i;
	}

	// Etat initial de l'objet
	// ---------------------------------------------
	mBroken          = false;
	mHasFocus        = false;
	mStatus          = CLOSED;
	mPhase           = IDLE;
	mTemperature     = TheShip::getSingletonPtr()->getAmbiantTemperature(); // Initialisation par défaut
	mInitialDuration = 0;		// Unité = jours
	mInitialDate     = 0;
	mFinalDate       = 0;
	mSubject         = NULL;
	mLocked          = mBroken;

	// Chargement de la configuration initiale de l'objet
	// --------------------------------------------------
	this->loadObject();
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Hibernation::~Hibernation()
{
	// Arreter la gestion
	mRoot->removeFrameListener(this);

	// Detacher les particules et l'Entity
	mHibernationNode->detachAllObjects();
	
	// TODO : faut-t-il destroyer l'Entity???
}


/* ------------------------------------------------------------------------- */
/// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long Hibernation::getTypeID (void) const
{
	return HIBERNATION_ID;
}


/* ------------------------------------------------------------------------- */
/// Charge les parametres initiaux du caisson
/* ------------------------------------------------------------------------- */
void Hibernation::loadObject ()
{
    String filename = "../savedgames/"+mHibernationName+".xml";
	TRACE("   Loading "+filename);
	XMLParser_T* SavedGame = new XMLParser_T(filename);
	if (!SavedGame) return;

	// Chargement des reglages du caisson
	SavedGame->gotoElementPath("hibernation/settings");
	mInitialDuration   = SavedGame->getIntAttribute("InitialDuration");
	mInitialDate       = SavedGame->getIntAttribute("InitialDate");
	mFinalDate         = SavedGame->getIntAttribute("FinalDate");
	mSubjectName       = SavedGame->getStringAttribute("Subject");

	// Chargementr de l'état du caisson d'hibernation
	SavedGame->gotoElementPath("hibernation/status");
	mTemperature= SavedGame->getIntAttribute("Temperature");
	mBroken     = SavedGame->getBooleanAttribute("Broken");
	mLocked     = SavedGame->getBooleanAttribute("Locked");
	String Phase= SavedGame->getStringAttribute("Phase");
	if (Phase=="idle")      mPhase=IDLE;   
	if (Phase=="starting")  mPhase=STARTING;
	if (Phase=="operating") mPhase=OPERATING;
	if (Phase=="ending")    mPhase=ENDING;
	
//	try	{}	catch (Ogre::Exception& e) 
//	{
//		// On affiche le number juste pour ne pas avoir de warning à la compilation sur la variable e
//		DEBUG_LOG("WARNING: saved object not found - code "+ITOA(e.getNumber()));
//	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie le nom du caisson
/* ------------------------------------------------------------------------- */
String Hibernation::getName()
{
	return mHibernationName;
}

/* ------------------------------------------------------------------------- */
/// Renvoie la position du Caisson. RelativeTo peut valoir "PARENT" ou "SHIP"
/* ------------------------------------------------------------------------- */
Vector3 Hibernation::getPosition(String RelativeTo )
{
	if (RelativeTo=="SHIP"  ) 
	{
		int Level_Nb = 3;
		if (mMeshFile == "hib_pod.mesh") Level_Nb=4;
		// Pas très joli, mais permet de savoir à quel etage se trouve ce caisson.
		Vector3 Pos_Level = TheShip::getSingletonPtr()->getShipLevel(Level_Nb)->getPosition();
		Vector3 Pos_Room  = mParentNode->getPosition();
		Vector3 Pos_Hiber = mHibernationNode->getPosition();
//		DEBUG_LOG ("Level Position = " + StringConverter::toString(Pos_Level));
//		DEBUG_LOG ("Room  Position = " + StringConverter::toString(Pos_Room));
//		DEBUG_LOG ("Hiber Position = " + StringConverter::toString(Pos_Hiber));
//		DEBUG_LOG ("Returned Position = " + StringConverter::toString(Pos_Level + Pos_Room + Pos_Hiber));
		return (Pos_Level + Pos_Room + Pos_Hiber);
	}
	else
	{
		// RelativeTo == "PARENT"
		return mHibernationNode->getPosition();
	}
}


/* ------------------------------------------------------------------------- */
/// Positionne le caisson
/* ------------------------------------------------------------------------- */
void Hibernation::setPosition(Vector3 Position)
{
	mHibernationNode->setPosition(Position);
}


/* ------------------------------------------------------------------------- */
/// Oriente le caisson
/* ------------------------------------------------------------------------- */
void Hibernation::setOrientation(int Orientation)
{
	mHibernationNode->yaw(Degree(Orientation));
}

/* ------------------------------------------------------------------------- */
/// Oriente le caisson
/* ------------------------------------------------------------------------- */
void Hibernation::setOrientation(Quaternion Orientation)
{
	mHibernationNode->setOrientation(Orientation);
}

/* ------------------------------------------------------------------------- */
/// Ajoute la caisson à la scene courante
/* ------------------------------------------------------------------------- */
void Hibernation::addToScene()
{
	mHibernationNode->attachObject(mEntity);
	// met à jour l'image du screen
	this->setMaterialScreen();	
}


/* ------------------------------------------------------------------------- */
/// Retire la capsule de la scene courante
/* ------------------------------------------------------------------------- */
void Hibernation::removeFromScene()
{
	mRoot->removeFrameListener(this);
	mHibernationNode->detachObject(mEntity);
	if (mSmoke)  mHibernationNode->detachObject(mSmoke);
	if (mSparks) mHibernationNode->detachObject(mSparks);
}


/* ------------------------------------------------------------------------- */
/// Gere les pannes du caisson
/* ------------------------------------------------------------------------- */
void Hibernation::setBroken(bool broken)
{
	mBroken = broken;
	mLocked = broken;
}


/* ------------------------------------------------------------------------- */
/// On active le focus au moment ou la souris passe sur l'objet
/* ------------------------------------------------------------------------- */
void Hibernation::setFocusOn()
{
	if (!mHasFocus)
	{
		DEBUG_LOG(">> setFocusOn()");
		/// S'il n'a pas deja le focus: on émet un son.
		mHelloSoundChannel = SoundManager::getSingletonPtr()->playSoundShort(mHelloSoundFile);
		/// Et on affiche un message
		Clock::getSingletonPtr()->showMessageOverlay("Show Control Panel");
		// S'il y a un sujet dans le caisson, on le fait apparaitre
		mSubject = NPC_Manager::getSingletonPtr()->getBody(mSubjectName);
		if (mSubject)
		{
			// On rattache le Body au node du Caisson
			mSubject->addToScene(mHibernationNode);
			// On ajuste la position du Body
			mSubject->setPosition(Vector3(0,20,60));
		}
		// met à jour l'image du petit ecran de controle
		this->setMaterialScreen();	
		mHasFocus = true;
	}
}



/* ------------------------------------------------------------------------- */
/// Quand on perd le focus, on détruit le GUI d'interface Hibernation
/* ------------------------------------------------------------------------- */
void Hibernation::setFocusOff()
{
	DEBUG_LOG(">> setFocusOff()");
	mHasFocus = false;
	// On libère le channel Audio
	SoundManager::getSingletonPtr()->freeSound(mHelloSoundChannel);
	/// On efface l'interface
	removeInterface();
}


/* ------------------------------------------------------------------------- */
/// Détruit un objet d'Interface Hibernation (GUI)
/* ------------------------------------------------------------------------- */
void Hibernation::removeInterface() 
{
	if (mHibernation_GUI)  delete mHibernation_GUI;
	mHibernation_GUI = NULL;
	mListenerNeeded ^= FOR_GUI;
}

/* ------------------------------------------------------------------------- */
/// Crée un objet d'Interface Hibernation (GUI)
/* ------------------------------------------------------------------------- */
void Hibernation::createInterface() 
{
	// Si l'interface existe deja on ne la recrée pas
	// Si une demande de sortie de la GUI est en cours, on ne la recrée pas
	if (!mHibernation_GUI && !mExitGui)
	{
		mExitGui   = false;
		mHibernation_GUI = new Hibernation_Interface(this, mInterfaceFile);
		mHibernation_GUI->createInterface();
		this->addFrameListener(FOR_GUI);
	}
}


/* ------------------------------------------------------------------------- */
/// L'interface est masquée mais pas détruite
/* ------------------------------------------------------------------------- */
void Hibernation::hideInterface() 
{
	if (mHibernation_GUI)  
	{
		mHibernation_GUI->hideMouse();
		mHibernation_GUI->hideInterface();
	}
}


/* ------------------------------------------------------------------------- */
/// Mémorise une demande de fin de la GUI
/* ------------------------------------------------------------------------- */
void Hibernation::requestExitInterface(Real seconds)
{
	TRACE(">> requestExitInterface()");
	
	// on vérifie qu'une demande n'est pas déjà en cours
	if (!mExitGui)
	{
		mExitGui = true;
		mExitGuiDelay=seconds;
	}
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si une demande de sortie de GUI est en cours.
/* ------------------------------------------------------------------------- */
bool Hibernation::isExitInterfaceRequiered()
{
	/// Renvoie FALSE si aucune demande de sortie n'est en cours.
	return mExitGui;
}


/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le GUI est inactif  (cad non affiché)
/* ------------------------------------------------------------------------- */
bool Hibernation::isInterfaceDisabled()
{
	return (mHibernation_GUI==NULL);
}


/* ------------------------------------------------------------------------- */
/// Renvois TRUE si le caisson est endommagé.
/* ------------------------------------------------------------------------- */
bool Hibernation::isBroken()
{
	return mBroken;
}


/* ------------------------------------------------------------------------- */
/// Ouvre le couvercle de la capsule.
/* ------------------------------------------------------------------------- */
bool Hibernation::openDoor()
{
	if (mLocked) return false;

	if (mStatus==CLOSED) 
	{
		TRACE("Opening Hibernation "+mHibernationName);

		// On enleve un eventuel brouillard
		mSceneMgr->setFog(FOG_NONE);
		// Fin du bruit de respiration (éventuel)
		SoundManager::getSingletonPtr()->freeSound(mOperatingSoundChannel);
		// Joue le son de l'ouverture du caisson
		mOpenSoundChannel = SoundManager::getSingletonPtr()->playSoundObject(mOpenSoundFile);
		FSOUND_SetVolume(mOpenSoundChannel,255);
		// Il faut fournir la position relative au Ship (car la camera est positionné par rapport au Ship)
		// TheShip-->Level-->Room-->Hibernation
		// TheShip-->CameraNode-->Camera
		SoundManager::getSingletonPtr()->updateSoundSource(mOpenSoundChannel, this->getPosition("SHIP")+Vector3(0,60,100));

		// On demarre l'animation open
		Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);
		mAnimState = mEntity->getAnimationState("open");
		mAnimState->setEnabled(true);
		mAnimState->setLoop(false);
		mAnimationSpeed = 1; // durée de l'animation = 10/1 = 10 secondes
		mStatus = OPENING;
		this->addFrameListener(FOR_ANIMATION);

		// S'il n'existe pas, on attache le Node des particules
		if (!mSmoke)   
		{
			// (le script est dans vapeur.material et vapeur.particle)
			mSmoke  = ParticleSystemManager::getSingleton().createSystem(mHibernationName+"/vapeur", "Effect/vapeur");
			mParticulesNode->attachObject(mSmoke);
		}
		else
		{
			DEBUG_LOG("mSmoke already exist");
		}
		// Active l'émission des particules
		mSmoke->getEmitter(0)->setEnabled(true);

		return true;
	}
	else
	{
		TRACE(mHibernationName+" is already open");
		return false;
	}
}



/* ------------------------------------------------------------------------- */
/// Ferme le couvercle de la capsule.
/* ------------------------------------------------------------------------- */
bool Hibernation::closeDoor()
{
	if (mLocked) return false;
	if (mStatus==OPENED) 
	{
		TRACE("Closing Hibernation "+mHibernationName);
		// Joue le son de fermeture du caisson
		mOpenSoundChannel = SoundManager::getSingletonPtr()->playSoundObject(mOpenSoundFile);
		// Il faut fournir la position relative au Ship (car la camera est positionné par rapport au Ship)
		// TheShip-->Level-->Room-->Hibernation
		// TheShip-->CameraNode-->Camera
		SoundManager::getSingletonPtr()->updateSoundSource(mOpenSoundChannel, this->getPosition("SHIP")+Vector3(0,60,100));

		// On demarre l'animation de "close" (=open à l'envers)
		Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);
		mAnimState = mEntity->getAnimationState("open");
		mAnimState->setEnabled(true);
		mAnimState->setLoop(false);
		mAnimationSpeed = -1;
		mStatus = CLOSING;
		this->addFrameListener(FOR_ANIMATION);
		return true;
	}
	else
	{
		TRACE(mHibernationName+" is already closed");
		return false;
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le caisson est ouvert
/* ------------------------------------------------------------------------- */
bool Hibernation::isOpened()
{
	return (mStatus==OPENED);
}


/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le caisson est fermé
/* ------------------------------------------------------------------------- */
bool Hibernation::isClosed()
{
	return (mStatus==CLOSED);
}


/* ------------------------------------------------------------------------- */
/// Gère les animations de la capsule d'hibernation
/* ------------------------------------------------------------------------- */
bool Hibernation::frameStarted (const FrameEvent& evt)
{
	// DEBUG_LOG("Length = "+ITOA(mAnimState->getTimePosition())+" sur "+ITOA(mAnimState->getLength()));

	if (mExitGui)           {mExitGuiDelay-=evt.timeSinceLastFrame;}
	if (mExitGuiDelay<0)	{this->setFocusOff(); mExitGui=false; mExitGuiDelay=5;}

	switch (mStatus)
	{
	case OPENING:
		// Le couvercle est en train de s'ouvrir,
		mAnimState->addTime(evt.timeSinceLastFrame * mAnimationSpeed);
		// Si l'animation d'ouverture du couvercle est finie.
		if (mAnimState->getTimePosition() >= mAnimState->getLength())
		{
			mStatus = OPENED;
			mListenerNeeded ^= FOR_ANIMATION;	// XOR
//			// On arrete le bruit du mécanisme
//			SFX_freeSound(mOpenSoundChannel);
			// TODO: On detache les particules (vapeur)
			// mHibernationNode->detachObject(mParticulesNode);
		}
		break;

	case CLOSING:
		// Le couvercle  est en train de se fermer,
		mAnimState->addTime(evt.timeSinceLastFrame * mAnimationSpeed);
		// On attend que l'animation soit finie.
		if (mAnimState->getTimePosition() == 0)
		{
			mStatus = CLOSED;
			mListenerNeeded ^= FOR_ANIMATION;	// XOR
//			// On arrete le bruit du mécanisme
//			SFX_freeSound(mOpenSoundChannel);
			// On active le son de respiration et battement de coeur
			if ((mSubject) && (mSubject->isPlayerBody()))	
			// Note: Les conditions ci-dessus doivent etre testées dans l'ordre!
			// Sinon plantage si mSubject est NULL... (=caisson vide)
			{
//				mOperatingSoundChannel   = SoundManager::getSingletonPtr()->playSound(mOperatingSoundFile,5,100,true);
				mOperatingSoundChannel   = SoundManager::getSingletonPtr()->playSoundObject(mOperatingSoundFile, true);
				mOperatingSoundFrequency = FSOUND_GetFrequency(mOperatingSoundChannel);
				mOperatingSoundVolume    = FSOUND_GetVolume(mOperatingSoundChannel);
			}

		}
		break;
	default:
		break;
	}

	// -------------------------------------------------------
	// Certaines actions ne sont pas executées à chaque passsage
	// mCptFrame varie entre 0 et 10 en 1 seconde
	// -------------------------------------------------------
	// On incrémente les clocks
	mFineClock += (evt.timeSinceLastFrame*10);  // unité de FineClock = le 10eme de seconde
	mGrossClock = int(mFineClock);              // unité de GrossClock = le 10eme de seconde
	// On incremente mCptFrame chaque fois que GrossClock change 
	// Sinon, on le met a zero (= pas d'action)
	if (mGrossClock != mLastGrossClock)   
	{
		// -------------------------------------------------
		// Actions à faire tous les diziemes de seconde:
		// -------------------------------------------------
		if ((mPhase==STARTING) && (mSubject->isPlayerBody()))
		{
			// On cree un effet de brouillard qui se resserre
			if (mEndFog>3) mEndFog -= 2;
			mSceneMgr->setFog(FOG_LINEAR,ColourValue::Black,0,0,mEndFog);
		}
		// -------------------------------------------------
		if ((mPhase==ENDING) && (mSubject->isPlayerBody()))
		{
			// Le brouillard se dissipe
			mEndFog+=2;
			mSceneMgr->setFog(FOG_LINEAR,ColourValue::Black,0,0,mEndFog);
		}
		// -------------------------------------------------
		mCptFrame = mGrossClock; 
	}
	else mCptFrame=0;
	mLastGrossClock = mGrossClock;
	if (mFineClock>10) {mFineClock=0; mGrossClock=0; mCptFrame=0;}

	// -------------------------------------------------
	// Actions à faire une fois par seconde:
	// -------------------------------------------------
	switch (mCptFrame)
	{
	case 0:
		// on ne fait rien
		break;

	case 1:
		// On fait clignoter certains voyants de l'interface
		if (mHibernation_GUI) mHibernation_GUI->swapFlash();	
		if (mHibernation_GUI) mHibernation_GUI->refreshTexts("mainWin");
		break;

	case 2:
	case 7:
		if (mPhase==STARTING)
		{
			// Gestion de l aphase de démarrage de la séquence d'hibernation
			if (mTemperature>-44)	
			{
				// On fait baisser la température du caisson 
				mTemperature-=1;
			}
			else
			{
				// La température a atteint le minimum: on change de phase
				mPhase = OPERATING;
				SoundManager::getSingletonPtr()->freeSound(mStartSoundChannel);
			}
		}
		if (mPhase==ENDING)
		{
			// On fait augmenter la température du caisson 
			if (mTemperature<18)	mTemperature+=1;
			else 	
			{ 
				// la température est revenue à la normale: on change de phase
				mPhase = IDLE; 
				mInitialDuration=0;
				mLocked=false;
				SoundManager::getSingletonPtr()->freeSound(mEndSoundChannel);
			}
		}
		break;

	case 4:
		if ((mPhase==OPERATING) )
		{
			if (mSubject->isPlayerBody()) 
			{
				// On accélère (puis ralentit) le temps
				Clock::getSingletonPtr()->shrinkTime(mFinalDate);
				// On ralentit les battements de coeur
				int Frequency = FSOUND_GetFrequency(mOperatingSoundChannel)*0.98;
				FSOUND_SetFrequency(mOperatingSoundChannel,Frequency);
				// On augmente un peu le volume
				int Volume = FSOUND_GetVolume(mOperatingSoundChannel)*1.1;
				FSOUND_SetVolume(mOperatingSoundChannel,Volume);
			}
			// On déclenche le réveil si le temps d'hibernation est écoulé
			if (Clock::getSingletonPtr()->getCurrentDate() >= mFinalDate)
			{
				// Et si le sujet n'est pas dans un état critique ou mort
				// Et sauf le joueur, qui est réveillé quelquesoit son état physique
				if ((mSubject->isPlayerBody()) || (mSubject->getVitalite()>10)) this->endSequence();
			}
		}
		break;


	case 6:
		// TODO: faire une remise à la normale progressive
		if ((mPhase==ENDING) && (mSubject->isPlayerBody()))
		{
			// On rétabli le temps et le coeur à une vitesse normale
			Clock::getSingletonPtr()->setSpeed(1);
			FSOUND_SetFrequency(mOperatingSoundChannel,mOperatingSoundFrequency);
			FSOUND_SetVolume(mOperatingSoundChannel,mOperatingSoundVolume);
		}
		break;
	}
	// -------------------------------------------------------
	// -------------------------------------------------------


	// Si plus personne n'en n'a besoin, on enleve le FrameListener
	if (mListenerNeeded==FOR_NOBODY)
	{
		DEBUG_LOG ("removing Hibernation FrameListener");
		mRoot->removeFrameListener(this);
	}

	return true;
}


/* ------------------------------------------------------------------------- */
/// Gère le lancement du FrameListener
/* ------------------------------------------------------------------------- */
void Hibernation::addFrameListener(short User)
{
	TRACE("adding Hibernation FrameListener");
	mFineClock=0;
	mGrossClock=0;
	mLastGrossClock=0;

	if (mListenerNeeded==FOR_NOBODY) mRoot->addFrameListener(this);
	mListenerNeeded |= User;
}


/* ------------------------------------------------------------------------- */
/// Lance la séquence d'hibernation
/* ------------------------------------------------------------------------- */
void Hibernation::startSequence()
{
	mPhase  = STARTING;
	mLocked = true;
	mInitialDate     = Clock::getSingletonPtr()->getCurrentDate();
	mFinalDate       = Clock::getSingletonPtr()->getFutureDate(mInitialDuration);
	DEBUG_LOG("Hibernation: InitialDate("+ITOA(mInitialDate)+") + mInitialDuration("+ITOA(mInitialDuration)+") = FinalDate("+ITOA(mFinalDate)+")");

	// Son de l'air qui se vide
	mStartSoundChannel = SoundManager::getSingletonPtr()->playSoundObject(mStartSoundFile); // ,5,10);

	// Petit nuage de givre: S'il n'existe pas, on crée (et attache) le Node des particules
	if (!mGivre)   
	{
		// (le script est dans givre.material et givre.particle)
		mGivre  = ParticleSystemManager::getSingleton().createSystem(mHibernationName+"/givre", "Effect/givre");
		mParticulesNode->attachObject(mGivre);
	}
	// Active l'émission des particules
	mGivre->getEmitter(0)->setEnabled(true);

	// Initialisation du brouillard
	mEndFog = 500;
	this->addFrameListener(FOR_WORK);
}



/* ------------------------------------------------------------------------- */
/// Lance la séquence de fin d'hibernation
/* ------------------------------------------------------------------------- */
void Hibernation::endSequence()
{
	mPhase = ENDING;
	this->addFrameListener(FOR_WORK);

	// Son de l'air qui se remplit
	mEndSoundChannel = SoundManager::getSingletonPtr()->playSoundObject(mEndSoundFile);	//,5,10);
}


/* ------------------------------------------------------------------------- */
/// Monte ou descent du caisson (pour le joueur)
/* ------------------------------------------------------------------------- */
void Hibernation::climb()
{
	if (mSubject && mSubject->isPlayerBody()) climbOut();
	else if (!mSubject) climbIn();
}


/* ------------------------------------------------------------------------- */
/// Le joueur monte dans le caisson 
/* ------------------------------------------------------------------------- */
void Hibernation::climbIn()
{
	TRACE(">> ClimbIn()");
	// On recupère le nom du sujet
	Player* player = Player::getSingletonPtr();
	mSubjectName = player->getName();
	// On recupère le sujet (qui n'est pas obligatoirement le player)
	// mSubject     = NPC_Manager::getSingletonPtr()->getPlayerBody();
	mSubject     = NPC_Manager::getSingletonPtr()->getBody(mSubjectName);
	// On fait apparaitre le sujet
	if (mSubject)
	{
		// On rattache le Body au node du Caisson
		mSubject->addToScene(mHibernationNode);
		// On ajuste la position du Body
		mSubject->setPosition(Vector3(0,20,60));
	}
	// Le joueur est alongé et ne peut plus se deplacer
	player->setLieMode(true);
	// Le Node du player est rattaché au caisson
	player->attachToNode(mHibernationNode);
	player->changeHeadPosition(Vector3(0,28,16));
	player->lookAt(Vector3(0,40,1000));
}


/* ------------------------------------------------------------------------- */
/// Le joueur descend du caisson 
/* ------------------------------------------------------------------------- */
void Hibernation::climbOut()
{
	TRACE(">> ClimbOut()");
	// On fait disparaitre le sujet
	mSubject = NPC_Manager::getSingletonPtr()->getBody(mSubjectName);
	mSubject->removeFromScene();
	mSubjectName="";
	mSubject=NULL;
	// Le joueur est debout (et peut se deplacer)
	Player::getSingletonPtr()->setLieMode(false);
	// Le Node du player est rattaché au ship
	Player::getSingletonPtr()->attachToNode(TheShip::getSingletonPtr()->getNode());
	Player::getSingletonPtr()->restaureHeadPosition();
}


/* ------------------------------------------------------------------------- */
/// Mémorise la durée de l'hibernation à effectuer
/* ------------------------------------------------------------------------- */
void Hibernation::setDuration(int duration)
{
	mInitialDuration = duration;
}


/* ------------------------------------------------------------------------- */
/// Met à jour l'image réduite de l'interface de commande du Caisson
/* ------------------------------------------------------------------------- */
void Hibernation::setMaterialScreen()
{
	TRACE(">>setMaterialScreen()");

	if (mBroken)
		mEntity->getSubEntity(mScreenEntity)->setMaterialName("hibernation/dyn/alarm");
	else if (!mSubject)
		mEntity->getSubEntity(mScreenEntity)->setMaterialName("hibernation/dyn/empty");
	else if (mSubject->getVitalite()==0)
		mEntity->getSubEntity(mScreenEntity)->setMaterialName("hibernation/dyn/dead");
	else if (mSubject->getVitalite()<10)
		mEntity->getSubEntity(mScreenEntity)->setMaterialName("hibernation/dyn/critical");
	else
		mEntity->getSubEntity(mScreenEntity)->setMaterialName("hibernation/dyn/hiberning");
}


/* ------------------------------------------------------------------------- */
/// Renvoie le status du caisson d'hibernation sous forme de chaine XML
/* ------------------------------------------------------------------------- */
String Hibernation::getStatus()
{

	TRACE (">> "+mHibernationName+"::getStatus()");
	String Xstatus = "";

	if (mBroken)
	{
		Vector3 PositionRelativeToShip = mParentNode->getPosition() + mHibernationNode->getPosition();
		SceneNode* WpNode = TheShip::getSingletonPtr()->getClosestWaypoint(PositionRelativeToShip);
		Xstatus  = "<item";
		Xstatus += " id='"+ITOA((int)mHibernationNode)  +"' ";
		Xstatus += " location='"+WpNode->getName()      +"' ";
		Xstatus += " spares='"  +ITOA(FREON)            +"' ";
		Xstatus += " ETR='300' ";		// "Estimated Time for Repair" 3 heures
		Xstatus += " text='Some Freon leaks have been detected. Cryogenic circuit has to be purged, and some parts need to be replaced.' ";
		Xstatus += " call = 'repair("+ITOA((int)mHibernationNode)+")' ";	
		Xstatus += ">";
		Xstatus += mHibernationName;
		Xstatus += "</item> \n";
		TRACE ("   "+mHibernationName+" has a bad status...");
	}

	return Xstatus;
}

