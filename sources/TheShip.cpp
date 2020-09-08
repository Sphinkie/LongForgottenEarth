/* ------------------------------------------------------------------------- */
// File       : TheShip.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "TheShip.h"
#include "Player.h"
#include "Clock.h"
#include "Macros.h"
#include "XMLparser_T.h"
#include "GalaxyBox.h"

// Pour les tests
#include "Robot.h"
#include "OgreOpcode.h"

/* ------------------------------------------------------------------------- */
///	On cree un Node pour le Ship, et un plan correspondant au plancher
/**	On cree les differents niveaux qui composent le ship */
/* ------------------------------------------------------------------------- */
TheShip::TheShip (SceneManager* SceneMgr) : SpaceShip(SceneMgr)
{
	// -----------------------------------------------------
	// On cree un ensemble de sprites, pour les lampes du vaisseau
	// -----------------------------------------------------
	BillboardSet* Lights_billboard_set;
	Lights_billboard_set= mSceneMgr->createBillboardSet("Sphinx/lightset");
	Lights_billboard_set->setMaterialName("light");
	Lights_billboard_set->setCullIndividually(true);
	Lights_billboard_set->setQueryFlags(VOIDOBJECT); // Ils ne sont pas detectés par les rayons.
	mShipNode->attachObject(Lights_billboard_set);

	/************************************************************************/
	/* Création d'un Node pour les particules de l'espace                  */
	/************************************************************************/
	mParticleNode = mShipNode->createChildSceneNode();
	// On place la surces des particules bien devant, et à la hauteur de la cabine de pilotage
	mParticleNode->setPosition(0,3*LEVEL_HIGH+100,1200);
	// (le script est dans spacedust.material et spacedust.particle)
	mSpaceDust = ParticleSystemManager::getSingleton().createSystem("SpaceDust", "Effect/spacedust");
	mParticleNode->attachObject(mSpaceDust);

	// -----------------------------------------------------
	// On cree les differents niveaux qui composent le ship.
	// -----------------------------------------------------
	// Créer en premier les niveaux contenant des Room qui sont référencées par la suite
	mLevels[4]= new ShipLevel("level4", 4*LEVEL_HIGH, mSceneMgr, mShipNode);
	mLevels[2]= new ShipLevel("level2", 2*LEVEL_HIGH, mSceneMgr, mShipNode);
	mLevels[3]= new ShipLevel("level3", 3*LEVEL_HIGH, mSceneMgr, mShipNode);
	mLevels[5]= new ShipLevel("level5", 5*LEVEL_HIGH, mSceneMgr, mShipNode);
	DEBUG_LOG("ShipLevel");
	// -----------------------------------------------------
	// On cree le plancher avec une entity plane
	// -----------------------------------------------------
	this->createFloor(4);
	this->createFloor(2);
	this->createFloor(3);
	this->createFloor(5);

	// -----------------------------------------------------
	// On cree les différents sous-éléments du vaisseau (et leur production horaire)
	// -----------------------------------------------------
	/// TODO: A ajuster. Produit 5 moles par heure (=120 par jour)
	mHydProdSystem1    = new ShipElement("H2 Collector I",  0, 1000,"mol"  , 5  );
	/// 1 mole = 22 litres de gaz
	mHydProdSystem2    = new ShipElement("H2 Collector II", 0, 1000,"mol"  , 5  );
	/// Produit 24 MW par jour
	mEnergyProdSystem  = new ShipElement("Energy Module",   0, 900, "MWatt", 1  );
	/// Air Prod  = 10 m3/jour/pers    (reserve=21 jours à 8)
	mAirProdSystem     = new ShipElement("Air Prod Unit",   0, 1680,"m3"   , 80.0/24 );
	/// Water Prod= 5 litre/jour/pers  (reserve=14 jours à 8)
	mWaterProdSystem   = new ShipElement("Water Prod Unit", 0, 560, "l"    , 40.0/24 );
	/// Food Prod = 3 repas/jour/pers  (reserve= 4 mois à 8)
	mFoodProdSystem    = new ShipElement("Food Prod Unit",  0, 3200,"doses", 24/24   );

    /// Hibernator  = ne produit rien (pas de reserve)
	mHybernationSystem = new ShipElement("Hibernation Central unit",0,10,"", 0    );
    /// Antigrav  = ne produit rien (pas de reserve)
	mAntigravitySystem = new ShipElement("AntiGravity Central unit",0,10,"", 0    );
	
	/// Engine 1 et 2 = produisent de la poussée
	mEngine1          = new ShipEngine ("Engine I",     0,2900, "kN",2900);
	/// On atteint le max en 1 unité de temps
	mEngine2          = new ShipEngine ("Engine II",    0,2900, "kN",2900);
	/// Système de Turbines = produit de l'accélération
//	mReactor          = new ShipElement ("Main Reactor", 0,5800, "kN",2000);
	mPassenger1       = NULL;
	mPassenger2       = NULL;

	mSpot1            = new ShipLight(mName+"/spot1", Lights_billboard_set, mSceneMgr, mShipNode);
	mSpot2            = new ShipLight(mName+"/spot2", Lights_billboard_set, mSceneMgr, mShipNode);
	mSpot1->setDirection(Vector3(-40,0,100));
	mSpot2->setDirection(Vector3(+40,0,100));
	mSpot1->setPosition(Vector3(-120,700,160));
	mSpot2->setPosition(Vector3(+120,700,160));

	/*
	mTractorSystem    = new ShipElement();
	mWeaponSystem     = new ShipElement();
	mShieldSystem     = new ShipElement();
	mLightsSystem     = new ShipElement();
	*/

	// -----------------------------------------------------
	// On cree les dépendances des differents sous-éléments du vaisseau (consommation par heure)
	// -----------------------------------------------------
	mEnergyProdSystem->setResourceElement(mHydProdSystem1   ,0.5);   // 24 moles d'Hyd pour 24 MWatt d'Energie
	mEnergyProdSystem->setResourceElement(mHydProdSystem2   ,0.5);   // prélevés sur les deux collecteurs (par jour)
	mAirProdSystem->setResourceElement(mEnergyProdSystem    ,0.8/24);// 1 Mwatt pour l'air
	mWaterProdSystem->setResourceElement(mEnergyProdSystem  ,1.1/24);// 1 Mwatt pour l'eau
	mFoodProdSystem->setResourceElement(mEnergyProdSystem   ,1.4/24);// 1 MWatt pour la nourriture
	mHybernationSystem->setResourceElement(mEnergyProdSystem,2.3/24);// 2 MWatt pour le système d'hibernation
	mAntigravitySystem->setResourceElement(mEnergyProdSystem,3.2/24);// 3 MWatt pour le système d'hibernation
	mEngine1->setResourceElement(mHydProdSystem1, 4);                // 4 moles d'Hyd pour le moteur 1 /// TODO: Ajuster
	mEngine2->setResourceElement(mHydProdSystem2, 4);                // 4 moles d'Hyd pour le moteur 2 /// TODO: Ajuster
//	mReactor->setResourceElement(mEngine1, 4);                       /// TODO: Ajuster
//	mReactor->setResourceElement(mEngine2, 4);                       /// TODO: Ajuster

	// -----------------------------------------------------
	// Journal des alertes du Vaisseau & Journal de bord
	// -----------------------------------------------------
	mAlertBook  = new TextList("AlertBook");
	mLogBook    = new TextList("LogBook");
	mCrewBook   = new TextList("CrewBook");
	mMissionBook= new TextList("MissionBook");

	// -----------------------------------------------------
	// Initialisation des membres
	// -----------------------------------------------------
	mActiveRoom        = NULL;
	mDelayDoorClosed   = 0;
	mPiloting          = false;
	mJoyDisabled       = true;
	mJoyStick          = NULL;

/*	minX = minY = minZ = -3000;
	maxX = maxY = maxZ = +3000;
	rX   = rY   = rZ   =  6000;
	sX   = sY   = sZ   =  0;
*/

	DEBUG_LOG("TheShip members initialized");
}


