/* ------------------------------------------------------------------------- */
// File       : ss_SolarSystem.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SS_SOLARSYSTEM_H_
#define _SS_SOLARSYSTEM_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "CatalogStar.h"
#include "ss_Star.h"
#include "ss_Planet.h"
#include "Stargen_XMLparser.h"

using namespace Ogre;

#define MAX_PLANETS 12

/* ------------------------------------------------------------------------- */
/// Un système Solaire.
/**
	Un système solaire est composé d'une étoile en son centre, et de planetes en orbite.
	Il peut y avoir éventuellement une autre étoile en orbite (cas d'une étoile double). (TODO)
	Il peut y avoir aussi une ceinture d'astéroides (TODO).
	Il peut y avoir aussi un nuage de Oost (TODO).
*/
/* ------------------------------------------------------------------------- */
class SolarSystem
{
public:
	SolarSystem(SceneNode* ParentNode, SceneManager* SceneMgr, CatalogStar* PrimaryStar);
	~SolarSystem();
	void    show();
	void    hide();
	void    setPosition(Vector3 pos);
	void    setStarLight(bool on);
	int     getCatalogID();
	int     getPlanetNumber();
	String  getName();
	Star*   getStar();
	Vector3     getPosition();
	ColourValue getStarColour();
	void  _changeStar();
	
protected:

	void    generateSystemXMLfile(int CatNo);
	void    createSystem(int CatNo);
	void    createPlanet(Stargen_XMLparser* ss_file, int index);
	String  toRoman(int number);

	String        mName;
	SceneNode*    mParentNode;
	SceneManager* mSceneMgr;
	SceneNode*    mNode;

	// Planet parameters
	int          mPlanetNumber;
	Planet*      mPlanets[MAX_PLANETS];  ///< Tableau des planetes

	// Star parameters 
	CatalogStar* mPrimaryStar;     ///< Reference de l'étoile dans le catalogue
	Star*        mStar;            ///< Reference de l'étoile dans le systeme
	int          mCatalogID;
	
};

#endif 
