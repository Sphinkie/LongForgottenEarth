/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Ice.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_ICE_H_
#define _PLANET_TEXTURER_ICE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "PlanetTexturer.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Cr�ation de textures pour les plan�tes glaciaires (Ice Planet).
/** Sp�cialisation de la classe PlanetTexturer pour les plan�tes glaciaires.
*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer_Ice : public PlanetTexturer
{
public:
	PlanetTexturer_Ice();
	virtual ~PlanetTexturer_Ice();
	void createTexture  (String BitmapName);

protected:

};

#endif
