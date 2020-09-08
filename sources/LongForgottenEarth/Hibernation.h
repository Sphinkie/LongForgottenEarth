/* ------------------------------------------------------------------------- */
// File       : Hibernation.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _HIBERNATION_H_
#define _HIBERNATION_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"
#include "Hibernation_Interface.h"
#include "body.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des objets de type caisson d'Hibernation.
/* ------------------------------------------------------------------------- */
class Hibernation  : public UserDefinedObject, public FrameListener
{

friend class Hibernation_Interface;		// Hibernation_Interface peut accéder aux données protégées de Hibernation

public:
	Hibernation(String Name, String MeshFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Hibernation();
	bool    frameStarted(const FrameEvent& evt);
	void    addFrameListener(short User);
	long    getTypeID (void) const;
 	void    addToScene();
 	void    removeFromScene();
	void    setPosition(Vector3 Position);
	Vector3 getPosition(String RelativeTo="PARENT");
	void    setOrientation(int Orientation);
	void    setOrientation(Quaternion Orientation);
	void    setBroken(bool broken);
	void    setFocusOn();
	void    setFocusOff();
	void    createInterface();
	void    removeInterface();
	void    hideInterface();
	void    requestExitInterface(Real seconds);
	bool    isExitInterfaceRequiered();
	bool    isInterfaceDisabled();
	bool    isBroken();
	bool    isOpened();
	bool    isClosed();
	bool    closeDoor();
	bool    openDoor();
	void    startSequence();
	void    endSequence();
	void    loadObject();
	void    climb();
	void    climbIn();
	void    climbOut();
	void    setDuration(int duration);
	void    setMaterialScreen();
	String  getStatus();
	String  getName();


protected:

	enum tStatus {OPENED, CLOSED, OPENING, CLOSING};
	enum tPhase  {IDLE, STARTING, OPERATING, ENDING}; // valeurs = 0-1-2-3

	// Variables d'environnement
	Root*           mRoot;
	SceneManager*   mSceneMgr;

	// Variable de l'objet
	String          mHibernationName;
	SceneNode*      mHibernationNode;
	SceneNode*      mParentNode;
	Entity*         mEntity;
	int             mScreenEntity;

	bool            mBroken;
	bool            mLocked;
//	bool            mWorking;
	bool            mHasFocus;
	tPhase          mPhase;
	Real            mFineClock;	
	int             mGrossClock;	     
	int             mLastGrossClock;	     
	int             mCptFrame;
	
	int             mTemperature;
	int             mInitialDuration;
	long            mInitialDate;
	long            mFinalDate;
	Body*           mSubject;
	String          mSubjectName;

	// Variables pour l'interface
	bool            mExitGui;
	Real            mExitGuiDelay;

	// Variables pour l'animation
	tStatus         mStatus;
	AnimationState* mAnimState;
	Real            mAnimationSpeed;
	short           mListenerNeeded;
	Real            mEndFog;

	// Variables pour l'audio
	int             mOpenSoundChannel;
	int             mStartSoundChannel;
	int             mEndSoundChannel;
	int             mOperatingSoundChannel;
	int             mHelloSoundChannel;
	int             mOperatingSoundFrequency;
	int             mOperatingSoundVolume;

	// Variables pour les effets de particules
	SceneNode*      mParticulesNode;
	ParticleSystem*	mSmoke;
	ParticleSystem* mSparks;
	ParticleSystem* mGivre;

	// Donnees du fichier de configuration
	String          mMeshFile;
	String          mInterfaceFile;
	String          mOpenSoundFile;
	String          mStartSoundFile;
	String          mEndSoundFile;
	String          mHelloSoundFile;
	String          mOperatingSoundFile;

	Hibernation_Interface*      mHibernation_GUI;
};

#endif 
