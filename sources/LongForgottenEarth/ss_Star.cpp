/* ------------------------------------------------------------------------- */
// File       : ss_Star.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "ss_Star.h"

#include "macros.h"
#include "classes.h"
#include "Gradient.h"
#include "fstream.h"
#include "constellation.h"

#define SOLAR_RADIUS_IN_KM     (696000)     /* km      */
#define SOLAR_TEMPERATURE      (6000)       /* Kelvin  */
#define MAX_INTENSITY           0.1

/* ------------------------------------------------------------------------- */
/// Contructeur
/** 
Dans le constructeur, on fait apparaitre uniquement le sprite de l'�toile.
On charge les informations de l'�toile du catalogue. 
*/
/* ------------------------------------------------------------------------- */
Star::Star(CatalogStar* RefStar, SceneNode* ParentNode, SceneManager* SceneMgr)
{

	mRefStar    = RefStar;
	mParentNode = ParentNode;
	mSceneMgr   = SceneMgr;
	mIntensity  = MAX_INTENSITY;

	if (!mParentNode) return;
	if (!mSceneMgr)   return;
	if (!mRefStar)    return;

	// -------------------------------
	// On renseigne et on calcule les donn�es physiques de l'�toile.
	// -------------------------------
	mSpectrum    = mRefStar->getSpectralType();
	mMass        = mRefStar->getMass();
	mTemperature = mRefStar->getTemperature();
	mName        = this->getPrimaryStarName(mRefStar);
	mLuminosity  = this->calculateLuminosity(mRefStar->getBolometricMagnitude());
	mWaveLength  = this->calculateWavelength(mTemperature);
	mStarColor   = this->calculateColour(mWaveLength);


	// ----------------------------------
	// On cree un Node pour la lumiere
	// ----------------------------------
	mStarNode = mParentNode->createChildSceneNode();
	mStarNode->setInheritScale(false);
	mStarNode->setPosition(0,0,0); // La position de l'�toile est au centre du systeme solaire


	// -----------------------------------------------------
	// On cree un ensemble de sprites pour l'�toile (1 ou 2 sprites)
	// -----------------------------------------------------
	mBillboardset= mSceneMgr->createBillboardSet("system/star");
	mBillboardset->setMaterialName("stars/F");
	mBillboardset->setCullIndividually(true);
	mBillboardset->setQueryFlags(VOIDOBJECT);	// Ils ne sont pas detect�s par les rayons.
	mStarNode->attachObject(mBillboardset);

	// -------------------------------
	// On fait apparaitre le sprite
	// -------------------------------
	mBillboardset->setVisible(true);
	mSprite = mBillboardset->createBillboard(0,0,0);
	Real DimSprite = 5000;
	mSprite->setDimensions(DimSprite,DimSprite);
	
	// -------------------------------
	// On cree la lumiere
	// -------------------------------
	mLight = mSceneMgr->createLight(mName);
	// Position de la lumi�re p/r au Node
	mLight->setPosition(0,0,0); 
	mLight->setType(Light::LT_POINT);
	mLight->setCastShadows(false);		// Cette lumiere ne projette pas d'ombres
	mLight->setAttenuation(1e7,1,0,0);  // Cette lumi�re porte tr�s loin

	mLight->setDiffuseColour(mStarColor * mIntensity);
	mSceneMgr->setAmbientLight(mStarColor * mIntensity*mIntensity);
	mSprite->setColour(mStarColor);

};

/* ------------------------------------------------------------------------- */
/// Destructeur
/** Cette fonction est appel�e quand on quitte le systeme solaire. */
/* ------------------------------------------------------------------------- */
Star::~Star()
{
	mSceneMgr->removeLight(mLight);
	mSceneMgr->removeBillboardSet(mBillboardset); // attention lire les commentaires de la fonction
	mParentNode->removeChild(mStarNode); /// TODO : le node doit etre d�truit
}


