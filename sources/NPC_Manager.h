/* ------------------------------------------------------------------------- */
// File       : NPC_Manager.h
// Project    : Long Forgotten Earth
/* ------------------------------------------------------------------------- */

#ifndef _NPC_MANAGER_H_
#define _NPC_MANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
//#include "OgreConfigFile.h"
#include "OgreSingleton.h"	

// Les differents types de NPC
#include "body.h"
#include "robot.h"


using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gere tous les personnages autres que le joueur (NPCs). C'est un Singleton.
/* ------------------------------------------------------------------------- */
class NPC_Manager  : public Singleton <NPC_Manager>	
{
public:
	NPC_Manager(SceneManager* SceneMgr);
	virtual      ~NPC_Manager();
	SceneNode*   getNode(void);
	Body*        addBody(String name, String descfilename, bool player=false);
	Body*        getBody(String name);
	Body*        getPlayerBody();
	void         removeBody(String name);
	Robot*       addRobot(String name, String descfilename);
	Robot*       getRobot(String name);

	// Pour le singleton
	static NPC_Manager& getSingleton(void);
	static NPC_Manager* getSingletonPtr(void);

protected:

	typedef struct NPC		/// Liste des NPC actifs
	{
		Robot*      robot;
		Body*       body;
		String      name;
		NPC*        next;
		// valeurs initiales via un constructeur spécifique appelé par la fonction new
		NPC()
		{
			this->robot=NULL;
			this->body=NULL;
			this->name="";
			this->next=NULL;
		}
	} ;

	NPC*       mFirstNPC;	   /// Debut de la liste des NPC
	NPC*       mLastNPC;	   /// Fin de la liste des NPC
	
	SceneManager* mSceneMgr;
	SceneNode*    mNPCmgrNode;
};

#endif