/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
TheShip::~TheShip ()
{
	mRoot->removeFrameListener(this);

	OIS::InputManager* InputMgr = OIS::InputManager::getSingletonPtr();
	if (InputMgr)
	{
		if (mJoyStick) InputMgr->destroyInputObject(mJoyStick);
		InputMgr->destroyInputSystem();
		InputMgr= NULL;
	}
}


template<> TheShip* Singleton<TheShip>::ms_Singleton = 0;
/* ------------------------------------------------------------------------- */
/// Override the Singleton Class method
/* ------------------------------------------------------------------------- */
TheShip* TheShip::getSingletonPtr(void)
{
	return ms_Singleton;
}


/* ------------------------------------------------------------------------- */
///   Renvoie un pointer sur un Level Object
/* ------------------------------------------------------------------------- */
ShipLevel* TheShip::getShipLevel (int Number)
{
	return mLevels[Number];
}

/* ------------------------------------------------------------------------- */
/// Renvoie un ID permettant d'identifier l'objet UserDefined de type "The Ship"
/* ------------------------------------------------------------------------- */
long TheShip::getTypeID (void) const
{
	return THESHIP_ID;
}

/* ------------------------------------------------------------------------- */
/// Fait apparaitre le Vaisseau
/* ------------------------------------------------------------------------- */
void TheShip::addToScene (Vector3 Position)
{

	//mLevels[3] ->setAllLights(true);
/*
	mLevels[3] ->addToScene();	
	mLevels[4] ->addToScene();	
	mLevels[5] ->addToScene();	
*/
	// On construit le réseau de waypoints du vaisseau
	mShipNetwork = new WaypointNetwork(mSceneMgr, mShipNode, "wp_Ship_");	
	
}


/* -------------------------------------------------------------------------- */
/// On determine la distance verticale entre la camera (Origin) et le plan "Floor".
/* -------------------------------------------------------------------------- */
Real TheShip::detectGround(Vector3 Origin) 
{
	Real   FloorDistance = 0;

	// Cree un rayon partant de la camera vers le bas.
	static Ray VerticalRay;
	VerticalRay.setOrigin(Origin);
	VerticalRay.setDirection(Vector3::NEGATIVE_UNIT_Y);

	// Lancement du rayon
	RaySceneQuery*  VerticalRayQuery = mSceneMgr->createRayQuery(VerticalRay, WALKABLE);
	VerticalRayQuery->setSortByDistance(true);	// On a les elements les plus pres en premier
	VerticalRayQuery->execute();

 	// Recuperation des objets traversés par le rayon
  	RaySceneQueryResult QueryResult = VerticalRayQuery->getLastResults();

	// Parcours les objets traversés par le rayon
	RaySceneQueryResult::iterator RayIterator;
	RaySceneQueryResult::iterator LastIterator = QueryResult.end();

	for (RayIterator=QueryResult.begin();  RayIterator!=LastIterator; RayIterator++ )
	{
		MovableObject*     FoundMovableObject = RayIterator->movable;
//		UserDefinedObject* FoundUserObject    = FoundMovableObject->getUserObject();

//		DEBUG_LOG("ray found : "+FoundMovableObject->getName()+" at "+ITOA(RayIterator->distance));
		// On trouve la distance avec le sol
		if (FoundMovableObject)
		{
			FloorDistance = RayIterator->distance;

			// On detruit le rayon
			VerticalRayQuery->clearResults();
			mSceneMgr->destroyQuery(VerticalRayQuery);
			return FloorDistance;
		}
/*
		// On garde une mémorisation de la piece où l'on se trouve
		// (utile uniquement si l'origine est le joueur =la caméra)
		if (DetectRoom && (FoundUserObject != NULL))
	 	{
			if (FoundUserObject->getTypeID() == ROOM_ID)
					mActiveRoom = (Room*)FoundUserObject;
		}
*/
     }

	// On detruit le rayon
	VerticalRayQuery->clearResults();
	mSceneMgr->destroyQuery(VerticalRayQuery);
	return 0;
};



