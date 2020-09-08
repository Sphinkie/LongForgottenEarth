/* ------------------------------------------------------------------------- */
// File       : Body.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _BODY_H_
#define _BODY_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include <OgreOpcode.h>

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des PNJ 
/**
 Cet objet g�re les PNJ : leur Mesh associ�, leur nom, leur vitalit�.
 Il g�re aussi le cas particulier du joueur (Player).
 Il g�re aussi leur inventaire (TODO)
*/
/* ------------------------------------------------------------------------- */

class Body  : public UserDefinedObject
{
public:
	Body(String name, String descfilename);
	virtual ~Body();
	long     getTypeID (void) const;
	void     initCollisionShape();
	void     getCollision();

	// Gestion du nom du PNJ
	String   getName();
	void     addToScene(SceneNode* ParentNode=NULL);
	void     removeFromScene();
	void     setPosition(Vector3 Position);

	// Gestion du joueur
	void     setPlayerBody(bool player=true);
	bool     isPlayerBody();

	// Gestion de l'�tat physique (vitalit�)
	String   getStatus();
	short    getVitalite();
	short    getBoneStatus();
	short    getHeadStatus();
	short    getTorsoStatus();
	short    getRadiatedStatus();
	short    getNecrosedStatus();
	short    getNbMembersOk(short status);

	enum tStatus {DEAD=0, R_LEG=1, L_LEG=2, R_ARM=4, L_ARM=8, TORSO=16, HEAD=32, ALL_OK=63};

protected:

	int      getVitaParameter(String P);

	SceneNode*  mNode;
	Entity*     mEntity;
	String      mName;
	SceneNode*  mParentNode;
	short       mVitaBones;		///< Indice de vitalit� des os du corps: 0..128 (somme de tStatus)
	short       mVitaRadiated;	///< Indice de radioactivit� du corps  : 0..128 (somme de tStatus)
	short       mVitaNecrosed;	///< Indice de n�crose du corps        : 0..128 (somme de tStatus)
	short       mVitaWounded;	///< Indice de blessures des membres   : 0..128 (somme de tStatus)
	short       mLastChance;    ///< Ce petit indice de vitalit� permet de ne pas etre mort, m�me si un des mVita est a z�ro. 0..4
	bool        mPlayer;		///< Ce flag indique si ce body est celui du player
	bool		mIsOnScene;     ///< TRUE if the body is displayed on the scene.
	OgreOpcode::CollisionObject* mCollideObject; ///< Objet englobant le body, utilis� pour les collisions.
};

#endif 
