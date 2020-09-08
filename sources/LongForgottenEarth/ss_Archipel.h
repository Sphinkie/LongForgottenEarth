/* ------------------------------------------------------------------------- */
// File       : ss_Archipel.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _ARCHIPEL_H_
#define _ARCHIPEL_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "ss_Asteroide.h"

using namespace Ogre;

#define MAX_ASTEROS 100

/* ------------------------------------------------------------------------- */
/// Un archipel d'Asteroides.
/** 
 Un archipel est un Node situé dans l'espace, et entouré d'asteroides en 3D et en 2D. 
 Tous les asteroides de l'archipel ont la même couleur.
*/
/* ------------------------------------------------------------------------- */
class Archipel  : public UserDefinedObject
{
public:
	Archipel(String Name, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Archipel();
	long    getTypeID (void) const;
	void    addToScene();
	void    removeFromScene();
	void    addDebris();
	void    setDebrisColour(ColourValue starColour);
	void    show();
	void    hide();

protected:
	String        mArchipelName;
	SceneNode*    mArchipelNode;
	Entity*       mArchipelEntity;
	SceneManager* mSceneMgr;
	ColourValue   mColour;
	bool          mOnScene;

	Asteroide**   pAsteros;       ///< Pointeur sur le tableau des asteroides
	int           mAsteros;       ///< Taille du tableau des asteroides.
};

#endif 
