/* -------------------------------------------------------------------------- */
// File       : Player.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* -------------------------------------------------------------------------- */

#ifndef _PLAYER_H_
#define _PLAYER_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include "fmod.h"
#include "Ogre.h"


using namespace Ogre;


/* -------------------------------------------------------------------------- */
/// Gestion du joueur. C'est un singleton.
/* -------------------------------------------------------------------------- */
class Player : public FrameListener, public Singleton <Player>	
{

public:
	Player::Player(Camera* cam, SceneNode* ParentNode);
	~Player();
	void       moveHead(float mRotX, float mRotY);
	void       moveBody(Vector3 TranslateVector);
	void       moveEars();
	void       setRunMode();
	void       setLieMode(bool lie);
	void       attachToNode(SceneNode* ParentNode);
	Vector3    getPosition();
	SceneNode* getNode();
	String     getName();
	void       setPosition(Vector3 pos);
	void       changeHeadPosition(Vector3 NewPosition);
	void       restaureHeadPosition();
	void       lookAt(Vector3 NewDirection);
	void       goToLevel(int Level);
	void       jumpToLevel(int Level);
	int        getCurrentLevel();
	String     getInitialRoom();
	void       translateY(Real deltaY);
	void       loadgame();
	Camera*    getCamera();
	void       initCollisionShape();
	static Player* getSingletonPtr(void);

protected:

    bool    frameStarted(const FrameEvent& evt);
	Vector3 CheckCollision(const Vector3& pos, float radius, const Vector3& vel);


	SceneNode*     mParentNode;
	SceneNode*     mCameraNode;    ///< Node du corps du joueur
	Camera*        mCamera;        ///< Les yeux du joueur
	float          mRun;           ///< Flag: le joueur court
	int            mLie;           ///< Flag: le joueur est allongé
	Vector3        mHeadPosition;  ///< Position de la Caméra par rapport à son Node
	Vector3        mSavedPosition; ///< Memorisation de mHeadPosition
	bool           mFloat;         ///< Flag: le joueur flotte dans les airs
	int            mTargetLevel;
	int	           mTargetAltitude;
	String         mName;          ///< Le nom du joueur
	String         mCity;          ///< La ville d'origine du joueur
	String         mInitialRoom;
	FSOUND_SAMPLE* mStepSound;
	int            mStepSoundChannel;
	int            mRunSoundFrequency;
	void*          mCollideObject; ///< L'objet correspondant au player, utilisé pour les collisions.
	Entity*        mEntity;        ///< Un cylidre représentant l'espace occupé par le joueur

};

#endif
