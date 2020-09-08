/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Gas.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_GAS_H_
#define _PLANET_TEXTURER_GAS_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "PlanetTexturer.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Cr�ation de textures pour les plan�tes gazeuses.
/** 
Sp�cialisation de la classe PlanetTexturer pour les plan�tes g�antes gazeuses
du type de Jupiter, Neptune, etc.
*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer_Gas : public PlanetTexturer
{
public:
	PlanetTexturer_Gas();
	virtual ~PlanetTexturer_Gas();
	void createTexture(String BitmapName);

protected:

	void createTexture_Jovian   (String BitmapName);
	void createTexture_Neptunian(String BitmapName);
	void renderColors(RendererImage& renderer, int PlanetType);

};

#endif
