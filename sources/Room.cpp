/* ------------------------------------------------------------------------- */
// File       : Room.cpp
// Project    : Long Forgotten Earth
// auteur     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Room.h"

#include "Clock.h"		
#include "XMLparser_T.h"
#include "SoundManager.h"
#include "mainApplicationListener.h"		// DEBUG: ajustement des couleurs
#include "macros.h"
#include "GalaxyBox.h"

// Objets contensu dans les pieces (gestion du status)
#include "spares.h"
#include "Hibernation.h"
#include "fmod.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
Room::Room(String Name, String MeshFile, Vector3 Position, SceneManager* SceneMgr, SceneNode* ParentNode)
{
	mSceneMgr      = SceneMgr;
	mParentNode    = ParentNode;
	mRoomName      = Name;
	mMeshFile      = MeshFile;
	mRoomPanel     = "";
	mHasExternView = false;
	mBroken        = false;
	mDamage_ETR    = 0;
	mElevatorRoom  = false;
	mAlertLevel    = 0;
	mOnScene       = false; 
	for (int i=0; i<4; i++)	{mLight[i]= NULL;}
	
	mRoomNode   = ParentNode->createChildSceneNode(Name);
	mRoomNode->setPosition(Position);	 // X=droite - Y=Haut - Z=devant	
	mParentNode->removeChild(mRoomNode); // Au debut les room ne sont pas attachées au Ship.

	// Initialisation du son
	mRoomSoundFile   = "";
	mRoomSoundChannel= NULL_CHANNEL;
	mRoomAlertChannel= NULL_CHANNEL;

	// Initialisations
	mRoomEntity  = NULL;
	mLightEntity = NULL;
	mLightSprite = NULL;
//	mRedColour   = ColourValue(0.94,0.55,0.55); // très clair
//	mRedColour   = ColourValue(0.45,0.00,0.00);
	mRedColour   = ColourValue(0.45,0.10,0.10);
	mLightColour = ColourValue::White;

	this->loadObject();
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Room::~Room()
{
}

/* ------------------------------------------------------------------------- */
/// Cette fonction cree une Entité (mesh) pour la piece.
/* ------------------------------------------------------------------------- */
void Room::createRoom()
{
	mRoomEntity = mSceneMgr->createEntity(mRoomName, mMeshFile);
	// Les room ne projettent pas d'ombres (receive only)
	mRoomEntity->setCastShadows(false);		
	mRoomEntity->setQueryFlags(ROOMS);	
	mRoomEntity->setUserObject(this); 
	mRoomNode->attachObject(mRoomEntity);

	this->initCollisionShape();
	this->setAlertLevel(mAlertLevel);
	DEBUG_LOG(mRoomName+": Entity created");
}


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie un identifiant pour les userDefinedObject Room
/* ------------------------------------------------------------------------- */
long Room::getTypeID (void) const
{
	return ROOM_ID;
}
 
/* ------------------------------------------------------------------------- */
/// Ajoute la piece à la scene courante.
/* ------------------------------------------------------------------------- */
void Room::addToScene()
{
	// On verifie que la Room n'est pas deja dans la scene
	if (!mOnScene) 
	{
		// Si le mesh n'existe pas, on le crée.
		if (!mRoomEntity) this->createRoom();

		// Il faut allumer les lumières de la piece
		for (int i=0; i<4; i++)	
		if (mLight[i]) mLight[i]->setVisible(true);

		TRACE(">> addToScene ("+mRoomName+")");
		mParentNode->addChild(mRoomNode);
		//	if (!mRoomEntity->isAttached()) mRoomNode->attachObject(mRoomEntity);
		// Joue le bruit d'ambiance associé à la piece
		if (mRoomSoundFile!="") 
		{
			mRoomSoundChannel = SoundManager::getSingletonPtr()->playSoundAmbiance(mRoomSoundFile);
			SoundManager::getSingletonPtr()->updateSoundSource(mRoomSoundChannel,mRoomNode->getPosition());
		}
		// S'il y a une ouverture sur l'exterieur, on affiche le décor extérieur
		if (mHasExternView) 
		{
			GalaxyBox::getSingletonPtr()->show();
		};
	}
	mOnScene = true;  
}


/* ------------------------------------------------------------------------- */
/// Cette fonction oriente le mesh correspondant à la pièce.
/** 
@param Orientation Angle (en degrés) autour de l'axe vertical, dont doit etre orienté la pièce.
@param Qorient     Quaternion d'orientation de la pièce.
@param dotScene    Si la pièce a été décrite au format dotScene, il faut utiliser le quaternion.
*/
/* ------------------------------------------------------------------------- */
void Room::setOrientation(Degree angle, Quaternion Qorient, bool dotScene)
{
	if (dotScene) mRoomNode->setOrientation(Qorient);
	else mRoomNode->yaw(angle);	// format Level.xml
}


/* ------------------------------------------------------------------------- */
/// Cette fonction efface la Room, (pour gagner en framerate).
/** Note: Cette fonction n'efface pas le décor (galaxy box) car il peut rester 
affiché en raison d'autres pièces. C'est le ship qui se charge de gérer le bon 
moment où effacer le décor. */
/* ------------------------------------------------------------------------- */
void Room::removeFromScene()
{
	// Il faut éteindre les lumières de la piece
	for (int i=0; i<4; i++)	
		if (mLight[i]) mLight[i]->setVisible(false);

	if (mOnScene) 
	{
		// Arrete le bruit d'ambiance associé à la piece (fading)
		SoundManager::getSingletonPtr()->setFading(mRoomSoundChannel);
		mParentNode->removeChild(mRoomNode);
		//	if (mRoomEntity->isAttached()) mRoomNode->detachObject(mRoomEntity); (autre possibilité)
	}
	mOnScene = false;
}


/* ------------------------------------------------------------------------- */
/// Allume (ou éteint) une lumière dans la piece:
/**
@param On   Indique s'il faut allumer la lumiere. 
@param name Le nom de la lumière.
*/
/* ------------------------------------------------------------------------- */
void Room::setLight(bool On,String name)
{
	try
	{
		// On vérifie que cette lumière existe
		if (name==ROOMLIGHT) 
		{
			// S'il s'agit de la lumière principale
			if (mLight[0]) mLight[0]->setVisible(On);
		}
		else
		{
			// Sinon, on recupère la lumiere demandée
			Light* l = mSceneMgr->getLight(mRoomName+"/"+name);
			l->setVisible(On);
		}
	}
	catch (Ogre::Exception& e) 
	{
		// Attention on a demandé une lumière qui n'existe pas.
		DEBUG_LOG("WARNING: The light "+mRoomName+"/"+name+" does not exist. "+ e.getFullDescription());
	}
}



/* ------------------------------------------------------------------------- */
/// Gestion de la lumiere rouge de la piece.
/** 
@param On Indique s'il faut allumer la lumiere rouge. 
- TRUE   : on allume la veilleuse rouge 
- FALSE  : on allume la lumiere principale 
*/
/* ------------------------------------------------------------------------- */
void Room::setNightLight(bool On)
{
	TRACE(">> "+mRoomName+"::setNightLight("+StringConverter::toString(On)+")");
	if (!mRoomEntity) return;
	

	if (On )
	{
		// Lumière Principale rouge
		Real Range = mLightRange*0.60;	// ou =200 pour une lumière plus faible; 		// Portée de la lumière:  formule A=1/(a+bR+cR²): 
		if (mLight[0])
		{
			mLight[0]->setDiffuseColour(mRedColour);
			mLight[0]->setAttenuation(Range,1,1/Range,0);	// mi-puissance à Range 
		}
		// change la couleur emissive des lampes murales
		this->setLampColour(mRedColour);
	}
	else
	{
		// On eteint la lumière rouge, et on rétablit la lumière normale
		if (mLight[0])
		{
			mLight[0]->setDiffuseColour(mLightColour);
			mLight[0]->setAttenuation(mLightRange,1,1/mLightRange,0);	// mi-puissance à Range 
		}
		// Change la couleur emissive des lampes murales
		this->setLampColour(mLightColour);
	}
	TRACE("<< "+mRoomName+"::setNightLight");
}




/* ------------------------------------------------------------------------- */
/// Cette fonction cree une lumiere à l'interieur de la pièce (maximum 4).
/* ------------------------------------------------------------------------- */
void Room::createLight(Light::LightTypes lumtype, String name, bool castShadows)
{
	// -------------------------------
	// On cree un Node pour la lumiere
	// -------------------------------
	SceneNode* LightNode = mRoomNode->createChildSceneNode(mRoomName+"/"+name);
	//LightNode->setInheritScale(false);

	// -------------------------------
	// On cree la lumiere
	// -------------------------------
	Light* light = mSceneMgr->createLight(mRoomName+"/"+name);
	light->setType(lumtype);

	// -------------------------------
	// Gestion des ombres générées par cette lumière
	// -------------------------------
	light->setCastShadows(castShadows);

	// Pas de gestion des ombres pour Lumiere Omnidirectionnelle (le texture-shadows ne fonctionne pas avec les LT_POINT).
	if (lumtype==Light::LT_POINT) light->setCastShadows(false);		// Cette lumiere ne projette pas d'ombres
	
	if (lumtype==Light::LT_SPOTLIGHT) 
	{
		light->setSpotlightRange(Degree(35),Degree(70));  // 15-30 // valeurs par defaut= 35-70.
	}


	// On attache la lumière au Node
	LightNode->attachObject(light);
//	light->setPosition(0,0,0); 

	// -------------------------------
	// Memorisation
	// -------------------------------
	if (name==ROOMLIGHT) 
	{
		mLight[0] = light;
	}
	else
	{
		// On parcourt le tableau des Light de cette Room
		for (int i=1; i<4; i++)
		{
			// On mémorise la nouvelle Light dans le premier emplacement libre
			if (!mLight[i]) {mLight[i]= light; break;}
		} 
	}
	
	DEBUG_LOG(mRoomName+": light "+mRoomName+"/"+name+" created");
}


/* ------------------------------------------------------------------------- */
/// Definit la position d'une lumiere de la piece
/* ------------------------------------------------------------------------- */
void Room::setLightPosition(Vector3 position, String name)
{
	SceneNode* N;
	// On recupère la lumiere demandée, et on la positionne:
	N= mSceneMgr->getSceneNode(mRoomName+"/"+name);
	N->setPosition(position);
	_showLightPosition(N);
}


/* ------------------------------------------------------------------------- */
/// Definit le cone d'une lumière spot.
/* ------------------------------------------------------------------------- */
void Room::setSpotLightRange(Real inner, Real outer, Real falloff, String name)
{
	// On recupère la lumiere demandée
	Light* l = mSceneMgr->getLight(mRoomName+"/"+name);
	if (l->getType()!=Light::LT_SPOTLIGHT) return;

	// On définit sa portée
	l->setSpotlightRange(Degree(inner),Degree(outer), falloff);
}

/* ------------------------------------------------------------------------- */
/// Debug: On visualise un objet à l'endroit de la source lumineuse.
/* ------------------------------------------------------------------------- */
void Room::_showLightPosition(SceneNode* N)
{
	BillboardSet*  billboardset = mSceneMgr->getBillboardSet("Sphinx/lightset");
	mLightSprite = billboardset->createBillboard(mParentNode->getPosition()+mRoomNode->getPosition()+N->getPosition());
	mLightSprite->setDimensions(20,20);
//	if (!mLightEntity) mLightEntity = mSceneMgr->createEntity(N->getName()+"/marker", "wpbox.mesh");
//	N->attachObject(mLightEntity);
	Light* l = mSceneMgr->getLight(N->getName());
	l->setVisible(true);
}


/* ------------------------------------------------------------------------- */
/// Debug: On actualise la position de l'objet à l'endroit de la source lumineuse.
/* ------------------------------------------------------------------------- */
void Room::_updateLightMarker(SceneNode* N)
{
	mLightSprite->setPosition(mParentNode->getPosition()+mRoomNode->getPosition()+N->getPosition());
}

/* ------------------------------------------------------------------------- */
/// Definit la portée d'une lumiere de la piece, selon la formule A=1/(a+bR+cR²)
/* ------------------------------------------------------------------------- */
void Room::setLightRange(int range, String name)
{
	// On recupère la lumiere demandée
	Light* l = mSceneMgr->getLight(mRoomName+"/"+name);
	// On définit sa portée
	l->setAttenuation(range,1,1/range,0);	// mi-puissance à range 

	// S'il s'agit de la lumière principale de la pièce, on la mémorise
	if (name==ROOMLIGHT) mLightRange=range;
}


/* ------------------------------------------------------------------------- */
/// Definit la direction d'une lumiere de la piece (pour les spots)
/* ------------------------------------------------------------------------- */
void Room::setLightDirection(Vector3 direction, String name)
{
	// On recupère la lumiere demandée, et on l'oriente
	SceneNode* N = mSceneMgr->getSceneNode(mRoomName+"/"+name);
	N->setDirection(direction);
}


/* ------------------------------------------------------------------------- */
/// Permet de changer dynamiquement la couleur d'une lumière
/* ------------------------------------------------------------------------- */
void Room::setLightColour(ColourValue newcolour, String name)
{
	// On recupère la lumiere demandée
	Light* l = mSceneMgr->getLight(mRoomName+"/"+name);
	// On change sa couleur
	l->setDiffuseColour(newcolour);

	// S'il s'agit de la lumière principale de la pièce, on la mémorise
	if (name==ROOMLIGHT) mLightColour=newcolour;
}


/* ------------------------------------------------------------------------- */
/// Active la sirène d'alarme, et les écrans d'alerte
/* ------------------------------------------------------------------------- */
void Room::setAlertLevel(short alert)
{
	mAlertLevel = alert;

	// On recupère le materiel associé aux écrans muraux
	SubEntity* ScreenWall_Entity = this->getSubEntity("Room1/screen_wall");

	String AlertSoundFile = "../media/sounds/sci-fi79.wav";	/// TODO: choisir le meilleur son
//	String AlertSoundFile = "../media/sounds/buzzer.wav";

	switch(alert) 
	{
	case 0:		// Tout va bien
		if (ScreenWall_Entity) ScreenWall_Entity->setMaterialName("Room/dynamic/screen_void");
		this->setNightLight(false);
		SoundManager::getSingletonPtr()->freeSound(mRoomAlertChannel);
		break;
	case 1:
		/// TODO: gerer Alerte niveau 1
		break;
	case 4:
	case 2:		// Collision
		if (ScreenWall_Entity) ScreenWall_Entity->setMaterialName("Room/dynamic/screen_collision");
		this->setNightLight(true);
		SoundManager::getSingletonPtr()->freeSound(mRoomAlertChannel);
		mRoomAlertChannel = SoundManager::getSingletonPtr()->playSound(AlertSoundFile,20,200,true);
		FSOUND_SetVolume(mRoomAlertChannel, 40);
		break;
	case 3:
		/// TODO: gerer Alerte niveau 3
		break;
//	case 4:
		/// TODO: gerer Alerte niveau 4
//		break;
	case 5:		// Il faut évacuer
		/// TODO: gerer Alerte niveau 5
		break;
	}
	/// TODO:  Audio sirene d'alarme

}


/* ------------------------------------------------------------------------- */
/// Renvoie le nom de la piece
/* ------------------------------------------------------------------------- */
String Room::getName()
{
	return mRoomName;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le Node associé à la piece
/* ------------------------------------------------------------------------- */
SceneNode*  Room::getNode()
{
	return mRoomNode;
}


/* ------------------------------------------------------------------------- */
/// Indique si la Room a une ouverture vers l'Exterieur.
/* ------------------------------------------------------------------------- */
void Room::setExternView(bool externview)
{
	mHasExternView = externview;
}

/* ------------------------------------------------------------------------- */
/// Indique si la Room a une ouverture vers l'Exterieur.
/* ------------------------------------------------------------------------- */
bool Room::hasExternView()
{
	return mHasExternView;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le status de la Room sous forme de chaine XML
/* ------------------------------------------------------------------------- */
String Room::getStatus()
{
	TRACE (">> "+mRoomName+"::getStatus()");
	String Xstatus = "";

	// On construit le status de la Room
	if (mBroken)
	{
		Xstatus  = "<item";
		Xstatus += " id='"+ITOA((int)mRoomNode)  +"' ";
//		Xstatus += " location='outside' ";
		Xstatus += " spares='"+ITOA(PLASTA)      +"' ";
		Xstatus += " ETR='"   +ITOA(mDamage_ETR) +"' ";
		Xstatus += " text='Negative.\nThe room is depressurized. All the air has escaped. An external intervention is needed.' ";
		Xstatus += ">";
		Xstatus += mRoomName;
		Xstatus += "</item> \n";
	}

	// On ajoute le status des objets contenus dans la Room
	Xstatus += this->getChildStatus();

	return Xstatus;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le status des objets appartenant à cette Room.
/* ------------------------------------------------------------------------- */
String Room::getChildStatus()
{
	SceneNode* ChildNode   = NULL;
	MovableObject*     MO  = NULL;
	UserDefinedObject* UDO = NULL;
	String Xstatus         = "";
	long OID = 0;

	TRACE (">> "+mRoomName+"::getChildStatus()");

	// On parcourt tous les Childs du RoomNode
	SceneNode::ChildNodeIterator ObjectIterator = mRoomNode->getChildIterator(); 

	// Iterate all nodes under the parent node: the tested nodes
	while ( ObjectIterator.hasMoreElements( )) 
	{ 
		// Get the next node 
		ChildNode = (SceneNode*)ObjectIterator.getNext(); 
		if (ChildNode)
		{
			// On recupère les infos sur ce Node
			TRACE("   found Child "+ChildNode->getName());
			MO  = ChildNode->getAttachedObject(0);		
			/// Attention: je suppose que l'objet est toujours en indice 0. 
			/// (Pour etre plus robuste, il faudrait gerer un iterateur)
			if (MO)  UDO = MO->getUserObject(); else UDO = NULL;
			if (UDO) OID = UDO->getTypeID(); else OID=0;
			switch (OID)
			{
			case HIBERNATION_ID:	Xstatus += ((Hibernation*)UDO)->getStatus();	break;
			}
		}
	}
	return Xstatus;
}


/* ------------------------------------------------------------------------- */
/// La Room est dépressurisée
/* ------------------------------------------------------------------------- */
void Room::setDepressurized()
{
	mBroken     = true;
	mDamage_ETR = -1;   // ETR inconnu (il faut une intervention d'un NPC).
}

/* ------------------------------------------------------------------------- */
/// On memorise un son d'ambiance (en boucle) associé à la pièce.
/* ------------------------------------------------------------------------- */
void Room::setSoundFile(String SoundFile)
{
	mRoomSoundFile   = SoundFile;
}


/* ------------------------------------------------------------------------- */
/// Retrouve un material de la Room par son nom.
/* ------------------------------------------------------------------------- */
MaterialPtr Room::getMaterial(String MaterialName)
{
	MaterialPtr RequestedMaterial;
	RequestedMaterial.setNull();

	int NumSubEntities = mRoomEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mRoomEntity->getSubEntity(i)->getMaterialName();
		if (MatName == MaterialName)     RequestedMaterial=mRoomEntity->getSubEntity(i)->getMaterial();
	}
	return RequestedMaterial;
}

/* ------------------------------------------------------------------------- */
/// Retrouve une SubEntity de la Room par son nom
/* ------------------------------------------------------------------------- */
SubEntity* Room::getSubEntity(String MaterialName)
{
	SubEntity* RequestedEntity = NULL;
	if (!mRoomEntity) return NULL;

	int NumSubEntities = mRoomEntity->getNumSubEntities();
	for (int i=0; i<NumSubEntities; i++) 
	{
		String MatName = mRoomEntity->getSubEntity(i)->getMaterialName();
		if (MatName == MaterialName)   RequestedEntity = mRoomEntity->getSubEntity(i);
	}
	return RequestedEntity;
}


/* ------------------------------------------------------------------------- */
/// Déplace le Node de la Room. La pièce suit ainsi la position de la scene à laquelle elle appartient.
/* ------------------------------------------------------------------------- */
void Room::translate(Vector3 offsetVector)
{
	mRoomNode->translate(offsetVector);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la position de la pièce (par rapport à son parent).
/* ------------------------------------------------------------------------- */
Vector3 Room::getPosition()
{
	return mRoomNode->getPosition();
}


/* ------------------------------------------------------------------------- */
/// Quand on importe de DotScene de Blender, bien souvent il faut corriger l'orientation de -90 degres.
/* ------------------------------------------------------------------------- */
void Room::adjustPitch()
{
	mRoomNode->pitch(Degree(-90));
}


/* ------------------------------------------------------------------------- */
/// On mémorise le nom du material portant le nom de la piece, et qui doit etre affiché à l'entrée.
/* ------------------------------------------------------------------------- */
void Room::setPanelMaterialName(String MaterialName)
{
	mRoomPanel = MaterialName;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction retourne le nom du material portant le nom de la piece, et qui doit etre affiché à l'entrée.
/* ------------------------------------------------------------------------- */
String Room::getPanelMaterialName()
{
	return mRoomPanel;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction affecte (ou non) à la piece le status d'ascenseur.
/* ------------------------------------------------------------------------- */
void Room::setElevatorRoom(bool elevator)
{
	mElevatorRoom = elevator;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction indique si la piece est un ascenseur.
/* ------------------------------------------------------------------------- */
bool Room::isElevatorRoom()
{
	return mElevatorRoom;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction indique si la piece est depressurisée
/* ------------------------------------------------------------------------- */
bool Room::isBroken()
{
	return mBroken;
}


/* ------------------------------------------------------------------------- */
/// Charge les paramètres initiaux de la pièce.
/* ------------------------------------------------------------------------- */
void Room::loadObject()
{
    String filename = "../savedgames/rooms.xml";
	TRACE("Loading "+filename);

	XMLParser_T* SavedGame = new XMLParser_T(filename);

	if (!SavedGame) return;
	if (!SavedGame->gotoRootElement("rooms")) return;
	if (!SavedGame->gotoSubNamedElement("status",mRoomName)) return;

	mBroken     = SavedGame->getBooleanAttribute("broken");
	mDamage_ETR = SavedGame->getIntAttribute("etr");
	delete(SavedGame);
}

/* ------------------------------------------------------------------------- */
/// Gere les collisions avec les murs de la pièce.
/* ------------------------------------------------------------------------- */
void Room::initCollisionShape()
{
	using namespace OgreOpcode;
	
	CollisionContext* collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();

	// Creation de la forme englobante
	CollisionShape *collideShape = CollisionManager::getSingletonPtr()->NewShape(mRoomName.c_str());
	// Chargement du mesh associé à cette forme
	collideShape->Load(mRoomEntity);
	// Creation de l'objet
	mCollideObject = collideContext->NewObject();
	mCollideObject->SetCollClass("room");
	mCollideObject->SetShape(collideShape);

	// Ajout de cet objet dans le contexte
	collideContext->AddObject(mCollideObject);
	TRACE("Collision System initialized for "+mRoomName);
	mCollideObject->setDebug(false);
}


/* ------------------------------------------------------------------------- */
/// Change la couleur luminescente des lampes murales.
/** CE n'est pas très optimibé de metter cette fonction au niveau de chaque room .... */
/* ------------------------------------------------------------------------- */
void Room::setLampColour(ColourValue newcolor)
{
	// On retrouve le material associé aux lampes
	MaterialPtr MatLight;
	MatLight.setNull();

	// Ce materiaux peut porter différents noms, selon les pièces....
	MatLight=MaterialManager::getSingletonPtr()->getByName("Lamps");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);
	MatLight=MaterialManager::getSingletonPtr()->getByName("Lamps/1");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);
	MatLight=MaterialManager::getSingletonPtr()->getByName("Lamps/2");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);
	MatLight=MaterialManager::getSingletonPtr()->getByName("Lamps/3");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);
	MatLight=MaterialManager::getSingletonPtr()->getByName("Lamps/4");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);
	MatLight=MaterialManager::getSingletonPtr()->getByName("Lamps/5");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);
	MatLight=MaterialManager::getSingletonPtr()->getByName("Room1/light");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);
	MatLight=MaterialManager::getSingletonPtr()->getByName("Corridor/light");
	if (!MatLight.isNull()) MatLight->setSelfIllumination(newcolor);

}