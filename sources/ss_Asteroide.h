/* ------------------------------------------------------------------------- */
// File       : ss_Asteroide.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SS_ASTEROIDE_H_
#define _SS_ASTEROIDE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Un asteroide en 3D qui tourne sur lui-même.
/** Solar System Object */
/* ------------------------------------------------------------------------- */
class Asteroide  : public UserDefinedObject, public FrameListener
{
public:
	Asteroide(String Name, String MeshFile, String MaterialName, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Asteroide();
	long    getTypeID (void) const;
	void    addToScene(Vector3 Pos);
	void    setScale(Real Size);
	void    setColor(Real R, Real G, Real B);
	void    setRotationSpeed(Radian RotationSpeed);
	bool    frameStarted (const FrameEvent& evt);
	void    show();
	void    hide();

protected:
	String        mAsteroName;
	Entity*       mAsteroEntity;
	SceneNode*    mAsteroNode;
	SceneManager* mSceneMgr;
	Root*         mRoot;

	Radian        mRotationSpeed;
};

#endif
