/* ------------------------------------------------------------------------- */
// File    : Circle3D.h
// Project : Long Forgotten Earth
// Author  : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _CIRCLE_3D_H_
#define _CIRCLE_3D_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"

using namespace Ogre;

#define NB_PTS 360

/* ------------------------------------------------------------------------- */
/// Dessin de cercles en 3D.
/** 
On peut utiliser les classes (wiki) line3D ou DynamicLines pour dessiner ces cercles.
J'ai choisi DynamicLines, car il permet de rajouter (ou de déplacer) des points 
dynamiquement et donc d'affiner la courbure quand (et où) nécessaire.
*/
/* ------------------------------------------------------------------------- */
class Circle3D
{
public:
	Circle3D(SceneManager* SceneMgr, SceneNode* ParentNode, Real Radius);
	virtual    ~Circle3D();
	void       draw();
	void       update();
	void       setColour(ColourValue CircleColour);

protected:

	SceneManager*    mSceneMgr;            ///< Le SceneManager Ogre
	SceneNode*       mCircleNode;          ///< Le node auquel est rattaché le cercle
	Vector3          mCirclePoints[NB_PTS];///< Les points qui composent le cercle

};

#endif
