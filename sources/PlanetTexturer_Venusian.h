/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Venusian.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_VENUSIAN_H_
#define _PLANET_TEXTURER_VENUSIAN_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "PlanetTexturer.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Cr�ation de textures pour les plan�tes rocheuses et chaudes (Venusian Planet).
/** 
Sp�cialisation de la classe PlanetTexturer pour les plan�tes de type V�nus.
- VENUSIAN pour les planetes avec faible atmosph�re: donc peu d'�rosion et des 
montagnes abruptes.
- VENUSIAN_2: pour des plan�tes avec une atmosph�re �paisse et des vents violents: 
donc beaucoup d'�rosion et un relief en terrasses.
*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer_Venusian : public PlanetTexturer
{
public:
	PlanetTexturer_Venusian();
	virtual ~PlanetTexturer_Venusian();
	void createTexture(String BitmapName);

protected:
	void renderColors(RendererImage& renderer, int PlanetType);

};

#endif
