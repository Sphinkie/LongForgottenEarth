/* ------------------------------------------------------------------------- */
// File       : Scene_XMLparser.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SCENE_XMLPARSER_H_
#define _SCENE_XMLPARSER_H_


#include "Ogre.h"
#include "XMLParser_T.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Gestion des fichiers de description de scene au format DotScene.
/* ------------------------------------------------------------------------- */
class Scene_XMLparser  : public XMLParser_T
{
public:
	Scene_XMLparser(String XMLfile);
	virtual ~Scene_XMLparser();
	// Fonctions spécifiques pour la gestion des Levels du vaisseau
	void    gotoTopOfFile();
	bool    gotoRoom(int index);
	bool    gotoNextRoom();
	bool    gotoCameraNode();
	bool    gotoRoomDoor();
	bool    gotoNextDoor();
	bool    gotoDoorPanel();
	bool    getNextSubObject();
	bool    getNextSubLight();
	String  getUserdata(String PropertyName);
	bool    getBooleanUserdata(String PropertyName);
	bool    hasMoreNodes();


protected:
	String  getUserdata(void* node, String PropertyName);

	int   mRoomIndex;		///< position courante dans les listes d'éléments XML
	int   mDoorIndex;		///< position courante dans les listes d'éléments XML
	int   mObjectIndex;		///< position courante dans les listes d'éléments XML
	int   mLightIndex;		///< position courante dans les listes d'éléments XML
	bool  mNoMoreNodes;     ///< indicateur de fin de fichier XML
	void* mRoomElement;		///< Handle sur la ROOM de la scene
	void* mDoorElement;		///< Handle sur la DOOR de la scene
};

#endif 
