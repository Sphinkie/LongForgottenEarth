/* ------------------------------------------------------------------------- */
// File       : ShipLight.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SHIPLIGHT_H_
#define _SHIPLIGHT_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Ogre.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Spot lumineux associé à un vaisseau. Un projecteur exterieur.
/* ------------------------------------------------------------------------- */
class ShipLight  
{
public:
	ShipLight(String Name, BillboardSet* billboardset, SceneManager* SceneMgr, SceneNode* ParentNode);
	virtual ~ShipLight();
	void     setHilight(bool on);
	bool     getHilightStatus();
	void     setPosition(Vector3 Posit);
	void     setDirection(Vector3 Direc);


protected:

	SceneManager* mSceneMgr;
	SceneNode*    mNode; 
	Light*        mLight;
	String        mName;
	Billboard*    mSprite;
	BillboardSet* mBillboardset;
	bool          mHilights;   /// Status des projecteurs exterieurs (allumés ou éteints)
};

#endif 