/* ------------------------------------------------------------------------- */
/// Cette fonction fait apparaitre l'�toile.
/** 
Le sprite est toujours pr�sent, mais quand on est proche de l'�toile, 
on peut faire apparaitre sa lumi�re et ses planetes.
*/
/* ------------------------------------------------------------------------- */
void Star::show()
{
	// On attache la lumi�re au Node
	mStarNode->attachObject(mLight);
	mLight->setVisible(true);
	mBillboardset->setVisible(true);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction fait disparaitre l'�toile de la scene.
/* ------------------------------------------------------------------------- */
void Star::hide()
{
	mLight->setVisible(false);
	mBillboardset->setVisible(false);
}


/* ------------------------------------------------------------------------- */
/// On positionne l'�toile.
/* ------------------------------------------------------------------------- */
void Star::setPosition(Vector3 pos)
{
	mStarNode->setPosition(pos);
}


/* ------------------------------------------------------------------------- */
/// On calcule la longueur d'onde du rayonnement de l'�toile de l'�toile
/** 
	@param Temperature Temperature of the star (unit = Kelvin)
	@return La longueur d'onde du rayonnement de l'�toile de l'�toile (Units = nanometre)
*/
/* ------------------------------------------------------------------------- */
Real Star::calculateWavelength(Real Temperature)
{
	/// TODO = calculs
	Real L = 2898000.0 / Temperature;
	TRACE("Star Wavelength = "+StringConverter::toString(L));
	return L;
}


/* ------------------------------------------------------------------------- */
/// On calcule la couleur de l'�toile en fonction de la longueur d'onde des rayonnements �mis.
/**  
	Le rayonnement normalement visible s'�tend de 380nm � 780nm.
	On "corrige" un peu cette plage de fa�on � voir dans le jeu, les rayonnements de 
	70nm � 1400nm. On n'utilise pas d'�quations mais un gradient (spectrum.svg).
	Moins r�aliste, mais plus souple d'utilisation.

	@param Wavelength Longeur d'onde du rayonnement de l'�toile (Unit = Nanometer)
	@return La couleur du rayonnement de l'�toile (corrig�e).
*/
/* ------------------------------------------------------------------------- */
ColourValue Star::calculateColour(Real Wavelength)
{
	ColourValue couleur;

	// On "corrige" la longueur d'onde pour l'amener dans le spectre visible.
	if (Wavelength < 439) Wavelength = (0.16)*Wavelength +369;	//70-439   => 380-439
																//439-700  => 439-700
	if (Wavelength > 700) Wavelength = (0.14)*Wavelength +600;	//700-1440 => 700-800
	DEBUG_LOG ("Corrected Wavelength = "+StringConverter::toString(Wavelength));
	
	// On utilise un gradient pour transformer cette valeur en couleur.
	// C'est une solution assez souple.
	// On transforme les nanom�tres en une valeur [0..1]
	Real G_Offset = Wavelength/1000; 
	
	Gradient* Spectrum = new Gradient("spectrum.svg");
	couleur = Spectrum->getColour(G_Offset); 
	couleur = this->adjustColour(couleur, 0.3);
	return (couleur);
}
	
/* ------------------------------------------------------------------------- */
/// Cette fonction transforme un rayonnement en sa couleur r�elle.
/**  
	Cette fonction calcule la couleur r�elle du rayonnement. 
	On ne l'utilise pas car les couleur obtenues ne sont pas satisfaisantes pour le jeu.
	(trop de rayonnement invisible).
	@param Wavelength Longeur d'onde du rayonnement (Unit = Nanometer)
	@return La couleur r�elle du rayonnement.
*/
/* ------------------------------------------------------------------------- */
ColourValue Star::_wavelength2colour(Real Wavelength)
{
    int factor;
    Real Blue, Green, Red;

	if (Wavelength < 380)
	{	// En dessous de 380nm, la lumi�re est invisible (noire).
		Red   = 0.0;
		Green = 0.0;
		Blue  = 0.0;
	}
	else if (Wavelength < 439)
	{
		Red   = -(Wavelength - 440) / (440 - 380);
		Green = 0.0;
		Blue  = 1.0;
	}
	else if (Wavelength < 489)
	{
		Red   = 0.0;
		Green = (Wavelength - 440) / (490 - 440);
		Blue  = 1.0;
	}
	else if (Wavelength < 509)
	{
		Red   = 0.0;
		Green = 1.0;
		Blue  = -(Wavelength - 510) / (510 - 490);
	}
	else if (Wavelength < 579)
	{	
		Red   = (Wavelength - 510) / (580 - 510);
		Green = 1.0;
		Blue  = 0.0;
	}
	else if (Wavelength < 644)
	{
		Red   = 1.0;
		Green = -(Wavelength - 645) / (645 - 580);
		Blue  = 0.0;
	}
	else if (Wavelength < 780)
	{
		Red   = 1.0;
		Green = 0.0;
		Blue  = 0.0;
	}
	else
	{
		Red   = 0.0;
		Green = 0.0;
		Blue  = 0.0;
	}

	/************************************************************************/
	/* Att�nuation:                                                         */
	/*  - sous 380nm = invisible                                            */
	/*  - sous 420nm = des UV peu visibles                                  */
	/*  - sous 700nm = les couleurs visibles(non att�nu�es)                 */
	/*  - sous 780nm = les IR peu visibles                                  */
	/*  - au dela de 780nm = invisible                                      */
	/************************************************************************/
	// Let the intensity fall off near the vision limits
	if (Wavelength < 380)		factor = 0.0; 
	else if (Wavelength < 419)	factor = 0.3 + 0.7*(Wavelength - 380) / (420 - 380);
	else if (Wavelength < 700)	factor = 1.0; 
	else if (Wavelength < 780)	factor = 0.3 + 0.7*(780 - Wavelength) / (780 - 700); 
	else						factor = 0.0; 

	factor=1;

	return this->_adjustColour(Red, Green, Blue, factor, 0.3);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction corrige la couleur en fonction de la visibilit� de la couleur et du gamma.
/** 
	On applique un Facteur d'att�nuation de la lumiere quand on approche des rayonnements 
	invisibles � l'oeil nu (infrarouges, ou ultraviolets). 
	La fonction prend aussi en compte le Gamma.
	@param R      Composante Rouge de la couleur � att�nuer [0..1].
	@param G      Composante Verte de la couleur � att�nuer [0..1].
	@param B      Composante Bleue de la couleur � att�nuer [0..1].
	@param Factor Facteur d'att�nuation.
	@param Gamma  Facteur de luminosit� [0..1] (les faibles valeurs augmentent la luminosit�).
	@return La couleur modifi�e.
*/
/* ------------------------------------------------------------------------- */
ColourValue Star::_adjustColour(Real R, Real G, Real B, Real Factor, Real Gamma)
{
	Real adjustedR, adjustedG, adjustedB;

	// We don't want 0^g=1 for g<>0
    if (R == 0.0)	adjustedR = 0;     
	else			adjustedR = pow(R * Factor, Gamma);
    if (G == 0.0)	adjustedG = 0;
    else			adjustedG = pow(G * Factor, Gamma);
    if (B == 0.0)	adjustedB = 0;
    else			adjustedB = pow(B * Factor, Gamma);

	return ColourValue(adjustedR, adjustedG, adjustedB);
};



/* ------------------------------------------------------------------------- */
/// Cette fonction corrige la couleur en fonction du gamma (augmente ou diminue sa luminosit�).
/** 
	Note: les faibles valeurs de Gamma augmentent la luminosit�.
	@param couleur La couleur initiale.
	@param Gamma   Facteur de luminosit� [0..1]
	@return La couleur modifi�e.
*/
/* ------------------------------------------------------------------------- */
ColourValue Star::adjustColour(ColourValue Couleur, Real Gamma)
{
	Real adjustedR, adjustedG, adjustedB;

	// si r, g, b est egal a z�ro, on ne veut pas que cette composante passe � 1.
	// (car 0^g = 1, en g�n�ral, si g!=0) 
    if (Couleur.r == 0)	adjustedR = 0; else adjustedR = pow(Couleur.r, Gamma);
    if (Couleur.g == 0)	adjustedG = 0; else adjustedG = pow(Couleur.g, Gamma);
    if (Couleur.b == 0)	adjustedB = 0; else adjustedB = pow(Couleur.b, Gamma);

	return ColourValue(adjustedR, adjustedG, adjustedB);
};


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la couleur de la lumi�re �mise par cette �toile.
/* ------------------------------------------------------------------------- */
ColourValue Star::getColour()
{
	return mStarColor;
}


/* ------------------------------------------------------------------------- */
/// Fonction de debug pour tester toutes les vari�t�s de spectres.
/* ------------------------------------------------------------------------- */
void Star::_changeSpectrum()
{
	if (mSpectrum == "O")
		mRefStar->setSpectralType(CatalogStar::Spectral_B); 
	else if (mSpectrum == "B")
		mRefStar->setSpectralType(CatalogStar::Spectral_A); 
	else if (mSpectrum == "A")
		mRefStar->setSpectralType(CatalogStar::Spectral_F); 
	else if (mSpectrum == "F")
		mRefStar->setSpectralType(CatalogStar::Spectral_G); 
	else if (mSpectrum == "G")
		mRefStar->setSpectralType(CatalogStar::Spectral_K); 
	else if (mSpectrum == "K")
		mRefStar->setSpectralType(CatalogStar::Spectral_M); 
	else if (mSpectrum == "M")
		mRefStar->setSpectralType(CatalogStar::Spectral_L); 
	else if (mSpectrum == "L")
		mRefStar->setSpectralType(CatalogStar::Spectral_T); 
	else if (mSpectrum == "T")
		mRefStar->setSpectralType(CatalogStar::Spectral_O); 
	
	String sp = mRefStar->getSpectralType();
	DEBUG_LOG ("*** Star Spectrum Class is now "+ sp);
	DEBUG_WIN ("*** Star Spectrum Class is now "+ sp);

	
	mTemperature= mRefStar->getTemperature();
	mWaveLength = this->calculateWavelength(mTemperature);
	mStarColor  = this->calculateColour(mWaveLength);
	mSprite->setColour(mStarColor);

	mLight->setDiffuseColour(mStarColor*mIntensity);
//	mSceneMgr->setAmbientLight(mStarColor*mIntensity);
}

/* ------------------------------------------------------------------------- */
/// Allume ou �teint la lumi�re de l'�toile.
/* ------------------------------------------------------------------------- */
void Star::setStarLight(bool on)
{
	mLight->setVisible(on);
}



/* ------------------------------------------------------------------------- */
/// Allume ou �teint la lumi�re de l'�toile progressivement.
/** 
On utilise un fading pour �teindre la lumi�re progressivement quand on ferme une porte, par exemple.
@param on Mettre TRUE si on veut allumer la lumi�re, ou FALSE si on veut l'�teindre.
@return La fonction renvoie TRUE, qunad l'op�ration de fading est termin�e. 
*/
/* ------------------------------------------------------------------------- */
bool Star::fadeStarLight(bool on)
{
	bool done = false;

	if (on)
	{
		mIntensity += 0.01;
		if (mIntensity>=MAX_INTENSITY) 
		{
			mIntensity=MAX_INTENSITY;
			done=true;
		}
	}
	else
	{
		mIntensity -= 0.01;
		if (mIntensity<=0)
		{
			mIntensity=0;
			mLight->setVisible(false);
			done=true;
		}
	}
	mLight->setDiffuseColour(mStarColor*mIntensity);
	return done;
}


/* ------------------------------------------------------------------------- */
/// Return the radius of the star.
/** 
The function use the Stefan-Boltzmann law to estimate the radius of a star 
from its surface temperature and luminosity.
@return unit = Kilometers
*/
/* ------------------------------------------------------------------------- */
Real Star::getRadius() 
{        
    return SOLAR_RADIUS_IN_KM * sqrt(mLuminosity) * pow(SOLAR_TEMPERATURE/mTemperature, 2);
}

/* ------------------------------------------------------------------------- */
/// Return the temperature of the star.
/** @return unit = kelvin */
/* ------------------------------------------------------------------------- */
Real Star::getTemperature() 
{        
    return mTemperature;
}

/* ------------------------------------------------------------------------- */
/// D�termine la masse de l'�toile.
/** @param mass unit = solar mass */
/* ------------------------------------------------------------------------- */
void Star::setMass(Real mass)
{
	mMass = mass;
}

/* ------------------------------------------------------------------------- */
/// Renvoie la luminosit� de l'�toile.
/** @return La luminosit� de l'�toile (unit = solar luminosity) */
/* ------------------------------------------------------------------------- */
Real Star::getLuminosity()
{
	return mLuminosity;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le nom de l'�toile.
/* ------------------------------------------------------------------------- */
String Star::getName()
{
	return mName;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le Type Spectral de l'�toile.
/* ------------------------------------------------------------------------- */
String Star::getSpectralType()
{
	return mRefStar->getSpectralType();
}


/* ------------------------------------------------------------------------- */
/// Calculate the luminosity of the star, based on its bolometric magnitude.
/** 
Luminosity (L) is related to bolometric magnitude (Mbol) by the formula: 
	Mbol = 2.5 log (L/Lsun) + 4.72 
	L    = exp((Mbol -4.72)/2.5) * Lsun
	Mbolsun = 4.72

The bolometric magnitude is the magnitude of a star measured across all wavelengths, 
so that it takes into account the total amount of energy radiated. 
If a star is a strong infrared or ultraviolet emitter, its bolometric magnitude will 
differ greatly from its visual magnitude. 

@param magnitude The bolometric magnitude of the star 
@return The luminosity of the star (unit = solar luminosity)
*/
/* ------------------------------------------------------------------------- */
Real Star::calculateLuminosity(Real bolometricmagnitude)
{
    return exp((bolometricmagnitude - 4.72) / 2.5);
}


/* ------------------------------------------------------------------------- */
/// On r�cup�re le nom de l'�toile primaire du syst�me.
/** 
Le nom de 3000 �toiles est dans le fichier starnames.dat.
Le format est index:Nom1:Nom2:etc
Le nom est au format = No ou lettre grecque puis abr�viation de la constellation.
On n'utilise que le premier nom (le plus usuel).
- 32349:Sirius:Alhabor:ALF CMa:9 CMa:Gliese 244
- 32362:XI Gem:31 Gem
- 32404:32 Gem
- 32768:TAU Pup
*/
/* ------------------------------------------------------------------------- */
String Star::getPrimaryStarName(CatalogStar* RefStar)
{
	String   filename = "../media/data/starnames.dat";
	ifstream starNameFile;
	char     number[10];
	char     name[128];
	int      CatNo;
	bool     found;
	bool     eof;

	starNameFile.open(filename.c_str(), ios::in);
	if (!starNameFile.good())
	{
		TRACE ("Error opening "+filename);
		return "";
	}

	CatNo = RefStar->getCatalogNumber();

	do 
	{
		starNameFile.getline(number, 10, ':');
		starNameFile.getline(name, 128);
		found = (atoi(number)==CatNo);
		eof   = starNameFile.eof();
	} while(!found && !eof);

	starNameFile.close();

	String firstName;
	if (found) 
	{
		// On recherche un �ventuel caract�re ":" dans le nom de l'�toile
		int separatorPos = String(name).find(':',0);
		// S'il y en a un, on ne garde que le nom le plus usuel situ� avant le s�parateur.
		firstName = String(name).substr(0,separatorPos);
		TRACE("Star short name = "+firstName);
		firstName = this->expandStarName(firstName);
	}
	else
	{
		firstName = "GGC"+ITOA(CatNo);  /// GGC = Global Galactic Catalog
	}
	TRACE("Star expanded Name = "+firstName);
	return firstName;
}



/* ------------------------------------------------------------------------- */
/// Transforme le nom abr�g� de l'�toile en un nom complet.
/** Cette fonction reconnait les designations Bayer et Flamsteed 
(une lettre grecque suivie de l'abreviation de la constellation).
*/
/* ------------------------------------------------------------------------- */
String Star::expandStarName(String name)
{
    unsigned int pos = name.find(' ');

	// S'il y a un blanc, le nom est sous la forme "prefixe constellation"
    if ((pos!=0) && (pos!=String::npos) && (pos<name.length()-1))
    {
		// On s�pare le nom de l'�toile en un "prefix" et un nom de constellation
        String prefix(name, 0, pos);
        String conName(name, pos + 1, String::npos);
        Constellation* constellations = new Constellation;
		int constelID = constellations->getConstellation(conName);
        if (constelID != -1)
        {
			// S'il s'agit d'une constellation connue, on r�cup�re son nom
			conName = constellations->getGenitive(constelID);
		}

		char digit = ' ';
		int len = prefix.length();

		// If the first character of the prefix is a letter
	    // and the last character is a digit, we may have
		// something like 'Alpha2 Cen' . . . Extract the digit
		// before trying to match a Greek letter.
		if (len>2 && isalpha(prefix[0]) && isdigit(prefix[len-1]))
		{
			len--;
			digit = prefix[len];
		}

		// We see if the first part of the name is a greek letter.
		String letter = constellations->getGreekLetter(prefix.substr(0,len));
		if (letter != "") prefix = letter;

		delete constellations;

		if (digit == ' ') return prefix+" "+conName;
		else return prefix+digit+" "+conName;
	}
	else
		// sinon, on ne change pas le nom
		return name;
}

