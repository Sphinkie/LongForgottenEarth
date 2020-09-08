/* ------------------------------------------------------------------------- */
// File       : CatalogStar.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "CatalogStar.h"
#include "macros.h"
#include "const.h"

/* ------------------------------------------------------------------------- */
/// Contructeur
/** 
@param Number       Number of the star in the Celestia catalog
@param Magn         Absolute magnitude of the star
@param Pos          Cartesian position of the star (unit = al)
@param Spectrum     Spectrum of the star (@sa SpectralClass)
@param SubSpectrum  Sub Spectrum of the star (0..9)
@param YerkesType   Luminosity type of the star (@sa LuminosityClass)
*/
/* ------------------------------------------------------------------------- */
CatalogStar::CatalogStar(int Number, Real Magn, Vector3 Coord, short spectrum, short SubSpectrum, short YerkesType)
{
	mCatalogNumber = Number;
	mPosition      = Coord * SPACE_WU_PER_AL;
	mAbsMag        = Magn;
	mSpectrum      = static_cast<SpectralClass>(spectrum);
	mSubSpectrum   = SubSpectrum;
	mType          = static_cast<LuminosityClass>(YerkesType);
	mSprite        = NULL;
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
CatalogStar::~CatalogStar()
{}

/* ------------------------------------------------------------------------- */
/// Renvoie la Magnitude Absolue de l'�toile indiqu� dans le fichier stars.dat
/** 
La magnitude absolue est la magnitude qu'aurait cette �toile a une distance de 10 parsecs.
@return La magnitude absolue de l'�toile 
*/
/* ------------------------------------------------------------------------- */
Real CatalogStar::getAbsoluteMagnitude() 
{
	return mAbsMag;
}

/* ------------------------------------------------------------------------- */
/// Renvoie la Magnitude Bolom�trique de l'�toile.
/** Il s'agit de la magnitude de l'�toile comprenant les rayonnements invisibles 
(infra-rouges). On l'obtient en ajoutant un coefficient correcteur � la magnitude 
absolue. 
De fa�on simplifi�e, les coefficients correcteurs sont (suivant le spectre) :
- O		-4.3
- B		-3.0
- A		-0.2
- F		+0.0
- G		-0.1
- K		-0.3
- M		-1.2
*/
/* ------------------------------------------------------------------------- */
Real CatalogStar::getBolometricMagnitude()
{
	switch(mSpectrum) 
	{
	case Spectral_O: return (mAbsMag-4.3); break;
	case Spectral_B: return (mAbsMag-3.0); break;
	case Spectral_A: return (mAbsMag-0.2); break;
	case Spectral_G: return (mAbsMag-0.1); break;
	case Spectral_K: return (mAbsMag-0.3); break;
	case Spectral_M: return (mAbsMag-1.2); break;
	default:         return (mAbsMag);
	}
}

/* ------------------------------------------------------------------------- */
/// Renvoie la masse de l'�toile en fonction de son type Yerkes.
/** 
Note: La regle appliqu�e ici pour l'interpolation lin�aire p/r au sous-spectre 
est bas�e sur le fait qu'un �toile plus lumineuse est aussi plus massive (sous-spectre de 0 � 9)..
L'interpolation sur le No de catalogue est fantaisiste (g�n�ration d'une valeur al�atoire).
@return unit = solar mass 
*/
/* ------------------------------------------------------------------------- */
Real CatalogStar::getMass()
{
	Real  Mass = 1;
	short interpol = (int)(abs(cos(mCatalogNumber+20)*100)) % 10;

	switch(mType) 
	{
	case Lum_Ia0 : Mass = (70 + mSubSpectrum*3.1);   break; // hyper giant :  de 70 � 100 MS
	case Lum_Ia  : Mass = (50 + mSubSpectrum*2.0);   break; // super giant :  de 50 �  70 MS
	case Lum_Ib  : Mass = (25 + mSubSpectrum*2.5);   break; // super giant :  de 25 �  50 MS
	case Lum_II  : Mass = ( 9 + mSubSpectrum*1.1);   break; // bright giant:  de  9 �  20 MS
	case Lum_III : Mass = ( 7 + mSubSpectrum*0.1);   break; // normal giant:  de  7 �   8 MS
	case Lum_IV  : Mass = ( 4 + mSubSpectrum*0.2);   break; // sub giant   :  de  4 �   6 MS
	case Lum_V   : Mass = ( 1 +abs(mSubSpectrum-2)*0.2);break;//main sequence: de  1 �   3 MS (sol=G2)
	case Lum_VI  : // sub dwarf
	case Lum_VII : Mass = (0.8+ mSubSpectrum*0.06);  break; // white dwarfs : de 0.8 � 1.4 MS
	case Lum_Neutron  : Mass = (1.5 + interpol*0.15);break; // neutron star : de 1.5 � 3.0 MS
	case Lum_BlackHole: Mass = (3.0 + interpol*1.10);break; // trou noir    : de   3 �  14 MS
	}
	// Naine rouge : de 0.08 � 0.80 MS (s�rie principale, classe M)
	if ((mType==Lum_V) && (mSpectrum==Spectral_M)) Mass = (0.08+ interpol*0.07);
	// Naine brunes: de 0.01 � 0.08 MS (s�rie principale, classe L)
	if ((mType==Lum_V) && (mSpectrum==Spectral_L)) Mass = (0.01+ interpol*0.01);
	return Mass;
}

/* ------------------------------------------------------------------------- */
/// D�finit la magnitude absolue de l'�toile.
/* ------------------------------------------------------------------------- */
void CatalogStar::setAbsoluteMagnitude(Real Magnitude)
{
	mAbsMag = Magnitude;
}


/* ------------------------------------------------------------------------- */
/// D�finit la classe spectrale de l'�toile.
/** @param spectralType Variable de type SpectralClass. */
/* ------------------------------------------------------------------------- */
void CatalogStar::setSpectralType(short spectralType)
{
	mSpectrum      = static_cast<SpectralClass>(spectralType);
}

/* ------------------------------------------------------------------------- */
/// D�finit le No de l'�toile.
/* ------------------------------------------------------------------------- */
void CatalogStar::setCatalogNumber(int Number)
{
    mCatalogNumber = Number;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le No de l'�toile dans le catalogue.
/* ------------------------------------------------------------------------- */
int CatalogStar::getCatalogNumber()
{
    return mCatalogNumber;
}

/* ------------------------------------------------------------------------- */
/// D�finit la position de l'�toile.
/* ------------------------------------------------------------------------- */
void CatalogStar::setPosition(Real x, Real y, Real z)
{
	mPosition = Vector3(x, y, z);
	if (mSprite) mSprite->setPosition(mPosition);
}


/* ------------------------------------------------------------------------- */
/// D�finit la position de l'�toile.
/* ------------------------------------------------------------------------- */
void CatalogStar::setPosition(Vector3 Pos)
{
	mPosition = Pos;
	if (mSprite) mSprite->setPosition(mPosition);
}


/* ------------------------------------------------------------------------- */
/// Renvoie la position de l'�toile
/* ------------------------------------------------------------------------- */
Vector3 CatalogStar::getPosition() 
{
    return mPosition;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la temperature de la photosphere de l'�toile, en fonction de sa classe spectrale.
/** 
- R-class stars have the same temperature as K-class stars but have high abundances of carbon and carbon molecules. 
- N-class stars are carbon-rich stars with the same temperature as M-class stars.
- S-class stars are similar temperature to M stars but have bands of zirconium oxide and lanthanum oxide. 
- WN and WC are two types of Wolf-Rayet stars, the same temperature as O-class stars but showing strong broad emission lines of carbon and nitrogen respectively.
- All class D stars use the same sequence from 1 to 9, with 1 indicating a temperature above 37,500 K and 9 indicating a temperature below 5,500 K.
- Note: The interior temperature of an average star is about 20.000.000 K.
@return La temperature de l'�toile (unit = Kelvin)
@sa http://outreach.atnf.csiro.au/education/senior/astrophysics/spectral_class.html
*/
/* ------------------------------------------------------------------------- */
Real CatalogStar::getTemperature() 
{
    switch (mSpectrum)
    {
	case Spectral_WC: 
	case Spectral_WN: 
	case Spectral_O:  return (25000 + mSubSpectrum*2500); break;
	case Spectral_B:  return (10000 + mSubSpectrum*1500); break;
	case Spectral_A:  return ( 8000 + mSubSpectrum* 100); break;
	case Spectral_F:  return ( 6000 + mSubSpectrum* 200); break;
	case Spectral_G:  return ( 5000 + mSubSpectrum* 100); break;
	case Spectral_R:  
	case Spectral_C: 
	case Spectral_K:  return ( 4000 + mSubSpectrum* 100); break;
	case Spectral_N:  
	case Spectral_S:  
	case Spectral_M:  return ( 2000 + mSubSpectrum* 200); break;
	case Spectral_L:  return ( 1300 + mSubSpectrum*  70); break;
	case Spectral_T:  return (  200 + mSubSpectrum* 110); break;
	case Spectral_D:  
	case Spectral_DA: return ( 5000 + mSubSpectrum* 5000); break; // ?
	case Spectral_DB: return (11000 + mSubSpectrum* 1700); break;
	case Spectral_DC: // ?
	case Spectral_DO: return (45000 + mSubSpectrum* 6000); break;
	case Spectral_DQ: // ?
	case Spectral_DZ: // ?
	case Spectral_DX: return ( 5000 + mSubSpectrum* 5000); break; // ?
	case Spectral_NS: return (1000000);   break; // superieur � 600.000
	case Spectral_BH: return (1);         break; // depend normalement de sa masse
	case Spectral_P:  return (500);       break; // entre 300 et 1000
	case Spectral_Q:  return (1200000);   break; // superieur � 1.000.000
	case Spectral_Unknown: return (6000); break; // le plus frequent
	default: return (6000);
    };

}

/* ------------------------------------------------------------------------- */
/// Renvoie la lettre correspondant au spectre de l'�toile.
/* ------------------------------------------------------------------------- */
String CatalogStar::getSpectralType() 
{
    switch (mSpectrum)
    {
	case Spectral_O:  return "O"; break;
	case Spectral_B:  return "B"; break;
	case Spectral_A:  return "A"; break;
	case Spectral_F:  return "F"; break;
	case Spectral_G:  return "G"; break;
	case Spectral_K:  return "K"; break;
	case Spectral_M:  return "M"; break;
	case Spectral_S:  return "S"; break;
	case Spectral_R:  return "R"; break;
	case Spectral_N:  return "N"; break;
	case Spectral_C:  return "C"; break;
	case Spectral_WC: return "W"; break;
	case Spectral_WN: return "W"; break;
	case Spectral_L:  return "L"; break;
	case Spectral_T:  return "T"; break;
	case Spectral_DA:
	case Spectral_DB:
	case Spectral_DC:
	case Spectral_DO:
	case Spectral_DQ:
	case Spectral_DZ:
	case Spectral_D:
	case Spectral_DX: return "Dwarf";      break;
	case Spectral_NS: return "Pulsar";     break;
	case Spectral_BH: return "Black Hole"; break;
	case Spectral_P:  return "Nebula";     break;
	case Spectral_Q:  return "Nova";       break;
	case Spectral_Unknown: return "?";     break;
	default: return "?";
    };
}


/* ------------------------------------------------------------------------- */
/// Cr��e et affiche le sprite de l'�toile.
/* ------------------------------------------------------------------------- */
void CatalogStar::show(BillboardSet* starsSet)
{
	// Les trous noirs ne sont pas visibles
	if (mSpectrum != Spectral_BH)
	{
		mSprite = starsSet->createBillboard(mPosition);
		mSprite->setDimensions(10000,10000);
	}
}


/* ------------------------------------------------------------------------- */
/// Efface (et d�truit) le sprite de l'�toile.
/* ------------------------------------------------------------------------- */
void CatalogStar::hide(BillboardSet* starsSet)
{
	if (mSprite) starsSet->removeBillboard(mSprite);
	mSprite = NULL;
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'age de l'�toile (years).
/** 
Cet age n'est pas calcul�, mais est d�finit al�atoirement, en respectant la distribution
constat�e dans l'univers. Il n'existe pas de loi pour calculer l'age d'une �toile.
@sa  Star age distribution from John Dollan 
24%		= 1e0  - 1e9 
14%		= 1e9  - 2e9
9%		= 2e9  - 3e9
12%		= 3e9  - 4e9
8%		= 4e9  - 5e9
7%		= 5e9  - 6e9
6%		= 6e9  - 7e9
3%		= 7e9  - 8e9
6%		= 8e9  - 9e9
5%		= 9e9  - 10e9
5%		= 10e9 - 11e9
*/
/* ------------------------------------------------------------------------- */
Real CatalogStar::getAge()
{
	short interpol = (int)(abs(cos(mCatalogNumber+28)*1000)) % 100; // Ajust� pour Sol.

	if (interpol <1)	return 1e3;
	if (interpol <2)	return 1e4;
	if (interpol <3)	return 1e5;
	if (interpol <4)	return 1e6;
	if (interpol <5)	return 1e7;
	if (interpol <10)	return 1e8;
	if (interpol <24)	return 1e9;
	if (interpol <38)	return 2e9;
	if (interpol <47)	return 3e9;
	if (interpol <59)	return 4e9;
	if (interpol <67)	return 5e9;
	if (interpol <74)	return 6e9;
	if (interpol <80)	return 7e9;
	if (interpol <83)	return 8e9;
	if (interpol <89)	return 9e9;
	if (interpol <94)	return 10e9;
	if (interpol <99)	return 11e9;
	return 12e9;  // cad if (interpol <100)
}