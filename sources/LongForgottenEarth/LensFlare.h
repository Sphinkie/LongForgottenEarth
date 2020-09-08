/* ------------------------------------------------------------------------- */
// File       : LensFlare.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _LENSFLARE_H_
#define _LENSFLARE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Un effet de lens flare.
/**
L'effet de 'Lens Flare' peut etre géré en 2D ou en 3D. Ici c'est la gestion en 3D qui est utilisée.
Il semble que je n'ai pas toutes les fonctions nécessaires pour le gerer en 2D avec Ogre.
*/
/* ------------------------------------------------------------------------- */
class LensFlare
{
public:
	LensFlare(Vector3 LightPosition, Camera* camera, SceneManager* SceneMgr, SceneNode* ParentNode=NULL);
	virtual ~LensFlare();
	void    createLensFlare(SceneNode* ParentNode);
	void    update();
	void    setVisible(bool visible);
	void    setLightPosition(Vector3 pos);
	void    setHaloColour(ColourValue color);
	void    setBurstColour(ColourValue color);


protected:
	SceneManager* mSceneMgr;
	Camera*       mCamera;
	ColourValue   mColour;
	SceneNode*    mNode;
   	BillboardSet* mHaloSet;
   	BillboardSet* mBurstSet;
	Vector3       mLightPosition;
	bool          mHidden;
};

#endif 
