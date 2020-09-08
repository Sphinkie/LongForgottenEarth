/* ------------------------------------------------------------------------- */
// File       : ss_Planet.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "ss_Planet.h"

#include "macros.h"
#include "PlanetTexturer.h"
#include "PlanetTexturer_Ice.h"
#include "PlanetTexturer_Rock.h"
#include "PlanetTexturer_Gas.h"
#include "PlanetTexturer_Granit.h"
#include "PlanetTexturer_Venusian.h"
#include "Const.h"

#include <fstream.h>

/* ------------------------------------------------------------------------- */
/// Contructeur
/** 
@param Name       Le nom de la planete (par exemple Sol_III)
@param Rank       Le numéro de la planète [1..N] (par  orbite croissante).
@param ParentNode Le node central du Système planétaire
@param SceneMgr   Le scene manager pour lui ajouter les entity.
/* ------------------------------------------------------------------------- */
Planet::Planet(String Name, int Rank, SceneNode* ParentNode, SceneManager* SceneMgr)
{
	mName       = Name;
	mParentNode = ParentNode;
	mSceneMgr   = SceneMgr;
	mStarColour = ColourValue::White;
	mPlanetType = 0;
	mCatalogID  = 0;
	mRank       = Rank;
	mPlanetID   = "00000_00";

	if (!mParentNode) return;
	mPlanetNode = mParentNode->createChildSceneNode();
//	mPlanetEntity=mSceneMgr->createEntity(mName, "planet.mesh");
//	mPlanetEntity->setNormaliseNormals(true); 
//	mPlanetNode->attachObject(mPlanetEntity);
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Planet::~Planet()
{
	mSceneMgr->removeEntity(mPlanetEntity);
	mParentNode->removeChild(mParentNode);
}


/* ------------------------------------------------------------------------- */
/// Genère la planete et la fait apparaitre.
/** La fonction setParameter doit avoir été appelée avant, afin que tous les 
paramètres nécessaires à la création de la texture soient présents. */
/* ------------------------------------------------------------------------- */
void Planet::createTexture()
{
	DEBUG_LOG(">> Planet::createTexture()");

	bool   normalMapped = true;
	String TextureName = "../media/materials/planets/"+mPlanetID; // nom sans extension
	/// TODO : génerer éventuellement la normalmap
	/// TODO : Gérer les dimensions de la map generée.

    ifstream fichier( (TextureName+".bmp").c_str() ); 
	// Si la texture n'existe pas, on la crée
    if (fichier==NULL) 
	{
		switch(mPlanetType) 
		{
		case TERRESTRIAL    : 
		case TERRESTRIAL_2  : 
			{
				PlanetTexturer* T = new PlanetTexturer();
				T->setSeed(mCatalogID+mRank);
				T->setPlanetType(mPlanetType); 
				T->setLightColor(mStarColour);
				T->createTexture(TextureName);    
				delete(T);
				break;
			}
		case ROCK: 
		case ROCK_2: 
			{
				PlanetTexturer_Rock* T = new PlanetTexturer_Rock();
				T->setSeed(mCatalogID+mRank);
				T->setPlanetType(mPlanetType); 
				T->setLightColor(mStarColour);
				T->createTexture(TextureName);   
				delete(T);
				break;
			}
		case VENUSIAN: 
			{
				// S'il y a une atmosphere epaisse, on utilise le type VENUSIAN_2
				if (mPlanetPressure>5) mPlanetType= VENUSIAN_2;
				PlanetTexturer_Venusian* T = new PlanetTexturer_Venusian();
				T->setSeed(mCatalogID+mRank);
				T->setPlanetType(mPlanetType); 
				T->setLightColor(mStarColour);
				T->createTexture(TextureName); 
				delete(T);
				break;
			}
		case GAZ_BLUE: 
		case GAZ_BROWN: 
		case GAZ_RED: 
			{
				normalMapped = false;
				PlanetTexturer_Gas* T = new PlanetTexturer_Gas();
				T->setSeed(mCatalogID+mRank);
				T->setPlanetType(mPlanetType); 
				T->setLightColor(mStarColour);
				T->createTexture(TextureName);
				delete(T);
				break;
			}
		case ICE:
			{
			PlanetTexturer_Ice* T = new PlanetTexturer_Ice();
			T->setSeed(mCatalogID+mRank);
//			T->setPlanetType(mPlanetType); 
			T->setLightColor(mStarColour);
			T->createTexture(TextureName);
			delete T;
			break;
			}

		case GRANIT         : 
			{
				PlanetTexturer_Granit* T = new PlanetTexturer_Granit();
				T->setSeed(mCatalogID+mRank);
				T->setPlanetType(mPlanetType); 
				T->setLightColor(mStarColour);
				T->createTexture(TextureName);   
				delete(T);
				break;
			}
		case DEFAULT_PLANET : 
			{
				PlanetTexturer* T = new PlanetTexturer();
				T->setSeed(mCatalogID+mRank);
				T->setPlanetType(mPlanetType); 
				T->setLightColor(mStarColour);
				T->createTexture(TextureName);
				delete(T);
				break;
			}
		default:
			{
				//	case TEMPERATE_1    :
				//	case TEMPERATE_2    :
				PlanetTexturer* T = new PlanetTexturer();
				T->setSeed(mCatalogID+mRank);
				T->setPlanetType(mPlanetType); 
				T->setLightColor(mStarColour);
				T->createReferencePlanet();
				// T->createComplexPlanet(TextureName);  break;
				delete(T);
			}
		}
	}
	// On affecte la texture à la planete.
	if (normalMapped) this->changeMaterial_NM();
	else              this->changeMaterial();
}



/* ------------------------------------------------------------------------- */
/// On efface la planète de la scene.
/* ------------------------------------------------------------------------- */
void Planet::hide()
{
	mPlanetEntity->setVisible(false);
//	mParentNode->setVisible(false);
}

/* ------------------------------------------------------------------------- */
/// On affiche la planète.
/* ------------------------------------------------------------------------- */
void Planet::show()
{
	mPlanetEntity->setVisible(true);
//	mParentNode->setVisible(true);
}

/* ------------------------------------------------------------------------- */
/// On positionne la planete, par rapport au centre du systeme solaire.
/* ------------------------------------------------------------------------- */
void Planet::setPosition(Vector3 pos)
{
	mPlanetNode->setPosition(pos);
}


/* ------------------------------------------------------------------------- */
/// On mémorise tous les paramètres caractéristiques de la planete.
/** 
@param starID     L'ID catalog de l'étoile centrale
@param planetType Le type de planete.
@param radius     Le rayon de la planete (km)
@param pressure   La pression atmosphérique en surface de la planète (Earth atm)
*/
/* ------------------------------------------------------------------------- */
void Planet::setParameters(int starID, int planetType, Real radius, Real pressure)
{
	mCatalogID      = starID;
	mPlanetType     = planetType;
	mPlanetPressure = pressure;
	radius          = radius/20e3;   
	/// TODO : expliquer la conversion (cela dépend du mesh)
	// ici planet.mesh a un diametre de xxxx wu
	mPlanetNode->setScale(radius, radius, radius);
	mPlanetID  = StringConverter::toString(mCatalogID,5,'0')+"_"+StringConverter::toString(mRank,2,'0');
}

/* ------------------------------------------------------------------------- */
/// On mémorise les paramètres de l'orbite de la planete.
/** 
@param orbit      Rayon de l'orbite de la planète (en UA).
@param year       Durée de l'orbite de la planete (en jours terrestres).
@param tilt       inclinaison axiale de la planète (en degres)
*/
/* ------------------------------------------------------------------------- */
void Planet::setOrbit(Real orbit, Real year, Real tilt)
{
	mOrbit      = orbit;
	mYear       = (int)year;
	mTilt       = tilt;
}

/* ------------------------------------------------------------------------- */
/// On change la couleur de l'étoile qui éclaire la planete.
/* ------------------------------------------------------------------------- */
void Planet::setLightColor(ColourValue StarColor)
{
	mStarColour = StarColor;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction recree un material simple pour la planete.
/** Le nouveau material est créé par clonage du material "planet/basic", et en 
modifiant la texture (colormap). */
/* ------------------------------------------------------------------------- */
void Planet::changeMaterial()
{
	DEBUG_LOG(">> changeMaterial");

	// Exemple de creation de material:
	// MaterialPtr mat = MaterialManager::getSingleton().create(id,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); 
	// TextureUnitState* textur = mat->getTechnique(0)->getPass(0)->createTextureUnitState();

	// Modification d'un material existant par clonage:
	MaterialPtr mat = MaterialManager::getSingleton().getByName("planet/basic"); 
	mat = mat->clone("planet/"+mName); 

	TextureUnitState* textur = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	textur->setTextureName(mPlanetID+".bmp"); 

	// Now you can individually change material properties:
	// mPlanetEntity->getSubEntity(0)->getMaterial()->setDiffuse(1,1,1,1); 
	// mPlanetEntity->getSubEntity(0)->getMaterial()->setAmbient(1,1,1); 

	mPlanetEntity=mSceneMgr->createEntity(mName, "planet.mesh");
	mPlanetNode->attachObject(mPlanetEntity);
	mPlanetEntity->setMaterialName("planet/"+mName); 
	mPlanetEntity->setVisible(true); 

	DEBUG_LOG("material planet/"+mName+" created");
}


/* ------------------------------------------------------------------------- */
/// Cette fonction recree un material (avec normalmap) pour la planete.
/** Le nouveau material est créé par clonage du material "planet/normalmap", et en 
modifiant les textures (colormap et normalmap). */
/* ------------------------------------------------------------------------- */
void Planet::changeMaterial_NM()
{
	// Modification d'un material existant par clonage:
	MaterialPtr mat = MaterialManager::getSingleton().getByName("planet/normalmap"); 
	mat = mat->clone("planet/"+mName+"_NM"); 

	TextureUnitState* textur0 = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	TextureUnitState* textur1 = mat->getTechnique(0)->getPass(0)->getTextureUnitState(1);
	TextureUnitState* textur2 = mat->getTechnique(1)->getPass(0)->getTextureUnitState(0);
	TextureUnitState* textur3 = mat->getTechnique(1)->getPass(0)->getTextureUnitState(1);
	textur0->setTextureName(mPlanetID+"_NM.bmp");
	textur1->setTextureName(mPlanetID+".bmp");
	textur2->setTextureName(mPlanetID+"_NM.bmp");
	textur3->setTextureName(mPlanetID+".bmp");
	DEBUG_LOG("material planet/"+mName+"_NM created");

	mPlanetEntity=mSceneMgr->createEntity(mName, "planet.mesh");
	mPlanetNode->attachObject(mPlanetEntity);
	DEBUG_LOG("   Entity created");
	mPlanetEntity->setMaterialName("planet/"+mName+"_NM"); 
	mPlanetEntity->setNormaliseNormals(true); 
	mPlanetEntity->setVisible(true); 

	DEBUG_LOG("material planet/"+mName+"_NM applied");
}

/* ------------------------------------------------------------------------- */
/// Cette fonction positionne la planete au bon endroit de son orbite, en fonction du jour courant.
/* ------------------------------------------------------------------------- */
void Planet::setOrbitalPosition(long date)
{
	Real angle = 6.28318 * (date%mYear) / mYear;
	Real X = mOrbit * Math::Cos(Radian(angle)) * ORBIT_WU_PER_AU;
	Real Z = mOrbit * Math::Sin(Radian(angle)) * ORBIT_WU_PER_AU;
	mPlanetNode->setPosition(X,0,Z);

	mPlanetNode->pitch(Degree(mTilt));
	DEBUG_LOG("orbital planet position: "+ITOA(angle)+" radians");
}


/* ------------------------------------------------------------------------- */
/// Cette fonction mémorise les proportions de sol, mer et glace dont est recouverte la surface de la planète.
/* ------------------------------------------------------------------------- */
void Planet::setCoverage(Real Ground, Real Water, Real Ice, Real Clouds)
{
	/// TODO
}

/* ------------------------------------------------------------------------- */
/// Cette fonction mémorise la couleur ed l'atmosphère de la planète.
/* ------------------------------------------------------------------------- */
void Planet::setAtmColour(ColourValue AtmColour)
{
	/// TODO
}

