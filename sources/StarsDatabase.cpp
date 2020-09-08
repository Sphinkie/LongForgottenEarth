/* ------------------------------------------------------------------------- */
// File       : StarDatabase.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
// Author     : Chris Laurel (Celestia Project)
/* ------------------------------------------------------------------------- */

#include "StarsDatabase.h"
#include "bytes.h"
#include "GalaxyBox.h"
#include "macros.h"
#include "const.h"

#undef  DEBUG_LOG
#define DEBUG_LOG(X) 

#define MAX_VISIBLE_DISTANCE_AL2 1e4     // c-a-d une sphere de 100 AL autour du ship.

/* ------------------------------------------------------------------------- */
/// Contructeur
/** 
@param StarArray est un pointeur sur un tableau de pointeurs.
@param sizeArray est la taille de ce tableau, � ne pas d�passer.
*/
/* ------------------------------------------------------------------------- */
StarDatabase::StarDatabase(CatalogStar** StarArray, int sizeArray, String filename)
{
	mStarsInFile = 0;
	mStarIndex   = 0;
	mMaxStars    = sizeArray;
	pStars       = StarArray;
	mClosestStar = NULL;
	mClosestDist = 1e10;
	
	this->openFile(filename);
	mNormalStarCount = 0;
	mWhiteDwarfCount = 0;
	mNeutronStarCount= 0;
	mBlackHoleCount  = 0;

	mShipCoord = GalaxyBox::getSingletonPtr()->getShipCoordinates();
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
StarDatabase::~StarDatabase()
{
	starFile.close();
}


/* ------------------------------------------------------------------------- */
/// Ouvre en lecture un fichier de type CELESTIA, et v�rifie son ent�te.
/* ------------------------------------------------------------------------- */
bool StarDatabase::openFile(String filename)
{
	starFile.open(filename.c_str(), ios::in | ios::binary);
	if (!starFile.good())
	{
		TRACE ("Error opening "+filename);
		return false;
	}

	// Verifie l'entete du fichier
	{
		char* FileHeader   = "CELSTARS";
		int   headerLength = strlen(FileHeader);
		char* header = new char[headerLength];
		starFile.read(header, headerLength);
		if (strncmp(header, FileHeader, headerLength)) return false;
		delete[] header;
	}

	// Verifie la version du format de fichier
	{
		uint16 version;
		starFile.read((char*) &version, sizeof version);
		LE_TO_CPU_INT16(version, version);
		if (version != 0x0100) return false;
	}

	// Lit le nombre d'�toiles dans le fichier
	starFile.read((char*) &mStarsInFile, sizeof mStarsInFile);
	LE_TO_CPU_INT32(mStarsInFile, mStarsInFile);

	return true;
}





/* ------------------------------------------------------------------------- */
/// Charge le contenu du fichier des �toiles stars.dat au format CELESTIA
/**
@param starNumber Le nombre d'�toiles � lire (ou 'ALL').
@return Le nombre d'�toiles lues.
Note: La lecture des 112000 �toiles du fichier prend moins d'une seconde.
Des conversions sont n�cesaires, car les valeurs sont stock�es en binaire inverse.
*/
/* ------------------------------------------------------------------------- */
int StarDatabase::readStars(int starNumber)
{
    if (!starFile.good()) return 0;

	int starCount = 0;
	if (starNumber==ALL) starNumber=mStarsInFile;
	while ((mStarIndex<mStarsInFile) && // On ne lit pas au dela du fichier
	       (mStarIndex<mMaxStars)    && // On ne lit pas au dela de la capacit� du tableau
	       (starCount<starNumber)    )  // On ne lit pas au dela de ce qui est demand�
	{
		uint32 catNo = 0;
		Real   x=0, y=0, z=0;
		short  absMag;
		short  spectralType;

		starFile.read((char*) &catNo, sizeof catNo);
		LE_TO_CPU_INT32(catNo, catNo);
		starFile.read((char*) &x, sizeof x);
		LE_TO_CPU_FLOAT(x, x);
		starFile.read((char*) &y, sizeof y);
		LE_TO_CPU_FLOAT(y, y);
		starFile.read((char*) &z, sizeof z);
		LE_TO_CPU_FLOAT(z, z);
		starFile.read((char*) &absMag, sizeof absMag);
		LE_TO_CPU_INT16(absMag, absMag);
		starFile.read((char*) &spectralType, sizeof spectralType);
		LE_TO_CPU_INT16(spectralType, spectralType);
		// if (starFile.bad()) break;

		Real    M = absMag / 256.0;
		Vector3 P = Vector3(x, z, -y); 
		short  TY = (spectralType >> 12);
		short  SP = CatalogStar::Spectral_Unknown;
		short  SS = 0;
		short  ST = CatalogStar::Lum_Unknown;

		switch (TY)
		{
		case 0:      // NormalStar  
			{
			mNormalStarCount++;
 			SP = (spectralType>>8) & 0x000F;  // spectre
			SS = (spectralType>>4) & 0x000F;  // sous-spectre
			ST = (spectralType)    & 0x000F;  // Yerkes Type
			break;
			}
		case 1:        // WhiteDwarf
			{
			mWhiteDwarfCount++;
			SP = (spectralType>>8) & 0x000F; // spectre
			// On transforme un octet 0..7 en 16..23 (spectre des naines blanches)
			SP += + 16;
			if (SP > 23) SP=23;
			SS = (spectralType>>4) & 0x000F; // sous-spectre
			ST = CatalogStar::Lum_VII;       // Yerkes Type
 			break;
			}
		case 2:        // NeutronStar:
			{
			mNeutronStarCount++;
			SP = CatalogStar::Spectral_NS;      // spectre
			SS = 0;                             // sub-spectre
			ST = CatalogStar::Lum_Neutron;      // Type
			break;
			}
		case 3:        // BlackHole:
			{
			mBlackHoleCount++;
			SP = CatalogStar::Spectral_BH;      // spectre
			SS = 0;                             // sub-spectre
			ST = CatalogStar::Lum_BlackHole;    // Type
			break;
			}
		}
		Real D = this->getDistance(P);      // on ne calcule la distance qu'une seule fois.
		DEBUG_LOG(ITOA(catNo)+": Star distance (al2) = "+ITOA(D));

		// On a diff�rentes fa�on de choisir les �toiles qui appaitront dans le ciel
//		if (this->isVisible(D,M))           // donne environ 1600 �toiles
		if (D<MAX_VISIBLE_DISTANCE_AL2)     // donne environ 2400 �toiles
//		if (this->isInQuadrant(P))          // trop d'etoiles: r�duire le quandrant
		{
			DEBUG_LOG(ITOA(mStarIndex)+": Star ID "+ITOA(catNo)+" at "+ITOA(P)+" (al)");
			pStars[mStarIndex] = new CatalogStar(catNo, M, P, SP, SS, ST);
			// On met a jour l'�toile la plus proche.
			if (this->isClosest(D))	mClosestStar = pStars[mStarIndex];
			mStarIndex++;
		}
		starCount++;
	}

	TRACE("StarDatabase "+ITOA(mStarsInFile)+" stars in binary database");
	TRACE("StarDatabase "+ITOA(starCount)+" stars read");
	TRACE("StarDatabase "+ITOA(mStarIndex)+" stars stored");

	TRACE("Normal Star Count  "+ITOA(mNormalStarCount));
	TRACE("White Dwarf Count  "+ITOA(mWhiteDwarfCount));
	TRACE("Neutron Star Count "+ITOA(mNeutronStarCount));
	TRACE("Black-Hole Count   "+ITOA(mBlackHoleCount));

	return (mStarIndex-1);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la distance jusqu'� l'�toile fournie en param�tre.
/** 
@param StarPos Position de l'�toile par rapport � la galaxie (unit = AL)
@return Distance du Ship � cette �toile (unit = AL�).
Note: L'origine de la Galaxie est le soleil.
*/
/* ------------------------------------------------------------------------- */
Real StarDatabase::getDistance(Vector3 StarCoord)
{
	Vector3 Star2Ship = StarCoord - mShipCoord; // unit = AL
	return Star2Ship.squaredLength();
}

/* ------------------------------------------------------------------------- */
/// Cette fonction indique si le vaisseau se trouve dans le voisinage proche de cette �toile.
/** 
@param SQ_Distance Distance de l'�toile en AL� (Square distance).
@retrun Renvoie vrai si cette �toile est la plus proche du vaisseau. 
Cette fonction affine son r�sultat au fur et � mesure qu'elle analyse les �toiles.
Renvoie FALSE si on est � plus de 1 Jour.Lumi�re.
*/
/* ------------------------------------------------------------------------- */
bool StarDatabase::isClosest(Real SQ_Distance)
{
	bool found;

	// Quelques exemples:
	// 590id27913(chi orionis: 8 planetes dont 1 presque habitable - etoile non visible) 
	// 0id57     (7 planets)
	// id25    (3 planets)
	// 118(Theta Octantis) 
	// 650(Alpheratz)

	if (SQ_Distance > JOUR_LUMIERE_AL2) found=false;
	// Si on est pr�s du centre de la galaxie, il peut y avoir des �toiles 
	// voisines de 1 jour.lumi�re... On d�termine alors la plus proche.
	if (SQ_Distance < mClosestDist)
	{
		mClosestDist = SQ_Distance;
		found = true;
	}
	return found;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction indique si l'�toile est visible � cette distance.
/** M�thode bas�e sur la magnitude apparente.
@param SQ_Distance Distance de l'�toile en AL� (Square distance).
@param M           Magnitude absolue de l'�toile.
@return TRUE si cette �toile est visible de la position actuelle.
On utilise les �quations:
   m = M-5 + 5Log(d)    (d en parsec)
   et: log(d�) = 2log(d)
   
*/
/* ------------------------------------------------------------------------- */
bool StarDatabase::isVisible(Real SQ_Distance, Real M)
{
	Real d2 = SQ_Distance * AL2_TO_PC2; // unit = pc�
	Real m  = M-5 + 2.5*log10(d2);     // magnitude apparente
	DEBUG_LOG("magn absolue: "+ITOA(M)+" app:"+ITOA(m));
	return (m<DARKER_VISIBLE_MAGNITUDE);
}


/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si cette �toile est proche de la position actuelle.
/** M�thode bas�e sur un d�coupage de l'univers en cubes (quadrans galactiques) */
/* ------------------------------------------------------------------------- */
bool StarDatabase::isInQuadrant(Vector3 StarCoord)
{
	Real dist_X = abs(StarCoord.x - mShipCoord.x);
	Real dist_Y = abs(StarCoord.y - mShipCoord.y);
	Real dist_Z = abs(StarCoord.z - mShipCoord.z);
	// Tout est en AL
	return ((dist_X<QUADRANT_X_SIZE) && (dist_Y<QUADRANT_Y_SIZE) && (dist_Z<QUADRANT_Z_SIZE)) ;
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'�toile la plus proche de la position actuelle, ou NULL si elle est plus loin que 1 jl.
/** On consid�re que l'on entre dans un systeme solaire, si on est � moins de 1 jour-lumi�re de l'�toile. */
/* ------------------------------------------------------------------------- */
CatalogStar* StarDatabase::getClosestStar()
{
	return mClosestStar;
}

