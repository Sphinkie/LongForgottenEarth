/* ------------------------------------------------------------------------- */
// File       : ss_Archipel.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */


#include "macros.h"
#include "classes.h"
#include "ss_Archipel.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/// @param Name       Le nom de l'archipel, utilisé comme nom d'entity et pour le jeu.
/// @param ParentNode Pointeur sur le node Parent de l'archipel d'asteroides.
/// @param SceneMgr   Pointeur sur le SceneManager du jeu.
/* ------------------------------------------------------------------------- */
Archipel::Archipel(String Name, SceneNode* ParentNode, SceneManager* SceneMgr)
{
	mArchipelName = Name;
	mSceneMgr     = SceneMgr;
	mOnScene      = false;

	/// On cree un node auquel seront ratachés tous les asteroides
	mArchipelNode   = ParentNode->createChildSceneNode(Name);
	// Pour creer un UserObjet, il faut avoir un Entity
	mArchipelEntity = SceneMgr->createEntity(Name,"wpbox.mesh");
	mArchipelEntity->setUserObject(this);
	// TODO: la boite englobe tous les asteroides (utiles pour les rayons)
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Archipel::~Archipel()
{
	/// Detruit les asteroides 3D
	for (int i=1; i<mAsteros; i++) if(pAsteros[i]) delete(pAsteros[i]);
	// TODO : Detruire tous les autres sous-objets
}

/* ------------------------------------------------------------------------- */
/// Identification de la classe UserDefinedObject (surcharge)
/* ------------------------------------------------------------------------- */
long Archipel::getTypeID (void) const
{
	return ARCHIPEL_ID;
}

/* ------------------------------------------------------------------------- */
/// On ajoute l'archipel a la scene
/* ------------------------------------------------------------------------- */
void Archipel::addToScene()
{
	// On verifie que l'objet n'est pas deja dans la scene
	if (!mOnScene) 
	{
		/// Lecture des parametres du fichier de config
		ConfigFile DescriptionFile;
		DescriptionFile.load("../media/objects/"+mArchipelName+".cfg");
		DEBUG_LOG("Loading ../media/objects/"+mArchipelName+".cfg");
		// TODO: Quid en cas de fichier inexistant ???

		int    Seed      = StringConverter::parseInt(DescriptionFile.getSetting("Seed"));
		int    Density   = StringConverter::parseInt(DescriptionFile.getSetting("Density"));
		int    Quantity  = StringConverter::parseInt(DescriptionFile.getSetting("Quantity"));
		String Dimension = DescriptionFile.getSetting("Dimension");
		String Position  = DescriptionFile.getSetting("Position");
		String Color     = DescriptionFile.getSetting("Color");

		int   Px,Py,Pz;
		int   Dx,Dy,Dz;
		float R,G,B;

		StringVector DimV, PosV, ColV;
	
		DimV = StringUtil::split(Dimension,",");
		PosV = StringUtil::split(Position,",");
		ColV = StringUtil::split(Color,",");

		Dx= StringUtil::toReal(DimV.at(0));
		Dy= StringUtil::toReal(DimV.at(1));
		Dz= StringUtil::toReal(DimV.at(2));

		Px= StringUtil::toReal(PosV.at(0));
		Py= StringUtil::toReal(PosV.at(1));
		Pz= StringUtil::toReal(PosV.at(2));

		R= StringUtil::toReal(ColV.at(0));
		G= StringUtil::toReal(ColV.at(1));
		B= StringUtil::toReal(ColV.at(2));
		mColour = ColourValue(R/255,G/255,B/255);

		mArchipelNode->setPosition(Px,Py,Pz);

		// On initialise le tableau des asteroides.
		pAsteros = new Asteroide*[Quantity];
		mAsteros = Quantity;

		// Note: les random ne sont plus reproductibles si on utilise Math::SymmetricRandom();
		srand(Seed);
		for (int i=1; i<Quantity; i++)
		{
			int  N     = rand()%6;					// de 0 a 5
			int  Mat   = rand()%6;					// de 0 à 5
			Real Size  = rand()%3;					// entre 0 et 3
			Real Sr    = 2*(0.5-rand()%2);
			Real Sx    = 2*(0.5-rand()%2);	// des signes aleatoires
			Real Sy    = 2*(0.5-rand()%2);	// 1 ou -1
			Real Sz    = 2*(0.5-rand()%2);
			int  X     = Sx*Dx/(1+rand()%Density);
			int  Y     = Sy*Dy/(1+rand()%Density);
			int  Z     = Sz*Dz/(1+rand()%Density);
	
			/// Ajout des asteroides
			Asteroide* Astero;
			Astero= new Asteroide("astero"+ITOA(i), "asteroid"+ITOA(N)+".mesh", "asteroide_"+ITOA(Mat), mArchipelNode, mSceneMgr);
			Astero->setScale(Size);
			Astero->addToScene(Vector3(X,Y,Z));
			Astero->setRotationSpeed(Radian(Sr*0.02*(rand()%10)));	// +/-0.2rad = 10° par seconde maximum
			Astero->setColor(R,G,B);		// Colorie tous les asteros de ce material (pas tres optimisé)
			pAsteros[i]=Astero;
		}

		// ----------------------------------------
		/// Ajout des objets particuliers
		// ----------------------------------------
		String Object = DescriptionFile.getSetting("Object_1");
		DEBUG_LOG("got Object_1="+Object);

		StringVector ObjectV;

		ObjectV = StringUtil::split(Object,",");
		int X = StringUtil::toReal(ObjectV.at(2));
		int Y = StringUtil::toReal(ObjectV.at(3));
		int	Z = StringUtil::toReal(ObjectV.at(4));

		Asteroide* Astero_special_1;
		Astero_special_1= new Asteroide(ObjectV.at(0), ObjectV.at(1), "",mArchipelNode, mSceneMgr);
//		Astero_special_1= new Asteroide(Name, MeshFile, "",mArchipelNode, mSceneMgr);
		Astero_special_1->addToScene(Vector3(X,Y,Z));
		DEBUG_LOG(ObjectV.at(0)+" created");
	}
	mOnScene = true;  
}

/* ------------------------------------------------------------------------- */
/// On enleve l'archipel de la scene
/* ------------------------------------------------------------------------- */
void Archipel::removeFromScene()
{
	if (mOnScene) 
	{
	/// TODO: remove from scene 
	}
	mOnScene = false;
}


/* ------------------------------------------------------------------------- */
/// On ajoute des debris sous forme de billboards
/* ------------------------------------------------------------------------- */
void Archipel::addDebris()
{
	int i;
	int N = 20; // Nombre de debris de chaque set.

	BillboardSet* Astero_billboard_set1;
	Astero_billboard_set1= mSceneMgr->createBillboardSet("astero_set1");
	Astero_billboard_set1->setMaterialName("astero/type1");
	Astero_billboard_set1->setCullIndividually(true);

	for (i=0; i<N; i++)
	{
		Billboard* B = Astero_billboard_set1->createBillboard(Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000));
		B->setRotation(Radian(Math::RangeRandom(0,6)));
	}


	BillboardSet* Astero_billboard_set2;
	Astero_billboard_set2= mSceneMgr->createBillboardSet("astero_set2");
	Astero_billboard_set2->setMaterialName("astero/type2");
	Astero_billboard_set2->setCullIndividually(true);

	for (i=0; i<N; i++)
	{
		Billboard* B = Astero_billboard_set2->createBillboard(Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000));
		B->setRotation(Radian(Math::RangeRandom(0,6)));
	}


	BillboardSet* Astero_billboard_set3;
	Astero_billboard_set3= mSceneMgr->createBillboardSet("astero_set3");
	Astero_billboard_set3->setMaterialName("astero/type3");
	Astero_billboard_set3->setCullIndividually(true);

	for (i=0; i<N; i++)
	{
		Billboard* B = Astero_billboard_set3->createBillboard(Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000));
		B->setRotation(Radian(Math::RangeRandom(0,6)));
	}

	BillboardSet* Astero_billboard_set4;
	Astero_billboard_set4= mSceneMgr->createBillboardSet("astero_set4");
	Astero_billboard_set4->setMaterialName("astero/type4");
	Astero_billboard_set4->setCullIndividually(true);

	for (i=0; i<N; i++)
	{
		Billboard* B = Astero_billboard_set4->createBillboard(Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000), Math::RangeRandom(-2000,2000));
		B->setRotation(Radian(Math::RangeRandom(0,6)));
	}

	Astero_billboard_set1->getMaterial()->setAmbient(mColour);
	Astero_billboard_set2->getMaterial()->setAmbient(mColour);
	Astero_billboard_set3->getMaterial()->setAmbient(mColour);
	Astero_billboard_set4->getMaterial()->setAmbient(mColour);

	// it will only be drawn once it is attached to a scenenode
	SceneNode* mDebrisNode;
	mDebrisNode = mArchipelNode->createChildSceneNode("DebrisNode");

	mDebrisNode->attachObject(Astero_billboard_set1);
	mDebrisNode->attachObject(Astero_billboard_set2);
	mDebrisNode->attachObject(Astero_billboard_set3);
	mDebrisNode->attachObject(Astero_billboard_set4);

}

