/* ------------------------------------------------------------------------- */
//  File       : ShipLevel.cpp
//  Project    : Long Forgotten Earth
//  Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SHIPLEVEL_H_
#define _SHIPLEVEL_H_


#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "OgreConfigFile.h"
#include "door.h"
#include "room.h"
#include "ShipLevel_XMLparser.h"
#include "Scene_XMLparser.h"

using namespace Ogre;

#define MAP_SIZE 12  // On se limite à 12 pieces par Level

/* ------------------------------------------------------------------------- */
/// Cette classe gère les élements d'un meme étage: pieces, lumières, portes...
/** 
Cette classe gère l'affichage (ou non) des pieces, selon l'emplacement du joueur (=camera) sur la carte.
Le level est décrit dans un fichier XML, qui contient des Room, des Door, et des Scenes. 
Une Scene est aussi un fichier XML (au format "dotScene" qui contient 1 Romm et 1 Door).
*/
/* ------------------------------------------------------------------------- */
class ShipLevel
{
public:
	ShipLevel(String Name, int Altitude, SceneManager* SceneMgr, SceneNode* ParentNode);
	virtual    ~ShipLevel();
	Vector3    getPosition();
	void       setAllLights(bool On);
	void       setRedLight(bool On);
	void       showOneRoom(Room* room);
	void       showOneRoom(String roomName);
	void       showAdjacentRooms(Door* Porte);
	void       showAllRooms();
	void       hideAllRooms();
	void       showElevator();
	void       setInitialRoom();
	bool       openDoor(Door* Porte);
	bool       openDoor(String DoorName);
	String     getStatus();
	void       setAlertLevel(short alert);
	Vector3    getCamPos();
	Room*      getElevatorRoom();
	Room*      getRoom(String RoomName);

protected:
	void    addScene();
	Room*   addRoom();
	Door*   addDoor();
	void    addObject(SceneNode* ParentNode);
	void    addLight(Room* ParentRoom);
	int     getMapIndex(String RoomName);
	void    linkScene();
	Vector3 readCamPosition();

	// Fonctions pour la double gestion des level.xml et scene.xml
	String     getXMLattribute(String attribute, String childnode ="");
	Vector3    getXMLvector(String childnode);
	bool       getXMLboolean(String attribute, bool DefaultValue, String childnode);
	Vector3    getXMLColour();
	Vector3    getXMLLightDirection();
	Real       getXMLLightRange();
	int        getXMLRotation();
	Quaternion getXMLOrientation();
	String     getXMLuserdata(String PropertyName);
	void       rotateNode(SceneNode* node, Real Orientation, Quaternion q = Quaternion::IDENTITY);


	typedef struct t_Map		///< Cette structure contient le plan du Level
	{
		Door*  Porte;
		Room*  insideRoom;
		Room*  outsideRoom;
		bool   Active;
	} ;



	String               mLevelName;
	SceneNode*	         mLevelNode;
	SceneManager*        mSceneMgr;
	SceneNode*	         mParentNode;
	ShipLevel_XMLparser* mXMLlevel;
	Scene_XMLparser*     mXMLscene;
	int                  mRoomIndex;
	int                  mDoorIndex;
	t_Map                mDoorMap[MAP_SIZE];		
	Room*                mRoomMap[MAP_SIZE]; ///< Le tableau de toutes les Room du niveau.
	Room*                mElevatorRoom;      ///< La piece du niveau qui joue le role d'ascensceur.
	bool                 mSceneLevel;        ///< Ce flag indique que le niveau entier est decrit par un fichier DotScene
	bool                 mSceneRoom;         ///< Ce flag indique que la Room est decrite individuellement par un fichier DotScene
	Vector3              mCamPos;            ///< Position initiale de la camera dans ce niveau (debug)
	bool                 mBlender;           ///< Indique si la source du Level est Blender. Auquel cas, il y a une correction d'orientation à faire.

};

#endif 
