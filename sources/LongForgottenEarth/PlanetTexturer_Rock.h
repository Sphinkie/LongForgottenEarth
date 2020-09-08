/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Rock.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_ROCK_H_
#define _PLANET_TEXTURER_ROCK_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "PlanetTexturer.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Cr�ation de textures pour les plan�tes rocheuse sans atmosph�re (Rock Planet).
/** Sp�cialisation de la classe PlanetTexturer pour les plan�tes rocheuses.
*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer_Rock : public PlanetTexturer
{
public:
	PlanetTexturer_Rock();
	virtual ~PlanetTexturer_Rock();
	void createTexture (String BitmapName);

protected:

};

#endif
