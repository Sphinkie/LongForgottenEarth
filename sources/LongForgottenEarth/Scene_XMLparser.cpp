/* ------------------------------------------------------------------------- */
// File       : Scene_XMLparser.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Scene_XMLparser.h"
#include "macros.h"
#include "lib_tinyxml\tinyxml.h"

// #undef  DEBUG_LOG
// #define DEBUG_LOG(X) 


/* ------------------------------------------------------------------------- */
/// Constructeur (on execute le Constructeur de la surclasse)
/* ------------------------------------------------------------------------- */
Scene_XMLparser::Scene_XMLparser(String XMLfile) : XMLParser_T(XMLfile)
{
	mRoomIndex = -1;
	mDoorIndex = -1;
	this->gotoTopOfFile();
}

/* ------------------------------------------------------------------------- */
/// Destructeur (on execute le destructeur de la surclasse)
/* ------------------------------------------------------------------------- */
Scene_XMLparser::~Scene_XMLparser()
{
}


/* ------------------------------------------------------------------------- */
/// On réinitialise les pointeurs des sous-objets d'une Room
/* ------------------------------------------------------------------------- */
void Scene_XMLparser::gotoTopOfFile()
{
//	mRoomIndex   = -1;
//	mDoorIndex   = -1;
	mObjectIndex = 0;
	mLightIndex  = 0;
	mNoMoreNodes = false;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur un node ROOM de la scene.
/** 
	@param index Position du node correspondant à la room dans le fichier XML.
	@return La fonction renvoie FALSE, si le node n'est pas trouvé.
	Les problemes rencontrés peuvent être un node inexistant, ou un node ne correspondant 
	pas à une Room.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::gotoRoom(int index)
{
	DEBUG_LOG(">> gotoRoom("+ITOA(index)+")");

	// On se positionne au debut des nodes de la scene
	if (!XMLParser_T::gotoElementPath("scene/nodes")) return false;

	// Les nodes de premier niveaux sont des Rooms, des Doors, etc.
	// On se positionnne sur la Nieme Room trouvée.
	if (!XMLParser_T::gotoSubElement("node",index)) 
	{
		mNoMoreNodes = true;
		return false;
	}
	
	// On vérifie que c'et bien un node de classe "room"
	if (this->getUserdata("CLASS") != "room") return false;

	mParentNode  = mCurrentNode;
	mRoomIndex   = index;
	return true;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le node DOOR de la scene, suivant la derniere porte detectée..
/** 
	@return La fonction renvoie FALSE, s'il n'y a plsu de nod DOOR, ou en cas de probleme.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::gotoNextDoor()
{
	DEBUG_LOG(">> gotoNextDoor()");

	// Les nodes de premier niveaux sont des Rooms, des Doors, etc.

	// On se positionne au debut des nodes de la scene
	if (!XMLParser_T::gotoElementPath("scene/nodes")) return false;

	// On se positionnne sur la Door suivante.
	mDoorIndex++;

	// on parcourt les nodes suivants
	while (XMLParser_T::gotoSubElement("node",mDoorIndex)) 
	{
		// On a trouvé un node.
		// On vérifie si c'est un node de classe "door"
		if (this->getUserdata("CLASS") == "door") 
		{
			// si oui, on a trouvé une porte. 
			mParentNode = mCurrentNode;
			// On peut quitter
			return true;
		}
		// Sinon, on passe au node suivant.
		mDoorIndex++;
	}
	// On a parcouru tous les nodes restants sans trouver de door.
	mNoMoreNodes = true;
	return false;
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le node ROOM de la scene, succedant à la derniere pièce détectée..
/** 
	@return La fonction renvoie FALSE, s'il n'y a plus de node ROOM, ou en cas de problème.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::gotoNextRoom()
{
	DEBUG_LOG(">> gotoNextRoom()");

	// Les nodes de premier niveaux sont des Rooms, des Doors, etc.
	
	// On se positionne au debut des nodes de la scene
	if (!XMLParser_T::gotoElementPath("scene/nodes")) return false;

	// si mRoomIndex est -1, on la "next room" est en fait la "first room".
	// donc, on commence la recherche à partir du node suivant.
	mRoomIndex++;

	// on parcourt les nodes suivants
	while (XMLParser_T::gotoSubElement("node",mRoomIndex)) 
	{
		// On a trouvé un node.
		// On vérifie si c'est un node de classe "room"
		if (this->getUserdata("CLASS") == "room") 
		{
			// si oui, on a trouvé une pièce. 
			mParentNode = mCurrentNode;
			// On peut quitter
			return true;
		}
		// Sinon, on passe au node suivant.
		mRoomIndex++;
	}
	// On a parcouru tous les nodes restants sans trouver de Room.
	mNoMoreNodes = true;
	return false;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le node CAMERA de la scene.
/** 
	Les fichier dotScene peuvent contenir une position de camera, utile pour le debug.
	@return La fonction renvoie FALSE, si le node n'est pas trouvé.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::gotoCameraNode()
{
	bool leave;
	int  index =0;

	DEBUG_LOG(">> gotoCameraNode()");

	if (!mDocument) return false;
	TiXmlHandle docHandle((TiXmlNode*)mDocument);
	TiXmlHandle NodesHandle = docHandle.FirstChild("scene").FirstChild("nodes");
	TiXmlNode*  camNode = NULL;		// the camera node
	TiXmlNode*  subNode = NULL;		// the camera subnode

	do 
	{
		camNode = NodesHandle.Child("node",index++).Node();
		if (camNode) subNode = camNode->FirstChild("camera");
		// s'il n'y a plus de node on sort, ou si on a trouvé la camera 
		leave    = subNode || !camNode;
	} while (!leave);

	mCurrentNode = camNode;
	mParentNode  = camNode;

	/// On renvoie FALSE si on trouve pas la camera
	return (camNode	!= NULL);
}



/* ------------------------------------------------------------------------- */
/// On se positionne sur le node DOOR de la ROOM courante.
/** 
	Dans la nouvelle structure des DotScene decrivant un Level, la porte est un
	node Child de la pièce.
	@return La fonction renvoie FALSE, si le node n'est pas trouvé.
	OBSOLETE
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::gotoRoomDoor()
{
	bool door_found;
	bool leave;
	int  index;	// index à partir duquel on veut commencer la recherche
	if (mDoorIndex==0) index=0;	// si on veut trouver la première door de la pièce.
	else index = mDoorIndex+1;  // si on veut commencer à la suite de la dernière porte trouvée

	TRACE(">>  gotoRoomDoor()");

	if (!mDocument) return false;
	TiXmlHandle docHandle((TiXmlNode*)mDocument);
	TiXmlHandle RoomHandle = docHandle.FirstChild("scene").FirstChild("nodes").Child("node",mRoomIndex);
	TiXmlNode*  DoorNode = NULL;

	do 
	{
		DoorNode = RoomHandle.Child("node",index++).Node();
		door_found = (this->getUserdata(DoorNode,"CLASS") == "door");
		// s'il n'y a plus de node on sort, ou si on a trouvé la porte
		leave    = door_found || !DoorNode;
	} while (!leave);

	mCurrentNode = DoorNode;
	mParentNode  = DoorNode;
	if (door_found) mDoorIndex=index-1;

	/// On renvoie FALSE si on trouve pas la porte.
	return (door_found);
}



/* ------------------------------------------------------------------------- */
/// On se positionne sur le node PANEL de la DOOR courante.
/** 
	Dans la nouvelle structure des DotScene decrivant un Level, le panel est le
	(seul) node Child de la porte.
	@return La fonction renvoie FALSE, si le node n'est pas trouvé.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::gotoDoorPanel()
{
	TRACE(">>  gotoDoorPanel()");

	if (!mDocument)   return false;
	if (mDoorIndex<0) return false;
	TiXmlHandle docHandle((TiXmlNode*)mDocument);
	TiXmlHandle PanelHandle  = docHandle.FirstChild("scene").FirstChild("nodes").Child("node",mDoorIndex).FirstChild("node");
//	TiXmlHandle RoomHandle  = docHandle.FirstChild("scene").FirstChild("nodes").Child("node",mRoomIndex);
//	TiXmlHandle PanelHandle = RoomHandle.Child("node",mDoorIndex).Child("node",0);
	TiXmlNode*  PanelNode   = PanelHandle.Node();


	if (!PanelNode) 
		TRACE("   No panel found for this door.");
	else
	{
		mCurrentNode = PanelNode;
		mParentNode  = PanelNode;
	}
	/// On renvoie FALSE si on trouve pas le panel.
	return (PanelNode!=NULL);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le tag "node" suivant, appartenant à la Room en cours, et contenant un element "entity".
/**
@return On renvoie FALSE si on n'a plus trouvé d'objet dans la room courante.
On renvoie TRUE, si on a trouvé un objet. Dans ce cas, le "current node" est 
positionné sur l'objet, et on peut lire ses attributs.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::getNextSubObject()
{
	bool found;
	bool child_ok;
	bool all_ok;
	bool leave;

	if (mRoomIndex<0) return false;
	// Initialisation: on se place sur le node de la Room
	this->gotoRoom(mRoomIndex);	
	do 
	{
		found = gotoSubElement("node",mObjectIndex++);
		TiXmlHandle nodeHandle((TiXmlNode*) mCurrentNode);
		child_ok = (nodeHandle.FirstChild("entity").Element() != NULL);
		leave    = child_ok || !found;
		all_ok   = found && child_ok;

	} while (!leave);
	// On cherche tant que l'on n'a pas trouvé de node avec un Child "entity"

	// Si on n'a pas trouvé de node =  on sort de la boucle
	// Si on a trouvé un node, avec child_ok = on sort
	// c-a-d: Si on a trouvé un node, sans child_ok, on reste dans la boucle.

	if (all_ok) this->setParentNode();	// On se positionne sur le node de l'objet

	/// On renvoie FALSE si on trouve pas d'objet.
	return (all_ok);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le tag "node" suivant, appartenant à la Room en cours, et contenant un element "light".
/**
@return On renvoie FALSE si on n'a plus trouvé de lumière dans la room courante.
On renvoie TRUE, si on a trouvé une lumière. Dans ce cas, le "current node" est 
positionné sur la lumière, et on peut lire ses attributs.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::getNextSubLight()
{
	bool found;
	bool child_ok;
	bool all_ok;
	bool leave;

	if (mRoomIndex<0) return false;
	this->gotoRoom(mRoomIndex);
	do 
	{
		found = XMLParser_T::gotoSubElement("node",mLightIndex++);
		DEBUG_LOG("mLightIndex="+StringConverter::toString(mLightIndex));
		TiXmlHandle nodeHandle((TiXmlNode*) mCurrentNode);
		child_ok = (nodeHandle.FirstChild("light").Element() != NULL);
		leave    = child_ok || !found;
		all_ok   = found && child_ok;

	} while (!leave);
	// On cherche tant que l'on n'a pas trouvé de node avec un Child "light"

	// Si on n'a pas trouvé de node =  on sort de la boucle
	// Si on a trouvé un node, avec child_ok = on sort
	// c-a-d: Si on a trouvé un node, sans child_ok, on reste dans la boucle.

	if (all_ok) this->setParentNode();// On se positionne sur le node de la lumière
	
	/// On renvoie FALSE si on trouve pas de light.
	return (all_ok);
}


/* ------------------------------------------------------------------------- */
/// On analyse les \<userdata\> du Node demandé, et on renvoie la DATA correspondant à la property demandée.
/**
	@param node         Node parent dont on veut lire une \<userdata\>
	@param PropertyName Nom du tag \<property\> dont on veut recuperer la DATA
	@return La valeur de l'attribut DATA correspondant.
*/
/* ------------------------------------------------------------------------- */
String Scene_XMLparser::getUserdata(void* node, String PropertyName)
{
	DEBUG_LOG(">> getUserdata("+PropertyName +")");
	/// Tous les attributs de Property (type, name et data) doivent être en majuscules.
	String Value  ="";

	TiXmlHandle   nodeHandle((TiXmlNode*) node);
	TiXmlElement* userProperty = nodeHandle.FirstChild("userData").FirstChild("property").Element();

	if (!userProperty) DEBUG_LOG("*** Warning: userData property not found");

	// On parcourt les <property> du node.
	for( userProperty; userProperty; userProperty=userProperty->NextSiblingElement() )
	{
		// String x_type = userProperty->Attribute("type") ;
		String x_name = userProperty->Attribute("name") ;
		// String x_data = userProperty->Attribute("data") ;
		// DEBUG_LOG("   found "+x_type + " " + x_name + " " + x_data);
		if (x_name==PropertyName)  Value=userProperty->Attribute("data");
	}
	DEBUG_LOG("<< getUserdata("+PropertyName +") = "+Value);

	return (Value);
}

/* ------------------------------------------------------------------------- */
/// On analyse les \<userdata\> du Node courant, et on renvoie la DATA correspondant à la property demandée.
/** 
	@param PropertyName Attribut NAME du tag \<property\> dont on veut recuperer la DATA
	@return La valeur de l'attribut DATA correspondant.
*/
/* ------------------------------------------------------------------------- */
String Scene_XMLparser::getUserdata(String PropertyName)
{
	/// Les attributs TYPE et NAME de "property" doivent être en majuscules
	// On ne verifie pas que l'attribut "TYPE" est bien "STRING".
	String Value = getUserdata(mCurrentNode, PropertyName);
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// On analyse les \<userdata\> du Node courant, et on renvoie la DATA correspondant à la property demandée.
/** 
	@param PropertyName Attribut NAME du tag \<property\> dont on veut recuperer la DATA
	@return La valeur de l'attribut DATA correspondant.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::getBooleanUserdata(String PropertyName)
{
	/// Les attributs TYPE et NAME de "property" doivent être en majuscules
	// On ne verifie pas que l'attribut "TYPE" est bien "BOOL".
	String Value = getUserdata(mCurrentNode, PropertyName);
	return (Value=="1");
}


/* ------------------------------------------------------------------------- */
/// Cette fonction indique si, d'une façon ou d'une autre, on a atteint la fin des \<node\> de \<nodes\>.
/** 
	@return FALSE si la fin des node a été atteinte.
*/
/* ------------------------------------------------------------------------- */
bool Scene_XMLparser::hasMoreNodes()
{
	return (!mNoMoreNodes);
}

