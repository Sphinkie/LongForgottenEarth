/* ------------------------------------------------------------------------- */
// File       : SpaceShip.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */
#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Classe de base pour tous les vaisseaux spatiaux, (y compris le Sphinx)
/* ------------------------------------------------------------------------- */
class SpaceShip  
{
public:
	SpaceShip(SceneManager* SceneMgr);
	virtual   ~SpaceShip();
	SceneNode* getNode();

	SceneManager* mSceneMgr;           ///< Le SceneManager Ogre
	Root*         mRoot;               ///< La racine Ogre
	SceneNode*    mShipNode;           ///< Le Node associé au vaisseau
	String        mName;               ///< Le nom du vaisseau
	String        mType;               ///< Type du vaisseau (militaire, marchand, etc)
	String        mClass;              ///< Classe du vaisseau (destroyer, intercepteur)
	String        mOwner;              ///< Propriétaire du vaisseau
	String        mOrigin;             ///< Origine du vaisseau (chantier naval)
	Vector3       mPosition;           ///< Position du Vaisseau
	Vector3       mSpeedVector;        ///< Vecteur vitesse du vaisseau en m/s
	Vector3       mAccelerationVector; ///< Vecteur accélération du vaisseau en m/s/s
	Vector3       mDirectionVector;    ///< Vecteur Direction du vaisseau (vecteur unitaire)
	int           mShield;             ///< Etat des boucliers (en pourcentage?? ou en centimètres ?)
	int           mHull;               ///< Etat de la coque (en pourcentage ??)
	int           mMass;               ///< Masse du vaisseau en tonnes
 	int           mCrew;               ///< Nombre de membres d'équipage

protected:

};


#endif 
