/* ------------------------------------------------------------------------- */
// File       : GalaxyBox.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _GALAXY_BOX_H_
#define _GALAXY_BOX_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "OgreSingleton.h"	
#include "ss_Archipel.h"
#include "ss_SolarSystem.h"	
#include "CatalogStar.h"
#include "LensFlare.h"   

using namespace Ogre;

#define MAX_VISIBLE_STARS 5000

/* ------------------------------------------------------------------------- */
/// Quadran Galactique. C'est un singleton.
/** 
Il gere tous les objets de type stellaire qui se trouvent dans le meme quadran 
galactique que le vaisseau. Le vaisseau TheShip restant immobile, c'est, 
en fait, le quadran qui se d�place vers l'arri�re quand le vaisseau avance.

Note sur les pr�cisions:
- Real/float = 4 octets = 3.4 E-38  ... 3.4 E+38  =  7 digits
- double     = 8 octets = 3.4 E-308 ... 3.4 E+308 = 15 digits
*/
/* ------------------------------------------------------------------------- */
class GalaxyBox  : public FrameListener, public Singleton <GalaxyBox>
{
public:
	GalaxyBox(SceneManager* SceneMgr);
	virtual ~GalaxyBox();
	void    show();
	void    hide();
	void    translate(Vector3 TranslationVector);
	void    rotate(Real Zrotation, Real Xrotation, Real Yrotation);
	void    setShipCoordinates(Real x, Real y, Real z);
	Vector3 getShipCoordinates();
	void    setShipLocalPosition(Real x, Real y, Real z);
	Vector3 getPosition();
	void    loadAllStars();
	void    createStars();
	void    removeSolarSys();
	Vector3 getStarPosition();
	String  getSystemParameter(int index);
	void    _changeStar();
	static GalaxyBox* getSingletonPtr(void);

protected:
	bool    frameStarted(const FrameEvent& evt);
	void    setMainStarLight(bool on);
	void    addLensFlare();
	int     loadStars(String filename);

	SceneManager* mSceneMgr;
	Camera*       mCamera;
	SceneNode*    mGalaxyPivotNode;///< Node central de la galaxie.
	SceneNode*    mGalaxyNode;     ///< Node pour tous les objets de la galaxie (faible pr�cision).
	SceneNode*    mLocalNode;      ///< Node pour les objets proche du vaisseau (besoin de pr�cision).
	bool          mLocalMode;      ///< TRUE si on est en mode local (cad dans un systeme solaire)
	BillboardSet* mFarStarsSet;    ///< Set of billboards for visible stars
	CatalogStar** pStars;          ///< Tableau des catalog-stars proches du vaisseau
	int           mNbStars;        ///< Nombre reel d'�toiles dans le tableau
	CatalogStar*  mNearestStar;    ///< L'�toile la plus proche du vaisseau (si < 1 jour.lum)
	Vector3       mShipCoordinates;///< Les coordonn�es du vaisseau dans la galaxie (unit=AL)
	Vector3       mShipLocalPos;   ///< Position pr�cise du vaisseau p/r au systeme solaire (unit=wu)

	Archipel*    mArchipel;        ///< Un archipel d'asteroide (si on est dedans)
	SolarSystem* mSolarSys;        ///< Un systeme solaire (si on est proche d'ne �toile)
	LensFlare*   mLens;            ///< Un effet de lensflare (li� � l'etoile du systeme solaire)

};

#endif 
