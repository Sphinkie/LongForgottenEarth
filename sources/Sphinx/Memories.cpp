/* ------------------------------------------------------------------------- */
// File       : Memories.cpp
// Project    : Long Forgotten Earth - Menu Principal
// Auteur     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Memories.h"
#include "macros.h"

/* ------------------------------------------------------------------------- */
// Constructeur
/* ------------------------------------------------------------------------- */
Memories::Memories(RenderWindow* win, SceneManager* SceneMgr, String MemoriesFile)
{
	TRACE(">> Memories()");
	mDescriptionFile = "../media/memories/" + MemoriesFile+".cfg";
	mSceneMgr        = SceneMgr;
	mCamera          = SceneMgr->getCamera("PlayerCam");
	mCameraNode      = (SceneNode*)mCamera->getParentNode();
	mWindow          = win;

	mHasParticles    = false;
	mHasLCDdisplays  = false;
	mHasBillboard    = false;
	TRACE("<< Memories()");
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Memories::~Memories()
{
	// - Suppression des particules (fusées pour Memories01, vapeur, etc)
	if (mHasParticles)
	{
		// J'ai l'impression que ce n'est pas la meilleure methode pour supprimer les particules, mais ca marche
		ParticleSystemManager::getSingleton().destroySystem(mParticObject);
		mParticleSourceNode->detachObject(mParticObject);
	}

	// - Suppression des billboards
	if (mHasBillboard)
	{
//		mBillboardNode->detachAllObjects();
		mSceneMgr->removeAllBillboardSets();
	}

	// - Suppression des cadrans pour Memories04
	if (mHasLCDdisplays)
	{
		// Suppression des Nodes créés par createDisplay()
		mSceneMgr->getRootSceneNode()->removeChild("Memories05/cadran1");
		mSceneMgr->getRootSceneNode()->removeChild("Memories05/cadran2");
		mSceneMgr->getRootSceneNode()->removeChild("Memories05/cadran3");
		mSceneMgr->getRootSceneNode()->removeChild("Memories05/cadran4");
		mSceneMgr->getRootSceneNode()->removeChild("Memories05/cadran5");

		// Suppression des Entities créées par createDisplay()
		mSceneMgr->removeEntity("Memories05/cadran1");
		mSceneMgr->removeEntity("Memories05/cadran2");
		mSceneMgr->removeEntity("Memories05/cadran3");
		mSceneMgr->removeEntity("Memories05/cadran4");
		mSceneMgr->removeEntity("Memories05/cadran5");
	}
}

/* ------------------------------------------------------------------------- */
/// Affichage des memories
/* ------------------------------------------------------------------------- */
void Memories::showMemories()
{
	DEBUG_LOG(">> showMemories ("+ mDescriptionFile +")");


	// Loading scene file 
	// We use the functions offered by OGRE
    ConfigFile scenefile;
    scenefile.load(mDescriptionFile);
	
	// *****************************************
	// On lit les parametres de la scene
	// *****************************************
    String CameraPos  = scenefile.getSetting( "CameraPos"  );
	String CameraYaw  = scenefile.getSetting( "CameraYaw"  );
	String CameraPitch= scenefile.getSetting( "CameraPitch");
	String LightPos   = scenefile.getSetting( "LightPos"   );
	String LightColor = scenefile.getSetting( "LightColor" );
	String Skybox     = scenefile.getSetting( "Skybox" );
	String Fog        = scenefile.getSetting( "Fog" );
	String FogColor   = scenefile.getSetting( "FogColor" );
	String Terrain    = scenefile.getSetting( "Terrain" );
	String SpherePos  = scenefile.getSetting( "SpherePos" );
	String SphereMat  = scenefile.getSetting( "SphereMat" );
	String Particules = scenefile.getSetting( "Particles" );
	String PartiPos   = scenefile.getSetting( "ParticlesPos" );
	String LCDdisplay = scenefile.getSetting( "LCDdisplay" );
	String Picto      = scenefile.getSetting( "Picto" );
	String PictoPos   = scenefile.getSetting( "PictoPos" );

	int   X,Y,Z,A;
	float R,G,B;
	StringVector PosV, ColV;

	// On repositionne la camera
	PosV = StringUtil::split(CameraPos, ",");
	X    = StringUtil::toReal(PosV.at(0));
	Y    = StringUtil::toReal(PosV.at(1));
	Z    = StringUtil::toReal(PosV.at(2));
	mCameraNode->setPosition(Vector3(X,Y,Z));

	// Look FRONT along Z
	mCamera->lookAt(Vector3(0,0,-100));
	// On oriente la camera
	A   = StringUtil::toReal(CameraYaw);
	mCamera->yaw(Degree(A));	
	A   = StringUtil::toReal(CameraPitch);
	mCamera->pitch(Degree(A));	
	DEBUG_LOG("Camera Node Position Updated ");

	// Modification de la lumiere 
	PosV = StringUtil::split(LightPos, ",");
	X    = StringUtil::toReal(PosV.at(0));
	Y    = StringUtil::toReal(PosV.at(1));
	Z    = StringUtil::toReal(PosV.at(2));
  	Light* lum;
	lum= mSceneMgr->getLight("MainLight");
	lum->setPosition(X,Y,Z);

	ColV = StringUtil::split(LightColor, ",");
	R    = StringUtil::toReal(ColV.at(0));
	G    = StringUtil::toReal(ColV.at(1));
	B    = StringUtil::toReal(ColV.at(2));
	lum->setDiffuseColour(R/255,G/255,B/255);  
	DEBUG_LOG("Light updated");

    // Décor: la sphère céleste: add a skybox
	mSceneMgr->setSkyBox(true, Skybox);
	DEBUG_LOG("Skybox updated");

	// On change la planete et sa texture
	// On retrouve la sphere
	Entity*	   SphereEntity;
	SceneNode* mSphereNode;
	SphereEntity  = mSceneMgr->getEntity("sphere1");	
	mSphereNode = (SceneNode*)SphereEntity->getParentNode();
	if (SpherePos!="")
	{
		// On change la texture et la position
		SphereEntity->setMaterialName(SphereMat);
		PosV = StringUtil::split(SpherePos, ",");
		X    = StringUtil::toReal(PosV.at(0));
		Y    = StringUtil::toReal(PosV.at(1));
		Z    = StringUtil::toReal(PosV.at(2));
		mSphereNode->setPosition(X,Y,Z); 
		mSphereNode->setVisible(true);
		DEBUG_LOG("Planet updated");
	}
	else
	{
		mSphereNode->setVisible(false);
		DEBUG_LOG("Planet hided");
	}
	
	// Mise en place du brouillard
	if (Fog!="No")
	{
		// NB it's VERY important to set this before calling setWorldGeometry 
		// because the vertex program picked will be different
		// un fog noir, pour l'obscurité au fond
		ColV = StringUtil::split(FogColor, ",");
		R    = StringUtil::toReal(ColV.at(0));
		G    = StringUtil::toReal(ColV.at(1));
		B    = StringUtil::toReal(ColV.at(2));
		ColourValue fadeColour(R/255, G/255, B/255);
		mSceneMgr->setFog (FOG_LINEAR, fadeColour, 0.01, 0, 420); 
		// mWindow->getViewport(0)->setBackgroundColour(fadeColour); /// TODO: pb de fog avec Ogre 1.0
		DEBUG_LOG("Fog updated");
	}

	// Mise en place du Terrain
	if (Terrain!="")
	{
		DEBUG_LOG("Updating terrain");
		mSceneMgr->setWorldGeometry(Terrain);
	}


	// Gestion des particules
	if (Particules!="")
	{
		mParticulesName = Particules;
		// On cree le system des particules (fusées, vapeur...)
		mParticObject=ParticleSystemManager::getSingleton().createSystem(mParticulesName, mParticulesName);

		// On attache le systeme de particules a un scenenode
		mParticleSourceNode  = (SceneNode*)mSceneMgr->getRootSceneNode()->getChild("ParticleSourceNode"); // Le node a deja ete créé par createScene
		PosV = StringUtil::split(PartiPos, ",");
		X    = StringUtil::toReal(PosV.at(0));
		Y    = StringUtil::toReal(PosV.at(1));
		Z    = StringUtil::toReal(PosV.at(2));
		mParticleSourceNode->setPosition(X,Y,Z);	

		mParticleSourceNode->attachObject(mParticObject);
		mHasParticles = true;
		DEBUG_LOG("Particles updated");
	}


	// Gestion de la navette pour Memories02
	if (Picto!="")
	{
		mBillboardName = Picto; 

		PosV = StringUtil::split(PictoPos, ",");
		X    = StringUtil::toReal(PosV.at(0));
		Y    = StringUtil::toReal(PosV.at(1));
		Z    = StringUtil::toReal(PosV.at(2));

		BillboardSet* billboard_set1;
		billboard_set1= mSceneMgr->createBillboardSet(mBillboardName);
		billboard_set1->setMaterialName(mBillboardName);
		billboard_set1->setCullIndividually(true);
		billboard_set1->setDefaultDimensions(1500,1500);
		billboard_set1->createBillboard(0,0,0);
		mBillboardNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		mBillboardNode->attachObject(billboard_set1);
		mBillboardNode->setPosition(X,Y,Z);	
		mHasBillboard = true;
		DEBUG_LOG("Pictos updated");
	}


		// Gestion des cadrans pour Memories04
	if (LCDdisplay=="yes")
	{
		SceneNode* Cadran1_Node;
		SceneNode* Cadran2_Node;
		SceneNode* Cadran3_Node;
		SceneNode* Cadran4_Node;
		SceneNode* Cadran5_Node;
		Cadran1_Node = createDisplay("Memories05/cadran1",300,300);
		Cadran1_Node->setPosition(250,160,0);		// cadran principal
		Cadran2_Node = createDisplay("Memories05/cadran2",100,100);
		Cadran2_Node->setPosition(-70,100,-250);	// temperature
		Cadran3_Node = createDisplay("Memories05/cadran3",200,200);
		Cadran3_Node->setPosition(580,240,-320);	// cardiogramme
		Cadran4_Node = createDisplay("Memories05/cadran4",200,200);
		Cadran4_Node->setPosition(580,40,-320);	// encephalogramme
		Cadran5_Node = createDisplay("Memories05/cadran5",200,200);
		Cadran5_Node->setPosition(580,440,-320);	// thermal imaging

		// On fait bouger les couleurs thermales...
		ParticleSystem* pHeart=ParticleSystemManager::getSingleton().createSystem("heart","Memories05/heartbeat");
		// On attache le systeme de particules a un scenenode
		Cadran5_Node->attachObject(pHeart);
		mHasLCDdisplays = true;
		DEBUG_LOG("LCD displays updated");
	}

	DEBUG_LOG("<< showMemories");
}


/* ------------------------------------------------------------------------- */
// Creation d'un ecran LCD pour le memory dans le caisson d'hibernation
/* ------------------------------------------------------------------------- */
SceneNode* Memories::createDisplay(String Name, int Largeur, int Hauteur)
{
	// On cree des objets "plans" pour les cadrans
	Plane      Cadran_Plan;
	Entity*    Cadran_Ent;
	SceneNode* Cadran_Node;
	Cadran_Plan.d = 0;					// Nb of world units from the camera (300)
	Cadran_Plan.normal= Vector3::UNIT_Z;	// in front of the camera
	MeshManager::getSingleton().createPlane("plane"+Name,"DisplayGroup",Cadran_Plan,Largeur,Hauteur,1,1,true,1,1,1,Vector3::UNIT_Y);
	// largeur, hauteur, xSegments, ySegments, normals,coord-tex,uTiles,vTiles,upvector
	Cadran_Ent = mSceneMgr->createEntity(Name, Ogre::SceneManager::PT_PLANE); 
	Cadran_Ent->setMaterialName(Name);
	// On cree un node et on lui attache le cadran
	Cadran_Node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Name); 
	Cadran_Node->attachObject(Cadran_Ent);
	return Cadran_Node;
}



/* ------------------------------------------------------------------------- */
/// Change l'angle de vue dans le memories
/* ------------------------------------------------------------------------- */
void Memories::yaw(Degree angle)
{
	mCamera->yaw(angle); 
}
