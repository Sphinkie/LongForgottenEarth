/* -------------------------------------------------------------------------- */
// File       : Player.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* -------------------------------------------------------------------------- */


#include "Player.h"
#include "classes.h"
#include "macros.h"
#include "TheShip.h"
#include "XMLparser_T.h"
#include "mainApplication.h"

#include "Clock.h" // debug

#include <fmod_errors.h>
#include <OgreOpcode.h>

#define FREEMOVE  300.0             ///< Pas de souci de mouvement si cette distance est sans obstacle
#define CAMRADIUS 6.0               ///< Grosseur du player (pour les collisons) : 80 cm 
#define BELLY_POS Vector3(0,-16,0)  ///< Position du ventre du joueur (pour les collisions)
/* -------------------------------------------------------------------------- */
/// Constructeur. 
/** 
On crée le Node de la camera et on lui attache la Camera.
On récupère aussi les données personnelles du joueur (nom, ville) dans le fichier de config.
*/
/* -------------------------------------------------------------------------- */
Player::Player(Camera* cam, SceneNode* ParentNode)
{
	mCamera = cam;
	mLie    = 1;
	mRun    = 1;
	mFloat  = false;
	
	mTargetAltitude   = 0;
	mTargetLevel      = 0;
	mInitialRoom      = "";

	mStepSoundChannel = NULL_CHANNEL;
	mStepSound        = NULL;
	
	mHeadPosition     = Vector3::ZERO;
	mSavedPosition    = mHeadPosition;

	mParentNode = ParentNode;
	mCameraNode = ParentNode->createChildSceneNode("CameraNode");
	mCameraNode->attachObject(mCamera);
	mCameraNode->setPosition(Vector3::ZERO);

	mCamera->setPosition(mHeadPosition);
	mCamera->lookAt(mHeadPosition + Vector3::UNIT_Z);	// regarde devant

	/// Lecture des parametres du fichier de config
    ConfigFile playerIDFile;
    playerIDFile.load("../savedgames/playerID.cfg");
	mName = playerIDFile.getSetting("name");
	mCity = playerIDFile.getSetting("city");

	mCollideObject= NULL;
	mEntity = NULL;
//	mEntity = mainApplication::getSingletonPtr()->getSceneMgr()->createEntity("player", "player.mesh");
//	mEntity->setQueryFlags(VOIDOBJECT);
//	mCameraNode->attachObject(mEntity);
//	// On gère ses collisions.
//	this->initCollisionShape();

	Root::getSingletonPtr()->addFrameListener(this);

	DEBUG_LOG(" Player Entity created");
}


/* -------------------------------------------------------------------------- */
/// Destructeur.
/* -------------------------------------------------------------------------- */
Player::~Player()
{
}


/* ------------------------------------------------------------------------- */
/// Renvoie un pointeur sur le joueur.
/* ------------------------------------------------------------------------- */
template<> Player* Singleton<Player>::ms_Singleton = 0;

