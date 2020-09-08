/* ------------------------------------------------------------------------- */
// File       : ss_Planet.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SS_PLANET_H_
#define _SS_PLANET_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Une planete (Solar System Object).
/**
*/
/* ------------------------------------------------------------------------- */
class Planet
{
public:
	Planet(String Name, int Rank, SceneNode* ParentNode, SceneManager* SceneMgr);
	~Planet();
	void createTexture();
	void show();
	void hide();
	void setPosition(Vector3 pos);
	void setParameters(int starID, int planetType, Real radius, Real pressure);
	void setOrbit(Real orbit, Real year, Real tilt);
	void setLightColor(ColourValue StarColor);
	void setOrbitalPosition (long date);
	void setCoverage(Real Ground, Real Water, Real Ice, Real Clouds);
	void setAtmColour(ColourValue AtmColour);


protected:

	void changeMaterial();
	void changeMaterial_NM();

	String        mName;
	SceneNode*    mParentNode;
	SceneManager* mSceneMgr;
	Entity*	      mPlanetEntity;
	SceneNode*    mPlanetNode;

	// Planet parameters
	int         mPlanetType;     ///< Le type de planète (gazeuse, rocheuse, etc).
	int         mCatalogID;      ///< L'ID catalogue de l'étoile centrale.
	int         mRank;           ///< Le No de la planète [1..N] par orbite croissante.
	String      mPlanetID;       ///< Un ID, composé de l'ID catalogue et du rank.
	ColourValue mStarColour;     ///< La couleur de l'étoile qui éclaire la planète.
	int         mYear;           ///< unit = Earth Day
	Real        mOrbit;          ///< unit = UA
	Real        mTilt;           ///< unit = degrees
	Real        mPlanetPressure; ///< unit = Earth atm

};

#endif 
