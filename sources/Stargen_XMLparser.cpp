/* ------------------------------------------------------------------------- */
// File       : Stargen_XMLparser.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Stargen_XMLparser.h"
#include "macros.h"
#include "lib_tinyxml\tinyxml.h"

#undef  DEBUG_LOG
#define DEBUG_LOG(X) TRACE(X)


/* ------------------------------------------------------------------------- */
/// Constructeur (on execute le Constructeur de la surclasse)
/* ------------------------------------------------------------------------- */
Stargen_XMLparser::Stargen_XMLparser(String XMLfile) : XMLParser_T(XMLfile)
{
	mRoomIndex = 0;
	mDoorIndex = 0;
	this->gotoTopOfFile();
}

/* ------------------------------------------------------------------------- */
/// Destructeur (on execute le destructeur de la surclasse)
/* ------------------------------------------------------------------------- */
Stargen_XMLparser::~Stargen_XMLparser()
{
}


/* ------------------------------------------------------------------------- */
/// On réinitialise les pointeurs parcourant le fichier DotScene 
/* ------------------------------------------------------------------------- */
void Stargen_XMLparser::gotoTopOfFile()
{
	mObjectIndex = 0;
	mLightIndex  = 0;
	mDoorIndex   = 0;
	mNoMoreNodes = false;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur une planete du systeme solaire.
/** 
	@param index Position du node correspondant à la planete dans le fichier XML [1..N].
	@return La fonction renvoie FALSE, si le node n'est pas trouvé.
*/
/* ------------------------------------------------------------------------- */
bool Stargen_XMLparser::gotoPlanet(int index)
{
	DEBUG_LOG(">> gotoPlanet("+ITOA(index)+")");

	if (!mDocument) return false;
	if (index<1)    return false;

	TiXmlHandle docHandle((TiXmlNode*)mDocument);
	TiXmlNode*  planetNode = docHandle.FirstChild("system").Child("planet",index-1).Node();

	mCurrentNode = planetNode;
	mParentNode  = planetNode;

	/// On renvoie FALSE si on trouve pas le node planet
	return (planetNode != NULL);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le node STAR de la scene.
/* ------------------------------------------------------------------------- */
bool Stargen_XMLparser::gotoStar()
{
	DEBUG_LOG(">> gotoStar()");

	if (!mDocument) return false;
	TiXmlHandle docHandle((TiXmlNode*)mDocument);
	TiXmlNode*  starNode = docHandle.FirstChild("system").FirstChild("star").Node();

	mCurrentNode = starNode;
	mParentNode  = starNode;

	/// On renvoie FALSE si on trouve pas le node Star
	return (starNode != NULL);
}



/* ------------------------------------------------------------------------- */
/// On se positionne sur le node DOOR de la ROOM courante.
/** 
	Dans la nouvelle structure des DotScene decrivant un Level, la porte est un
	node Child de la pièce.
	@return La fonction renvoie FALSE, si le node n'est pas trouvé.
*/
/* ------------------------------------------------------------------------- */
bool Stargen_XMLparser::gotoRoomDoor()
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
bool Stargen_XMLparser::gotoRoomDoorPanel()
{
	TRACE(">>  gotoRoomDoorPanel()");

	if (!mDocument) return false;
	TiXmlHandle docHandle((TiXmlNode*)mDocument);
	TiXmlHandle RoomHandle  = docHandle.FirstChild("scene").FirstChild("nodes").Child("node",mRoomIndex);
	TiXmlHandle PanelHandle = RoomHandle.Child("node",mDoorIndex).Child("node",0);
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
/* ------------------------------------------------------------------------- */
bool Stargen_XMLparser::getNextSubObject()
{
	bool found;
	bool child_ok;
	bool all_ok;
	bool leave;

	// Initialisation: on se place sur le node de la Room
	this->gotoPlanet(mRoomIndex);	
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
/// Renvoie l'attribut demandé du node child.
/** 
@param childnode Le nom du node fils du node courant.
@param attribute Le nom de l'attribut demandé.
@return la valeur de l'attribut (String)
*/
/* ------------------------------------------------------------------------- */
String Stargen_XMLparser::getPlanetAttribute(String childnode, String attribute)
{
	bool   found;
	String value;
	
	found = XMLParser_T::gotoSubElement(childnode,0);
	value = XMLParser_T::getStringAttribute(attribute);

	return (value);
}


/* ------------------------------------------------------------------------- */
/// On analyse les \<userdata\> du Node demandé, et on renvoie la DATA correspondant à la property demandée.
/**
	@param node         Node parent dont on veut lire une \<userdata\>
	@param PropertyName Nom du tag \<property\> dont on veut recuperer la DATA
	@return La valeur de l'attribut DATA correspondant.
*/
/* ------------------------------------------------------------------------- */
String Stargen_XMLparser::getUserdata(void* node, String PropertyName)
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

	return (Value);
}

/* ------------------------------------------------------------------------- */
/// On analyse les \<userdata\> du Node courant, et on renvoie la DATA correspondant à la property demandée.
/** 
	@param PropertyName Attribut NAME du tag \<property\> dont on veut recuperer la DATA
	@return La valeur de l'attribut DATA correspondant.
*/
/* ------------------------------------------------------------------------- */
String Stargen_XMLparser::getUserdata(String PropertyName)
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
bool Stargen_XMLparser::getBooleanUserdata(String PropertyName)
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
bool Stargen_XMLparser::hasMoreNodes()
{
	return (!mNoMoreNodes);
}

