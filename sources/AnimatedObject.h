/* ------------------------------------------------------------------------- */
// File       : AnimatedObject.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _ANIMATED_OBJECT_H_
#define _ANIMATED_OBJECT_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des object animés du vaisseau spatial.
/* ------------------------------------------------------------------------- */
class AnimatedObject : public UserDefinedObject, public FrameListener
{
public:
	AnimatedObject(String Name, String MeshFile, SceneManager* SceneMgr, SceneNode* mParentNode);
	virtual ~AnimatedObject();
	long	getTypeID (void) const;
	bool    frameStarted(const FrameEvent& evt);
	void    addToScene(Vector3 Pos);
	void    setOrientation(int Orientation, bool BlenderExport = false);
	void    setFocusOn();
	void    setFocusOff();
	bool    openDoor();
	void    closeDoor();
	String  getName();
	void    setAnimation(String AnimationName);
	void    translate(Vector3 offsetVector);


protected:

	enum tStatus {CLOSED, OPENING, WAITING, CLOSING};

	String          mName;
	SceneNode*	    mNode;
	Entity*		    mEntity;
	SceneManager*   mSceneMgr;
	Root*			mRoot;
	SceneNode*	    mParentNode;
	AnimationState* mAnimState;
	Real            mAnimationSpeed;
	String          mAnimationName;
	int             mOpenSoundChannel;
	tStatus         mStatus;
	Real            mWaitSeconds;
	bool            mHasFocus;

};

#endif
