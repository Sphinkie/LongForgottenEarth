/* ------------------------------------------------------------------------- */
// File       : Robot.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _ROBOT_H_
#define _ROBOT_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "Robot_Interface.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des Robots (déplacements, travail...)
/* ------------------------------------------------------------------------- */
class Robot  : public UserDefinedObject, public FrameListener
{

friend class Robot_Interface;		// Robot_Interface peut accéder aux données protégées de Robot


public:
	Robot(String Name, String DescFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Robot();
	long    getTypeID (void) const;
 	void    addToScene();
 	void    removeFromScene();
	void    setEngineOn();
	void    setEngineOff();
	void    setBroken(bool broken);
	void    setPosition(Vector3 Position);
	void    setOrientation(int Orientation);
	String  getName();
	Vector3 getPosition();
	void    adjustAltitude();
	void    gotoPoint(Vector3 Destination);
	void    gotoNode(String NodeName);
	void    setFocusOn();
	void    setFocusOff();
	void    activateRobot(bool quick=false);
	void    deactivateRobot(bool quick=false);
	void    removeInterface(); 
	void    createInterface(); 
	void    hideInterface(); 
	void    work(bool Working);
	void    requestExitInterface(Real seconds);
	bool    isExitInterfaceRequiered();
	bool    isInterfaceDisabled();
//	void    shutDown(bool down, bool quick=false);
	bool    isBroken();
	bool    isDown();
	bool    isVisible();
	void    attachToNode(SceneNode* ParentNode=NULL);
	void    setActionString(String ActionString);
	String  getActionString();
	SubEntity* getSubEntity(String MaterialName);

protected:

	enum SoundList {ENGINE, SPARKS, FOCUS, SHUTDOWN, ACTIVATION, ANALYSE, WORK};

	Radian  findAngle();
	void    updateSoundSource(int Channel);
	void    vibrate(const FrameEvent& evt);
	void    move(const FrameEvent& evt);
	void    moveVertical(const FrameEvent& evt);
	void    gotoNextWaypoint();
	void    rotate(const FrameEvent& evt);
	void    playSound(SoundList soundtype);
	bool    frameStarted(const FrameEvent& evt);
	void    startAnimation(String AnimationName, bool Loop);
	void    loadDescription (String DescFile);
	void    loadObject();


	// Variables d'environnement
	Root*           mRoot;
	SceneManager*   mSceneMgr;

	// Variable de l'objet
	SceneNode*      mRobotNode;
	SceneNode*      mParentNode;
	Entity*         mRobotEntity;

	String          mRobotName;
	bool            mEngineOn;
	bool            mBroken;
	bool            mTurning;
	bool            mMoving;
	bool            mMovingUpDown;
	bool            mWorking;
	bool            mHasFocus;
	bool            mDown;
	int             mNormalAltitude;
	int             mTargetAltitude;
	String          mActionString;

	// Variables pour l'interface
	Robot_Interface* mRobot_GUI;
	bool             mExitGui;
	Real             mExitGuiDelay;

	// Variables pour les deplacements
	Real            mSpeed;
	Real            mVy;
	Vector3         mDestination;
	SceneNode*      mNextWaypoint;
	SceneNode*      mLastWaypoint;
	SceneNode*      mCurrentWaypoint;

	// Variables pour l'audio
	int             mHelloChannel;
	int             mSparkChannel;
	int             mWork1Channel;
	int             mWork2Channel;
	int             mEngineChannel;
	int             mShutdownChannel;
	int             mActivationChannel;

	// Variables pour le particules
	SceneNode*      mSourceNode;
	ParticleSystem*	mSmoke;
	ParticleSystem* mSparks;

	// Donnees du fichier de configuration
	String          mMeshFile;
	String          mInterfaceFile;
	String          mHelloSoundFile;
	String          mSparkSoundFile;
	String          mWork1SoundFile;
	String          mWork2SoundFile;
	String          mEngineSoundFile;
	String          mShutdownSoundFile;
	String          mActivationSoundFile;
	String          mStatusFile;

	String          mEngineAnimation;
	AnimationState* mEngineAnimationState;
	String          mHelloAnimation;
	AnimationState* mHelloAnimationState;
	String          mWork1Animation;
	AnimationState* mWork1AnimationState;
	String          mWork2Animation;
	AnimationState* mWork2AnimationState;
	String          mShutdownAnimation;
	AnimationState* mShutdownAnimationState;
	String          mActivationAnimation;
	AnimationState* mActivationAnimationState;

	AnimationState* mAnimationState;
	Real            mAnimationSpeed;

};

#endif 
