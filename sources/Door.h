/* ------------------------------------------------------------------------- */
// File       : Door.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _DOOR_H_
#define _DOOR_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <Ogre.h>
#include <OgreOpcode.h>


using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des portes du vaisseau spatial.
/* ------------------------------------------------------------------------- */
class Door  : public UserDefinedObject, public FrameListener
{
public:
	Door(String Name, String MeshFile, SceneManager* SceneMgr, SceneNode* mParentNode);
	virtual ~Door();
	long	getTypeID (void) const;
	void    addToScene(Vector3 Pos);
	void    setAnimation(String AnimationName);
	void    setOrientation(int Orientation, Quaternion Qorient, bool BlenderExport = false);
	void    setPanelMaterialNames(String FrontPanel, String BackPanel);
	void    setPanels();
	void    setFocusOn();
	void    setFocusOff();
	bool    openDoor();
	void    closeDoor();
	void    setDoorMalfunction(bool broken);
	void    setDoorDepressurized();
	void    setDoorLocked();
	String  getStatus();
	String  getName();
	void    translate(Vector3 offsetVector);
	void    addPanel(String MeshFile, Vector3 Pos, Quaternion Ori);
	void    setElevatorDoor(bool elevator);
	bool    isElevatorDoor();


protected:

	void    loadObject();
	void    initCollisionShape();
	bool    frameStarted(const FrameEvent& evt);

	enum tStatus {CLOSED, OPENING, WAITING, CLOSING};

	SceneManager*   mSceneMgr;
	Root*			mRoot;
	SceneNode*	    mParentNode;

	String          mDoorName;
	SceneNode*	    mDoorNode;
	Entity*		    mDoorEntity;
	
	SceneNode*	    mPanelNode;
	Entity*		    mPanelEntity;
	int             mScreen1Entity;
	int             mScreen2Entity;
	String          mFrontPanelMaterial;
	String          mBackPanelMaterial;
	
	AnimationState* mAnimState;
	Real            mAnimationSpeed;
	String          mAnimationName;
	int             mOpenSoundChannel;
	tStatus         mStatus;
	Real            mWaitSeconds;   ///< Tempo d'attente avant fermeture de la porte.
	bool            mHasFocus;
	
	bool            mElevatorDoor;
	bool            mLocked;
	bool            mBroken;
	int             mDamage_ETR;
	bool            mDepressurized;

	OgreOpcode::CollisionObject* mCollideObject; ///< Objet pour les collisions
};

#endif