/* -------------------------------------------------------------------------- */
/// Cette fonctin renvoie l'altitude du plan "Floor" le plus proche. 
/** @param Origin  L'origine correspond à des coordonnées par rapport au vaisseau (mShipNode).
*/
/* -------------------------------------------------------------------------- */
Real TheShip::detectFloorAltitude(Vector3 Origin) 
{
	Real   FloorAltitude = 0;

	TRACE(">> detectFloorAltitude() for Origin Point in " + StringConverter::toString(Origin));
	// On cree un rayon partant de la camera vers le bas.
	static Ray VerticalRay;
	/// TODO: attention à la précision des Real
	VerticalRay.setOrigin(mShipNode->getPosition()+Origin);
	VerticalRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
	DEBUG_LOG("   Absolute Origin Point is " + StringConverter::toString(mShipNode->getPosition()+Origin));

	// Lancement du rayon
	RaySceneQuery*  VerticalRayQuery = mSceneMgr->createRayQuery(VerticalRay, WALKABLE);
	VerticalRayQuery->setSortByDistance(true);	// On a les elements les plus proches en premier
	VerticalRayQuery->execute();

 	// Recuperation des objets traversés par le rayon
  	RaySceneQueryResult QueryResult = VerticalRayQuery->getLastResults();

	// Parcourt les objets traversés par le rayon
	RaySceneQueryResult::iterator RayIterator;
	RaySceneQueryResult::iterator LastIterator = QueryResult.end();
	DEBUG_LOG("   Analysing Ray" );

	for (RayIterator=QueryResult.begin();  RayIterator!=LastIterator; RayIterator++ )
	{
		MovableObject* FoundMovableObject = RayIterator->movable;
		DEBUG_LOG("   RayIterator" );

		if (FoundMovableObject)
		{
			DEBUG_LOG("   Ray crossed: "+FoundMovableObject->getName()+" at "+ ITOA(RayIterator->distance));
			// On fait une vérification, car il peut y avoir d'autres entités détectées.
			if (StringUtil::startsWith(FoundMovableObject->getName(),"floor"))
			{
				FloorAltitude = FoundMovableObject->getParentNode()->getPosition().y;
				DEBUG_LOG ("   Ray found: "+FoundMovableObject->getName()+" at "+ ITOA(RayIterator->distance) + " with Y= " + ITOA(FloorAltitude));
				// Quand on a trouvé le sol, on quitte tout de suite.
				// On detruit le rayon
				VerticalRayQuery->clearResults();
				mSceneMgr->destroyQuery(VerticalRayQuery);
				return FloorAltitude;
			}
		}
     }

	// On detruit le rayon
	VerticalRayQuery->clearResults();
	mSceneMgr->destroyQuery(VerticalRayQuery);
	// Si on n'a pas trouvé de floor, on renvoie une valeur par defaut.
	return 500;
};



/* -------------------------------------------------------------------------- */
/// On détermine dans quelle Room se trouve le point fourni en paramètre 
/** 
La fonction ne renvoie rien, mais met à jour mActiveRoom.
@param Origin Coordonnées du point, relatives au Ship.
*/
/* -------------------------------------------------------------------------- */
void TheShip::detectRoom(SceneNode* Origin)
{
	TRACE(">> detectRoom()");
	// Cree un rayon partant de la camera vers le bas du vaisseau.
	static Ray VerticalRay;
	VerticalRay.setOrigin(Origin->getWorldPosition());
	VerticalRay.setDirection(-mShipNode->getWorldOrientation().yAxis());
//	VerticalRay.setOrigin(mShipNode->getPosition()+Origin);
//	VerticalRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
	DEBUG_LOG ("   ray starts at: "+StringConverter::toString(VerticalRay.getOrigin()));

	// Lancement du rayon
	RaySceneQuery*  VerticalRayQuery = mSceneMgr->createRayQuery(VerticalRay, ROOMS);
	VerticalRayQuery->setSortByDistance(true);	// On a les elements les plus près en premier
	VerticalRayQuery->execute();

 	// Recuperation des objets traversés par le rayon
  	RaySceneQueryResult QueryResult = VerticalRayQuery->getLastResults();

	// On parcourt la liste des objets traversés par le rayon
	RaySceneQueryResult::iterator RayIterator;
	RaySceneQueryResult::iterator LastIterator = QueryResult.end();

	for (RayIterator=QueryResult.begin();  RayIterator!=LastIterator; RayIterator++ )
	{
		MovableObject*     FoundMovableObject = RayIterator->movable;
		UserDefinedObject* FoundUserObject    = FoundMovableObject->getUserObject();

		DEBUG_LOG("   ray found: "+FoundMovableObject->getName()+" at "+ITOA(RayIterator->distance));

		// On garde une mémorisation de la piece où l'on se trouve
		// (utile uniquement si l'origine est le joueur =la caméra)
		if (FoundUserObject != NULL)
	 	{
//			if (FoundUserObject->getTypeID() == ROOM_ID)
			mActiveRoom = (Room*)FoundUserObject;
			TRACE(">> detectRoom() found: "+mActiveRoom->getName());
			// On detruit le rayon
			VerticalRayQuery->clearResults();
			mSceneMgr->destroyQuery(VerticalRayQuery);
			return;
		}
     }

	// On detruit le rayon
	VerticalRayQuery->clearResults();
	mSceneMgr->destroyQuery(VerticalRayQuery);
}
 
