/* ------------------------------------------------------------------------- */
// File       : ShipLight.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "ShipLight.h"

/* ------------------------------------------------------------------------- */
/// Cr�ation d'un projecteur � l'exterieur du vaisseau.
/* ------------------------------------------------------------------------- */
ShipLight::ShipLight(String Name, BillboardSet* billboardset, SceneManager* SceneMgr, SceneNode* ParentNode)
{
	mSceneMgr     = SceneMgr;
	mName         = Name;
	mBillboardset = billboardset;
	
	// -------------------------------
	// On cree un Node pour la lumiere
	// -------------------------------
	mNode = ParentNode->createChildSceneNode();
	mNode->setInheritScale(false);
	mNode->setPosition(0,0,0); 

	// -------------------------------
	// On cree la lumiere
	// -------------------------------
	mLight = mSceneMgr->createLight(mName);
	// Position de la lumi�re p/r au Node
	mLight->setPosition(0,0,0); 
	mLight->setType(Light::LT_SPOTLIGHT);
	mLight->setDirection(0,0,1);
	mLight->setDiffuseColour(1,1,1);
	mLight->setSpotlightRange(Radian(0.5),Radian(0.8));

	mLight->setCastShadows(false);		// Cette lumiere ne projette pas d'ombres
//	mLight->setCastShadows(true);		// Cette lumiere projette des ombres

	this->setHilight(false);
	// On attache la lumi�re au Node
	mNode->attachObject(mLight);

	mSprite = billboardset->createBillboard(0,0,0);

}


/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
ShipLight::~ShipLight()
{

}


/* ------------------------------------------------------------------------- */
/// Allume (ou �teint) les projecteurs � l'exterieur du vaisseau.
/* ------------------------------------------------------------------------- */
void ShipLight::setHilight(bool on)
{
	mHilights = on;
	mLight->setVisible(on);
	mBillboardset->setVisible(on);
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'�tat des projecteurs � l'exterieur du vaisseau (Allum� ou �teint).
/* ------------------------------------------------------------------------- */
bool ShipLight::getHilightStatus()
{
	return mHilights;
}

/* ------------------------------------------------------------------------- */
/// Positionne le projecteur.
/* ------------------------------------------------------------------------- */
void ShipLight::setPosition(Vector3 Posit)
{
	mNode->setPosition(Posit); 
	mSprite->setPosition(Posit);
}

/* ------------------------------------------------------------------------- */
/// Oriente le projecteur.
/* ------------------------------------------------------------------------- */
void ShipLight::setDirection(Vector3 Direc)
{
	mLight->setDirection(Direc);
}
