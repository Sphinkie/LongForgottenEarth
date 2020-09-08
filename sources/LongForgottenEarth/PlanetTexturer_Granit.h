/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Granit.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_GRANIT_H_
#define _PLANET_TEXTURER_GRANIT_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "PlanetTexturer.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Création de textures de test.
/** 
Spécialisation de la classe PlanetTexturer pour des boules de granit.

Performances :
-  512x256 = 4 secondes = ok jusqu'à planete en 70% ecran.
- 1024x512 = 7 secondes = ok jusqu'à planete en 110% ecran.

*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer_Granit : public PlanetTexturer
{
public:
	PlanetTexturer_Granit();
	virtual ~PlanetTexturer_Granit();
	void createTexture  (String BitmapName);

protected:

	void _renderGranitTexture(module::Cache& finalTerrain); // test

};

#endif
