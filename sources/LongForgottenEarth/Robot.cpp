/* ------------------------------------------------------------------------- */
// File       : Robot.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */


#include <conio.h>			// Pour la console de debug

#include "macros.h"
#include "classes.h"
#include "Clock.h"
#include "Robot.h"
#include "TheShip.h"		
#include "SoundManager.h"

// #define TRACE DEBUG_LOG

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
Robot::Robot(String Name, String DescFile, SceneNode* ParentNode, SceneManager* SceneMgr)
{
	TRACE(">> Robot");

	// stockage des paramètres
	// ---------------------------------------------
	mRobotName  = Name;
	mSceneMgr   = SceneMgr;
	mRoot       = Root::getSingletonPtr();
	mParentNode = ParentNode;

	// ---------------------------------------------
	// Lecture des parametres du fichier de config
	// et Chargement de la configuration initiale de l'objet
	// ---------------------------------------------
	this->loadDescription(DescFile);


	// Initialisation des variables membres
	// ---------------------------------------------
	mExitGuiDelay=5;

	mExitGui      = false;
	mEngineOn     = false;
	mBroken       = false;
	mMoving       = false;
	mMovingUpDown = false;
	mTurning      = false;
	mWorking      = false;
	mHasFocus     = false;
	mDown         = false;
	
	mVy              = 0;
	mDestination     = Vector3::ZERO;
	mCurrentWaypoint = NULL;
	mRobot_GUI       = NULL;

	mSourceNode  = NULL;
	mSmoke       = NULL;
	mSparks      = NULL;

	mSparkChannel     =NULL_CHANNEL;
	mHelloChannel     =NULL_CHANNEL;
	mWork1Channel     =NULL_CHANNEL;
	mWork2Channel     =NULL_CHANNEL;
	mEngineChannel    =NULL_CHANNEL;
	mShutdownChannel  =NULL_CHANNEL;
	mActivationChannel=NULL_CHANNEL;

	mEngineAnimationState     = NULL;
	mHelloAnimationState      = NULL;
	mWork1AnimationState      = NULL;
	mWork2AnimationState      = NULL;
	mShutdownAnimationState   = NULL;
	mActivationAnimationState = NULL;
	mAnimationState           = NULL;
	mAnimationSpeed           = 1;

	// Creation de l'entité
	// ---------------------------------------------
	mRobotEntity = SceneMgr->createEntity(Name, mMeshFile);
	mRobotNode   = ParentNode->createChildSceneNode(Name);
	mRobotEntity->setUserObject(this);
	mRobotEntity->setCastShadows(true);
	mRobotEntity->setQueryFlags(ACTIVABLE);

	this->setActionString("awaiting orders.");
	TRACE("<< Robot");
}

/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Robot::~Robot()
{
	TRACE(">> "+mRobotName+"::destructed");
	// Arreter le moteur
	setEngineOff();
	// Arreter les particules et les sons associés
	setBroken(false);
	// Arreter la gestion
	mRoot->removeFrameListener(this);

	// Detacher les particules et l'Entity
	mRobotNode->detachAllObjects();
	
	// TODO : faut-t-il destroyer l'Entity???

}

/* ------------------------------------------------------------------------- */
/// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long Robot::getTypeID (void) const
{
	return ROBOT_ID;
}

/* ------------------------------------------------------------------------- */
/// Ajoute le robot à la scene courante
/* ------------------------------------------------------------------------- */
void Robot::addToScene()
{
	// On charge la position initiale du robot, et son état.
	this->loadObject();
	mRobotNode->attachObject(mRobotEntity);
	mRoot->addFrameListener(this);
}

/* ------------------------------------------------------------------------- */
/// Retire le robot de la scene courante
/* ------------------------------------------------------------------------- */
void Robot::removeFromScene()
{
	TRACE(">> "+mRobotName+"::removedFromScene()");
	mRoot->removeFrameListener(this);
	mRobotNode->detachObject(mRobotEntity);
	if (mSmoke)  mSourceNode->detachObject(mSmoke);
	if (mSparks) mSourceNode->detachObject(mSparks);
}

/* ------------------------------------------------------------------------- */
/// Renvoie le Nom du robot
/* ------------------------------------------------------------------------- */
String Robot::getName()
{
	return (mRobotName);
}

/* ------------------------------------------------------------------------- */
/// Renvoie la position du robot (position relative a son ParentNode)
/* ------------------------------------------------------------------------- */
Vector3 Robot::getPosition()
{
	return (mRobotNode->getPosition());
}

