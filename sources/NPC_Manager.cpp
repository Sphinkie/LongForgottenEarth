/* ------------------------------------------------------------------------- */
// File       : NPC_Manager.cpp
// Project    : Long Forgotten Earth
// Description: Gere tous les NPC
/* ------------------------------------------------------------------------- */


#include "NPC_Manager.h"


/* ------------------------------------------------------------------------- */
// Constructeur
/* ------------------------------------------------------------------------- */
NPC_Manager::NPC_Manager(SceneManager* SceneMgr)
{
	mSceneMgr     = SceneMgr;
	mNPCmgrNode   = SceneMgr->getRootSceneNode()->createChildSceneNode();
	mFirstNPC    = NULL;
	mLastNPC     = NULL;
//	mVoidNPC     = {NULL,NULL,NULL,NULL};

}


/* ------------------------------------------------------------------------- */
// Destructeur
/* ------------------------------------------------------------------------- */
NPC_Manager::~NPC_Manager()
{
	// TOD: detruire les NPC attachés au NPC manager
}

/* ------------------------------------------------------------------------- */
/// Override the Singleton Class method
/* ------------------------------------------------------------------------- */
template<> NPC_Manager* Singleton<NPC_Manager>::ms_Singleton = 0;

NPC_Manager* NPC_Manager::getSingletonPtr(void)
{
	return ms_Singleton;
}


/* ------------------------------------------------------------------------- */
/// Override the Singleton Class method
/* ------------------------------------------------------------------------- */
NPC_Manager& NPC_Manager::getSingleton(void)
{
	assert (ms_Singleton);
	return (*ms_Singleton);
}

/* ------------------------------------------------------------------------- */
///   Renvoie le Node du gestionnaire de NPC (utile ?)
/* ------------------------------------------------------------------------- */
SceneNode* NPC_Manager::getNode(void)
{
	return mNPCmgrNode;
}


/* ------------------------------------------------------------------------- */
/// Crée un Body, et le mémorise dans la liste des NPC
/* ------------------------------------------------------------------------- */
Body* NPC_Manager::addBody(String name, String descfilename, bool player)
{
	NPC*    newNPC   = new NPC; 
	Body*   newBody  = new Body(name, descfilename);
	newBody->setPlayerBody(player);

	// On complete le record
	newNPC->body  = newBody;
	newNPC->name  = name;

	// Si c'est le premier NPC ajouté à la liste
	if (!mFirstNPC) 
	{
		// On met à jour le pointeur vers le premier body
		mFirstNPC = newNPC;
		// On met à jour le pointeur vers le dernier body
		mLastNPC  = newNPC;
	}
	else 
	{
		// Sinon, on ajoute le nouveau record à la fin de la liste
		mLastNPC->next = newNPC;
		// On met à jour le pointeur vers le dernier body
		mLastNPC       = newNPC;
	}

	return newBody;
}

/* ------------------------------------------------------------------------- */
/// Crée un Robot, et le mémorise dans la liste des NPC
/* ------------------------------------------------------------------------- */
Robot* NPC_Manager::addRobot(String name, String descfilename)
{

	Robot* newRobot = new Robot(name, descfilename, mNPCmgrNode, mSceneMgr);

	NPC*    newNPC   = new NPC; 

	// On complete le record
	newNPC->robot = newRobot;
	newNPC->name  = name;

	// Si c'est le premier NPC ajouté à la liste
	if (!mFirstNPC) 
	{
		// On met à jour le pointeur vers le premier body
		mFirstNPC = newNPC;
		// On met à jour le pointeur vers le dernier body
		mLastNPC  = newNPC;
	}
	else 
	{
		// Sinon, on ajoute le nouveau record à la fin de la liste
		mLastNPC->next = newNPC;
		// On met à jour le pointeur vers le dernier body
		mLastNPC       = newNPC;
	}
	return newRobot;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le pointeur sur le Body demandé
/* ------------------------------------------------------------------------- */
Body* NPC_Manager::getBody(String name)
{
	NPC*    p_npc  = mFirstNPC;
	Body*   found  = NULL;

	if (name=="") return NULL;

	while (p_npc)
	{
		// On parcourt le liste des NPC
		if (p_npc->name==name) 
		{
			// On a trouvé celui avec le nom demandé
			found=p_npc->body;
			// on renvoie le body demandé.
			return found;		
		}
		// sinon on passe au suivant
		p_npc = p_npc->next;
	}
	return NULL;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le pointeur sur le Body du joueur
/* ------------------------------------------------------------------------- */
Body* NPC_Manager::getPlayerBody()
{
	NPC*    p_npc  = mFirstNPC;
	Body*   found  = NULL;

	while (p_npc)
	{
		// On parcourt le liste des NPC
		if (p_npc->body) 
		{
			// Si c'est un body
			if (p_npc->body->isPlayerBody()) 
			{
				// Et si c'est le body deu Joueur
				found=p_npc->body;
				// on renvoie le body demandé.
				return found;		
			}
		}
		// sinon on passe au suivant
		p_npc = p_npc->next;
	}
	return NULL;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le pointeur sur le Robot demandé
/* ------------------------------------------------------------------------- */
Robot* NPC_Manager::getRobot(String name)
{
	NPC*    p_npc  = mFirstNPC;
	Robot*  found  = NULL;

	if (name=="") return NULL;

	while (p_npc)
	{
		// On parcourt le liste des NPC
		if (p_npc->name==name) 
		{
			// On a trouvé celui avec le nom demandé
			found=p_npc->robot;
			// on renvoie le Robot demandé.
			return found;		
		}
		// sinon on passe au suivant
		p_npc = p_npc->next;
	}
	return NULL;
}


/* ------------------------------------------------------------------------- */
/// Détruit le Body demandé
/* ------------------------------------------------------------------------- */
void NPC_Manager::removeBody(String name)
{
	// TODO: suppression de l'élément de la liste et libération de la mémoire
}


