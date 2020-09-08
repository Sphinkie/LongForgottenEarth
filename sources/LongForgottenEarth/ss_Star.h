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
/// Une �toile (Solar System Object).
/**
L'�toile est g�r�e avec ce niveau de d�tail quand on est entr� dans sa sph�re d'influence,
(c'est-�-dire, en g�n�ral, son syst�me plan�taire).
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

	Real          mTemperature; ///> Temperature de l'�toile (unit�=Kelvin)
	ColourValue   mStarColor;   ///< Couleur de l'�toile.
	Real          mWaveLength;  ///< Longueur d'onde correspondant � la couleur. (unit�=nm)
	Real          mIntensity;   ///< Intensit� lumineuse de l'�toile (rendu visuel)
	String        mSpectrum;    ///< Spectre de l'�toile (O, B, A, G, ...)
	Real          mMass;        ///< Masse de l'�toile (unit� = solar mass)
	Real          mLuminosity;  ///< Luminosit� de l'Etoile (unit� = Solar Lum)

};

#endif 