/* ------------------------------------------------------------------------- */
/// Definit la position du robot  (X=Gauche - Y=Haut - Z=devant)
/* ------------------------------------------------------------------------- */
void Robot::setPosition(Vector3 Position)
{
	TRACE(">> "+mRobotName+"::setPosition: ("+StringConverter::toString(Position)+")");
	// On positionne le Node du robot (par rapport à son parent)
	mRobotNode->setPosition(Position);
	// On ajuste sa position par rapport au sol.
	adjustAltitude();
	TRACE("   New Robot Position = "+StringConverter::toString(mRobotNode->getPosition()));
	TRACE("<< Robot::setPosition");
}

/* ------------------------------------------------------------------------- */
/// Definit l'orientation du robot  (en degres)
/* ------------------------------------------------------------------------- */
void Robot::setOrientation(int Orientation)
{
	mRobotNode->yaw(Degree(Orientation));
}

/* ------------------------------------------------------------------------- */
/// Positionne le Robot en Y à sa bonne altitude par rapport au sol le plus proche.
/* ------------------------------------------------------------------------- */
void Robot::adjustAltitude()
{
	// On ajuste la position en Y du Robot par rapport au Ship
	Vector3 PosRobot = mRobotNode->getPosition();
	Real PosG = TheShip::getSingletonPtr()->detectFloorAltitude(PosRobot);

	TRACE(">> "+mRobotName+"::adjustAltitude: Closest Floor has Y=" + ITOA(PosG));
	PosRobot.y = PosG + mNormalAltitude;

	mRobotNode->setPosition(PosRobot);
	TRACE("   New Robot Position = "+StringConverter::toString(PosRobot));
	TRACE("<< adjustAltitude");
}


/* ------------------------------------------------------------------------- */
/// Met en route le son du moteur du Robot
/* ------------------------------------------------------------------------- */
void Robot::setEngineOn()
{
	if (mEngineOn) return;	// Si le moteur tourne dejà, on sort.
	if (mBroken)   return;	// Si le moteur est cassé, on sort.

	this->adjustAltitude();
	this->playSound(ENGINE);
	if (mEngineAnimation != "")
	{
		mEngineAnimationState= mRobotEntity->getAnimationState(mEngineAnimation);
		mEngineAnimationState->setLoop(true);
		mEngineAnimationState->setEnabled(true);
	}
	mEngineOn=true;
}


/* ------------------------------------------------------------------------- */
/// Coupe le son du moteur du Robot
/* ------------------------------------------------------------------------- */
void Robot::setEngineOff()
{
	TRACE(">> "+mRobotName+"::setEngineOff()");
	if (mEngineOn == true)
	{
		// On arrete le son du moteur
		SoundManager::getSingletonPtr()->freeSound(mEngineChannel);
		// On arrete l'animation
		mEngineAnimationState=NULL;
	}
	mEngineOn = false;
	mMoving   = false;
}


/* ------------------------------------------------------------------------- */
/// Le robot est endommagé (fumée et buitage)
/* ------------------------------------------------------------------------- */
void Robot::setBroken(bool broken)
{
	TRACE(">> "+mRobotName+"::setBroken("+StringConverter::toString(broken)+")");

	mBroken = broken;
	
	// S'il est cassé et allumé: cela fait de la fumée
	if (broken && !mDown)
	{
		// S'il n'existe pas, on crée le Node source pour les particules
		// (note: on peut aussi le creer dans le constructeur)
		if (!mSourceNode)  
		{
			mSourceNode = mRobotNode->createChildSceneNode();
			mSourceNode->setPosition(0,5,0);
		}
		// S'ils n'existent pas, on crée (et attache) les particules
		if (!mSmoke)  
		{
			// (le script est dans smoke.material et smoke.particle)
			mSmoke = ParticleSystemManager::getSingleton().createSystem(mRobotName+"Smoke", "Effect/Smoke");
			mSmoke->setQueryFlags(VOIDOBJECT);
			mSourceNode->attachObject(mSmoke);
		}
		if (!mSparks)
		{
			// (le script est dans smoke.material et smoke.particle)
			mSparks = ParticleSystemManager::getSingleton().createSystem(mRobotName+"Sparks", "Effect/Sparks"); 
			mSparks->setQueryFlags(VOIDOBJECT);
			mSourceNode->attachObject(mSparks);
		}
		// Active l'émission des particules
		TRACE("   Switch the particles ON");
		if (mSmoke)   mSmoke->getEmitter(0)->setEnabled(true);
		if (mSparks)  mSparks->getEmitter(0)->setEnabled(true);
		// Active le son des étincelles 
		this->playSound(SPARKS);
	}

	// S'il est réparé ou éteint: cela fait disparaitre la fumée
	if (!broken || mDown)
	{
		TRACE("   Switch the particles OFF");
		// On tarit l'emetteur, il sera détaché lors du desructeur ou du removeFromScene
		if (mSmoke)   mSmoke->getEmitter(0)->setEnabled(false);
		if (mSparks)  mSparks->getEmitter(0)->setEnabled(false);
		// Désactive le son des étincelles 
		if(mSparkChannel!=NULL_CHANNEL) SoundManager::getSingletonPtr()->freeSound(mSparkChannel);
	}
}


