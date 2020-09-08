/* ------------------------------------------------------------------------- */
// File       : ss_SolarSystem.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "ss_SolarSystem.h"
#include "Const.h"
#include "macros.h"
#include "lib_stargen/SG_SolarSystem.h"
#include "PlanetTexturer.h"
#include "Clock.h"
#include <fstream.h>

#include "movableText.h"
#include "Circle3D.h"

/* ------------------------------------------------------------------------- */
/// Contructeur
/* ------------------------------------------------------------------------- */
SolarSystem::SolarSystem(SceneNode* ParentNode, SceneManager* SceneMgr, CatalogStar* PrimaryStar)
{
	mParentNode   = ParentNode;
	mSceneMgr     = SceneMgr;
	mPrimaryStar  = PrimaryStar;
	mPlanetNumber = 0;

	// On initialise le tableau des planètes.
	for (int i=0; i<MAX_PLANETS; i++) mPlanets[i]=NULL;

	// On cree un Node pour le centre du Système Solaire
	mNode = mParentNode->createChildSceneNode();
	DEBUG_LOG("Solar System Position = "+ITOA(mNode->getPosition()));
	
	// On cree l'étoile centrale du système solaire
	mStar      = new Star(mPrimaryStar, mNode, mSceneMgr);
	mCatalogID = mPrimaryStar->getCatalogNumber();
	mName      = mStar->getName();

	/// On genere le fichier XML contenant toutes les données sur le systeme planetaire.
	this->generateSystemXMLfile(mCatalogID);
	/// On importe le fichier XML contenant les données sur les planetes.
	this->createSystem(mCatalogID);

	// test
	MovableText* msg = new MovableText(mName+"MT",mName); 
	mNode->attachObject(msg);

};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
SolarSystem::~SolarSystem()
{
	if (mStar)   delete(mStar);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction change la position du Syteme Solaire.
/* ------------------------------------------------------------------------- */
void SolarSystem::setPosition(Vector3 pos)
{
	mNode->setPosition(pos);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction change les caractéristiques de l'étoile principale du systeme solaire (debug).
/* ------------------------------------------------------------------------- */
void SolarSystem::_changeStar()
{
	mStar->_changeSpectrum();
}


/* ------------------------------------------------------------------------- */
/// Allume (ou éteint) la lumière de l'étoile principale du système solaire.
/* ------------------------------------------------------------------------- */
void SolarSystem::setStarLight(bool on)
{
	mStar->setStarLight(on);
}

/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la couleur de l'étoile principale du Système Solaire.
/* ------------------------------------------------------------------------- */
ColourValue SolarSystem::getStarColour()
{
	return mStar->getColour();
}


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la position du Système Solaire par rapport au GalaxyBox.
/* ------------------------------------------------------------------------- */
Vector3 SolarSystem::getPosition()
{
	return mNode->getPosition();
}


/* ------------------------------------------------------------------------- */
/// Cette fonction genère un système Solaire avec la librairie StarGen.
/* ------------------------------------------------------------------------- */
void SolarSystem::generateSystemXMLfile(int CatNo)
{
	DEBUG_LOG(">> SolarSystem::generateSystemXMLfile()");

	String filename = "../media/data/system_"+ITOA(CatNo)+".xml";

	// Si le fichier existe déjà, on ne le regénère pas.
	ifstream fichier( filename.c_str() ); 
	if (fichier!=NULL) return; 

	SG_SolarSystem* SG = new SG_SolarSystem(CatNo);

	Real mass = mPrimaryStar->getMass();
	SG->getStar()->setMass(mass);  // determine: Mass + Luminosity + Ecosphere + Life

	Real bolomag = mPrimaryStar->getBolometricMagnitude();
	SG->getStar()->setMagnitude(bolomag); // determine: Luminosity (ajustement plus précis)

	Real age = mPrimaryStar->getAge();
	SG->getStar()->setAge(age);    // determine : Age

	if (CatNo==0)
		SG->generateSolarSystem(filename); // Cette fonction génère un meilleur système solaire
	else
		SG->generateSystem(filename);

	delete(SG);
}
 

/* ------------------------------------------------------------------------- */
/// Renvoie l'ID catalogue du systeme solaire.
/* ------------------------------------------------------------------------- */
int SolarSystem::getCatalogID()
{
	return mCatalogID;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le nom du systeme solaire (cad le nom de son étoile).
/* ------------------------------------------------------------------------- */
String SolarSystem::getName()
{
	return mName;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le nombre de planetes du systeme solaire.
/* ------------------------------------------------------------------------- */
int SolarSystem::getPlanetNumber()
{
	return mPlanetNumber;
}


/* ------------------------------------------------------------------------- */
/// Creation du système solaire.
/** Cette fonction lit le fichier XML (format StarGenII) contenant les données 
sur les planètes du système, et crée les planètes qui y sont décrites. */
/* ------------------------------------------------------------------------- */
void SolarSystem::createSystem(int CatNo)
{
	String filename = "../media/data/system_"+ITOA(CatNo)+".xml";
	Stargen_XMLparser* ss_file =  new Stargen_XMLparser(filename);

	ss_file->gotoStar();
	mPlanetNumber = ss_file->getIntAttribute("planets");

	for (int i=1; i<=mPlanetNumber; i++)
	{
		this->createPlanet(ss_file, i);
	}
	
	delete (ss_file);
}

/* ------------------------------------------------------------------------- */
/// On ajoute une planete au systeme solaire, à partir des données du fichier XML.
/** 
@param ss_file  Le fichier issu de StarGen décrivant tout le systeme solaire
@param index    Le No de la planete [1..N] dans ce fichier (par  orbite croissante).
*/
/* ------------------------------------------------------------------------- */
void SolarSystem::createPlanet(Stargen_XMLparser* ss_file, int index)
{
	DEBUG_LOG(">> addPlanet()");

	ss_file->gotoPlanet(index);

	String  planetName  = mName+"-"+toRoman(index);
	Real    planetOrbit = ss_file->getRealAttribute("orbit","radius"); // unit = UA
	Real    planetTilt  = ss_file->getRealAttribute("orbit","tilt");   // unit = degree
	Real    planetRadius= ss_file->getRealAttribute("physical", "radius");
	bool    planetAtmos = ss_file->getBooleanAttribute("general", "atmosphere", false);
	bool    hasMetal    = ss_file->getBooleanAttribute("composition", "metal", false);
	String  planetGType = ss_file->getPlanetAttribute ("general", "type");
	Real    planetYear  = ss_file->getRealAttribute("durations", "yearE");  // unit = Earth day
	Real    planetPress = ss_file->getRealAttribute("surface", "pressure"); // unit = Earth atm
	Real    planetGround= ss_file->getRealAttribute("coverage", "ground");  // unit = %
	Real    planetWater = ss_file->getRealAttribute("coverage", "water");   // unit = %
	Real    planetIce   = ss_file->getRealAttribute("coverage", "ice");     // unit = %
	Real    planetClouds= ss_file->getRealAttribute("coverage", "clouds");  // unit = %
	int     planetType;
	ColourValue AtmColor= ColourValue(1,1,1); /// TODO
	
	// Ces constantes sont définies dans PlanetTexturer.h
	if      (planetGType=="Rock" && hasMetal)  planetType=ROCK;    // Type Mercure     
	else if (planetGType=="Rock" && !hasMetal) planetType=ROCK_2;  // Type Pluton
	else if (planetGType=="Venusian")          planetType=VENUSIAN;// Type Venus
	else if (planetGType=="Terrestrial")planetType=TERRESTRIAL;    // Type Terre
	else if (planetGType=="Martian")    planetType=MARTIAN;/// TODO : créer un gradient spécifique
	else if (planetGType=="Water")      planetType=WATER;  /// TODO : créer un gradient spécifique
	else if (planetGType=="Ice")        planetType=ICE;    /// TODO : créer un gradient spécifique
	else if (planetGType=="GasGiant")   planetType=GAZ_BROWN;      // Type Jupiter
	else if (planetGType=="GasPlanet")  planetType=GAZ_BLUE;       // Type Neptune
	else if (planetGType=="GasDwarf")   planetType=GAZ_RED; /// TODO : créer un gradient spécifique
	else  
	{	/* planetGType=="Unknown"*/
		/* planetGType=="Asteroids"*/
		planetType=ROCK;           
		/// TODO : pas de planete!
	}

	Planet* newPlanet = new Planet(planetName, index, mNode, mSceneMgr);
	newPlanet->setParameters(mCatalogID, planetType, planetRadius, planetPress);
	newPlanet->setOrbit(planetOrbit, planetYear, planetTilt);
	newPlanet->setOrbitalPosition(Clock::getSingletonPtr()->getCurrentDate());
	newPlanet->setCoverage(planetGround, planetWater, planetIce, planetClouds);
	newPlanet->setAtmColour(AtmColor);
	newPlanet->setLightColor(ColourValue::White);  //this->getStarColour());
	newPlanet->createTexture();

	mPlanets[index]=newPlanet;

	Circle3D* orb = new Circle3D(mSceneMgr, mNode, planetOrbit*ORBIT_WU_PER_AU);
	orb->draw();
	orb->setColour(ColourValue(0.09, 0.57, 0.40, 0.4));

	DEBUG_LOG("<< addPlanet()");
}

/* ------------------------------------------------------------------------- */
/// Cette fonction convertit de chiffres arabes en chiffres romains (0..99).
/* ------------------------------------------------------------------------- */
String SolarSystem::toRoman(int number)
{
	int unite=number%10;
	int diz  =number-unite; 
	String roman="";

	switch(diz) 
	{
	case 0  : roman+= "";    break;
	case 10 : roman+= "X";   break;
	case 20 : roman+= "XX";  break;
	case 30 : roman+= "XXX"; break;
	case 40 : roman+= "XL";  break;
	case 50 : roman+= "L";   break;
	case 60 : roman+= "LX";  break;
	case 70 : roman+= "LXX"; break;
	case 80 : roman+= "LXXX";break;
	case 90 : roman+= "XC";  break;
	};

	switch(unite) 
	{
	case 0 : roman+= "";    break;
	case 1 : roman+= "I";   break;
	case 2 : roman+= "II";  break;
	case 3 : roman+= "III"; break;
	case 4 : roman+= "IV";  break;
	case 5 : roman+= "V";   break;
	case 6 : roman+= "VI";  break;
	case 7 : roman+= "VII"; break;
	case 8 : roman+= "VIII";break;
	case 9 : roman+= "IX";  break;
	};

	return roman;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction affiche le systeme solaire.
/* ------------------------------------------------------------------------- */
void SolarSystem::show()
{
	if (mStar)   mStar->show();
	for (int i=0; i<mPlanetNumber; i++) 
	{
		if (mPlanets[i]) mPlanets[i]->show();
	}
}

/* ------------------------------------------------------------------------- */
/// Cette fonction masque le systeme solaire.
/* ------------------------------------------------------------------------- */
void SolarSystem::hide()
{
	if (mStar)   mStar->hide();
	for (int i=0; i<mPlanetNumber; i++) 
	{
		if (mPlanets[i]) mPlanets[i]->hide();
	}
}


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la référence de l'étoile principale du système.
/* ------------------------------------------------------------------------- */
Star* SolarSystem::getStar()
{
	return mStar;
}