/* ------------------------------------------------------------------------- */
/// Gestion du vaisseau 
/* ------------------------------------------------------------------------- */
bool TheShip::frameStarted (const FrameEvent& evt)
{
	// TODO : peut être fait une fois par seconde:
	// On liste toutes les conditions qui interdisent le pilotage par joystick
	mPiloting= !(  mEngine1->getMode()==ShipEngine::STOPPED_ENGINE 
	            || mEngine2->getMode()==ShipEngine::STOPPED_ENGINE 
				|| mJoyDisabled
				|| !mJoyStick);

	if (mPiloting)
	{
 		mJoyStick->capture();
 		this->readJoyStick(evt.timeSinceLastFrame);
	}

	/************************************************************************/
	/* Gestion de  la fermeture des portes                                  */
	/************************************************************************/
	if (mDelayDoorClosed > 0) 	
	{
		// On attend que le delay soit arrivé à 0 avant de refermer la porte.
		mDelayDoorClosed -= evt.timeSinceLastFrame;
	}
	else if (mDelayDoorClosed > -5) 	
	{
		// Une fois que le delai est passé en dessous de 0, 
		// à chaque fermeture de porte, on vérifie où se trouve le joueur.
//		this->detectRoom(Player::getSingletonPtr()->getPosition());	
		this->detectRoom(Player::getSingletonPtr()->getNode());	

		// Le délai est écoulé.
		// La porte est supposée etre fermée.
		// TODO: On vérifie que la(les) porte(s) est bien fermée. 

		// On cache les pieces inutiles. 
		mLevels[2]->showOneRoom(mActiveRoom);
		mLevels[3]->showOneRoom(mActiveRoom);
		mLevels[4]->showOneRoom(mActiveRoom);
		mLevels[5]->showOneRoom(mActiveRoom);
		// On désactive la temporisation
		mDelayDoorClosed = -255;
		// On desactive le jostick quand on sort de la salle de pilotage
		// (ou de n'importe quelle salle)
		// TODO : à completer
		mJoyDisabled = false; 
		// S'il n'y a pas de fenetre, on peut effacer le décor
		if (mActiveRoom && !mActiveRoom->hasExternView()) 
			GalaxyBox::getSingletonPtr()->hide();
	};

	return true;
}


/* ------------------------------------------------------------------------- */
/// On fait descendre l'ascenseur (le floor)
/* ------------------------------------------------------------------------- */
void TheShip::setFloorLevel(int Level)
{
//	mFloorNode->setPosition(0,Level*200-5,0);
}


/* ------------------------------------------------------------------------- */
/// Renvoie la position en Y du Floor
/* ------------------------------------------------------------------------- */
Real TheShip::getFloorYPosition()
{
//	return mFloorNode->getPosition().y;
	return 0;
}


/* ------------------------------------------------------------------------- */
/// On envoie l'ordre au Level d'ouvrir une porte.
/** Cette function est plutot utilisée par le player. */
/* ------------------------------------------------------------------------- */
bool TheShip::openDoor(Door* Porte)
{
	bool DoorOpened = false;
	DEBUG_LOG(">> TheShip::openDoor(Door*)");
	if (!Porte) return false;
	
	// S'il s'agit d'une porte d'ascenseur.
	// On fait apparaitre toutes les sections de l'ascensceur.
	if (Porte->isElevatorDoor()) 
	{
		mLevels[2]->showElevator();
		mLevels[3]->showElevator();
		mLevels[4]->showElevator();
		mLevels[5]->showElevator();
	}

	// On demande à chaque niveau d'ouvrir la porte
	// (le bon niveau se reconnaitra)
	DoorOpened  = mLevels[2]->openDoor(Porte);
	DoorOpened |= mLevels[3]->openDoor(Porte);
	DoorOpened |= mLevels[4]->openDoor(Porte);
	DoorOpened |= mLevels[5]->openDoor(Porte);
	
	// On lance un décompte de N secondes, puis on cache les pieces inutiles.
	if (DoorOpened) mDelayDoorClosed = 16;
	return DoorOpened;
}


/* ------------------------------------------------------------------------- */
/// On envoie l'ordre au Level d'ouvrir une porte.
/** Cette fonction n'étant pas utilisée par le player, mais plus par les NPC,
on ne fait pas apparaitre toutes les sections de l'ascenseur. */
/* ------------------------------------------------------------------------- */
bool TheShip::openDoor(String DoorName)
{
	bool DoorOpened = false;
	DEBUG_LOG(">> TheShip::openDoor("+DoorName+")");

	// On demande à chaque niveau d'ouvrir la porte
	// (le bon niveau se reconnaitra)
	DoorOpened  = mLevels[2]->openDoor(DoorName);
	DoorOpened |= mLevels[3]->openDoor(DoorName);
	DoorOpened |= mLevels[4]->openDoor(DoorName);
	DoorOpened |= mLevels[5]->openDoor(DoorName);

	/// TODO: le cas d'une porte d'ascenseur n'est pas géré...
	
	// On lance un décompte de N secondes, puis on cache les pieces inutiles.
	if (DoorOpened) mDelayDoorClosed = 16;

	return DoorOpened;
}


/* ------------------------------------------------------------------------- */
/// Détermination du meilleur itinéraire
/* ------------------------------------------------------------------------- */
SceneNode* TheShip::findNextWaypointTo(SceneNode* FromNode,SceneNode* ToNode)
{
	return mShipNetwork->findNextWaypointTo(FromNode,ToNode);
}


/* ------------------------------------------------------------------------- */
/// Renvoie le Waypoint le plus proche de la position courante
/* ------------------------------------------------------------------------- */
SceneNode* TheShip::getClosestWaypoint(Vector3 CurrentPos)
{
	return mShipNetwork->getClosestWaypointNode(CurrentPos);
}


/* ------------------------------------------------------------------------- */
/// Fixe le niveau d'alerte général sur le vaisseau. (0=normal)
/* ------------------------------------------------------------------------- */
void TheShip::setAlertLevel(short level)
{
	mAlertLevel = level;
	mLevels[2]->setAlertLevel(level);
	mLevels[3]->setAlertLevel(level);
	mLevels[4]->setAlertLevel(level);
	mLevels[5]->setAlertLevel(level);
}