/* ------------------------------------------------------------------------- */
/// Le robot travaille. 
/* ------------------------------------------------------------------------- */
void Robot::work(bool Working)
{
	TRACE(">> "+mRobotName+"::work("+StringConverter::toString(Working)+")");
	if (Working)
	{
		if (mWork1Animation !="")
		{
			mWork1AnimationState = mRobotEntity->getAnimationState(mWork1Animation);
			mWork1AnimationState->setLoop(false);
			mWork1AnimationState->setTimePosition(0.0);
			mWork1AnimationState->setEnabled(true);
		}
	}
	mWorking = Working;
}



/* ------------------------------------------------------------------------- */
/// On active le focus au moment ou la souris passe sur l'objet
/* ------------------------------------------------------------------------- */
void Robot::setFocusOn()
{
	if (!mHasFocus)
	{
		TRACE(">> "+mRobotName+"::setFocusOn()");
		/// S'il n'a pas deja le focus: on émet un son.
		this->playSound(FOCUS);
		/// On affiche un message
		Clock::getSingletonPtr()->showMessageOverlay("Activate Robot");
		/// On lance l'animation Hello
		if (mHelloAnimation != "")
		{
			mAnimationSpeed = 1.0;
			mHelloAnimationState = mRobotEntity->getAnimationState(mHelloAnimation);
			mHelloAnimationState->setLoop(false);
			mHelloAnimationState->setEnabled(true);
		}		
	}
	mHasFocus = true;
}


/* ------------------------------------------------------------------------- */
/// Quand on perd le focus, on détruit le GUI d'interface Robot
/* ------------------------------------------------------------------------- */
void Robot::setFocusOff()
{
	TRACE(">> "+mRobotName+"::setFocusOff()");
	mHasFocus = false;
	/// On libère le channel Audio
//	SFX_freeSound(mHelloChannel);
//	SFX_freeSound(mWork2Channel);
	if (mHelloAnimation != "")
	{
		mAnimationSpeed = -1.0;
		mHelloAnimationState = mRobotEntity->getAnimationState(mHelloAnimation);
		mHelloAnimationState->setLoop(false);
		mHelloAnimationState->setEnabled(true);
	}
	/// On efface l'interface
	removeInterface();
}


/* ------------------------------------------------------------------------- */
/// Détruit un objet d'Interface Robot (GUI)
/* ------------------------------------------------------------------------- */
void Robot::removeInterface() 
{
		if (mRobot_GUI)  delete mRobot_GUI;
		mRobot_GUI = NULL;
}



/* ------------------------------------------------------------------------- */
/// Crée un objet d'Interface Robot (GUI)
/* ------------------------------------------------------------------------- */
void Robot::createInterface() 
{
//	mRobotNode->showBoundingBox(On);

	// Si une demande de sortie de la GUI est en cours
	// On n'en cree pas une nouvelle.
	if (!mExitGui)
	{
		mExitGui   = false;
		mRobot_GUI = new Robot_Interface(this, mInterfaceFile);
		mRobot_GUI->createInterface();
	}
}


/* ------------------------------------------------------------------------- */
/// L'interface est masquée mais pas détruite
/* ------------------------------------------------------------------------- */
void Robot::hideInterface() 
{
	if (mRobot_GUI)  
	{
		mRobot_GUI->hideMouse();
		mRobot_GUI->hideInterface();
	}
}


