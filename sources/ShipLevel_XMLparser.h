/* ------------------------------------------------------------------------- */
// File       : ShipLevel_XMLParser.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SHIPLEVEL_XMLPARSER_H_
#define _SHIPLEVEL_XMLPARSER_H_


#include "Ogre.h"
#include "XMLParser.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Gestion des fichiers XML (pour la description des niveaux: rooms, doors, objects, computers, etc)
/* ------------------------------------------------------------------------- */
class ShipLevel_XMLparser  : public XMLParser
{
public:
	ShipLevel_XMLparser(String XMLfile);
	virtual ~ShipLevel_XMLparser();
	// Fonctions spécifiques pour la gestion des du vaisseau
	void    gotoTopOfFile();
	bool    getNextRoom();
	bool    getNextDoor();
	bool    getNextSubObject();
	bool    getNextSubLight();
	bool    getNextScene();

protected:
	
	int   mSceneIndex;		/// position courante dans les listes d'éléments XML
	int   mRoomIndex;		/// position courante dans les listes d'éléments XML
	int   mDoorIndex;		/// position courante dans les listes d'éléments XML
	int   mObjectIndex;		/// position courante dans les listes d'éléments XML
	int   mLightIndex;		/// position courante dans les listes d'éléments XML

	void* mListScene;		/// Liste des tag <scene ...>   de type DOMNodeList*
	void* mListRoom;		/// Liste des tag <room ...>    de type DOMNodeList*
	void* mListDoor;		/// Liste des tag <door ...>    de type DOMNodeList*
	void* mObjectSubList;	/// Liste des tag <object ...>  de type DOMNodeList*
	void* mLightSubList;	/// Liste des tag <light ...>   de type DOMNodeList*
};

#endif 