/* ------------------------------------------------------------------------- */
/// Renvoie le niveau d'alert général sur le vaisseau (0 = normal)
/* ------------------------------------------------------------------------- */
short TheShip::getAlertLevel()
{
	return mAlertLevel;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le status du vaisseau sous forme de chaine XML
/* ------------------------------------------------------------------------- */
String TheShip::getStatus()
{
	String Xstatus;

	/// TODO: gerer le status des autres levels
	Xstatus = "\n<status> \n";
	Xstatus += mLevels[3]->getStatus();
	Xstatus += "<item id='150' text='Ready' submenu='menu'>Nothing...</item>\n";
	Xstatus += "</status>";

	return Xstatus;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la pièce où se trouve la caméra
/* ------------------------------------------------------------------------- */
Room* TheShip::getActiveRoom()
{
	return mActiveRoom;
}


/* ------------------------------------------------------------------------- */
/// Affiche la pièce où se trouve la caméra .
/** Ceci ne fonctionne pas au tout debut du jeu, car les mesh des rooms
ne sont pas encore tous forcement rendus.
*/
/* ------------------------------------------------------------------------- */
void TheShip::showActiveRoom(SceneNode* Origin)
{
	DEBUG_LOG(">> showActiveRoom("+StringConverter::toString(Origin->getWorldPosition())+")");
	// On recupère le niveau où se trouve le joueur
	int CurrentLevel = Player::getSingletonPtr()->getCurrentLevel();
	DEBUG_LOG("   CurrentLevel= "+StringConverter::toString(CurrentLevel));

	// On affiche toutes les Rooms du niveau 
	/// TODO: Le problème c'est que ca active aussi tous les sons...
	if (!mLevels[CurrentLevel]) 
		TRACE ("*** Warning: mLevels["+StringConverter::toString(CurrentLevel)+"] is NULL");
	mLevels[CurrentLevel]->showAllRooms();

	// On lance un rayon pour déterminer la pièce courante
	this->detectRoom(Origin);
	// On efface les pièces inutiles
	mLevels[CurrentLevel]->showOneRoom(mActiveRoom);
}


/* ------------------------------------------------------------------------- */
/// Affiche la pièce où se trouve la caméra
/* ------------------------------------------------------------------------- */
void TheShip::showActiveRoom(String RoomName)
{
	TRACE(">> showActiveRoom("+RoomName+")");
	// On recupère le niveau où se trouve le joueur
	int CurrentLevel = Player::getSingletonPtr()->getCurrentLevel();
	// On recupère la piece où se trouve le joueur
	String InitialRoomName = Player::getSingletonPtr()->getInitialRoom();

	// On détermine la pièce courante
	mActiveRoom = this->getRoom(InitialRoomName);
	// On efface les pièces inutiles
	mLevels[CurrentLevel]->showOneRoom(mActiveRoom);
}


/* ------------------------------------------------------------------------- */
/// Cette fonctin effectue l'action associée au Waypoint le plus proche de la position fournie
/** @return le resultat de l'action demandée.
TRUE = l'action s'est bien passée
FALSE= l'action s'est mal passée
*/
/* ------------------------------------------------------------------------- */
bool TheShip::doClosestWaypointAction(Vector3 CurrentPos)
{
	bool Result = true;
	String Action = mShipNetwork->getClosestWaypointAction(CurrentPos);
	StringVector FuncV = StringUtil::split(Action,"(,)");
	
	// *****************************************
	if (FuncV.at(0) == "openDoor")
	{
		Result = this->openDoor(FuncV.at(1));
	}
	// *****************************************

	if (!Result) TRACE("<< doClosestWaypointAction: The requested action could not been executed.");
	return Result;
}


/* ------------------------------------------------------------------------- */
/// Cree un plan (pour servir de plancher au joueurs et NPC)
/* ------------------------------------------------------------------------- */
void TheShip::createFloor(int Level)
{
	Plane      FloorPlane;
	Entity*    FloorEntity;
	SceneNode* FloorNode;
	String     Name = "floor"+ITOA(Level);

	//	mFloorNode = mShipNode->createChildSceneNode("FloorNode");
	
	// Creation du mesh
	FloorPlane.d = 0;                   // Nb of world units from the origin
	FloorPlane.normal= Vector3::UNIT_Y; // facing up
	MeshManager::getSingleton().createPlane(Name,"FloorGroup",FloorPlane,4000,4000,1,1,true,1,1,1,Vector3::UNIT_Z);
	// largeur, hauteur, 

	// Création de l'Entity: On met le même nom pour EntityName et MeshName (c'est plus simple).
	FloorEntity = mSceneMgr->createEntity(Name, Name);	
	FloorEntity->setVisible(false);
	FloorEntity->setQueryFlags(WALKABLE);

	// Creation du Node
	FloorNode = mShipNode->createChildSceneNode(Name);
	FloorNode->setPosition(0,Level*LEVEL_HIGH,-2000);
	FloorNode->attachObject(FloorEntity);

}


/* ------------------------------------------------------------------------- */
/// Retrouve une Room par son nom. Renvoie NULL si la piece n'existe pas.
/* ------------------------------------------------------------------------- */
Room* TheShip::getRoom(String RoomName)
{
	Room* room = NULL;
	if (!room) room = this->getShipLevel(2)->getRoom(RoomName);
	if (!room) room = this->getShipLevel(3)->getRoom(RoomName);
	if (!room) room = this->getShipLevel(4)->getRoom(RoomName);
	if (!room) room = this->getShipLevel(5)->getRoom(RoomName);
	return room;
}


/* ------------------------------------------------------------------------- */
/// Fonction de debug: affiche le réseau de Waypoints
/* ------------------------------------------------------------------------- */
void TheShip::_showWPnetwork()
{
	mShipNetwork->showLines();
}


/* ------------------------------------------------------------------------- */
/// Charge les paramètres initiaux du vaisseau.
/* ------------------------------------------------------------------------- */
void TheShip::loadObject()
{
	/// TODO: Charger aussi SpaceShip:loadObject
	TRACE(">> TheShip::loadObject()");
	String     filename = "../savedgames/TheShip.xml";
	TRACE("   Loading "+filename);

	XMLParser_T* SavedGame = new XMLParser_T(filename);

	if (!SavedGame) return;
	
	// On initialise la valeur des différents éléments du Vaisseau

	SavedGame->gotoElementPath("ship/reserves");
	mHydProdSystem1->setValue(SavedGame->getRealAttribute("PlasmaLevel1"));
	mHydProdSystem2->setValue(SavedGame->getRealAttribute("PlasmaLevel2"));
	mEnergyProdSystem->setValue(SavedGame->getIntAttribute("Energy"));
	mAirProdSystem->setValue(SavedGame->getIntAttribute("AirLevel"));
	mFoodProdSystem->setValue(SavedGame->getIntAttribute("FoodLevel"));
	mWaterProdSystem->setValue(SavedGame->getIntAttribute("WaterLevel"));
	
	SavedGame->gotoElementPath("ship/statusE1");
	mEngine1->setActivity(SavedGame->getRealAttribute("Activity"));
	mEngine1->setDamage(SavedGame->getRealAttribute("Damage"));
	mEngine1->setTemperature(SavedGame->getIntAttribute("Temperature"));
	mEngine1->setMode((ShipEngine::tMode)SavedGame->getIntAttribute("EngineMode"));

	SavedGame->gotoElementPath("ship/statusE2");
	mEngine2->setActivity(SavedGame->getRealAttribute("Activity"));
	mEngine2->setDamage(SavedGame->getRealAttribute("Damage"));
	mEngine2->setTemperature(SavedGame->getIntAttribute("Temperature"));
	mEngine2->setMode((ShipEngine::tMode)SavedGame->getIntAttribute("EngineMode"));

	SavedGame->gotoElementPath("ship/environnement");
	mAmbiantTemperature = SavedGame->getIntAttribute("AmbiantTemperature");
	mCurRadiation       = SavedGame->getRealAttribute("CurrentRadiation");
	mMaxRadiation       = SavedGame->getRealAttribute("MaxRadiation");
	mExternalTemp       = SavedGame->getIntAttribute("ExternalTemp");

	SavedGame->gotoElementPath("ship/alert");
	this->setAlertLevel(SavedGame->getIntAttribute("level"));

	// La position globale du ship dans la Galaxie en AL.
	SavedGame->gotoElementPath("ship/coordinates");
	Real x = SavedGame->getRealAttribute("x");
	Real y = SavedGame->getRealAttribute("y");
	Real z = SavedGame->getRealAttribute("z");
	GalaxyBox::getSingletonPtr()->setShipCoordinates(x,y,z);

	// Si le ship est dans un systeme solaire,
	// sa position précise par rapport à l'étoile.
	SavedGame->gotoElementPath("ship/position");
	x = SavedGame->getRealAttribute("x");
	y = SavedGame->getRealAttribute("y");
	z = SavedGame->getRealAttribute("z");
	GalaxyBox::getSingletonPtr()->setShipLocalPosition(x,y,z);

	delete(SavedGame);
}


/* ------------------------------------------------------------------------- */
/// Charge la situation initiale
/* ------------------------------------------------------------------------- */
void TheShip::loadInitialSituation()
{
	TRACE(">> loadInitialSituation()");

	this->loadObject();

	mLevels[3]->setInitialRoom();		// charge l'état de départ des Rooms
 
	Player::getSingletonPtr()->loadgame();

	this->addPassenger();

	mRoot->addFrameListener(this);
	DEBUG_LOG("TheShip Listener launched");
	TRACE("<< loadInitialSituation()");
}


/* ------------------------------------------------------------------------- */
/// Renvoie la Temperature Ambiante dans le vaisseau (Kelvin)
/* ------------------------------------------------------------------------- */
int TheShip::getAmbiantTemperature()
{
	return mAmbiantTemperature;
}

/* ------------------------------------------------------------------------- */
/// Les lumières principales de toutes les pieces du vaisseau passent au rouge.
/* ------------------------------------------------------------------------- */
void TheShip::setRedLight(bool ON)
{
	this->getShipLevel(2)->setRedLight(ON);
	this->getShipLevel(3)->setRedLight(ON);
	this->getShipLevel(4)->setRedLight(ON);
	this->getShipLevel(5)->setRedLight(ON);
}

/* ------------------------------------------------------------------------- */
/// Met à jour les éléments qui consomment sur une base quotidienne
/* ------------------------------------------------------------------------- */
void TheShip::updateElements_Day()
{
	// Fabrication de l'énergie
	mHydProdSystem1->updateValue_Hour(24);
	mHydProdSystem2->updateValue_Hour(24);
	mEnergyProdSystem->updateValue_Hour(24);
	// Consommation de l'énergie, et fabrication des resources
	mAirProdSystem->updateValue_Hour(24);
	mFoodProdSystem->updateValue_Hour(24);
	mWaterProdSystem->updateValue_Hour(24);
	// Consommation des resources
	mHybernationSystem->updateValue_Hour(24);
	mAntigravitySystem->updateValue_Hour(24);
	if (mPassenger1) mPassenger1->updateValue_Hour(24);
}

/* ------------------------------------------------------------------------- */
/// Met à jour les éléments qui consomment sur une base continue
/* ------------------------------------------------------------------------- */
void TheShip::updateElements_Hour(Real ElapsedHours)
{
	//TRACE(">> TheShip::updateElements_Hour()");

	// Les moteurs
	mEngine1->updateValue_Hour(ElapsedHours);
	mEngine2->updateValue_Hour(ElapsedHours);
	mEngine1->updateTemperature(ElapsedHours);
	mEngine2->updateTemperature(ElapsedHours);

	// TRACE (" Coll    (mol)= "+ StringConverter::toString(mHydProdSystem1->getValue()));

	/************************************************************************/
	/* Gestion des mouvements du vaisseau                                   */
	/************************************************************************/

	mDirectionVector = mShipNode->getOrientation().zAxis();
	/// Acceleration = Force / Masse
	Real kNewton      = mEngine1->getValue() + mEngine2->getValue();
	Real Acceleration = kNewton / mMass;

	if (kNewton != 0)
	{
		/// VecteurAccélération = Accelération * VecteurDirection
		mAccelerationVector = Acceleration * mDirectionVector ;
		/// VecteurVitesse = VecteurVitesseInitiale + VecteurAccélération * DeltaTemps
		mSpeedVector += mAccelerationVector * ElapsedHours;
	}
	this->fly(ElapsedHours);
	
	/// TODO : les autres elements du vaisseau
}

/* ------------------------------------------------------------------------- */
/// Ajoute un ShipElement consommateur de resources dans le vaisseau.
/** 
Un passager ne produit rien, mais il consomme:
- 5 litres d'oxygène par minute = 300 litres par heure = 0.3 m3/heure
- 5 litres d'eau par jour.
- 3 repas par jour.
*/
/* ------------------------------------------------------------------------- */
void TheShip::addPassenger()
{
	mPassenger1 = new ShipElement("Passenger 1", 0, 100,"",0); 
	mPassenger1 ->setResourceElement(mAirProdSystem,   0.3); 
	mPassenger1 ->setResourceElement(mWaterProdSystem, 5.0/24);
	mPassenger1 ->setResourceElement(mFoodProdSystem,  3.0/24);
}


/* ------------------------------------------------------------------------- */
/// Renvoie le taux de radiation actuel à l'interieur du vaisseau (après filtrage coque)
/* ------------------------------------------------------------------------- */
Real TheShip::getCurRadiation()
{
	return mCurRadiation;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le taux de radiation MAX à l'interieur du vaisseau (depuis le début de la partie)
/* ------------------------------------------------------------------------- */
Real TheShip::getMaxRadiation()
{
	return mMaxRadiation;
}


/* ------------------------------------------------------------------------- */
/// Le vaisseau avance. 
/** On applique une translation du vecteur vitesse mSpeedVector (calculé précédement)
au vaisseau (ou plus exactement à la galaxie), 
et on active un flux de particules donnant une illusion de vitesse. 
TODO : appliquer une fraction du vecteur vitesse en fonction du framerate. (fait en amont?)
@sa updateElements_Hour
*/
/* ------------------------------------------------------------------------- */
void TheShip::fly(Real ElapsedHours)
{
	if (mSpeedVector==Vector3::ZERO) 
	{
		// Quand le vaisseau s'arrête,
		// On désactive l'émission des particules
		mSpaceDust->getEmitter(0)->setEnabled(false);
	}
	else
	{
		GalaxyBox::getSingletonPtr()->translate(-mSpeedVector);  // *ElapsedHours);
		// On active l'émission des particules
		mSpaceDust->getEmitter(0)->setEnabled(true);
	}
}

/* ------------------------------------------------------------------------- */
/// Positionne le Ship
/* ------------------------------------------------------------------------- */
void TheShip::_setPosition(Real X, Real Y, Real Z)
{
	mShipNode->setPosition(X,Y,Z);
}

/* ------------------------------------------------------------------------- */
/// Renvoie la position du Ship
/* ------------------------------------------------------------------------- */
Vector3 TheShip::_getPosition()
{
	return mShipNode->getPosition();
}

/* ------------------------------------------------------------------------- */
/// Renvoie la position initiale d la camera dans ce level (debug)
/* ------------------------------------------------------------------------- */
Vector3 TheShip::_getCamPosition(int Level)
{
	Vector3 P = mLevels[Level]->getCamPos();
	return P;
}


/* -------------------------------------------------------------------------- */
/// Initialize the OIS library (for Joystick input).
/** 
*/
/* -------------------------------------------------------------------------- */
void TheShip::initOISSystem()
{
	TRACE("Initializing OIS system ");
	std::stringstream windowHndStr;
	OIS::ParamList    params;
	size_t            windowHnd = 0;

	RenderWindow* win = Clock::getSingletonPtr()->getOgreWindow();
	win->getCustomAttribute("HWND", &windowHnd);
	windowHndStr << windowHnd;
	params.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	OIS::InputManager &im = *OIS::InputManager::createInputSystem(params);
	try
	{
		mJoyStick = static_cast<OIS::JoyStick*>(im.createInputObject(OIS::OISJoyStick, false));
	}
	catch (...)
	{
		mJoyStick=NULL;
		TRACE(" No Joystick Found !");
		return;
	}

/*
	mMouse    = static_cast<OIS::Mouse*>(im.createInputObject(OIS::OISMouse, false));
	unsigned int width, height, depth;
	int left, top;
	mWindow->getMetrics(width, height, depth, left, top);

	//Set Mouse Region.. if window resizes, we should alter this to reflect as well
	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width  = width;
	ms.height = height;
	mMouse->setEventCallback(this);
*/

	TRACE("Found joystick!");
	TRACE("Joystick with "+ITOA(mJoyStick->axes())+" axis and "+ITOA(mJoyStick->buttons())+" buttons");
//	mJoyStick->setEventCallback(this);
	TRACE("OIS system initialized");
}


/* -------------------------------------------------------------------------- */
/// Renvoie un pointeur sur le Joystick.
/* -------------------------------------------------------------------------- */
OIS::JoyStick* TheShip::getJoyStick()
{
	if(mJoyStick) return mJoyStick;

	if( OIS::InputManager::getSingletonPtr()->numJoysticks() > 0 )
		mJoyStick = (OIS::JoyStick*)(OIS::InputManager::getSingletonPtr()->createInputObject(OIS::OISJoyStick, true));

	return mJoyStick;
}

/* -------------------------------------------------------------------------- */
/// Callback OIS: Cette fonction est appelée quand un bouton du Joystick est pressé.
/** 
button 0 = tir
button 3 = change weapon
TODO : configuration des boutons 
*/
/* -------------------------------------------------------------------------- */
bool TheShip::buttonPressed (const OIS::JoyStickEvent &e, int buttonId)
{
	Clock::getSingletonPtr()->showMessageOverlay("Button "+ITOA(buttonId)+" pressed");
	return true;
};

/* -------------------------------------------------------------------------- */
/// Callback OIS: Cette fonction est appelée quand un bouton du Joystick est relaché.
/** Inactif, car on n'est pas en mode bufferisé) */
/* -------------------------------------------------------------------------- */
bool TheShip::buttonReleased(const OIS::JoyStickEvent &e, int buttonId)
{
	return true;
};


/* -------------------------------------------------------------------------- */
/// Callback OIS: Cette fonction est appelée quand le Joystick est bougé.
/** Inactif, car on n'est pas en mode bufferisé) */
/* -------------------------------------------------------------------------- */
bool TheShip::axisMoved(const OIS::JoyStickEvent &e, int AxisId)
{
/*	if (AxisId==0)
		Clock::getSingletonPtr()->showMessageOverlay("Joystick axis "+ITOA(AxisId)+" moved:"
			  + " absX=" +ITOA(e.state.mAxes[AxisId].abX)
			  + " absY=" +ITOA(e.state.mAxes[AxisId].abY));
	if (AxisId==1)
		Clock::getSingletonPtr()->showMessageOverlay("Joystick axis "+ITOA(AxisId)+" moved:"
			  + " absZ=" +ITOA(e.state.mAxes[AxisId].abZ) );
*/
	return true;
};

/* -------------------------------------------------------------------------- */
/// Callback OIS: Cette fonction est appelée quand le TopHat est bougé.
/** La variable e.state.mPOV prend une valeur différente pour chacune des 9 positions 
possibles du hat: (+ des valeurs parasites par moment)
- centre    = -1
- haut      =   000
- haut/droit=  4500
- droite    =  9000
- bas/droit = 13500
- bas       = 18000
- bas/gauche= 22500
- gauche    = 27000
- haut/gche = 31500
/* -------------------------------------------------------------------------- */
bool TheShip::povMoved(const OIS::JoyStickEvent &e, int PovId)
{
	Clock::getSingletonPtr()->showMessageOverlay("POV: moved " + ITOA(e.state.mPOV[PovId]));
	return true;
};


/* -------------------------------------------------------------------------- */
/// Gestion des mouvements du joystick.
/**
Axis 0 : Joystick principal (stick) (avant/arriere=Y - lateral=X)
Axis 1 : Rotationnel (Z only)
Les valeurs sont normalement comprises entre -32767 et 32768.
Pour éviter des variations, le joystick doit être étaloné avant de lancer le jeu.
Cette fonction fait aussi bouger le mShipNode (en rotation uniquement)
*/
/* -------------------------------------------------------------------------- */
void TheShip::readJoyStick(Real elapsedTime)
{
	Real x = mJoyStick->getJoyStickState().mAxes[0].abX;
	Real y = mJoyStick->getJoyStickState().mAxes[0].abY;
	Real z = mJoyStick->getJoyStickState().mAxes[1].abZ;

	// Etalonnage temps reel
/*	if (x<minX) { minX=x; sX=minX+maxX; rX=maxX-minX; }
	if (x>maxX) { maxX=x; sX=minX+maxX; rX=maxX-minX; }
	if (y<minY) { minY=y; sY=minY+maxY; rY=maxY-minY; }
	if (y>maxY) { maxY=y; sY=minY+maxY; rY=maxY-minY; }
	if (z<minZ) { minZ=z; sZ=minZ+maxZ; rZ=maxZ-minZ; }
	if (z>maxZ) { maxZ=z; sZ=minZ+maxZ; rZ=maxZ-minZ; }
	
	// Valeur calibrée entre 0 et 1
	Real calibratedX = (2*x - sX) / rX;
	Real calibratedY = (2*y - sY) / rY;
	Real calibratedZ = (2*z - sZ) / rZ;

	// Dead zone entre +/- 15%
	if (calibratedX<0.15 && calibratedX>-0.15) calibratedX=0;
	if (calibratedY<0.15 && calibratedY>-0.15) calibratedY=0;
	if (calibratedZ<0.15 && calibratedZ>-0.15) calibratedZ=0;

	mShipNode->roll (Degree(- calibratedX * elapsedTime * 10));
	mShipNode->pitch(Degree(- calibratedY * elapsedTime * 5));
	mShipNode->yaw  (Degree(  calibratedZ * elapsedTime * 10));
*/

	// Dead zone entre +/- 15%
	if (x>-7000 && x<7000) x=0;
	if (y>-6000 && y<6000) y=0;
	if (z>-6000 && z<6000) z=0;

	// On applique au passage les coefficients correctifs d'amplitude du joystick
/*	if (x!=0) mShipNode->roll (Degree(  x * elapsedTime * 0.0003));
	if (y!=0) mShipNode->pitch(Degree(- y * elapsedTime * 0.0002));
	if (z!=0) mShipNode->yaw  (Degree(- z * elapsedTime * 0.0003));
*/
	GalaxyBox::getSingletonPtr()->rotate(-x * elapsedTime * 0.0003,
	                                      y * elapsedTime * 0.0002,
	                                      z * elapsedTime * 0.0003);

/*	Clock::getSingletonPtr()->showMessageOverlay(
	   "Joystick: X=" +ITOA(x)
			  + " Y=" +ITOA(y)
			  + " Z=" +ITOA(z)); */

//	if (x!=0) OgreOpcode::CollisionManager::getSingletonPtr()->GetDefaultContext()->Visualize();
	
};


/* -------------------------------------------------------------------------- */
/// Debug: affiche la position de toutes le lumières.
/* -------------------------------------------------------------------------- */
void TheShip::_showAllLightPosition()
{
	mSceneMgr->getBillboardSet("Sphinx/lightset")->setVisible(true);
	
};



/* -------------------------------------------------------------------------- */
/// Repositionne les "enveloppes de collision" de tous les objets du vaisseau lorsque celui-ci a bougé.
/* -------------------------------------------------------------------------- */
/*
void TheShip::refitAllCollideObjects()
{
	// On recupère le contexte de collision du vaisseau
	CollisionContext* collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();
	// On liste les objets du contexte
	collideContext->visualize();
	// On ajuste chaque enveloppe de collision
}
*/