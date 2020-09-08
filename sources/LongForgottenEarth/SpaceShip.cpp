/* ------------------------------------------------------------------------- */
// File       : SpaceShip.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "SpaceShip.h"

/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
SpaceShip::SpaceShip(SceneManager* SceneMgr)
{
	mRoot     = Root::getSingletonPtr();
	mSceneMgr = SceneMgr;
	mShipNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	mName              = "CTSS Sphinx VII";
	mMass              = 860;
	mOwner             = "Confederation Terrienne";
	mOrigin            = "Chantier Spatial de Gdansk. Terra.";
	mHull              = 65;
	mType              = "Military SpaceShip";
	mClass             = "Medium Interceptor";
	mCrew              = 8;
	mSpeedVector       = Vector3::ZERO;
	mAccelerationVector= Vector3::ZERO;
	mDirectionVector   = Vector3::UNIT_Z;
}

/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
SpaceShip::~SpaceShip()
{

}


/* ------------------------------------------------------------------------- */
/// Renvoie un pointeur sur le Node du vaisseau 
/* ------------------------------------------------------------------------- */
SceneNode* SpaceShip::getNode()
{
	return mShipNode;
}

