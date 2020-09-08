/* ------------------------------------------------------------------------- */
// File       : LensFlare.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */


#include "macros.h"
#include "classes.h"
#include "LensFlare.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/// @param LightPosition La position 3D de la lumière, relativement à la caméra.
/// @param camera        La camera sur laquelle apparaitra l'effet de lensflare.
/// @param SceneMgr      Pointeur sur le SceneManager du jeu.
/// @param ParentNode    Le node parent du lensflare. (si NULL ou omis: on prend le RootNode comme parent)
/* ------------------------------------------------------------------------- */
LensFlare::LensFlare(Vector3 LightPosition, Camera* camera, SceneManager* SceneMgr, SceneNode* ParentNode)
{
	mSceneMgr      = SceneMgr;
	mCamera        = camera;
	mHidden        = true;
	this->createLensFlare(ParentNode);
	this->setLightPosition(LightPosition);
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
LensFlare::~LensFlare()
{
	mNode->detachObject(mHaloSet);
	mNode->detachObject(mBurstSet);
	mSceneMgr->removeBillboardSet(mHaloSet);
	mSceneMgr->removeBillboardSet(mBurstSet);
	/// TODO detruire mNode
}


/* ------------------------------------------------------------------------- */
/// On cree tous les éléments du LensFlare, et on l'affiche.
/* ------------------------------------------------------------------------- */
void LensFlare::createLensFlare(SceneNode* ParentNode)
{
	Real LF_scale = 2000;

	// -----------------------------------------------------
	// On cree deux ensembles de billboards pour le lensflare
	// -----------------------------------------------------
	mHaloSet = mSceneMgr->createBillboardSet("halo");
	mHaloSet->setMaterialName("lensflare/halo");
	mHaloSet->setCullIndividually(true);
	mHaloSet->setQueryFlags(VOIDOBJECT);	// Ils ne sont pas detectés par les rayons.

	mBurstSet= mSceneMgr->createBillboardSet("burst");
	mBurstSet->setMaterialName("lensflare/burst");
	mBurstSet->setCullIndividually(true);
	mBurstSet->setQueryFlags(VOIDOBJECT);	// Ils ne sont pas detectés par les rayons.

	// Le node est situé à l'emplacement de la lumière
	if (!ParentNode) ParentNode=mSceneMgr->getRootSceneNode();
	mNode  = ParentNode->createChildSceneNode();

	mNode->attachObject(mBurstSet);
	mNode->attachObject(mHaloSet);

	// -------------------------------
	// On cree les billbords de halo
	// -------------------------------
	Billboard* LF_Halo1 = mHaloSet->createBillboard(0,0,0);
	LF_Halo1->setDimensions(LF_scale*0.5,LF_scale*0.5);
	Billboard* LF_Halo2 = mHaloSet->createBillboard(0,0,0);
	LF_Halo2->setDimensions(LF_scale,LF_scale);
	Billboard* LF_Halo3 = mHaloSet->createBillboard(0,0,0);
	LF_Halo3->setDimensions(LF_scale*0.25,LF_scale*0.25);


	// -------------------------------
	// On cree les billbords de burst
	// -------------------------------
	Billboard* LF_Burst1 = mBurstSet->createBillboard(0,0,0);
	LF_Burst1->setDimensions(LF_scale*0.25,LF_scale*0.25);
	Billboard* LF_Burst2 = mBurstSet->createBillboard(0,0,0);
	LF_Burst2->setDimensions(LF_scale*0.5,LF_scale*0.5);
	Billboard* LF_Burst3 = mBurstSet->createBillboard(0,0,0);
	LF_Burst3->setDimensions(LF_scale*0.25,LF_scale*0.25);

}


/* -------------------------------------------------------------------------- */
/// Cette fonction met à jour l'effet de lensflare. 
/** Cette fonction doit être appelée régulièrement dans la boucle principale.
*/
/* -------------------------------------------------------------------------- */
void LensFlare::update()
{
	if (mHidden) return;

	/// Si la lumière est hors du champ de la caméra, on masque l'effet de lensflare.
	if (!mCamera->isVisible(mLightPosition)) 
	{
		mHaloSet->setVisible(false);
		mBurstSet->setVisible(false);
		return;
	}

	Real LightDistance  = mLightPosition.length();
	Vector3 CameraVect  = mCamera->getDirection(); // vector normalisé (longueur 1)

	CameraVect = LightDistance * CameraVect;

	// L'effet de LensFlare a lieu le long de ce vecteur.
	Vector3 LFvect = (CameraVect - mLightPosition);

//	LFvect += Vector3(-64,-64,0);  // dimension du sprite

	// On place les differents sprites le long de ce vector.
	mHaloSet->getBillboard(0)->setPosition( LFvect*0.500);
	mHaloSet->getBillboard(1)->setPosition( LFvect*0.125);
	mHaloSet->getBillboard(2)->setPosition(-LFvect*0.250);

	mBurstSet->getBillboard(0)->setPosition( LFvect*0.333);
	mBurstSet->getBillboard(1)->setPosition(-LFvect*0.500);
	mBurstSet->getBillboard(2)->setPosition(-LFvect*0.180);

	// On fait reapparaitre le LensFlare, au cas où il serait sorti du champ de la caméra.
	this->setVisible(true);	
}


/* ------------------------------------------------------------------------- */
/// Affiche ou efface le lens flare.
/* ------------------------------------------------------------------------- */
void LensFlare::setVisible(bool visible)
{
	mHaloSet->setVisible(visible);
	mBurstSet->setVisible(visible);
	mHidden = !visible;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction met a jour la position de la lumière. 
/** Cette fonction est utilisée si la lumière se déplace.*/
/* ------------------------------------------------------------------------- */
void LensFlare::setLightPosition(Vector3 pos)
{
	mLightPosition = pos;
	mNode->setPosition(mLightPosition); 
}


/* ------------------------------------------------------------------------- */
/// Cette fonction change la couleur des burst. 
/* ------------------------------------------------------------------------- */
void LensFlare::setBurstColour(ColourValue color)
{
	mBurstSet->getBillboard(0)->setColour(color);
	mBurstSet->getBillboard(1)->setColour(color*0.8);
	mBurstSet->getBillboard(2)->setColour(color*0.6);
}

/* ------------------------------------------------------------------------- */
/// Cette fonction change la couleur des halos. 
/* ------------------------------------------------------------------------- */
void LensFlare::setHaloColour(ColourValue color)
{
	mHaloSet->getBillboard(0)->setColour(color*0.8);
	mHaloSet->getBillboard(1)->setColour(color*0.6);
	mHaloSet->getBillboard(2)->setColour(color);
}


