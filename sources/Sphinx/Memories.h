/* ------------------------------------------------------------------------- */
// File       : Memories.h
// Project    : Long Forgotten Earth - Menu Principal
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _MEMORIES_H_
#define _MEMORIES_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Ogre.h"
// #include "OgreConfigFile.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gere une scene 3D (à 360 degrees)
/* ------------------------------------------------------------------------- */
class Memories  
{
public:
	Memories(RenderWindow* win, SceneManager* SceneMgr, String MemoriesFile);
	virtual    ~Memories();
	void       showMemories();
	void       yaw(Degree angle);


protected:
	SceneNode* createDisplay(String Name, int Largeur, int Hauteur);

	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
	SceneNode*    mCameraNode;
	SceneNode*    mBillboardNode;
	Camera*       mCamera;
	String        mDescriptionFile;

	bool          mHasBillboard;
	String        mBillboardName;

	bool          mHasParticles;
	String        mParticulesName;
	SceneNode*    mParticleSourceNode;
	ParticleSystem* mParticObject;

	bool          mHasLCDdisplays;
};

#endif 