/* ------------------------------------------------------------------------- */
/// Mémorise une demande de fin de la GUI
/* ------------------------------------------------------------------------- */
void Robot::requestExitInterface(Real seconds)
{
	mExitGui = true;
	mExitGuiDelay=seconds;
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si une demande de sortie de GUI est en cours.
/* ------------------------------------------------------------------------- */
bool Robot::isExitInterfaceRequiered()
{
	/// Renvoie FALSE si aucune demande de sortie n'est en cours.
	return mExitGui;
}


/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le GUI est inactif  (cad non affiché)
/* ------------------------------------------------------------------------- */
bool Robot::isInterfaceDisabled()
{
	return (mRobot_GUI==NULL);
}


/* ------------------------------------------------------------------------- */
/// Mise a jour de la position des sons 3D (moteur, etc)
/* ------------------------------------------------------------------------- */
void Robot::updateSoundSource(int Channel)
{
	SoundManager::getSingletonPtr()->updateSoundSource(Channel,mRobotNode->getPosition());
}


/* ------------------------------------------------------------------------- */
/// Memorise le point de destination du Robot, et declenche le mouvement.
/* ------------------------------------------------------------------------- */
void Robot::gotoPoint(Vector3 Destination)
{
	// Si le robot est cassé, il ne bouge pas;
	if (mBroken) return;

	mDestination   = Destination;
	mDestination.y = mRobotNode->getPosition().y;
	mTurning       = true;
	mMoving        = true;
	setEngineOn();
}


/* ------------------------------------------------------------------------- */
/// Memorise le point de destination du Robot, et declenche le mouvement.
/* ------------------------------------------------------------------------- */
void Robot::gotoNode(String NodeName)
{
	String debug=mRobotName+": gotoNode("+NodeName+")\n";
	_cprintf(debug.c_str());

	// Si le robot est cassé, il ne bouge pas;
	if (mBroken) return;

	// On memorise la destination finale
	mLastWaypoint    = mSceneMgr->getSceneNode(NodeName);
	// On determine le WP le plus proche
	mCurrentWaypoint = TheShip::getSingleton().getClosestWaypoint(getPosition());

	debug=mRobotName+": joining the closest Waypoint: "+mCurrentWaypoint->getName()+"\n";
	_cprintf(debug.c_str());

	// On rejoint d'abord le WP le plus proche.
	mNextWaypoint = mCurrentWaypoint;
	adjustAltitude();
	
	if (mNextWaypoint)
	{
		mDestination   = mNextWaypoint->getPosition();
		mDestination.y = mRobotNode->getPosition().y;
		mTurning       = true;
		mMoving        = true;
		setEngineOn();
	}
}


/* ------------------------------------------------------------------------- */
/// Le Robot continue un trajet initié avec gotoNode().
/* ------------------------------------------------------------------------- */
void Robot::gotoNextWaypoint()
{
	String debug=mRobotName+": gotoNextWaypoint()\n";
	_cprintf(debug.c_str());

	mCurrentWaypoint = TheShip::getSingleton().getClosestWaypoint(this->getPosition());
	mNextWaypoint    = TheShip::getSingleton().findNextWaypointTo(mCurrentWaypoint, mLastWaypoint);
	
	if (mNextWaypoint)
	{
		mDestination   = mNextWaypoint->getPosition();
		mDestination.y = mRobotNode->getPosition().y;
		mTurning       = true;
		mMoving        = true;
	}
}

/* ------------------------------------------------------------------------- */
/// Effectue un mouvement en avant (progressif)
/* ------------------------------------------------------------------------- */
void Robot::move(const FrameEvent& evt)
{
	/// TODO: on a un bug si Angle = 180°: le robot, part tout droit au lieu de faire demi-tour...
	String debug;
	// On determine l'angle chaque fois, 
	// ce qui permet des ajustement en cours de trajet.
	Radian Angle = findAngle();

	// ---------------------------------------------------
	// Si on est presque aligné dans la bonne direction...
	// ---------------------------------------------------
	if ( Math::Abs(Angle) < Radian(0.05) )
	{
		if (Angle != Radian(0.0))
		{
			// ------------------------------------------------------
			// On est presque bien positionné: on fait la correction 
			// ------------------------------------------------------
			debug = "Erreur Angle de "+ITOA(Angle)+"=> Le robot se corrige \n";	
			_cprintf(debug.c_str());	// écrit dans la console
			mRobotNode->rotate(Vector3::UNIT_Y, Angle);
		}
		// ----------------------------------------------
		// On est parfaitement bien positionné: on avance
		// ----------------------------------------------
		mTurning = false;
		Real Distance = (mDestination - mRobotNode->getPosition()).squaredLength();
		//debug = "Squared Distance= "+ITOA(int(Distance))+"\n";
		//_cprintf(debug.c_str());	// écrit dans la console
		// Vector3 Vitesse = Vector3(0, 0, 10*evt.timeSinceLastFrame);	// vitesse 1m/sec
		Vector3 Vitesse = mSpeed*evt.timeSinceLastFrame*Vector3::UNIT_Z;	// vitesse 1m/sec
		// ---------------------------
		// On est arrivé à un waypoint
		// ---------------------------
		if (Distance <9)	// cad 3 units = 30 cm (c'est une squared distance)
		{
			if (mNextWaypoint==mLastWaypoint)
			{
				// -----------------------------------
				// Si on vient d'atteindre le WP final
				// -----------------------------------
				String debug=mRobotName+": reached the Last Waypoint\n";
				_cprintf(debug.c_str());
				// on s'arrete, et on coupe le moteur
				this->setEngineOff();
			}
			else
			{
				// -------------------------------------------
				// Si on vient d'atteindre un WP intermédiaire
				// -------------------------------------------
				String debug=mRobotName+": Waypoint reached\n";
				_cprintf(debug.c_str());
				// On déclenche les actions associées à ce Waypoint (On fait faire par le Ship)
				if (TheShip::getSingleton().doClosestWaypointAction(this->getPosition())) 
				{
					// Si l'action s'est bien déroulée, on continue
					// Il reste du chemin a faire
					this->gotoNextWaypoint();
				}
				else
				{
					// -------------------------------------------
					// L'action du WP a échouée, on s'arrete ici (cad porte vérouillée)
					// -------------------------------------------
					this->setEngineOff();
					this->setActionString("cannot complete. Orders cancelled.");
				}
			}
		}
		else
			// ---------------------------------
			// On n'est pas arrivé et on avance.
			// ---------------------------------
			mRobotNode->translate(Vitesse, Ogre::Node::TransformSpace::TS_LOCAL);
	}
	else
	{
		// -------------------------------------------------------
		// On est mal positionné: on pivote dans le meilleur sens
		// -------------------------------------------------------
		// vitesse angulaire =  1rad/sec = 1tour en 6 sec
		Radian vitangul = Math::Sign(Angle)*evt.timeSinceLastFrame;
		debug = "Erreur Angle de "+ITOA(Angle)+"=> Le robot pivote de "+ITOA(vitangul)+"\n";
		_cprintf(debug.c_str());
		mRobotNode->rotate(Vector3::UNIT_Y, vitangul);
	}
}

/* ------------------------------------------------------------------------- */
/// Effectue un mouvement vertical (progressif)
/* ------------------------------------------------------------------------- */
void Robot::moveVertical(const FrameEvent& evt)
{
	Real deltaY = evt.timeSinceLastFrame * mSpeed;
	Vector3 Pos =mRobotNode->getPosition();
	
	// Si on est trop haut, on descend
	if (Pos.y > mTargetAltitude+0.1)      Pos.y-=deltaY;
	// Si on est trop bas, on monte
	else if (Pos.y < mTargetAltitude-0.1) Pos.y+=deltaY;
	// On est arrivé à la bonne altitude (+/- 0.1)
	else 
	{
		mMovingUpDown = false;
	}

	// On deplace le joueur.
	mRobotNode->setPosition(Pos);
}

	
/* ------------------------------------------------------------------------- */
/// Determine l'angle vers la destination
/* ------------------------------------------------------------------------- */
Radian Robot::findAngle()
{
	// On determine l'angle entre l'axe avant (axe Z) du Robot
	// et la direction du point de destination

	// On trouve l'axe avant du robot
	Quaternion AxesRobot = mRobotNode->getOrientation();
	Vector3    AxeZRobot = AxesRobot.zAxis();
	
	// On trouve la direction de la destination
	Vector3    PosR      = mRobotNode->getPosition();
	Vector3    Direction = mDestination - PosR;

	// On determine l'angle entre ces deux axes
	Quaternion Q = AxeZRobot.getRotationTo(Direction);

	// Le quaternion est composé d'un vecteur3 (axe pivot) et d'un angle (en radians).
	Radian     Angle;		
	Vector3    AxePivot;
	Q.ToAngleAxis(Angle,AxePivot);
	// if (Angle != Radian(0)) DEBUG_LOG("Axe Pivot= "+ITOA(AxePivot.x)+" "+ITOA(AxePivot.y)+" "+ITOA(AxePivot.z));
	if (AxePivot.y < 0) Angle= -Angle;
	return (Angle);
}




/* ------------------------------------------------------------------------- */
/// Desactivation du Robot: Il s'éteint et tombe sur le sol.
/**
Si le robot est endommagé, cela arrète aussi les particules de fumée.
@param quick Si TRUE, l'action est faite sans faire de bruit.
*/
/* ------------------------------------------------------------------------- */
void Robot::deactivateRobot(bool quick)
{
	TRACE(">> "+mRobotName+"::deactivateRobot(quick="+StringConverter::toString(quick)+")");

	// S'il n'est pas déjà mDown, On éteint le Robot
	if (!mDown)
	{
		// On memorise le nouvel état du robot
		mDown=true;
		this->setEngineOff();
		this->work(false);
		// On émet un son (sauf si en mode Quick)
		if (!quick) 
		{
			this->playSound(SHUTDOWN);
		};
		// On arrete l'animation de floating
		if (mEngineAnimationState) mEngineAnimationState->setEnabled(false);
		// On se positionne au niveau du sol
		mTargetAltitude = TheShip::getSingletonPtr()->detectFloorAltitude(mRobotNode->getPosition()) + 2;
		// On lance l'action de descendre 
		mMovingUpDown = true;

		// Si il est cassé, cela éteint les particules.
		if (mBroken) 
		{
			TRACE("   Switch the particles OFF");
			// On tarit l'emetteur, il sera détaché lors du desructeur ou du removeFromScene
			if (mSmoke)   mSmoke->getEmitter(0)->setEnabled(false);
			if (mSparks)  mSparks->getEmitter(0)->setEnabled(false);
			// Désactive le son des étincelles 
			if (mSparkChannel!=NULL_CHANNEL) SoundManager::getSingletonPtr()->freeSound(mSparkChannel);
		}

	}

}


/* ------------------------------------------------------------------------- */
/// On active le Robot.
/**
Si le robot est endommagé, cela active aussi les particules de fumée.
@param quick Si TRUE, l'action est faite sans faire de bruit.
*/
/* ------------------------------------------------------------------------- */
void Robot::activateRobot(bool quick)
{
	TRACE(">> "+mRobotName+"::activateRobot(quick="+StringConverter::toString(quick)+")");

	// Si le Robot est inactif, on le réveille
	if (mDown)
	{
		// On memorise le nouvel état du robot
		mDown=false;
		// On émet un son
		if (!quick) 
		{
			this->playSound(ACTIVATION);
		}
		// On se remet au bon niveau
		mTargetAltitude = TheShip::getSingletonPtr()->detectFloorAltitude(mRobotNode->getPosition()) + mNormalAltitude;
		// On lance l'action de monter
		mMovingUpDown = true;
		// Si il est cassé, cela active les particules.
		if (mBroken) this->setBroken(true);
	}
	// On charge l'animation de floating
	if (mEngineAnimation != "")
	{
		mEngineAnimationState= mRobotEntity->getAnimationState(mEngineAnimation);
		mEngineAnimationState->setLoop(true);
		mEngineAnimationState->setEnabled(true);
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie si le robot est activé ou non (shutdown).
/* ------------------------------------------------------------------------- */
bool Robot::isDown()
{
	return mDown;
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le robot est endommagé.
/* ------------------------------------------------------------------------- */
bool Robot::isBroken()
{
	return mBroken;
}


/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le robot est visible.
/* ------------------------------------------------------------------------- */
bool Robot::isVisible()
{
	return mRobotEntity->isVisible();
}


/* ------------------------------------------------------------------------- */
/// Gère les mouvements du robot (déplacement, vibration, etc)
/* ------------------------------------------------------------------------- */
bool Robot::frameStarted (const FrameEvent& evt)
{
	if (mEngineOn)     this->updateSoundSource(mEngineChannel);
	if (mBroken)       this->updateSoundSource(mSparkChannel);
//	if (mTurning)      this->rotate(evt);
	if (mMoving)       this->move(evt);
//	if (mWorking)      this->vibrate(evt);
	if (mMovingUpDown) this->moveVertical(evt);

	if (mExitGui)           {mExitGuiDelay-=evt.timeSinceLastFrame;}
	if (mExitGuiDelay<0)	{this->setFocusOff(); mExitGui=false; mExitGuiDelay=5;}


	// On joue les animations
	/// TODO : peut-il y avoir plusieurs animations à la fois ?
	if (mHelloAnimationState)
	{
		// DEBUG_LOG("Length = "+ITOA(mHelloAnimationState->getTimePosition())+" sur "+ITOA(mHelloAnimationState->getLength()));
		mHelloAnimationState->addTime(evt.timeSinceLastFrame * mAnimationSpeed);
		if (mHelloAnimationState->getTimePosition() >= mHelloAnimationState->getLength()) mHelloAnimationState=NULL;
		else if (mHelloAnimationState->getTimePosition() <  0 ) mHelloAnimationState=NULL;
	}
	if (mActivationAnimationState)
	{
		mActivationAnimationState->addTime(evt.timeSinceLastFrame * mAnimationSpeed);
		if (mActivationAnimationState->getTimePosition() >= mActivationAnimationState->getLength()) mActivationAnimationState=NULL;
		else if (mActivationAnimationState->getTimePosition() <0 ) mActivationAnimationState=NULL;
	}
	if (!mBroken && !mMoving && !mWorking && !mHasFocus && !mDown)
	{
		// Le floating est une animation séparée. Elle est démarrée quand le robot est activé.
		if (mEngineAnimationState) mEngineAnimationState->addTime(evt.timeSinceLastFrame);
	}
	if (mWork1AnimationState)      
	{
		mWork1AnimationState->addTime(evt.timeSinceLastFrame);
		if (mWork1AnimationState->getTimePosition() >= mWork1AnimationState->getLength()) mWork1AnimationState=NULL;
	}
	if (mWork2AnimationState)
	{
		mWork2AnimationState->addTime(evt.timeSinceLastFrame);
		if (mWork2AnimationState->getTimePosition() >= mWork2AnimationState->getLength()) mWork2AnimationState=NULL;
	}

	if (mAnimationSpeed == 2)	// Pour le debug
	{
		mAnimationState->addTime(evt.timeSinceLastFrame);
		if (mAnimationState->getTimePosition() >= mAnimationState->getLength()) mAnimationSpeed = 0;
	}


	return true;
}



/* ------------------------------------------------------------------------- */
/// Joue les sons du robot
/* ------------------------------------------------------------------------- */
void Robot::playSound(SoundList soundtype)
{
	// Loads and decodes a static soundfile into memory. 
	switch (soundtype)
	{
	case ENGINE:
		// Initialisation du son du moteur (loopmode = yes)
		mEngineChannel = SoundManager::getSingletonPtr()->playSoundMoving(mEngineSoundFile);
		this->updateSoundSource(mEngineChannel);
		break;
	case SPARKS:
		mSparkChannel = SoundManager::getSingletonPtr()->playSoundMoving(mSparkSoundFile);
		this->updateSoundSource(mSparkChannel);
		DEBUG_LOG(">> playSound("+mSparkSoundFile+") in channel "+ITOA(mSparkChannel));
		break;
	case FOCUS:
		mHelloChannel = SoundManager::getSingletonPtr()->playSoundObject(mHelloSoundFile);
		this->updateSoundSource(mHelloChannel);
		break;
	case ANALYSE:
		mWork2Channel = SoundManager::getSingletonPtr()->playSoundObject(mWork2SoundFile);
		this->updateSoundSource(mWork2Channel);
		DEBUG_LOG(">> playSound("+mWork2SoundFile+") in channel "+ITOA(mWork2Channel));
		break;
	case SHUTDOWN:
		mShutdownChannel = SoundManager::getSingletonPtr()->playSoundObject(mShutdownSoundFile);
		this->updateSoundSource(mShutdownChannel);
		break;
	case ACTIVATION:
		mActivationChannel = SoundManager::getSingletonPtr()->playSoundObject(mActivationSoundFile);
		this->updateSoundSource(mActivationChannel);
		DEBUG_LOG(">> playSound("+mActivationSoundFile+") in channel "+ITOA(mActivationChannel));
		break;
	}
}


/* -------------------------------------------------------------------------- */
/// Le Node du Robot est rattaché à un nouveau ParentNode
/* -------------------------------------------------------------------------- */
void Robot::attachToNode(SceneNode* ParentNode)
{
	if (!ParentNode) 
	{
		// Si on passe NULL, on le rattache au parent existant
		mParentNode->addChild(mRobotNode);
	}
	else
	{
		TRACE(">> "+mRobotName+"::attachToNode("+ParentNode->getName()+")");
		// On a passé un parent en parametre
		if (ParentNode != mParentNode)
		{
			// Si le nouveau parent est différent de l'ancien, 
			// On rattache le Robot au nouveau parent
			mParentNode->removeChild(mRobotNode);
			ParentNode->addChild(mRobotNode);
		}
		// On mémorise ce nouveau parent
		mParentNode = ParentNode;
	}
}

/* -------------------------------------------------------------------------- */
/// Change la Description de l'action en cours du robot.
/* -------------------------------------------------------------------------- */
void Robot::setActionString(String ActionString)
{
	mActionString = mRobotName + ": " + ActionString;
	DEBUG_LOG(mActionString);
}

/* -------------------------------------------------------------------------- */
/// Renvoie la description de l'action en cours du robot.
/* -------------------------------------------------------------------------- */
String Robot::getActionString()
{
	return(mActionString);
}


/* ------------------------------------------------------------------------- */
/// Commence a jouer l'animation demandée
/* ------------------------------------------------------------------------- */
void Robot::startAnimation(String AnimationName, bool Loop)
{
	if (mAnimationState) mAnimationState->setEnabled(false);
	if (AnimationName != "") 
	{
		mAnimationState = mRobotEntity->getAnimationState(AnimationName);
		mAnimationState->setTimePosition(0.0);
		mAnimationState->setLoop(Loop);
		mAnimationState->setEnabled(true);
	}
}



/* ------------------------------------------------------------------------- */
/// Retrouve une SubEntity du Robot par son nom de Materiel
/* ------------------------------------------------------------------------- */
SubEntity* Robot::getSubEntity(String MaterialName)
{
	SubEntity* RequestedEntity = NULL;

	int NumSubEntities = mRobotEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mRobotEntity->getSubEntity(i)->getMaterialName();
		if (MatName == MaterialName)   RequestedEntity = mRobotEntity->getSubEntity(i);
	}
	return RequestedEntity;
}



/* ------------------------------------------------------------------------- */
/// Charge les parametres initiaux du Robot
/* ------------------------------------------------------------------------- */
void Robot::loadDescription (String DescFile)
{
    String filename = "../media/objects/"+DescFile;
	TRACE(">> loadDescription("+filename+")");

	try
	{
		/// Lecture des parametres du fichier de config
	    ConfigFile DescriptionFile;
	    DescriptionFile.load(filename);
		mMeshFile           =DescriptionFile.getSetting("MeshFile");
		mInterfaceFile      =DescriptionFile.getSetting("InterfaceFile");
		mStatusFile         =DescriptionFile.getSetting("StatusFile");

		mEngineSoundFile    =DescriptionFile.getSetting("EngineSoundFile");
		mSparkSoundFile     =DescriptionFile.getSetting("BrokenSoundFile");
		mHelloSoundFile     =DescriptionFile.getSetting("HelloSoundFile");
		mWork1SoundFile     =DescriptionFile.getSetting("Work1SoundFile");
		mWork2SoundFile     =DescriptionFile.getSetting("Work2SoundFile");
		mShutdownSoundFile  =DescriptionFile.getSetting("ShutdownSoundFile");
		mActivationSoundFile=DescriptionFile.getSetting("ActivationSoundFile");

		mEngineAnimation    =DescriptionFile.getSetting("EngineAnimation");
		mHelloAnimation     =DescriptionFile.getSetting("HelloAnimation");
		mWork1Animation     =DescriptionFile.getSetting("Work1Animation");
		mWork2Animation     =DescriptionFile.getSetting("Work2Animation");
		mActivationAnimation=DescriptionFile.getSetting("ActivationAnimation");

		mNormalAltitude     =StringConverter::parseInt(DescriptionFile.getSetting("Altitude"));
		mSpeed              =StringConverter::parseInt(DescriptionFile.getSetting("Speed"));
	}
	catch (...) 
	{
		TRACE("WARNING: Robot config file not found");
	}
}


/* ------------------------------------------------------------------------- */
/// Charge l'etat initial du Robot (position, état, etc).
/* ------------------------------------------------------------------------- */
void Robot::loadObject()
{
	TRACE(">> loadObject: "+mStatusFile);

	/// Lecture des parametres du fichier de status
	XMLParser_T* StatusFile = new XMLParser_T(mStatusFile);

	StatusFile->gotoElementPath("robot/status");
	// Note: Il est préférable de  positionner d'abord le Down, puis le Broken, de manière à ne pas 
	// avoir une demi-seconde de fumée entre les deux...
	if (StatusFile->getBooleanAttribute("active")) this->activateRobot(true);
	else                                           this->deactivateRobot(true);
	this->setBroken(StatusFile->getBooleanAttribute("broken"));


	StatusFile->gotoElementPath("robot/position");
	int Level = 50 + LEVEL_HIGH * StatusFile->getIntAttribute("level");
	Real x = StatusFile->getRealAttribute("x");
	Real y = StatusFile->getRealAttribute("y");
	Real z = StatusFile->getRealAttribute("z");
	Real w = StatusFile->getRealAttribute("orientation");
	Vector3 Pos = Vector3(x,y+Level,z);
	this->setPosition(Pos);
	this->setOrientation(w);

	delete (StatusFile);
}

