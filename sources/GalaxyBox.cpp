/* ------------------------------------------------------------------------- */
// File       : GalaxyBox.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Classes.h"
#include "Macros.h"
#include "GalaxyBox.h"
#include "Player.h"
#include "StarsDatabase.h"
#include "Const.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
GalaxyBox::GalaxyBox(SceneManager* SceneMgr)
{
	mSceneMgr   = SceneMgr;
	mCamera     = mSceneMgr->getCamera("PlayerCam");
	mSolarSys   = NULL;
	mArchipel   = NULL;
	mLens       = NULL;
	mNearestStar= NULL;
	mFarStarsSet= NULL;
	mNbStars    = 0;

    // D�cor: la sph�re c�leste: add a skybox
	DEBUG_LOG ("Creating Skybox");
	mSceneMgr->setSkyBox(true, "SpaceSkyBox/1_8");

	mGalaxyPivotNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mGalaxyNode      = mGalaxyPivotNode->createChildSceneNode();
	mLocalNode       = mGalaxyPivotNode->createChildSceneNode();
	mLocalMode       = false;

	// On ajoute une l�g�re lumi�re ambiante, pour ne pas �tre dans le noir complet
	mSceneMgr->setAmbientLight(ColourValue(0.35, 0.35, 0.35));

	// On cree un tableau dynamique de N �toiles.
	pStars   = new CatalogStar*[MAX_VISIBLE_STARS];
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
GalaxyBox::~GalaxyBox()
{
	// On detruit tout le contenu du catalogue
	for (int i=0; i<mNbStars; i++) 
	{
		if (pStars[i]) delete(pStars[i]);
	}
	delete(pStars);
	if (mArchipel)    delete(mArchipel);
	if (mSolarSys)    delete(mSolarSys);
	if (mLens)        delete(mLens);
	// if (mFarStarsSet) mSceneMgr->removeBillboardSet(mFarStarsSet); automatic
}

/* ------------------------------------------------------------------------- */
/// Cr�e l'effet de LensFlare
/* ------------------------------------------------------------------------- */
void GalaxyBox::addLensFlare()
{
	if (mSolarSys)
	{
		// On affiche le lensFlare
//		Vector3     StarPos  = mGalaxyNode->getPosition() + mSolarSys->getPosition();
		Vector3     StarPos  = mLocalNode->getPosition();
		ColourValue StarColor= mSolarSys->getStarColour();
		// exemple bleu = ColourValue(106/255.0,85/255.0,213/255.0)
		Camera* PlayerCam = Player::getSingletonPtr()->getCamera();
		mLens = new LensFlare (StarPos, PlayerCam, mSceneMgr, mGalaxyPivotNode);
		mLens->setHaloColour(StarColor);
		mLens->setVisible(true);
		TRACE("Lens Flare created.");
	}
}


template<> GalaxyBox* Singleton<GalaxyBox>::ms_Singleton = 0;
/* ------------------------------------------------------------------------- */
/// Renvoie un pointeur sur l'objet (unique) GalaxyBox
/* ------------------------------------------------------------------------- */
GalaxyBox* GalaxyBox::getSingletonPtr(void)
{
	return ms_Singleton;
}


/* ------------------------------------------------------------------------- */
/// Renvoie quelques parametres concernant le systeme solaire
/** 
@param index identifiant du param�tre demand�.
- 1 : renvoie le catalog ID du syst�me solaire.
- 2 : renvoie le nom du syst�me solaire.
- 3 : renvoie le nombre de planetes du systeme solaire.
- 4 : renvoie le spectre de l'�toile
*/
/* ------------------------------------------------------------------------- */
String GalaxyBox::getSystemParameter(int index)
{
	if (!mSolarSys && (index==1))	return "Deep space";
	if (!mSolarSys)	return "";

	if (index==1) return "Ref: "+StringConverter::toString(mSolarSys->getCatalogID());
	if (index==2) return "Star Name "+mSolarSys->getName();
	if (index==3) return (ITOA(mSolarSys->getPlanetNumber())+" planets");
	if (index==4) return "Type "+mSolarSys->getStar()->getSpectralType();
	return "";
}

/* ------------------------------------------------------------------------- */
/// Change l'�toile (DEBUG)
/* ------------------------------------------------------------------------- */
void GalaxyBox::_changeStar()
{
	mSolarSys->_changeStar();
	mArchipel->setDebrisColour(mSolarSys->getStarColour());
}


/* ------------------------------------------------------------------------- */
/// Allume ou �teint la lumi�re de l'�toile principale du syst�me solaire le plus proche.
/* ------------------------------------------------------------------------- */
void GalaxyBox::setMainStarLight(bool on)
{
	if(mSolarSys) mSolarSys->setStarLight(on);
}


/* ------------------------------------------------------------------------- */
/// Charge toutes les �toiles de toutes les bases de donn�es.
/* ------------------------------------------------------------------------- */
void GalaxyBox::loadAllStars()
{
	mNbStars += this->loadStars("../media/data/stars.dat");
}


/* ------------------------------------------------------------------------- */
/// Charge les �toiles d'un fichier DATA.
/* ------------------------------------------------------------------------- */
int GalaxyBox::loadStars(String filename)
{
	StarDatabase* DB = new StarDatabase(pStars, MAX_VISIBLE_STARS, filename);
	int nb_stars = DB->readStars();
	// On m�morise l'�toile la plus proche d�tect�e
	mNearestStar = DB->getClosestStar();
	TRACE("Found a close star ? "+ITOA(mNearestStar!=NULL));
	if (mNearestStar) 
	{
		TRACE("Found Star with ID "+ITOA(mNearestStar->getCatalogNumber()));
		TRACE("Star Space Coord = "+ITOA(mNearestStar->getPosition()));
	}
	delete(DB);
	return nb_stars;
}


/* ------------------------------------------------------------------------- */
/// Affiche toutes les �toiles du tableau (sous-ensemble de la base de donn�es Celestia).
/* ------------------------------------------------------------------------- */
void GalaxyBox::createStars()
{
	DEBUG_LOG(">> createStars");
	// On cree un ensemble de sprites, pour les �toiles lointaines
	mFarStarsSet= mSceneMgr->createBillboardSet("quadrant/stars");
	mFarStarsSet->setMaterialName("stars/far");
	mFarStarsSet->setCullIndividually(true);
	mFarStarsSet->setQueryFlags(VOIDOBJECT);	// Ils ne sont pas detect�s par les rayons.
	mGalaxyNode->attachObject(mFarStarsSet);
	mFarStarsSet->setVisible(true);
	for (int i=0; i<mNbStars; i++)
	{
		if (pStars[i]) pStars[i]->show(mFarStarsSet);
	}
	DEBUG_LOG("<< createStars");
}

/* ------------------------------------------------------------------------- */
/// Gestion de la dynamique de l'univers.
/* ------------------------------------------------------------------------- */
bool GalaxyBox::frameStarted(const FrameEvent& evt)
{
//	if (evt.timeSinceLastFrame>0.1) return true;

	if (mLens) mLens->update(); // Effet de lensflare

	/// TODO : ne pas passer � chaque tour ...

	if (!mArchipel && mLocalMode)
	{
		/************************************************************************/
		/* Cr�ation des ast�roides                                              */
		/************************************************************************/
		mArchipel= new Archipel("ArchipelLorenz", mLocalNode, mSceneMgr);
		mArchipel->addDebris();
		mArchipel->addToScene();
		TRACE("mArchipel added to scene.");
	}
	
	if (!mSolarSys)
	{
		/************************************************************************/
		/* Cr�ation d'un Syst�me Solaire                                        */
		/************************************************************************/
		if (mNearestStar) 
		{
			mLocalMode = true; // On entre dans un syst�me solaire
			mLocalNode->setPosition(mGalaxyNode->getPosition() + mNearestStar->getPosition()-mShipLocalPos);
			// On efface le sprite repr�sentant l'�toile lointaine.
			mNearestStar->hide(mFarStarsSet);
			// On cr�e le systeme solaire
			mSolarSys = new SolarSystem(mLocalNode, mSceneMgr, mNearestStar);
			TRACE("Solar System created.");
			mSolarSys->show();
			this->addLensFlare();
			TRACE("Solar System added to scene.");
		}
	}
	return true;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la position du Systeme Solaire par rapport au GalaxyBox 
/** Elle  correspond � celle de l'�toile, qui est en son centre. */
/* ------------------------------------------------------------------------- */
Vector3 GalaxyBox::getStarPosition()
{
	if (mSolarSys) return mSolarSys->getPosition();
	else return Vector3(0,0,0);
}


/* ------------------------------------------------------------------------- */
/// Affiche le contenu de la galaxie. 
/* ------------------------------------------------------------------------- */
void GalaxyBox::show()
{
	mCamera->setFarClipDistance(0); // distance de vue infinie
	if (mArchipel)    mArchipel->show();
	if (mSolarSys)    mSolarSys->show();
	if (mLens)        mLens->setVisible(true);
	if (mFarStarsSet) mFarStarsSet->setVisible(true);
}


/* ------------------------------------------------------------------------- */
/// Efface le contenu de la galaxie. 
/* ------------------------------------------------------------------------- */
void GalaxyBox::hide()
{
	mCamera->setFarClipDistance(1000);  // distance de vue r�duite (ship)
	if (mArchipel)    mArchipel->hide();
	if (mSolarSys)    mSolarSys->hide();
	if (mLens)        mLens->setVisible(false);
	if (mFarStarsSet) mFarStarsSet->setVisible(false);
}


/* ------------------------------------------------------------------------- */
/// Positionne le vaisseau de mani�re pr�cise dans un systeme solaire.
/** unit = wu (world units) */
/* ------------------------------------------------------------------------- */
void GalaxyBox::setShipLocalPosition(Real x, Real y, Real z)
{
	mShipLocalPos = Vector3(x,y,z);
}


/* ------------------------------------------------------------------------- */
/// Positionne le vaisseau dans la galaxie (unit = AL).
/** Peut etre interpr�t� de diff�rentes fa�on.
- soit la position du vaisseau par rapport au soleil "Sol" (point 0 de la galaxie)
- soit la position inverse du point 0 de la galaxie par rapport au vaisseau.
- En pratique, c'est la deuxi�me vision qui est appliqu�e: On positionne la 
galaxie sur une position de d�part, et le ship est en (0.0.0) par rapport au 
rootNode. */
/* ------------------------------------------------------------------------- */
void GalaxyBox::setShipCoordinates(Real x, Real y, Real z)
{
	mShipCoordinates = Vector3(x,y,z);
	// On convertit les AL en WU.
	mGalaxyNode->setPosition(-SPACE_WU_PER_AL*mShipCoordinates);
}

/* ------------------------------------------------------------------------- */
/// Renvoie la position du vaisseau dans la galaxie.
/** Position par rapport au soleil "Sol", unit = al */
/* ------------------------------------------------------------------------- */
Vector3 GalaxyBox::getShipCoordinates()
{
	return mShipCoordinates;
}

	
/* ------------------------------------------------------------------------- */
/// Renvoie la position du centre galactique (Sol) p/r au RootNode.
/* ------------------------------------------------------------------------- */
Vector3 GalaxyBox::getPosition()
{
	return mGalaxyNode->getPosition();
}


/* ------------------------------------------------------------------------- */
/// Supprime le syst�me solaire.
/* ------------------------------------------------------------------------- */
void GalaxyBox::removeSolarSys()
{
	mLocalMode = false;
	if (mSolarSys) delete(mSolarSys);
	if (mLens)     delete(mLens);
}


/* ------------------------------------------------------------------------- */
/// D�place l'ensemble du GalaxyBox le long d'un vecteur. 
/** Utilis� g�n�ralement quand on fait deplacer l'univers autour d'un vaisseau (plutot que l'inverse).*/
/* ------------------------------------------------------------------------- */
void GalaxyBox::translate(Vector3 TranslationVector)
{
	if (TranslationVector==Vector3::ZERO) return;
	
	// Si on est dans un syst�me solaire, on ne s'embete pas � faire bouger 
	// toutes les �toiles.
	if (mLocalMode) mLocalNode->translate(TranslationVector);
	else            mGalaxyNode->translate(TranslationVector); 
}


/* ------------------------------------------------------------------------- */
/// Cette fonction fait pivoter l'ensemble du GalaxyBox sur lui m�me (3 axes). 
/** Utilis� g�n�ralement quand on fait pivoter l'univers autour d'un vaisseau (plutot que l'inverse).*/
/* ------------------------------------------------------------------------- */
void GalaxyBox::rotate(Real Zrotation, Real Xrotation, Real Yrotation)
{

	mGalaxyPivotNode->roll (Degree(Zrotation));
	mGalaxyPivotNode->pitch(Degree(Xrotation));
	mGalaxyPivotNode->yaw  (Degree(Yrotation));

}
