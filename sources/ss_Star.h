/* ------------------------------------------------------------------------- */
// File       : ss_Star.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SS_STAR_H_
#define _SS_STAR_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "CatalogStar.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Une étoile (Solar System Object).
/**
L'étoile est gérée avec ce niveau de détail quand on est entré dans sa sphère d'influence,
(c'est-à-dire, en général, son système planétaire).
*/
/* ------------------------------------------------------------------------- */
class Star
{
public:
	Star(CatalogStar* RefStar, SceneNode* ParentNode, SceneManager* SceneMgr);
	~Star();
	void        show();
	void	    hide();
	void        setPosition(Vector3 pos);
	void        setMass(Real mass);
	void        setStarLight(bool on);
	bool        fadeStarLight(bool on);

	ColourValue getColour();
	Real        getRadius();
	Real        getTemperature();
	Real        getLuminosity();
	String      getName();
	String      getSpectralType();
	void        _changeSpectrum();


protected:

	Real          calculateLuminosity(Real bolometricmagnitude);
	Real          calculateWavelength(Real Temperature);
	ColourValue   calculateColour(Real Wavelength);
	ColourValue   adjustColour(ColourValue Couleur, Real Gamma);
	ColourValue   _adjustColour(Real R, Real G, Real B, Real Factor, Real Gamma);
	ColourValue   _wavelength2colour(Real Wavelength);
	String        getPrimaryStarName(CatalogStar* RefStar);
	String        expandStarName(String name);

	String        mName;
	SceneNode*    mParentNode;
	SceneManager* mSceneMgr;
	Entity*	      mStarEntity;
	SceneNode*    mStarNode;
	CatalogStar*  mRefStar;

	Light*        mLight;
	Billboard*    mSprite;
	BillboardSet* mBillboardset;

	Real          mTemperature; ///> Temperature de l'étoile (unité=Kelvin)
	ColourValue   mStarColor;   ///< Couleur de l'étoile.
	Real          mWaveLength;  ///< Longueur d'onde correspondant à la couleur. (unité=nm)
	Real          mIntensity;   ///< Intensité lumineuse de l'étoile (rendu visuel)
	String        mSpectrum;    ///< Spectre de l'étoile (O, B, A, G, ...)
	Real          mMass;        ///< Masse de l'étoile (unité = solar mass)
	Real          mLuminosity;  ///< Luminosité de l'Etoile (unité = Solar Lum)

};

#endif 
