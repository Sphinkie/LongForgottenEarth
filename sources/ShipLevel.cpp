/* ------------------------------------------------------------------------- */
//  File       : ShipLevel.cpp
//  Project    : Long Forgotten Earth
//  Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "macros.h"
#include "ShipLevel.h"
#include "GalaxyBox.h"

// Objets presents dans les pieces
#include "Button.h"
#include "Hibernation.h"
#include "Computer.h"
#include "Computer_Base.h"
#include "Computer_Logs.h"
#include "Computer_Control.h"
#include "Computer_Navigation.h"
#include "Computer_Maintenance.h"

/* ------------------------------------------------------------------------- */
/// Constructeur: Crée un étage vide.
/* ------------------------------------------------------------------------- */
ShipLevel::ShipLevel(String Name, int Altitude, SceneManager* SceneMgr, SceneNode* ParentNode)
{
	mLevelName = Name;
	mSceneMgr  = SceneMgr;
	mParentNode= ParentNode;
	mLevelNode = ParentNode->createChildSceneNode(Name);
	mLevelNode ->setPosition(0,Altitude,0);
	
	mRoomIndex   = 0;
	mDoorIndex   = 0;
	mXMLscene    = NULL;
	mElevatorRoom= NULL;
	mSceneLevel  = false;
	mSceneRoom   = false;
	mBlender     = false;

	// On initialise le Map
	struct t_Map VOIDdoor = {NULL, NULL, NULL, false};
	for (int i=0; i<MAP_SIZE; i++) mDoorMap[i] = VOIDdoor;
	for (int j=0; j<MAP_SIZE; j++) mRoomMap[j] = NULL;

	// On charge les informations sur ce niveau
	mXMLlevel = new ShipLevel_XMLparser("../media/objects/"+mLevelName+".xml");

	// On cree la scene
	DEBUG_LOG(mLevelName+": adding the scene -------------------------------- ");
	mXMLlevel->gotoTopOfFile();
	this->addScene();


	// On connecte les scenes au level
//	DEBUG_LOG(mLevelName+": connecting the scene -------------------------------- ");
//	mXMLlevel->gotoTopOfFile();
//	while (mXMLlevel->getNextScene())	{ this->linkScene(); }

	// Normalement, on n'a plus besoin du fichier Level.xml dans la suite.
	delete (mXMLlevel);
	mXMLlevel = NULL;
	DEBUG_LOG(mLevelName+": done  -------------------------------- ");
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
ShipLevel::~ShipLevel()
{
}


/* ------------------------------------------------------------------------- */
/// Renvoie la position du Level
/* ------------------------------------------------------------------------- */
Vector3 ShipLevel::getPosition()
{
	return mLevelNode->getPosition();
}


/* ------------------------------------------------------------------------- */
/// Ajoute une scene (contenant N Room + des porte et des objets)
/* ------------------------------------------------------------------------- */
void ShipLevel::addScene()
{
	mXMLlevel->getNextScene();

	mSceneLevel=true;

	// On analyse le node <scene> du fichier Level.xml
	String  SceneFile = mXMLlevel->getStringAttribute("file");
	Vector3 Pos       = mXMLlevel->getVectorAttribute("position");	
	mBlender          = (mXMLlevel->getStringAttribute("source")=="blender");	
	mLevelNode->translate(Pos);

	// On parse le fichier scene
	mXMLscene = new Scene_XMLparser("../media/"+SceneFile);

	if (mXMLscene->isPresent()) do 
	{
		// On cree les pieces 
		DEBUG_LOG(mLevelName+": adding the rooms -------------------------------- ");
		mXMLscene->gotoTopOfFile();
		while (mXMLscene->gotoNextRoom())	 {this->addRoom(); };

		// On cree les portes 
		DEBUG_LOG(mLevelName+": adding the doors -------------------------------- ");
		mXMLscene->gotoTopOfFile();
		while (mXMLscene->gotoNextDoor())	 {this->addDoor(); };
//		{
//			Door* SceneDoor = ; 
//
//		// On cree chaque piece
//		Room* SceneRoom = this->addRoom(index);
//		if (SceneRoom)
//		{	
//			// On cree la (ou les) porte de cette piece
//			Door* SceneDoor;
//			do {
//				SceneDoor = this->addDoor();
//				if (SceneDoor) 
//				{
//					// On repositionne la port par rapport à la piece à laquelle elle appartient.
//					SceneDoor->translate(SceneRoom->getPosition());
//					// On cree la connexion entre la Room et la Door de la scene.
//					mDoorMap[mDoorIndex].Porte = SceneDoor;
//					mDoorMap[mDoorIndex].insideRoom= SceneRoom;
//					DEBUG_LOG("   Door '"+SceneDoor->getName()+"' mapped in "+StringConverter::toString(mDoorIndex));
//					mDoorIndex++;
//			}
//			} while (SceneDoor);
			// On créera la connexion entre la scene et le reste du Level 
			// après avoir traité toutes les scenes.
//		}
//		index++;
	} while(mXMLscene->hasMoreNodes());

//	if (mBlender) mLevelNode->pitch(Degree(-90));	// On corrige l'orientation (bug Blender). 

	// On récupère la posiiton de la camera pour ce niveau.
	mCamPos = this->readCamPosition();

	delete (mXMLscene);
	mXMLscene=NULL;
	mSceneLevel=false;
}



/* ------------------------------------------------------------------------- */
/// Ajoute une piece (décrite dans le fichier level.xml ou scene.xml)
/** 
	Note: Le nom de la Room est le node name défini dans le fichier DotScene.
	@param index (optionel) Le No du node correspondant à la Room dans le fichier XML (DotScene)
	@return un pointeur sur la Room créée, ou NULL sinon.
*/
/* ------------------------------------------------------------------------- */
Room* ShipLevel::addRoom ()
{
	Room* RoomN= NULL;
//	bool  found_room = true;

	DEBUG_LOG(">> addRoom ()");
	DEBUG_LOG("   current RoomIndex: "+ITOA(mRoomIndex));

	// Si la piece est décrite dans une scene, on se place dessus.
//	if (mXMLscene)	 found_room = mXMLscene->gotoRoom(index);
//	if (!found_room) return NULL;

	/************************************************************************/
	/*  On recupere les parametres de <room> dans le fichier XML            */
	/************************************************************************/
	String  Name          = this->getXMLattribute("name");		
	String  MeshFile      = this->getXMLattribute("meshFile", "entity");
	Vector3 Pos           = this->getXMLvector("position");
	int     Orientation   = this->getXMLRotation();
	Quaternion Qorient    = this->getXMLOrientation();
	bool    ExtView       = false;
	bool    Elevator      = false;
	String  SoundFile     = "";
	String  ReferencedRoom= "";
	String  PanelMaterial = "";

//	if (mXMLscene)	 
//	{
		// Dans une scene, les données sont dans le fichier de configuration attaché à la Room
		String ConfigFileName = this->getXMLuserdata("DESCFILE");
		try
		{
			/// Lecture des parametres du fichier de config
			ConfigFile DescriptionFile;
			DescriptionFile.load("../media/objects/"+ConfigFileName);
			SoundFile    = DescriptionFile.getSetting("Sound");
			ExtView      =(DescriptionFile.getSetting("ExtView")=="true");
			Elevator     =(DescriptionFile.getSetting("elevator")=="true");
			PanelMaterial= DescriptionFile.getSetting("panel");
		}
		catch (Ogre::Exception& e) 
		{ TRACE("WARNING: file "+ConfigFileName+" not found "+ITOA(e.getNumber()));	}
//	}
//	else
//	{
//		// Dans un level, les données sont definis dans Level.xml
//		// au niveau du node courant qui est un node <room>.
//		SoundFile      = mXMLlevel->getStringAttribute("sound");
//		ExtView        = mXMLlevel->getBooleanAttribute("extview",false);	
//		ReferencedRoom = mXMLlevel->getStringAttribute("ref");				
//	}

	DEBUG_LOG("   name       ="+Name);
	DEBUG_LOG("   MeshFile   ="+MeshFile);
	DEBUG_LOG("   Pos        ="+StringConverter::toString(Pos));
	DEBUG_LOG("   Orientation="+StringConverter::toString(Qorient));
	DEBUG_LOG("   SoundFile  ="+SoundFile);
	DEBUG_LOG("   PanelMat   ="+PanelMaterial);
	TRACE(Name+": parameters loaded. ");

	if (ReferencedRoom=="")
	{
		// On cree la piece
		RoomN= new Room(Name, MeshFile, Pos, mSceneMgr,mLevelNode);
		// On memorise quelques caracteristiques
		RoomN->setOrientation(Degree(Orientation), Qorient, mSceneLevel); // Si dotScene, on utilise le quaternion
		RoomN->setExternView(ExtView);
		RoomN->setSoundFile(SoundFile);
		RoomN->setPanelMaterialName(PanelMaterial);
		RoomN->setElevatorRoom(Elevator);
		if (Elevator) mElevatorRoom=RoomN;
	}
	else
	{
		/* N'est plus utilisé avec les dotScene */
		// Cas d'une reference vers une Room existante (d'un autre niveau)
		/// Attention, on a une exception si ReferencedRoom n'existe pas...
		// On ne crée pas la Room, on la retrouve
		Entity*            E = mSceneMgr->getEntity(ReferencedRoom);
		UserDefinedObject* U = E->getUserObject();
		if (U != NULL)
			if (U->getTypeID() == ROOM_ID) RoomN = (Room*)U;	
		TRACE(Name+": ReferencedRoom found: "+RoomN->getName());
	}
	
	/************************************************************************/
	/* On ajoute la piece dans le plan du niveau (map)                      */
	/************************************************************************/	
	TRACE(Name+": mapped with index "+StringConverter::toString(mRoomIndex));
	mRoomMap[mRoomIndex++] = RoomN;

	/************************************************************************/
	/*  On cree tous les <object> associés à cette <room>                   */
	/************************************************************************/
	DEBUG_LOG(Name+": adding the objects");
	mXMLscene->gotoTopOfFile();
	// On attache tous les objets trouvés au node de la Room courante.
	while (mXMLscene->getNextSubObject())	{ this->addObject(RoomN->getNode()); }


	/************************************************************************/
	/* On cree tous les lumières associées à cette pièce                    */
	/************************************************************************/
	DEBUG_LOG(Name+": adding the lights");
	mXMLscene->gotoTopOfFile();
	while (mXMLscene->getNextSubLight())	{ this->addLight(RoomN); }
	
	// On n'affiche pas les pieces et leurs objets
	RoomN->removeFromScene();
	DEBUG_LOG(Name+": Room added to Level");
	return RoomN;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la position du Node <\camera\> (décrit dans le fichier scene.xml)
/** 
	Dans le fichier DotScene, il y a possibilité de placer un node 'camera', avec une position.
	C'est utilie pour la mise au point des niveau.
	@return La position de la caméra.
*/
/* ------------------------------------------------------------------------- */
Vector3 ShipLevel::readCamPosition()
{
	DEBUG_LOG(">> readCamPosition()");
	Vector3 Pos = Vector3::ZERO;
	String  Name= "Camera";

	if (mXMLscene->gotoCameraNode())
	{
		Name = this->getXMLattribute("name");	
		Pos  = this->getXMLvector("position");
		DEBUG_LOG("   name="+Name);
		DEBUG_LOG("   Pos="+StringConverter::toString(Pos));
		TRACE(Name+": parameters loaded");
	}

	return Pos;
}


/* ------------------------------------------------------------------------- */
/// Ajoute une Porte (décrite dans un fichier XML dotScene)
/** 
	Dans le fichier XML, on doit être positionné sur un node DOOR.
	@return Un pointeur vers l'objet Door créé. Ou NULL sinon.
*/
/* ------------------------------------------------------------------------- */
Door* ShipLevel::addDoor()
{
	DEBUG_LOG(">> addDoor()");

//	bool door_found = false;
	bool panel_found= false;

//	if (mXMLscene)	door_found = mXMLscene->gotoDoor(index);
//	if (!door_found) return NULL;

	String  Name        = this->getXMLattribute("name");	
	String  MeshFile    = this->getXMLattribute("meshFile", "entity");
	Vector3 Pos         = this->getXMLvector("position");
	int     Orientation = this->getXMLRotation();
	Quaternion Qorient  = this->getXMLOrientation();
	String  Anim        = this->getXMLuserdata("ANIMATION"); 
	String  InsideRoom  = this->getXMLuserdata("INSIDE"); 
	String  OutsideRoom = this->getXMLuserdata("OUTSIDE"); 
	bool    Elevator    = false;
	String  InsidePanel = "";
	String  OutsidePanel= "";
	String  PanelMesh   = "";
	Vector3 PanelPos    = Vector3::ZERO;
	Quaternion PanelOri = Quaternion::ZERO;

	// Dans la syntaxe SCENE, il y a un objet "panel" attaché à la porte.
	panel_found = mXMLscene->gotoDoorPanel();
	// Des données supplementaires sont présentes dans le fichier de configuration attaché à la Room
	String ConfigFileName = this->getXMLuserdata("DESCFILE");
	if (ConfigFileName!="") try
	{
		/// Lecture des parametres du fichier de config
		ConfigFile DescriptionFile;
		DescriptionFile.load("../media/objects/"+ConfigFileName);
		// SoundFile    = DescriptionFile.getSetting("Sound");
		Elevator     =(DescriptionFile.getSetting("elevator")=="true");
	}
	catch (...) 
	{ TRACE("WARNING: Door config file not found ");	}

	if (panel_found)
	{
		PanelMesh = this->getXMLattribute("meshFile", "entity");
		PanelPos  = this->getXMLvector("position");
		PanelOri  = this->getXMLOrientation();
//		Fpanel    = this->getXMLuserdata("INSIDE");
//		Bpanel    = this->getXMLuserdata("OUTSIDE");
	}

	DEBUG_LOG("   name        ="+Name);
	DEBUG_LOG("   MeshFile    ="+MeshFile);
	DEBUG_LOG("   Pos         ="+StringConverter::toString(Pos));
	DEBUG_LOG("   Qorient     ="+StringConverter::toString(Qorient));
	DEBUG_LOG("   Anim        ="+Anim);
	DEBUG_LOG("   InsideRoom  ="+InsideRoom);
	DEBUG_LOG("   OutsideRoom ="+OutsideRoom);
	DEBUG_LOG("   Elevat Door ="+StringConverter::toString(Elevator));
	DEBUG_LOG("   Panel found ="+StringConverter::toString(panel_found));
	DEBUG_LOG("   Panel mesh  ="+PanelMesh);
	DEBUG_LOG("   Panel inside="+InsidePanel);
	TRACE(Name+": parameters loaded");

	// On cree la porte
	Door* DoorN= new Door(Name, MeshFile, mSceneMgr, mLevelNode);
	DoorN->setOrientation(Orientation, Qorient, true); // dotScene: on utilise le quaternion
	if (panel_found) DoorN->addPanel(PanelMesh,PanelPos,PanelOri);
	DoorN->addToScene(Pos);
	//DoorN->setPanelMaterialNames(Fpanel, Bpanel);      // On initialise les panels avec "inside" et "outside"
	DoorN->setAnimation(Anim);
	DoorN->setElevatorDoor(Elevator);

	
	// on peut etablir dès maintenant la connexion entre les deux pieces adjacentes à la porte.
	// On connecte les pieces adjacentes dans le plan du niveau (map)
	int Index1 = -1;
	int Index2 = -1;

	if (InsideRoom !="")  
	{
		Index1=getMapIndex(InsideRoom);
		// On récupère le panel des 2 pièces.
		// Note: Le nom de la pièce va à l'exterieur, et vice-versa
		OutsidePanel= mRoomMap[Index1]->getPanelMaterialName();	
	}
	if (OutsideRoom!="") 
	{
		Index2 = getMapIndex(OutsideRoom);
		mDoorMap[mDoorIndex].Porte = DoorN;
		mDoorMap[mDoorIndex].insideRoom  = mRoomMap[Index1];
		mDoorMap[mDoorIndex].outsideRoom = mRoomMap[Index2];
		InsidePanel = mRoomMap[Index2]->getPanelMaterialName();
	}
	DoorN->setPanelMaterialNames(InsidePanel,OutsidePanel);
	TRACE(Name +": connecting "+ITOA(Index2)+" to "+ITOA(Index1)+" in the map");
	TRACE(Name +": connecting "+OutsideRoom +" to "+InsideRoom  +" in the map");


	DEBUG_LOG("   Door '"+DoorN->getName()+"' mapped in "+StringConverter::toString(mDoorIndex));
	mDoorIndex++;

	return DoorN;
}


/* ------------------------------------------------------------------------- */
/// Ajoute une lumière à une piece (décrit dans le fichier level.xml ou scene.xml)
/* Cette fonction gère les lumières de type:
- point
- spot
- directionnelle
*/
/* ------------------------------------------------------------------------- */
void ShipLevel::addLight(Room* ParentRoom)
{
	if (!mXMLscene) return;

	// On recupere les parametres de <light> dans le fichier XML
	String  LightName  = this->getXMLattribute("name");	
	Vector3 Pos        = this->getXMLvector("position");
	String  LightType  = this->getXMLattribute("type", "light");
	int     Range      = this->getXMLLightRange();
	Vector3 Couleur    = this->getXMLColour();
	Vector3 Direction  =-this->getXMLLightDirection();
	bool    castShadows= this->getXMLboolean("castShadows", false, "light");
	bool    mainLight  =(this->getXMLuserdata("MAINLIGHT")=="1");
	
	// Paramètres supplémentaires dans le cas d'un spot
	mXMLscene->gotoSubElement("light","lightRange");
	Real InnerRange = mXMLscene->getRealAttribute("inner",35); // default=35
	Real OuterRange = mXMLscene->getRealAttribute("outer",70); // default=70
	Real Falloff    = mXMLscene->getRealAttribute("falloff",1);// default=1
	mXMLscene->returnToParentNode();
	
	ColourValue CouleurV = ColourValue(Couleur.x,Couleur.y,Couleur.z);

	// S'il s'agit de la lumière pricipale de la Piece, on force son nom.
	if (mainLight) LightName=ROOMLIGHT;  

	if (mBlender)
	{
		// Correctif Blender: Les axes Y et Z sont inversés
		Real swap = Pos.z;
		Pos.z = Pos.y;
		Pos.y = swap;
	}

	DEBUG_LOG("   Name      = "+LightName);
	DEBUG_LOG("   LightType = "+LightType);
	DEBUG_LOG("   Position  = "+StringConverter::toString(Pos));
	DEBUG_LOG("   Range     = "+StringConverter::toString(Range));
	DEBUG_LOG("   Couleur   = "+StringConverter::toString(CouleurV));
	DEBUG_LOG("   Direction = "+StringConverter::toString(Direction));
	DEBUG_LOG("   Shadows   = "+StringConverter::toString(castShadows));
	DEBUG_LOG("   mainLight = "+StringConverter::toString(mainLight));

	// On cree la lumière
	// Lumière de type omnidirectionnelle (par defaut)
	if ((LightType == "point") || (LightType == ""))
	{
		ParentRoom->createLight(Ogre::Light::LT_POINT, LightName, castShadows);
		ParentRoom->setLightPosition(Pos,LightName);
		ParentRoom->setLightColour(CouleurV,LightName);
		ParentRoom->setLightRange(Range,LightName);
		ParentRoom->setLight(true,LightName);
	}

	// Lumiere de type spot
	if (LightType == "spot")	
	{
		if (Range==0) Range=600;
		ParentRoom->createLight(Ogre::Light::LT_SPOTLIGHT, LightName, castShadows);
		ParentRoom->setLightPosition(Pos, LightName);
		ParentRoom->setLightDirection(Direction,LightName);
		ParentRoom->setLightRange(Range, LightName);
		ParentRoom->setLightColour(CouleurV, LightName);
		ParentRoom->setLight(true, LightName);
		ParentRoom->setSpotLightRange(InnerRange, OuterRange, Falloff, LightName);
	}


	// Lumière de type directionnelle 
	if (LightType == "directional")
	{
		ParentRoom->createLight(Ogre::Light::LT_DIRECTIONAL,LightName, castShadows);
		ParentRoom->setLightDirection(Direction,LightName);
		ParentRoom->setLightColour(CouleurV,LightName);
		ParentRoom->setLight(true,LightName);
	}
	DEBUG_LOG(LightName+": added to Room");
}


/* ------------------------------------------------------------------------- */
/// Ajoute un objet à une piece.
/** 
	Cet objet est décrit dans le fichier XML dotScene.
	Il est ajouté là où est positionné le pointeur mCurrentNode.
	@param ParentNode Le node Parent de cet objet au sens 'Ogre" (en général: la Room).
*/
/* ------------------------------------------------------------------------- */
void ShipLevel::addObject(SceneNode* ParentNode)
{
	DEBUG_LOG(">> addObject()");
	// On recupere les parametres de <object> dans le fichier XML
	String  ObjectClass= this->getXMLuserdata("class");
	if (ObjectClass == "door") return;		// Objet Door: Ils sont gérés par une autre fonction
	String  Name          = this->getXMLattribute("name");		  
	String  MeshFile      = this->getXMLattribute("meshFile", "entity");
	Vector3 Pos           = this->getXMLvector("position");
	String  DescFile      = this->getXMLuserdata("descfile");
	int     Orientation   = this->getXMLRotation();
	Quaternion Qorient    = this->getXMLOrientation();
	Vector3 Scale         = this->getXMLvector("scale");
	bool    Shadows       = true; // default value

	Shadows = !(this->getXMLuserdata("CASTSHADOWS")=="0"); // TRUE si non defini (default value)

	DEBUG_LOG("   name       ="+Name);
	DEBUG_LOG("   MeshFile   ="+MeshFile);
	DEBUG_LOG("   Class      ="+ObjectClass);
	DEBUG_LOG("   DescFile   ="+DescFile);
	DEBUG_LOG("   Shadows    ="+StringConverter::toString(Shadows));
	DEBUG_LOG("   Position   ="+StringConverter::toString(Pos));
	DEBUG_LOG("   Orientation="+StringConverter::toString(Orientation));
	DEBUG_LOG(Name+": parameters loaded");

	// On cree l'objet decrit dans le XML, selon sa classe

	using namespace OgreOpcode;
	CollisionContext* collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();

	// -----------------------------------------------------------
	// Objet simple: juste un Node immobile avec une Entity
	// -----------------------------------------------------------
	if ((ObjectClass == "simple") || (ObjectClass==""))
	{
		// On cree l'entité
		SceneNode* ObjectNode  = ParentNode->createChildSceneNode(Name);
		Entity*    ObjectEntity= mSceneMgr->createEntity(Name, MeshFile);
		ObjectEntity->setQueryFlags(IMMOBILE);
		ObjectNode->setPosition(Pos);
		this->rotateNode(ObjectNode, Orientation, Qorient);
		// Eventuellement, on lui enleve ses ombres projetées
		if (!Shadows) ObjectEntity->setCastShadows(false);
		if (Scale != Vector3::ZERO)
		{
			ObjectNode->setScale(Scale);
		}
		ObjectNode->attachObject(ObjectEntity);
		CollisionShape *CollideShape = CollisionManager::getSingletonPtr()->NewShape(Name.c_str());
		CollideShape->Load(ObjectEntity);
		CollisionObject* CollideObject = collideContext->NewObject();
		CollideObject->SetCollClass("object");
		CollideObject->SetShape(CollideShape);
		CollideObject->setDebug(false);
		collideContext->AddObject(CollideObject);
	}

	// -----------------------------------------------------------
	// Un Node simple, une Entity et une texture de type Cube Mapping
	// -----------------------------------------------------------
	else if (ObjectClass == "cubemap")
	{
		// On cree l'entité
		SceneNode* ObjectNode = ParentNode->createChildSceneNode(Name);
		Entity* ObjectEntity = mSceneMgr->createEntity(Name, MeshFile);
		ObjectEntity->setMaterialName("Test/CubeMap");
		ObjectEntity->setQueryFlags(IMMOBILE);
		ObjectNode->setPosition(Pos);
		this->rotateNode(ObjectNode, Orientation, Qorient);
		// Eventuellement, on lui enleve ses ombres projetées
		if (!Shadows) ObjectEntity->setCastShadows(false);
		ObjectNode->attachObject(ObjectEntity);
	}

	// -----------------------------------------------------------
	// Un Node simple, une Entity et une texture de type Normal Mapping
	// -----------------------------------------------------------
	else if (ObjectClass == "normalmap")
	{
		SceneNode* ObjectNode = ParentNode->createChildSceneNode(Name);
		

        // Load the meshes with non-default HBU options
		
		MeshPtr pMesh = MeshManager::getSingleton().load
			(MeshFile,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,    
            HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
			true, true); //so we can still read it

		// Build tangent vectors, all our meshes use only 1 texture coordset 
		// On peut aussi creer les tangeantes (=normales) lors de l'exportation du fichier mesh
		unsigned short src, dest;
		if (!pMesh->suggestTangentVectorBuildParams(src, dest))
		{
			pMesh->buildTangentVectors(src, dest);
		}

		// On cree l'entité
		Entity* ObjectEntity = mSceneMgr->createEntity(Name, MeshFile);
//		ObjectEntity->setMaterialName("Test/NormalMap");
		ObjectEntity->setMaterialName("Test/NormalMapped");
//		ObjectEntity->setMaterialName("Test/NormalMappedSpecular");
		ObjectEntity->setVisible(true);
		ObjectEntity->setQueryFlags(IMMOBILE);

		
		/*
		/// TODO: à mettre dans le début.
		
        // check that vertex programs and dot3 or fragment programs are supported
		const RenderSystemCapabilities* caps = Root::getSingleton().getRenderSystem()->getCapabilities();
        if (!caps->hasCapability(RSC_VERTEX_PROGRAM))
        {
            OGRE_EXCEPT(1, "Your card does not support vertex programs, so cannot "
                "run this demo. Sorry!", 
                "Dot3Bump::createScene");
        }
        if (!(caps->hasCapability(RSC_FRAGMENT_PROGRAM) 
			|| caps->hasCapability(RSC_DOT3)) )
        {
            OGRE_EXCEPT(1, "Your card does not support dot3 blending or fragment programs, so cannot "
                "run this demo. Sorry!", 
                "Dot3Bump::createScene");
        }
        if (!caps->hasCapability(RSC_FRAGMENT_PROGRAM))
        {
            mInfo->setCaption("NOTE: Light colours and specular highlights are not supported by your card.");
        }
		*/

		ObjectNode->setPosition(Pos);
		this->rotateNode(ObjectNode, Orientation,Qorient);
		// Eventuellement, on lui enleve ses ombres projetées
		if (!Shadows) ObjectEntity->setCastShadows(false);
		ObjectNode->attachObject(ObjectEntity);
	}

	// -----------------------------------------------------------
	// Un Node simple, une Entity et une texture de type Environnement Mapping
	// -----------------------------------------------------------
	else if (ObjectClass == "envmap")
	{
		// On cree l'entité
		SceneNode* ObjectNode = ParentNode->createChildSceneNode(Name);
		Entity* ObjectEntity = mSceneMgr->createEntity(Name, MeshFile);
		ObjectEntity->setMaterialName("Test/EnvMap");
		ObjectEntity->setQueryFlags(IMMOBILE);
		ObjectNode->setPosition(Pos);
		this->rotateNode(ObjectNode, Orientation,Qorient);
		// Eventuellement, on lui enleve ses ombres projetées
		if (!Shadows) ObjectEntity->setCastShadows(false);
		ObjectNode->attachObject(ObjectEntity);
	}

	// -----------------------------------------------------------
	// Objet de type CAISSON D'HIBERNATION
	// -----------------------------------------------------------
	else if (ObjectClass == "hibernation")
	{
		// On a un fichier de description par instance: donc il porte le nom de l'instance.
		Hibernation* Object = new Hibernation(Name, MeshFile, ParentNode, mSceneMgr);
		Object->setPosition(Pos);
		Object->setOrientation(Qorient);
		Object->addToScene();
	}

	// -----------------------------------------------------------
	// Objet de type BOUTON POUSSOIR
	// -----------------------------------------------------------
	else if (ObjectClass == "button")
	{
		// On a un fichier de description par famille de boutons (UP, DOWN, etc)
		Button* Object = new Button(Name, MeshFile, DescFile, ParentNode, mSceneMgr);
		Object->setPosition(Pos);
		Object->setOrientation(Qorient);
		Object->addToScene();
	}

	// -----------------------------------------------------------
	// Objet de type Computer (maintenance, control, etc)
	// -----------------------------------------------------------
	else if (ObjectClass == "computer/maint")
	{
		Computer* Object = new Computer_Maintenance(MeshFile, DescFile, ParentNode, mSceneMgr);
		Object->setPosition(Pos);
		Object->setOrientation(Qorient);
		Object->addToScene();
	}
	else if (ObjectClass == "computer/control")
	{
		Computer* Object = new Computer_Control(MeshFile, DescFile, ParentNode, mSceneMgr);
		Object->setPosition(Pos);
		Object->setOrientation(Qorient);
		Object->addToScene();
	}
	else if (ObjectClass == "computer/basic")
	{
		Computer* Object = new Computer_Base(MeshFile, DescFile, ParentNode, mSceneMgr);
		Object->setPosition(Pos);
		Object->setOrientation(Qorient);
		Object->addToScene();
	}
	else if (ObjectClass == "computer/nav")
	{
		Computer* Object = new Computer_Navigation(MeshFile, DescFile, ParentNode, mSceneMgr);
		Object->setPosition(Pos);
		Object->setOrientation(Qorient);
		Object->addToScene();
	}
	else if (ObjectClass == "computer/logs")
	{
		Computer* Object = new Computer_Logs(MeshFile, DescFile, ParentNode, mSceneMgr);
		Object->setPosition(Pos);
		Object->setOrientation(Qorient);
		Object->addToScene();
	}

	DEBUG_LOG(   Name+": added to Room");
}



/* ------------------------------------------------------------------------- */
/// Allume (ou éteint) les lumières de toutes les pieces du niveau.
/* ------------------------------------------------------------------------- */
void ShipLevel::setAllLights(bool On)
{
	/// Le tableau mRoomMap doit être construit avant d'appeler cette fonction.
	int j = 0;

	while (mRoomMap[j])
	{
		DEBUG_LOG ("Set light for room "+mRoomMap[j]->getName());
		mRoomMap[j]->setLight(On); 
		j++;
	}
}


/* ------------------------------------------------------------------------- */
/// Les lumières principales de toutes les pieces du niveau passent au rouge.
/* ------------------------------------------------------------------------- */
void ShipLevel::setRedLight(bool On)
{
	/// Le tableau mRoomMap doit être construit avant d'appeler cette fonction.
	int j = 0;

	while (mRoomMap[j])
	{
		DEBUG_LOG ("Switch Red light for room "+mRoomMap[j]->getName());
		mRoomMap[j]->setNightLight(On); 
		j++;
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'index dans le tableau mRoomMap de la Room demandée.
/** 
	@param RoomName Le nom de la Room recherchée.
	@return L'index de la pièce dans la map, ou -1 en cas d'erreur.
*/
/* ------------------------------------------------------------------------- */
int ShipLevel::getMapIndex(String RoomName)
{
	if (RoomName=="") return -1;

	for (int j=0; j<=MAP_SIZE; j++)
	{
		if (mRoomMap[j]) 
			if (mRoomMap[j]->getName() == RoomName) return j;
	}
	return -1;
}



/* ------------------------------------------------------------------------- */
/// On connecte les differentes pièces du ShipLevel entre elles, grace aux portes.
/* ------------------------------------------------------------------------- */
void ShipLevel::linkScene()
{
	DEBUG_LOG(">> linkScene()");
 
	int    index       = 0;
	int    indexIn     = -1;
	int    indexOut    = -1;
	bool   linkfound   = false;
	Room*  insideRoom  = NULL;
	Room*  outsideRoom = NULL;
	String outsidePanel;
	String insidePanel;

	mXMLlevel->gotoElement("map");

	do
	{	
		linkfound = mXMLlevel->gotoSubElement("link", index++);
		String RoomIn  = mXMLlevel->getStringAttribute("nodename");
		String RoomOut = mXMLlevel->getStringAttribute("previous");	// outside
		TRACE("   connecting "+RoomIn+" to "+RoomOut+" in the map");

		// On recupère les index des deux pièces
		indexIn  = this->getMapIndex(RoomIn);
		indexOut = this->getMapIndex(RoomOut);

		// On recupère les pointeurs des deux piècss
		if (indexIn!=-1)  insideRoom =mRoomMap[indexIn];
		if (indexOut!=-1) outsideRoom=mRoomMap[indexOut];

		// On récupère le panel des 2 pièces.
		// Note: Le nom de la pièce va à l'exterieur, et vice-versa
		if (insideRoom)  outsidePanel= insideRoom->getPanelMaterialName();
		if (outsideRoom) insidePanel = outsideRoom->getPanelMaterialName();

		// On parcourt la liste des portes, pour trouver celles qui donnent sur la première pièce.
		for (int i=0; i<MAP_SIZE; i++)
		{
			if (mDoorMap[i].insideRoom==insideRoom)
			{
				// Quand on en trouve, on cree l'association avec la pièce exterieure
				mDoorMap[i].outsideRoom=outsideRoom;
				// et on affecte les panels.
				if(mDoorMap[i].Porte) mDoorMap[i].Porte->setPanelMaterialNames(insidePanel,outsidePanel);
			}
		}


		DEBUG_LOG("   done with index: "+StringConverter::toString(indexIn)+" and "+StringConverter::toString(indexOut));
	} while (linkfound);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction affiche la Room demandée, et efface toutes les autres.
/** 
Cela permet de gagner du framerate opur l'affichage.
Si la pièce à afficher est un Elevator (de ce niveau, ou d'un autre niveau) alors
alors on n'efface pas l'ascenseur de ce niveau.
*/
/* ------------------------------------------------------------------------- */
void ShipLevel::showOneRoom(Room* room)
{
	int j=0;
	if (room==NULL) return;
	DEBUG_LOG(">> showOneRoom(*"+room->getName()+")");

	// Si la pièce demandée appartient à ce niveau, on l'affiche.
	// Et on efface toutes les autres.
	while (mRoomMap[j])
	{
		if (mRoomMap[j] == room)  
		{
			room->addToScene();
		}
		else
		{
			// Si la pièce examinée est l'ascenseur de ce niveau
			// et que la pièce a afficher est un ascesuer (d'un autre niveau, à priori)
			if (mRoomMap[j]->isElevatorRoom() && room->isElevatorRoom())	
			{
				// Alors, on ne fait rien.
				// A priori, la section d'ascenseur est déjà affichée
			}
			else
				// Sinon, on efface la pièce.
				mRoomMap[j]->removeFromScene();
		}
		j++;
	}
}


/* ------------------------------------------------------------------------- */
/// Affiche la pièce demandée (les autres sont effacées pour gagner en framerate).
/* ------------------------------------------------------------------------- */
void ShipLevel::showOneRoom(String roomName)
{
	int j=0;
	if (roomName=="") return;
	DEBUG_LOG(">> showOneRoom("+roomName+")");

	while (mRoomMap[j])
	{
		if (mRoomMap[j]->getName() == roomName)  
		{
			mRoomMap[j]->addToScene();
		}
		else
		{
			// TODO: A améliorer: cette fonction n'efface pas les ascenseurs.
			// il faudrait la méthode meilleure de l'autre fonction
			if (!mRoomMap[j]->isElevatorRoom() )	
				mRoomMap[j]->removeFromScene();
		}
		j++;
	}
}

/* ------------------------------------------------------------------------- */
/// Affiche les pièces voisines de la porte.
/* ------------------------------------------------------------------------- */
void ShipLevel::showAdjacentRooms(Door* Porte)
{
	DEBUG_LOG(">> ShipLevel::showAdjacentRooms()");
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (mDoorMap[j].Porte == Porte) 
		{
			if (mDoorMap[j].insideRoom)  mDoorMap[j].insideRoom->addToScene();
			if (mDoorMap[j].outsideRoom) mDoorMap[j].outsideRoom->addToScene();
		}
	}
}

/* ------------------------------------------------------------------------- */
/// Affiche toutes les pièces du niveau.
/* ------------------------------------------------------------------------- */
void ShipLevel::showAllRooms()
{
	DEBUG_LOG(">> "+mLevelName+"::showAllRooms()" );
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (mRoomMap[j]) mRoomMap[j]->addToScene();
	}
}


/* ------------------------------------------------------------------------- */
/// Enleve toutes les pièces du niveau.
/* ------------------------------------------------------------------------- */
void ShipLevel::hideAllRooms()
{
	DEBUG_LOG(">> "+mLevelName+"::hideAllRooms()" );
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (mRoomMap[j]) mRoomMap[j]->removeFromScene();
	}
}


/* ------------------------------------------------------------------------- */
/// Affiche la pièce qui fait partie de l'ascenseur.
/* ------------------------------------------------------------------------- */
void ShipLevel::showElevator()
{
	if (mElevatorRoom)
	{
		mElevatorRoom->addToScene();
	}
}

/* ------------------------------------------------------------------------- */
/// Cette fonction demande l'ouverture d'une porte.
/**
@param Porte La référence de la porte à ouvrir.
@return La fonction renvoie TRUE  si la porte appartient bien à ce level, et a été ouverte.
	Elle renvoie FALSE si la porte n'appartient pas à ce level.
	ou si la porte appartient à ce level, mais n'a pu etre ouverte.
*/
/* ------------------------------------------------------------------------- */
bool ShipLevel::openDoor(Door* Porte)
{
	bool found = false;
	DEBUG_LOG(">> ShipLevel::openDoor()");
	if (!Porte) return false;

	// Parcourt toutes les portes, pour trouver celle dont le pointeur correspond
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (mDoorMap[j].Porte == Porte) 
		{
			// on demande l'affichage des pieces adjacentes à cette porte
			this->showAdjacentRooms(Porte);
			// On ouvre la porte
			found = Porte->openDoor();
		}
	}
	return found;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction demande l'ouverture d'une porte.
/**
@param DoorName Le nom de la porte à ouvrir.
@return La fonction renvoie TRUE  si la porte appartient bien à ce level, et a été ouverte.
	Elle renvoie FALSE si la porte n'appartient pas à ce level.
	ou si la porte appartient à ce level, mais n'a pu etre ouverte.
*/
/* ------------------------------------------------------------------------- */
bool ShipLevel::openDoor(String DoorName)
{
	bool found = false;

	// Parcourt toutes les portes du Level, pour trouver celle dont le nom correspond
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (mDoorMap[j].Porte) 
		{
			if (mDoorMap[j].Porte->getName()==DoorName)
			{
				// on demande l'affichage des pieces adjacentes à cette porte
				this->showAdjacentRooms(mDoorMap[j].Porte);
				// On ouvre la porte
				found = mDoorMap[j].Porte->openDoor();
				// On quitte dès qu'on a trouvé la bonne porte : TODO
			}
		}
	}
	return found;
}



/* ------------------------------------------------------------------------- */
/// Envoie l'état d'alerte à toutes les pièces du niveau
/* ------------------------------------------------------------------------- */
void ShipLevel::setAlertLevel(short alert)
{
	// Parcourt toutes les pièce
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (mRoomMap[j])    mRoomMap[j]->setAlertLevel(alert);
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie le status du niveau sous forme de chaine XML
/* ------------------------------------------------------------------------- */
String ShipLevel::getStatus()
{
	String Xstatus="";

	// Parcourt toutes les portes
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (mDoorMap[j].Porte)  Xstatus += mDoorMap[j].Porte->getStatus();
		if (mRoomMap[j])        Xstatus += mRoomMap[j]->getStatus();
	}
	// DEBUG_LOG(">> ShipLevel Status = \n" + Xstatus);
	return Xstatus;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la position initiale de la camera dans ce niveau telle que décrite dans le fichier scene.xml (debug).
/** 
	@return La position de la camera par rapport au node parent (TheShip)
*/
/* ------------------------------------------------------------------------- */
Vector3 ShipLevel::getCamPos()
{
	return (mCamPos + mLevelNode->getPosition());
}

/* ------------------------------------------------------------------------- */
/// Etablit l'état initial du vaisseau.
/* ------------------------------------------------------------------------- */
void ShipLevel::setInitialRoom()
{
	// Si des pieces sont depressurisées, leur porte est bloquée.
	for (int j=0; j<MAP_SIZE; j++)
	{
		if (!mDoorMap[j].Porte) break;
		if (!mDoorMap[j].insideRoom) break;
		if (mDoorMap[j].insideRoom->isBroken()) mDoorMap[j].Porte->setDoorDepressurized();
	}
}



/* ------------------------------------------------------------------------- */
/// Retrouve une piece par son nom. Renvoie NULL si la piece n'est pas à ce niveau.
/* ------------------------------------------------------------------------- */
Room* ShipLevel::getRoom(String RoomName)
{
	Room* Room_found = NULL;

	// Parcourt toutes les pieces du Level, pour trouver celle dont le nom correspond
	for (int i=0; i<MAP_SIZE; i++)
	{
		if (mRoomMap[i]) 
		{
			if (mRoomMap[i]->getName()==RoomName)
			{
				Room_found = mRoomMap[i];
			}
		}
	}
	return Room_found;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le vecteur demandé, soit du fichier XML de la scene ou du niveau.
/** 
Dans le cas d'un fichier scene.xml, les attributs attendus dans le vecteur sont "x=", "y=" et "z=".
On l'utilise en paticulier pour "position" et "scale".
La valeur par defaut est un vecteur Nul.
Note: ne fonctionne pas pour "colour", car les attributs de dotScene sont alors r, g, b.
*/
/* ------------------------------------------------------------------------- */
Vector3 ShipLevel::getXMLvector(String childnode)
{
	Vector3 Value = Vector3::ZERO;
	if (mXMLscene) 
	{
		// Si ce child element n'existe pas, on ne fait rien.
		if (mXMLscene->gotoSubElement(childnode,0))
		{
			Real x = mXMLscene->getRealAttribute("x");
			Real y = mXMLscene->getRealAttribute("y");
			Real z = mXMLscene->getRealAttribute("z");
			Value = Vector3(x,y,z);
			mXMLscene->gotoParentElement();
		}
	}
	else
		Value = mXMLlevel->getVectorAttribute(childnode);
	return Value ;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la couleur du node "light".
/** La valeur par defaut est le noir. */
/* ------------------------------------------------------------------------- */
Vector3 ShipLevel::getXMLColour()
{
	Vector3 Value = Vector3::ZERO;

	if (mXMLscene->gotoSubElement("light","colourDiffuse"))
	{
		Real r = mXMLscene->getRealAttribute("r");
		Real g = mXMLscene->getRealAttribute("g");
		Real b = mXMLscene->getRealAttribute("b");
		Value = Vector3(r,g,b);
		mXMLscene->returnToParentNode();
	}
	return Value ;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la direction d'une lumière directionnelle
/* ------------------------------------------------------------------------- */
Vector3 ShipLevel::getXMLLightDirection()
{
	Vector3 Value ;
	if (mXMLscene)
	{
		if (mXMLscene->gotoSubElement("light","normal"))
		{
			Real x = mXMLscene->getRealAttribute("x");
			Real y = mXMLscene->getRealAttribute("y");
			Real z = mXMLscene->getRealAttribute("z");
			Value = Vector3(x,y,z);
			mXMLscene->returnToParentNode();
		}
	}
	else
	{
		Value = mXMLlevel->getVectorAttribute("direction");
	}
	return Value ;
}



/* ------------------------------------------------------------------------- */
/// Renvoie le range du node "light"
/* ------------------------------------------------------------------------- */
Real ShipLevel::getXMLLightRange()
{
	Real Value ;
	if (mXMLscene)
	{
		Value = StringConverter::parseReal(this->getXMLuserdata("RANGE"));
		// Ci-dessous serait la façon normale de récupérer le range, 
		// mais elle ne marche pas en raison d'un bug de l'exporter Blender.
		// mXMLscene->gotoSubElement("light","lightAttenuation");
		// Value = mXMLscene->getRealAttribute("range",10);
		// mXMLscene->returnToParentNode();
	}
	else
	{
		Value = mXMLlevel->getRealAttribute("range");
	}
	return Value;
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'orientation du node décrit dans le XML (Room, Door, Object, Light, etc) 
/// par rapport à l'axe vertical.
/** 
	La valeur lue dans le XML "dotScene" est un réel [0..1] pour lequel "1" correspond à 380.
	(je ne sais pas ce que c'est comme unité).
	La valeur lue dans le XML "level" est un entier en degrés.
	@return La valeur correspondante en degrés.
	@sa getXMLOrientation
*/
/* ------------------------------------------------------------------------- */
int ShipLevel::getXMLRotation()
{
	int Value;

	if (mXMLscene)
		Value = 0; // dans le dotScene, il faut utiliser le quaternion
	else
		Value = mXMLlevel->getIntAttribute("rotation");
	return Value ;
}



/* ------------------------------------------------------------------------- */
/// Renvoie un attribut du fichier XML.
/** 
Dans le cas d'un attribut de scene.xml, c'est un attribut de l'un des fils du node courant.
Dans le cas d'un attribut de level.xml, c'est un attribut direct du node courant, et n'y a pas besoin de le chercher dans un node fils.
Si childnode est vide, alors c'est un attribut du node courant (et pas d'un fils)
Les noms des attributs sont les mêmes pour les deux formats XML.
*/
/* ------------------------------------------------------------------------- */
String ShipLevel::getXMLattribute(String attribute, String childnode)
{
	String Value ;
	DEBUG_LOG(">> getXMLattribute("+attribute+", "+childnode+")");

	if (mXMLscene)
	{
		if (childnode!="")	mXMLscene->gotoSubElement(childnode,0);
		Value = mXMLscene->getStringAttribute(attribute);
		if (childnode!="")	mXMLscene->gotoParentElement();
	}
	else
	{
		Value = mXMLlevel->getStringAttribute(attribute);
	}
	DEBUG_LOG("<< getXMLattribute("+attribute+", "+childnode+") = "+Value);
	return Value ;
}


/* ------------------------------------------------------------------------- */
/// Renvoie un attribut boolean du fichier XML.
/** 
Dans le cas d'un attribut de scene.xml, c'est un attribut de l'un des fils du node courant.
Dans le cas d'un attribut de level.xml, c'est un attribut direct du node courant, et n'y a pas besoin de le checher dans un node fils.
Si childnode est vide, alors c'est un attribut du node courant (et pas d'un fils)
Les noms des attributs sont les mêmes pour les deux formats XML.
*/
/* ------------------------------------------------------------------------- */
bool ShipLevel::getXMLboolean(String attribute, bool DefaultValue, String childnode)
{
	bool Value ;
	DEBUG_LOG(">> getXMLboolean("+attribute+", "+childnode+")");

	if (mXMLscene)
	{
		if (childnode!="")	mXMLscene->gotoSubElement(childnode,0);
		Value = mXMLscene->getBooleanAttribute(attribute, DefaultValue);
		if (childnode!="")	mXMLscene->gotoParentElement();
	}
	else
	{
		Value = mXMLlevel->getBooleanAttribute(attribute, DefaultValue);
	}
	DEBUG_LOG("<< getXMLboolean("+attribute+", "+childnode+") = "+StringConverter::toString(Value));
	return Value ;
}



/* ------------------------------------------------------------------------- */
/// Renvoie le userData demandé.
/** 
	Dans les dotScene, les nodes "userdata" sont composés de sous-nodes "property".
	Ces "property" ont trois attributs: TYPE, NAME et DATA. (TYPE est ignoré).
	@param PropertyName La valeur de l'attribut NAME à lire.
	@return La valeur de l'attribut DATA correspondant, (au format String).
*/
/* ------------------------------------------------------------------------- */
String ShipLevel::getXMLuserdata(String PropertyName)
{
	String Value ;
	DEBUG_LOG(">> getXMLuserdata("+PropertyName+")");

	if (mXMLscene)
	{
		StringUtil::toUpperCase(PropertyName);
		Value = mXMLscene->getUserdata(PropertyName);
	}
	else
	{
		Value = mXMLlevel->getStringAttribute(PropertyName);
	}
	DEBUG_LOG("<< getXMLuserdata("+PropertyName+") = "+Value);
	return Value ;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction oriente un Node.
/** 
	Simple rotation pour le format LEVEL, un quaternion pour le format DotScene.
*/
/* ------------------------------------------------------------------------- */
void ShipLevel::rotateNode(SceneNode* node, Real Orientation, Quaternion q)
{
		if (mSceneLevel) 
			node->setOrientation(q); 
		else 
			node->yaw(Degree(Orientation));
}


/* ------------------------------------------------------------------------- */
/// Extrait le quaternion "orientation" du node courant du fichier DotScene.
/* ------------------------------------------------------------------------- */
Quaternion ShipLevel::getXMLOrientation()
{
	Quaternion Value = Quaternion::IDENTITY;

	if (mXMLscene)
	{
		if (mXMLscene->gotoSubElement("rotation",0))
		{
			Real qx = mXMLscene->getRealAttribute("qx");	
			Real qy = mXMLscene->getRealAttribute("qy");
			Real qz = mXMLscene->getRealAttribute("qz");	// Blender: l'axe vertical est Z
			Real qw = mXMLscene->getRealAttribute("qw");	
			mXMLscene->gotoParentElement();
			Value = Quaternion(qw,qx,qy,qz);
		}
	}
	return Value;
}


/* ------------------------------------------------------------------------- */
/// Extrait un quaternion du fichier DotScene.
/* ------------------------------------------------------------------------- */
Room* ShipLevel::getElevatorRoom()
{
	return mElevatorRoom;
}
