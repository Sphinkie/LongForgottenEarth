/* ------------------------------------------------------------------------- */
// File       : StarDatabase.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
// Author     : Chris Laurel (Celestia Project)
/* ------------------------------------------------------------------------- */

#ifndef _STAR_DATABASE_H_
#define _STAR_DATABASE_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"
#include <fstream.h>
#include "CatalogStar.h"

using namespace Ogre;

#define ALL -1

/* ------------------------------------------------------------------------- */
/// Interface avec les fichiers Celestia.
/** 
TODO : interface avec les fichiers d'objets (archipels, epaves, trous noirs)
-header     = CELSTARS
-version    = 0x0001
-starcount  = 0x87B70100.....0001B787.....=112519 = Nombre d'étoiles dans le fichier.
-catalog No = 0x01000000.....00000001.....= No de catalog de l'étoile
-position X = 0x654D6644.....float........= Position de l'étoile
-position Y = 0x0F7C8041.....float........  unit = AL
-position Z = 0xB754DFC0.....float........  origin = Sun
-Abs magn   = 0xD701.........0x01D7........=471 = 1.84 * 256
-Spect. Type= 0x5803.........0x0358........champ de bits
Exemple 0x0358 = 0000 0011 0101 1000
soit           = tttt ssss cccc llll
- tttt est le type global de l'étoile (normal, naine blanche, neutron, trou noir)
- ssss est le spectre de l'étoile (O, B, A, etc)
- cccc est la sous classe du spectre (0..9)
- llll est la luminosité de l'étoile (classification Yerkes)
*/
/* ------------------------------------------------------------------------- */
class StarDatabase  
{
public:
	StarDatabase(CatalogStar** StarArray, int sizeArray, String filename);
	virtual     ~StarDatabase();
	int          readStars(int starNumber = ALL);
	CatalogStar* getClosestStar();

protected:

	bool     openFile(String filename);
	Real     getDistance(Vector3 StarCoord);
	bool     isClosest(Real SQ_Distance);
	bool     isVisible(Real SQ_Distance, Real M);
	bool     isInQuadrant(Vector3 StarCoord);

	ifstream starFile;           ///< Fichier Celestia à lire.
	int      mStarsInFile;       ///< Nombre d'étoiles dans le fichier
	CatalogStar** pStars;        ///< Pointeur sur le tableau des étoiles à remplir
	int      mMaxStars;          ///< Taille du tableau des étoiles.
	int      mStarIndex;         ///< index courant dans le tableau.
	CatalogStar* mClosestStar;   ///< Pointeur l'étoile la plus proche du vaisseau
	Real     mClosestDist;       ///< Distance de l'étoile la plus proche (unit = AL)
	Vector3  mShipCoord;         ///< Coordonnées du vaisseau dans l'univers (unit = AL).

	int      mNormalStarCount;   ///< Compteur pour statistiques
	int      mWhiteDwarfCount;   ///< Compteur pour statistiques
	int      mNeutronStarCount;  ///< Compteur pour statistiques
	int      mBlackHoleCount;    ///< Compteur pour statistiques
};

#endif 