/* ------------------------------------------------------------------------- */
/// Change la couleur "Diffuse" des billboards, de façon à refleter la couleur de l'étoile.
/* ------------------------------------------------------------------------- */
void Archipel::setDebrisColour(ColourValue starColour)
{
	/// TODO : ne marche pas !!
	for (int i=0; i<20; i++)
	{
		mSceneMgr->getBillboardSet("astero_set1")->getBillboard(i)->setColour(starColour);
		mSceneMgr->getBillboardSet("astero_set2")->getBillboard(i)->setColour(starColour);
		mSceneMgr->getBillboardSet("astero_set3")->getBillboard(i)->setColour(starColour);
		mSceneMgr->getBillboardSet("astero_set4")->getBillboard(i)->setColour(starColour);
	}
}

/* ------------------------------------------------------------------------- */
/// Affiche les asteroides de l'archipel.
/* ------------------------------------------------------------------------- */
void Archipel::show()
{
	mSceneMgr->getBillboardSet("astero_set1")->setVisible(true);
	mSceneMgr->getBillboardSet("astero_set2")->setVisible(true);
	mSceneMgr->getBillboardSet("astero_set3")->setVisible(true);
	mSceneMgr->getBillboardSet("astero_set4")->setVisible(true);
	/// Affiche les asteroides 3D
	for (int i=1; i<mAsteros; i++) pAsteros[i]->show();
	/// TODO : les objets spéciaux
}

/* ------------------------------------------------------------------------- */
/// Efface les asteroides de l'archipel.
/* ------------------------------------------------------------------------- */
void Archipel::hide()
{
	mSceneMgr->getBillboardSet("astero_set1")->setVisible(false);
	mSceneMgr->getBillboardSet("astero_set2")->setVisible(false);
	mSceneMgr->getBillboardSet("astero_set3")->setVisible(false);
	mSceneMgr->getBillboardSet("astero_set4")->setVisible(false);
	/// Efface les asteroides 3D
	for (int i=1; i<mAsteros; i++) pAsteros[i]->hide();
	/// TODO : les objets spéciaux
}