Player* Player::getSingletonPtr(void)
{
	return ms_Singleton;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la camera située à l'emplacement du joueur.
/* ------------------------------------------------------------------------- */
Camera* Player::getCamera()
{  
	return (mCamera);  
}


/* -------------------------------------------------------------------------- */
/// Le FrameListener du Player gère les déplacements du joueur.
/* -------------------------------------------------------------------------- */
bool Player::frameStarted(const FrameEvent& evt)
{
	// Si il faut changer de niveau, on le fait doucement
	if (mFloat) this->translateY(evt.timeSinceLastFrame*30); // 30 wu = 3m par seconde

	return true;
}


/* -------------------------------------------------------------------------- */
/// On fait bouger la tête (cad la Camera).
/* -------------------------------------------------------------------------- */
void Player::moveHead(float mRotX, float mRotY)
{
	// On oriente la caméra.sur le plan horizontal (axe de déplacement)
    mCamera->yaw(Radian(mRotX));
	// On oriente la caméra.sur le plan vertical 
	mCamera->pitch(Radian(mRotY));

	// Si la camera regarde trop haut ou trop bas, on corrige
	Vector3 camVect = mCamera->getDirection();
	Vector3 maxVect = Vector3( 1, 0.72, 1); // Le regard le plus haut possible = 45°
	Vector3 minVect = Vector3(-1,-0.62,-1);	// Le regard le plus bas possible  = 50°
	camVect.makeFloor(maxVect);
	camVect.makeCeil(minVect);
	mCamera->setDirection(camVect);
}


/* -------------------------------------------------------------------------- */
/// On fait avancer le joueur (cad le CameraNode).
/* -------------------------------------------------------------------------- */
void Player::moveBody(Vector3 Translation)
{
	if (Translation==Vector3::ZERO) return;

	// On applique l'acceleration eventuelle du RUN mode
	Vector3 TranslateVector = Translation * mRun * mLie;	

	// mCamera->moveRelative(mTranslateVector); // Methode classique

	// On applique la translation sur l'axe d'orientation de la camera
	TranslateVector = mCamera->getOrientation()*TranslateVector; 
	// On force les mouvements a etre horizontaux (pour éviter les oscillations)
	TranslateVector.y=0;   
	// On fait avancer la caméra (desactivé: MEP des collisons)
	// mCameraNode->translate(TranslateVector); 
	
	
// ----------------------------------------
// Avec gestion des collisions
// ----------------------------------------
	Vector3 camPosition = mCameraNode->getPosition()+BELLY_POS;
//	Vector3 camPosition = mCamera->getDerivedPosition();
	Vector3 amount2Move(TranslateVector);

      // Yep, pressing V will also turn off crappy camera collision handling ..
//	if(!mVisualizeObjects)
	amount2Move = CheckCollision(camPosition, CAMRADIUS, TranslateVector);

//	mCamera->yaw(mRotX);
//	mCamera->pitch(mRotY);
//	mCamera->moveRelative(amount2Move);
	amount2Move.y=0; // DDL
	mCameraNode->translate(amount2Move); // DDL
/*
	// Apply some gravity in a second collision detection pass.
	amount2Move = Vector3(0, -0.89f, 0);
	camPosition = mCamera->getDerivedPosition();
	amount2Move = CheckCollision(camPosition, CAMRADIUS, amount2Move);
//	mCamera->moveRelative(amount2Move);
	mCameraNode->translate(TranslateVector); // DDL
// ----------------------------------------
*/
	// On fait monter ou descendre la camera (ajustement de niveau)
//	mCamera->move(mUpDownVector); 
	mRun = 1;

	// On joue le son des pas. (pas en boucle, mais en répetant le son dès qu'il se termine.
	// Ainsi on s'arrete de jouer le son dès que la touche UP est relachée.
	if (!FSOUND_IsPlaying(mStepSoundChannel)) 
//		mStepSoundChannel = FSOUND_PlaySoundEx(FSOUND_FREE, mStepSound , NULL, true);
		mStepSoundChannel = FSOUND_PlaySound(FSOUND_FREE, mStepSound);
//		mStepSoundChannel = SFX_playSound(mStepSoundFile,50,100);
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
Real IntersectRayPlane(Vector3 rOrigin, Vector3 rVector, Vector3 pOrigin, Vector3 pNormal)
{
	Real d     = - pNormal.dotProduct(pOrigin);
	Real numer = pNormal.dotProduct(rOrigin) + d;
	Real denom = pNormal.dotProduct(rVector);

	if (denom == 0)  // normal is orthogonal to vector, cant intersect
		return (-1.0f);
	return -(numer / denom);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void SetLength(Vector3& v, Real l)
{
	Real len = Math::Sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x *= l/len;
	v.y *= l/len;
	v.z *= l/len;

	// On pourrait aussi normaliser et multiplier par L....(DDL)
}

/* -------------------------------------------------------------------------- */
/// test - Peut-être mettre cette fonction hors de toute classe
/* -------------------------------------------------------------------------- */
Vector3 Player::CheckCollision(const Vector3& pos, float radius, const Vector3& vel)
{
	using namespace OgreOpcode;
	static int recursionDepth = 0;         // keeping track of recursions
//	static Vector3 lastknownGood(0,0,0);   // last safe position if everything else fails   // DDL
	static Vector3 lastknownGood(pos);     // last safe position if everything else fails   // DDL

	// Collision results
	CollisionPair** ray_pick_report;
	CollisionPair** pick_report;

	// The position to test against
	Vector3 destinationPoint = pos + vel;
	Vector3 new_vel          = Vector3::ZERO;
//	Vector3 new_vel(0,0,0);
	int     numCamColls      = 0;

	// First, construct a ray along the velocity vector and test for collisions.
	Ray testRay(pos, vel);
	numCamColls = CollisionManager::getSingletonPtr()->GetDefaultContext()->RayCheck(testRay, FREEMOVE, COLLTYPE_CONTACT, COLLTYPE_ALWAYS_CONTACT, ray_pick_report);
	if (numCamColls == 0)
	{
		// This means that we are more than 'FREEMOVE' units away from any potential collisions
		// update last known good position, reset recursion count and return original velocity
//		DEBUG_WIN("free line of move (FREEMOVE)");
		lastknownGood = pos + vel;
		recursionDepth = 0;
		return vel;
	}

	// Okay, we are maybe hitting something here..
	// Measure the distance to the nearest possible collision
	Real distToPlaneIntersection = IntersectRayPlane(ray_pick_report[0]->contact, vel, pos, ray_pick_report[0]->co2_normal);
	// Construct a Vector3 and set its length to that distance
	Vector3 adjVel = vel;
	SetLength(adjVel, distToPlaneIntersection);
	// If we are further away than the nearest possible collision, we have travelled too far
	if(vel.length() > (adjVel.length() + 0.05f))
	{
//		DEBUG_WIN("obstacle at "+ITOA(adjVel.length));
		destinationPoint = pos + adjVel;   // modify the destination point if we are behind it.
	}
	// Now - perform the real collision detection - this time with a sphere.
	Ogre::Sphere cameraSphere = Ogre::Sphere(destinationPoint, radius);
	numCamColls = 0;
	numCamColls = CollisionManager::getSingletonPtr()->GetDefaultContext()->SphereCheck(cameraSphere, COLLTYPE_EXACT, COLLTYPE_ALWAYS_EXACT, pick_report);

	if (numCamColls == 0)
	{
		// Nope. We are not hitting anything. Update last known good, reset recursion counter and return the velocity.
//		DEBUG_WIN("no close obstacle");
		lastknownGood = pos + vel;
		recursionDepth = 0;
		return vel;
	}

	// We are hitting something!
	for(int i = 0; i < numCamColls; ++i)
	{
//		DEBUG_WIN("close obstacle found");
		// Adjust the velocity to prevent the camera from passing through walls, etc.
		Plane collPlane(pick_report[i]->co2_normal, pick_report[i]->contact);
		new_vel += pick_report[i]->co2_normal * ( - pick_report[i]->co2_normal.dotProduct(destinationPoint) - (collPlane.d - radius));
		if(recursionDepth > 50)
		{
//			DEBUG_WIN("stuck to obstacle");
			// Shit. We are stuck. 
			// Update the camera to match the last known good position and return nothing.
			recursionDepth = 0;
//			mCameraNode->setPosition(lastknownGood); // DDL
//			mCamera->setPosition(lastknownGood);     // DDL
			return Vector3(0,0,0);
		}
	}
	// Don't recurse if the velocity vector is small enough
	if (new_vel.length() < 0.001f)
	{
		recursionDepth = 0;
		return new_vel;
	}

	// Recurse
	recursionDepth++;
	return CheckCollision(pos, radius, new_vel);
}

/* -------------------------------------------------------------------------- */
/// On met a jour la position du Listener Audio.
/* -------------------------------------------------------------------------- */
void Player::moveEars()
{
//	Vector3 ListenerPos = mCamera->getPosition(); // migration
	Vector3 ListenerPos = mCameraNode->getPosition();
	FSOUND_3D_Listener_SetAttributes((float*)&ListenerPos,NULL, 0,0,1,  0,1,0);
	FSOUND_Update();
}


/* -------------------------------------------------------------------------- */
/// Le joueur court.
/* -------------------------------------------------------------------------- */
void Player::setRunMode()
{
	mRun = 2.5;
	FSOUND_SetFrequency(mStepSoundChannel,mRunSoundFrequency);
}


/* -------------------------------------------------------------------------- */
/// Le joueur est alongé.
/* -------------------------------------------------------------------------- */
void Player::setLieMode(bool lie)
{
	if (lie) mLie = 0;
	else     mLie = 1;
}


/* -------------------------------------------------------------------------- */
/// Le Node du joueur (mCameraNode) est attaché à un nouveau ParentNode.
/* -------------------------------------------------------------------------- */
void Player::attachToNode(SceneNode* ParentNode)
{

	if (!ParentNode) 
	{
		// Si on passe NULL, on le rattache au parent existant
		mParentNode->addChild(mCameraNode);
	}
	else
	{
		// On a passé un parent en parametre
		if (ParentNode != mParentNode)
		{
			// Si le nouveau parent est différent de l'ancien, 
			// On rattache le body au nouveau parent
			mParentNode->removeChild(mCameraNode);
			ParentNode->addChild(mCameraNode);
		}
		// On mémorise ce nouveau parent
		mParentNode = ParentNode;
	}
	// On verifie que l'entité n'est pas déjà attachée
//	if (!mEntity->isAttached())
//		mCameraNode->attachObject(mEntity);
}


/* -------------------------------------------------------------------------- */
/// Change la position de la camera par rapport au CameraNode. (L'ancienne position est sauvegardée)
/* -------------------------------------------------------------------------- */
void Player::changeHeadPosition(Vector3 NewPosition)
{
	mSavedPosition = mCamera->getPosition();
	mCamera->setPosition(NewPosition);
	mCamera->lookAt(mHeadPosition + Vector3::UNIT_Z);	// regarde devant
}


/* -------------------------------------------------------------------------- */
/// Oriente le regard de la camera par rapport au CameraNode.
/* -------------------------------------------------------------------------- */
void Player::lookAt(Vector3 NewDirection)
{
	mCamera->lookAt(NewDirection);
}


/* -------------------------------------------------------------------------- */
/// Restaure la position de la camera par rapport au CameraNode, après un changePosition.
/* -------------------------------------------------------------------------- */
void Player::restaureHeadPosition()
{
	mCamera->setPosition(mSavedPosition);
}


/* -------------------------------------------------------------------------- */
/// Renvoie le nom du joueur.
/* -------------------------------------------------------------------------- */
String Player::getName()
{
	return mName;
}


/* -------------------------------------------------------------------------- */
/// Renvoie le SceneNode du joueur.
/* -------------------------------------------------------------------------- */
SceneNode* Player::getNode()
{
	return mCameraNode;
}

/* -------------------------------------------------------------------------- */
/// Déplace le joueur lentement vers un niveau.
/* -------------------------------------------------------------------------- */
void Player::goToLevel(int Level)
{
	TRACE(">> goToLevel("+ITOA(Level)+")");
	mTargetLevel    = Level;
	mTargetAltitude = Level*LEVEL_HIGH-5+50;
//	Root::getSingletonPtr()->addFrameListener(this);
	mFloat          = true;
}


/* -------------------------------------------------------------------------- */
/// Translate le joueur lentement vers l'altitude cible.
/* -------------------------------------------------------------------------- */
void Player::translateY(Real deltaY)
{
	Vector3 Pos =mCameraNode->getPosition();
	
	// Si on est trop haut, on descend
	if (Pos.y > mTargetAltitude+0.1)      Pos.y-=deltaY;
	// Si on est trop bas, on monte
	else if (Pos.y < mTargetAltitude-0.1) Pos.y+=deltaY;
	// On est arrivé à la bonne altitude (+/- 0.1)
	else 
	{
		// On stabilise la camera
		Pos.y = mTargetAltitude;
		mCameraNode->setPosition(Pos);
		// On affiche la piece ou se trouve le joueur
		TheShip::getSingletonPtr()->showActiveRoom(mCameraNode);	/// Pour le debug
//		TheShip::getSingletonPtr()->showActiveRoom(Pos);	/// Pour le debug
		// On arrete la gestion du mouvement
		mFloat = false;
		//Root::getSingletonPtr()->removeFrameListener(this);
	}

	// On deplace le joueur.
	mCameraNode->setPosition(Pos);
}


/* -------------------------------------------------------------------------- */
/// Positionne le joueur directement à un niveau.
/* -------------------------------------------------------------------------- */
void Player::jumpToLevel(int Level)
{
	mTargetLevel    = Level;
	mTargetAltitude = Level*LEVEL_HIGH-5+50;

	Vector3 Pos =mCameraNode->getPosition();
	Pos.y = mTargetAltitude ;

	mCameraNode->setPosition(Pos);
	// On gère l'affichage de la piece ou se trouve le joueur
	// TheShip::getSingletonPtr()->showActiveRoom(Pos); // plutot ailleurs
}


/* -------------------------------------------------------------------------- */
/// Renvoie le niveau où le joueur a été positionné.
/* -------------------------------------------------------------------------- */
int Player::getCurrentLevel()
{
	return mTargetLevel;
}


/* -------------------------------------------------------------------------- */
/// Renvoie la position du joueur.
/* -------------------------------------------------------------------------- */
Vector3 Player::getPosition()
{
	return mCameraNode->getPosition();
}

/* -------------------------------------------------------------------------- */
/// Change la position du joueur.
/* -------------------------------------------------------------------------- */
void Player::setPosition(Vector3 pos)
{
	mCameraNode->setPosition(pos);
}


/* -------------------------------------------------------------------------- */
/// Renvoie la Room initiale où le joueur se trouve en début de partie (savedgame).
/* -------------------------------------------------------------------------- */
String Player::getInitialRoom()
{
	return mInitialRoom;
}


/* -------------------------------------------------------------------------- */
/// Charge la situation initiale du joueur. 
/* -------------------------------------------------------------------------- */
void Player::loadgame()
{
	/// Lecture des parametres du fichier de config
	XMLParser_T* DescriptionFile = new XMLParser_T("../savedgames/player.xml");
	String       StepSoundFile;

	DescriptionFile->gotoElement("player","situation");
	this->setPosition(DescriptionFile->getVectorAttribute("position"));	// surtout pour la position en X-Z
	this->jumpToLevel(DescriptionFile->getIntAttribute("level"));       // surtout pour la position en Y
	mInitialRoom    = DescriptionFile->getStringAttribute("room");

	DescriptionFile->gotoElement("player","misc");
	StepSoundFile  = DescriptionFile->getStringAttribute("step");

	delete (DescriptionFile);


	/// On charge aussi le son des pas en mémoire
	mStepSound = FSOUND_Sample_Load(FSOUND_FREE, StepSoundFile.c_str(),FSOUND_FORCEMONO,0,0);

	if (!mStepSound )
	{
		TRACE("*** Player::loadgame: Error Loading SoundFile:" + StepSoundFile);
		TRACE(FMOD_ErrorString(FSOUND_GetError()) );
		FSOUND_Sample_Free(mStepSound );
	}

	// On regle le volume du son (minDistance et maxDistance)
	FSOUND_Sample_SetMinMaxDistance(mStepSound, 5, 6);	
	
	// On le joue une premiere fois pour initialiser le channel, et mémoriser les fréquences audio.
	mStepSoundChannel  = FSOUND_PlaySound(FSOUND_FREE, mStepSound);
	mRunSoundFrequency = FSOUND_GetFrequency(mStepSoundChannel) * 1.4;
	// (la position par defaut est celle du listener)

}


/* ------------------------------------------------------------------------- */
/// Initialise les collisions du joueur.
/* ------------------------------------------------------------------------- */
void Player::initCollisionShape()
{
	using namespace OgreOpcode;

	// On recupère un pointeur sur le contexte
	CollisionContext* collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();

	// Creation de la forme englobante
	CollisionShape* collideShape = CollisionManager::getSingletonPtr()->NewShape("player");
	// Chargement du mesh associé à cette forme
	collideShape->Load(mEntity);
	// Creation de l'objet
	mCollideObject = collideContext->NewObject();
	CollisionObject* collideObject  = static_cast<CollisionObject*>(mCollideObject);
	collideObject->SetCollClass("npc");
	collideObject->SetShape(collideShape);
	collideContext->AddObject(collideObject);
	
	TRACE("Collision System initialized for player");
}

