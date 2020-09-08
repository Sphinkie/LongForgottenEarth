/* ------------------------------------------------------------------------- */
// File    : WaypointNetwork.h
// Project : Long Forgotten Earth
// Author  : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _WAYPOINT_NETWORK_H_
#define _WAYPOINT_NETWORK_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion d'un réseau de waypoints.
/* ------------------------------------------------------------------------- */
class WaypointNetwork  
{
public:
	WaypointNetwork(SceneManager* SceneMgr, SceneNode* ParentNode, String FilePrefix);
	virtual    ~WaypointNetwork();
	String     getClosestWaypointAction(Vector3 Pos);
	SceneNode* getClosestWaypointNode(Vector3 Pos);
	SceneNode* findNextWaypointTo(SceneNode* FromNode,SceneNode* ToNode);
	SceneNode* hasLineWith(SceneNode* FromNode, SceneNode* ToNode);
	SceneNode* existCrossingLinesWith(SceneNode* FromNode, SceneNode* ToNode);
	SceneNode* findWayWith2changes(SceneNode* FromNode, SceneNode* ToNode);
	SceneNode* getWaypointNode(String Name);

	void       showLines();
	void       createWaypointEntity(String WPname, SceneNode* WPnode);

protected:
	

	typedef struct Waypoint		///< Cette structure décrit un Waypoint
	{
		SceneNode*   WaypointNode;
		String       Action;
		Waypoint*    Previous;
		Waypoint*    Next;
	} ;

	typedef struct Ligne		///< Cette structure decrit une ligne de Waypoints
	{
		int         LevelNb;
		int         LigneID;
		Waypoint*	FirstWaypoint;
		Ligne*      NextLine;
	} ;

	SceneManager*    mSceneMgr;            ///< Le SceneManager Ogre
	SceneNode*       mNetworkNode;         ///< Le node auquel est rattaché l'ensemble du réseau
	Ligne*           mLineList;	           ///< Debut de la liste des lignes du réseau
	Ligne*           mLastLine;	           ///< Fin de la liste des lignes du réseau

	bool       containLineWaypoint(Ligne* line, SceneNode* node);
	bool       addWPline(String WPfile);
};

#endif
