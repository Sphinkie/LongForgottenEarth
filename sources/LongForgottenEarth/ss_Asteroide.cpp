/* ------------------------------------------------------------------------- */
// File       : ss_Asteroide.cpp
// Project    : Long Forgotten Earth
// Author    : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "ss_Asteroide.h"
#include "classes.h"

/* ------------------------------------------------------------------------- */
/// Constructeur: Creation de Node et de l'Entity pour chaque Asteroide
/// @param Name         Le nom de l'Asteroide, utilisé comme nom d'entity et pour le jeu.
/// @param MeshFile     Le ficher mesh contenant le modele 3D de l'asteroide.
/// @param MaterialName Le ficher Material contenant la	texture de l'asteroide.
/// @param ParentNode   Pointeur sur le node Parent de l'asteroides (normalement: l'Archipel).
/// @param SceneMgr     Pointeur sur le SceneManager du jeu.
/* ------------------------------------------------------------------------- */
Asteroide::Asteroide(String Name, String MeshFile, String MaterialName, SceneNode* ParentNode, SceneManager* SceneMgr)
{
	// On cree un asteroide
	mAsteroName   = Name;
	mAsteroEntity = SceneMgr->createEntity(Name, MeshFile);
	mAsteroNode   = ParentNode->createChildSceneNode(Name);
	mAsteroEntity->setUserObject(this);
	if (MaterialName!="")  mAsteroEntity->setMaterialName(MaterialName);

	mRotationSpeed=0;

	mRoot     = Root::getSingletonPtr();
	mSceneMgr = SceneMgr;
}

/* ------------------------------------------------------------------------- */
// Destructeur
/* ------------------------------------------------------------------------- */
Asteroide::~Asteroide()
{
	// Arreter la gestion des mouvements
	mRoot->removeFrameListener(this);
}


/* ------------------------------------------------------------------------- */
// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long Asteroide::getTypeID (void) const
{
	return ASTEROIDE_ID;
}

/* ------------------------------------------------------------------------- */
/// Fait apparaitre l'Asteroide, et lance la gestion des mouvements.
/// @param Pos Position que l'on affecte à l'asteroide, relative au ParentNode.
/* ------------------------------------------------------------------------- */
void Asteroide::addToScene(Vector3 Pos)
{
	mAsteroNode->attachObject(mAsteroEntity);
	mAsteroNode->setPosition(Pos);
	mRoot->addFrameListener(this);
}


/* ------------------------------------------------------------------------- */
/// Change l'échelle de l'asteroide
/// @param Size Multiplicateur de taille que l'on affecte à l'asteroide.
/* ------------------------------------------------------------------------- */
void Asteroide::setScale(Real Size)
{
	mAsteroNode->setScale(Size,Size,Size);
}

/* ------------------------------------------------------------------------- */
/// Affecte une couleur au materiau de l'asteroide, qui se combinera à la couleur de la texture
/** Note: cela affecte tous les asteroides utilisant ce material. */
/// @param R composante RED de la nouvelle couleur (0..255)
/// @param G composante GREEN de la nouvelle couleur (0..255)
/// @param B composante BLUE de la nouvelle couleur (0..255)
/* ------------------------------------------------------------------------- */
void Asteroide::setColor(Real R, Real G, Real B)
{
	MaterialPtr AsteroMat;
	AsteroMat= mAsteroEntity->getSubEntity(0)->getMaterial();
//	AsteroMat->setAmbient(R/255, G/255, B/255); 
	/// TODO : comment bien colorer les asteros ?
	/// utiliser la couleur de l'étoile ET/OU la couleur de la roche ?
}

/* ------------------------------------------------------------------------- */
/// Mémorise la vitesse de rotation de l'asteroide
/// @param RotationSpeed Vitesse de rotation sur lui même de l'asteroide.
/* ------------------------------------------------------------------------- */
void Asteroide::setRotationSpeed(Radian RotationSpeed)
{
	mRotationSpeed = RotationSpeed;
}

/* ------------------------------------------------------------------------- */
/// Gestion des mouvements de l'asteroide
/* ------------------------------------------------------------------------- */
bool Asteroide::frameStarted (const FrameEvent& evt)
{
	/// Rotation sur lui-même
	mAsteroNode->yaw(mRotationSpeed*evt.timeSinceLastFrame);
	return true;
}

/* ------------------------------------------------------------------------- */
/// On efface l'asteroide.
/* ------------------------------------------------------------------------- */
void Asteroide::hide()
{
	mAsteroEntity->setVisible(false);
}

/* ------------------------------------------------------------------------- */
/// On affiche l'asteroide.
/* ------------------------------------------------------------------------- */
void Asteroide::show()
{
	mAsteroEntity->setVisible(true);
}