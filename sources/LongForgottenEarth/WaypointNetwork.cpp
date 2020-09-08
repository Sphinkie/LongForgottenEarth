/* ------------------------------------------------------------------------- */
// File    : WaypointNetwork.cpp
// Project : Long Forgotten Earth
// Author  : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "WaypointNetwork.h"
#include "line3d.h"
#include "XMLparser_T.h"
#include "macros.h"

/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
WaypointNetwork::WaypointNetwork(SceneManager* SceneMgr, SceneNode* ParentNode, String FilePrefix)
{
	int FileIndex = 1;

	mLineList = NULL;
	mLastLine = NULL;

	mSceneMgr    = SceneMgr;
	mNetworkNode = ParentNode->createChildSceneNode();

	while (addWPline("../media/waypoints/"+FilePrefix+ITOA(FileIndex)+".xml")) 
	{
		FileIndex++;
	}
		
}

/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
WaypointNetwork::~WaypointNetwork()
{
	/// TODO : detruire tous les items créés
	Waypoint* p_Waypoint;
	Ligne*    p_Ligne = mLineList;

	// On parcourt toutes les lignes
	while (p_Ligne) 
	{
		p_Waypoint = p_Ligne->FirstWaypoint;
	}
}


/* ------------------------------------------------------------------------- */
/// Ajoute une ligne de WP à partir d'un fichier XML
/* ------------------------------------------------------------------------- */
bool WaypointNetwork::addWPline(String WPfilename)
{
	String     WPitem;
	SceneNode* WPnode = NULL;
	Waypoint*  PreviousWaypoint = NULL;
	int        index = 0;

	// On ouvre le fichier XML 
	XMLParser_T* WPfile = new XMLParser_T(WPfilename);
	if (!WPfile->isPresent()) return false;
		
	// On crée une nouvelle ligne
	Ligne* ThisLine = new Ligne;
	ThisLine->NextLine= NULL;

	// On parcourt le fichier XML

	// On analyse les Elements <line> du fichier XML
    WPfile->gotoRootElement("line");
	ThisLine->LigneID = WPfile->getIntAttribute("number");
	ThisLine->LevelNb = WPfile->getIntAttribute("level");

	// On analyse les Elements <waypoint> du fichier XML
    while (WPfile->gotoSubElement("waypoint",index))
    {
		// On parse la ligne <waypoint>, pour récupérer le champ Texte
		String WPname = WPfile->getStringAttribute("id");
		String Action = WPfile->getStringAttribute("action");
	
		// On controle si ce WP existe déjà ou pas
		WPnode = getWaypointNode(WPname); 

		// Si ce WP existe, on recupère son Node
		// Si le node n'existe pas, on le crée:
		if (!WPnode)
		{
			WPnode = mNetworkNode->createChildSceneNode(WPname);
			WPnode->setPosition(WPfile->getRealAttribute("x"),WPfile->getRealAttribute("y"),WPfile->getRealAttribute("z"));
		}
		// On cree un nouveau Waypoint dans la ligne
		Waypoint* ThisWP     = new Waypoint;
		ThisWP->WaypointNode = WPnode;
		ThisWP->Action       = Action;
		ThisWP->Next         = NULL;
		ThisWP->Previous     = PreviousWaypoint;
		// On conserve le pointeur sur la premiere station de la ligne
		if (!PreviousWaypoint) ThisLine->FirstWaypoint = ThisWP;
		// On ajuste la station précédente
		if (PreviousWaypoint) PreviousWaypoint->Next = ThisWP;
		// On passe au suivant
		PreviousWaypoint = ThisWP;
		index++;
    }

	// On mémorise la ligne 

	// Si c'est la première ligne: on mémorise la tete de ligne
	if (!mLineList)		
	{
		// On met à jour le pointeur vers la premiere ligne
		mLineList = ThisLine;	
		// On met à jour le pointeur vers la dernière ligne
		mLastLine = ThisLine;
	}
	else
	{	
		// Sinon, il y a des lignes précédentes, et on s'y rattache
		mLastLine->NextLine = ThisLine;
		// On met à jour le pointeur vers la dernière ligne
		mLastLine = ThisLine;
	}

	delete(WPfile);
	return true;
}



/* ------------------------------------------------------------------------- */
/// Affiche en 3D toutes les lignes de stations existantes (debug)
/* ------------------------------------------------------------------------- */
void WaypointNetwork::showLines()
{
	Ligne*    p_Ligne;
	Waypoint* p_Waypoint;

	TRACE(">> showLines()");

	p_Ligne   = mLineList;

	// On parcourt toutes les lignes
	while (p_Ligne) 
	{

		DEBUG_LOG ("showing line "+ITOA(p_Ligne->LigneID));
		p_Waypoint = p_Ligne->FirstWaypoint;
		
		// On dessine la premiere station
		this->createWaypointEntity(p_Waypoint->WaypointNode->getName(), p_Waypoint->WaypointNode);

		while (p_Waypoint->Next) 
		{
			// On passe au WP suivant
			p_Waypoint=p_Waypoint->Next;

			// On trace une ligne entre le WP et le précédent
			Line3D *my3Dline = new Line3D(p_Waypoint->Previous->WaypointNode->getPosition(), p_Waypoint->WaypointNode->getPosition());
			my3Dline->setColor(ColourValue(1,0,0));
			mNetworkNode->attachObject(my3Dline);

			// On visualise un cube à l'endroit du Waypoint
			this->createWaypointEntity(p_Waypoint->WaypointNode->getName(), p_Waypoint->WaypointNode);
		}

		// On passe à la ligne suivante
		p_Ligne=p_Ligne->NextLine;
	}
}



/* ------------------------------------------------------------------------- */
/// Détermination du meilleur itinéraire
/* ------------------------------------------------------------------------- */
SceneNode* WaypointNetwork::findNextWaypointTo(SceneNode* FromNode,SceneNode* ToNode)
{
	/// On renvoie la prochaine étape sur le chemin vers la destination.
	SceneNode* WP = NULL;

	// Traitement des cas particuliers
	if (FromNode == ToNode) return NULL;

	// On regarde s'il existe un chemin direct (To et From sont sur la même ligne)
	if (!WP)
	{
		WP = hasLineWith(FromNode,ToNode);
		if (WP) DEBUG_LOG("NextWP "+WP->getName()+" found with a direct path");
	}

	// On regarde s'il existe un chemin indirect (To et From sont sur deux lignes concourantes)
	if (!WP)
	{
		WP = existCrossingLinesWith(FromNode,ToNode);
		if (WP) DEBUG_LOG("NextWP "+WP->getName()+" found with a one-change path");
	}

	// On regarde s'il existe un chemin complexe (avec 2 "changements")
	if (!WP)
	{
		WP = findWayWith2changes(FromNode,ToNode);
		if (WP) DEBUG_LOG("NextWP "+WP->getName()+" found with a two-change path");
	}

	return WP;
}





/* ------------------------------------------------------------------------- */
/// Renvoie le Node du Waypoint le plus proche de la position donnée
/* ------------------------------------------------------------------------- */
SceneNode* WaypointNetwork::getClosestWaypointNode(Vector3 Pos)
{
	Ligne*      p_Ligne;
	Waypoint*   p_Waypoint;
	Real        DistanceMin = -1;
	SceneNode*  ClosestWp = NULL;

	p_Ligne    = mLineList;

	// On parcourt toutes les lignes
	while (p_Ligne) 
	{
		p_Waypoint = p_Ligne->FirstWaypoint;
		do 
		{
			Vector3 WaypointPos = p_Waypoint->WaypointNode->getPosition();
			Real Distance = (WaypointPos-Pos).squaredLength();
			if ( (DistanceMin==-1) || (Distance<DistanceMin))
			{
				DistanceMin = Distance;
				ClosestWp   = p_Waypoint->WaypointNode;
			}
			p_Waypoint = p_Waypoint->Next;	
		}
		while (p_Waypoint);
		// On passe à la ligne suivante
		p_Ligne=p_Ligne->NextLine;
	}

	DEBUG_LOG("Closest waypoint is " + ClosestWp->getName());
	return ClosestWp;

}



/* ------------------------------------------------------------------------- */
/// Renvoie l'Action du Waypoint le plus proche de la position donnée
/* ------------------------------------------------------------------------- */
String WaypointNetwork::getClosestWaypointAction(Vector3 Pos)
{
	Ligne*      p_Ligne;
	Waypoint*   p_Waypoint;
	Real        DistanceMin = -1;
	Waypoint*   ClosestWp = NULL;

	p_Ligne    = mLineList;

	// On parcourt toutes les lignes
	while (p_Ligne) 
	{
		p_Waypoint = p_Ligne->FirstWaypoint;
		do 
		{
			Vector3 WaypointPos = p_Waypoint->WaypointNode->getPosition();
			Real Distance = (WaypointPos-Pos).squaredLength();
			if ( (DistanceMin==-1) || (Distance<DistanceMin))
			{
				DistanceMin = Distance;
				ClosestWp   = p_Waypoint;
			}
			p_Waypoint = p_Waypoint->Next;	
		}
		while (p_Waypoint);
		// On passe à la ligne suivante
		p_Ligne=p_Ligne->NextLine;
	}

	DEBUG_LOG("Closest waypoint Action is " + ClosestWp->Action);
	return ClosestWp->Action;
}


/* ------------------------------------------------------------------------- */
/// Vérifie si il y a une ligne avec les deux Node demandé, et renvoie le prochain dans cette direction.
/* ------------------------------------------------------------------------- */
SceneNode* WaypointNetwork::hasLineWith(SceneNode* FromNode, SceneNode* ToNode)
{
	Ligne*      p_Ligne;
	Waypoint*   p_Waypoint;
	bool        FromNodeFound = false;
	bool        ToNodeFound   = false;
	SceneNode*  NextWPNode;

	p_Ligne    = mLineList;

	while (p_Ligne) 
	{
		// On commence par le premier Waypoint de la ligne
		p_Waypoint = p_Ligne->FirstWaypoint;
		do
		{
			if (p_Waypoint->WaypointNode==FromNode)
			{
				// On a trouvé un des points (FromNode)
				FromNodeFound = true;
				if (ToNodeFound) 
				{
					// si on a deja trouvé la Station de Destination, il faut aller vers le Previous
					NextWPNode = p_Waypoint->Previous->WaypointNode;
					DEBUG_LOG("Direct path from "+FromNode->getName()+" to "+ToNode->getName()+" found: next step is "+ NextWPNode->getName());
					return NextWPNode;
				}
				else 
				{
					// Sinon il faut aller vers le Next (s'il existe)
					if (p_Waypoint->Next) NextWPNode= p_Waypoint->Next->WaypointNode;
				}
			}
			if (p_Waypoint->WaypointNode==ToNode)
			{
				// On a trouvé un des points (ToNode)
				ToNodeFound = true;
				if (FromNodeFound) 
				{
					DEBUG_LOG("Direct path from "+FromNode->getName()+" to "+ToNode->getName()+" found: next step is "+ NextWPNode->getName());
					return NextWPNode;
				}
			}
			p_Waypoint = p_Waypoint->Next;
		}
		while (p_Waypoint);
		// Si on n'a pas trouvé, on essaye avec une autre ligne
		ToNodeFound = false;
		FromNodeFound = false;
		p_Ligne=p_Ligne->NextLine;
	}

	// On n'a pas trouvé des deux Nodes
	DEBUG_LOG("Direct path from "+FromNode->getName()+" to "+ToNode->getName()+" not found");
	return NULL;
}


/* ------------------------------------------------------------------------- */
/// Vérifie si il existe deux lignes concurrentes, l'une contenant le FromNode, et l'autre le ToNode. Renvoie le prochain Node dans la direction de l'intersection.
/* ------------------------------------------------------------------------- */
SceneNode* WaypointNetwork::existCrossingLinesWith(SceneNode* FromNode, SceneNode* ToNode)
{
	Ligne*      p_Ligne;
	Waypoint*   p_Waypoint;
	SceneNode*  WPtemp = NULL;

	DEBUG_LOG("Searching two crossing lines with "+FromNode->getName()+" and "+ToNode->getName());

	// On parcourt toutes les lignes
	p_Ligne = mLineList;
	while (p_Ligne) 
	{
		// On trouve les lignes auxquelles appartient FromNode
		// Si la ligne ne contient pas le FromNode, on passe à la suivante
		if (!containLineWaypoint(p_Ligne,FromNode)) {p_Ligne=p_Ligne->NextLine; continue;};

		// On commence par le premier Waypoint de la ligne
		p_Waypoint = p_Ligne->FirstWaypoint;
		do 
		{
			// On parcourt tous les Waypoint de la ligne
			// Y-a-t-il une correspondance pour ToNode ?
			WPtemp = hasLineWith(p_Waypoint->WaypointNode, ToNode);

			// Si OUI, on a trouvé un chemin.
			// il faut aller vers ce WP de correspondance
			// inutile de continuer à scanner  les autres lignes
			if (WPtemp) return hasLineWith(FromNode, p_Waypoint->WaypointNode);
			// Si NON, on passe au WP suivant.
			p_Waypoint = p_Waypoint->Next;	
		}
		while (p_Waypoint);

		// Si on n'a pas trouvé, on essaye avec une autre ligne
		p_Ligne=p_Ligne->NextLine;
	}

	// Si on n'a pas trouvé, on renvoie NULL
	return NULL;

}


/* ------------------------------------------------------------------------- */
/// Vérifie s'il existe trois lignes concurrentes, l'une contenant le FromNode, et l'autre le ToNode. Renvoie le prochain Node dans la direction de l'intersection.
/* ------------------------------------------------------------------------- */
SceneNode* WaypointNetwork::findWayWith2changes(SceneNode* FromNode, SceneNode* ToNode)
{
	Ligne*      p_Ligne;
	Waypoint*   p_Waypoint;
	SceneNode*  WPtemp = NULL;

	DEBUG_LOG("Searching a way from "+FromNode->getName()+" to "+ToNode->getName() + " with three lines.");

	// On parcourt toutes les lignes
	p_Ligne = mLineList;
	while (p_Ligne) 
	{
		// On trouve les lignes auxquelles appartient FromNode.
		// Si la ligne ne contient pas le FromNode, on passe à la suivante
		if (!containLineWaypoint(p_Ligne,FromNode)) {p_Ligne=p_Ligne->NextLine; continue;};

		// Quand on a une ligne avec le FromNode, on la parcourt.
		// On commence par le premier Waypoint de la ligne
		p_Waypoint = p_Ligne->FirstWaypoint;
		do 
		{
			// On parcourt tous les Waypoint de la ligne
			// Y-a-t-il un chemin indirect pour ToNode ?
			WPtemp = existCrossingLinesWith(p_Waypoint->WaypointNode, ToNode);

			// Si OUI, on a trouvé un chemin.
			// Il faut aller vers ce WP de correspondance
			// Inutile de continuer à scanner  les autres lignes
			if (WPtemp) return hasLineWith(FromNode, p_Waypoint->WaypointNode);
			// Si NON, on passe au WP suivant.
			p_Waypoint = p_Waypoint->Next;	
		}
		while (p_Waypoint);

		// Si on n'a pas trouvé, on essaye avec une autre ligne
		p_Ligne=p_Ligne->NextLine;
	}

	// Si on n'a pas trouvé, on renvoie NULL
	return NULL;

}

/* ------------------------------------------------------------------------- */
/// Renvoie le Node du Waypoint correspondant au nom demandé.
/* ------------------------------------------------------------------------- */
SceneNode* WaypointNetwork::getWaypointNode(String Name)
{
	// We will iterate all the children of the parent node
	SceneNode::ChildNodeIterator WPIterator = mNetworkNode->getChildIterator(); 
	
	// Iterate all WP nodes under the parent node
	while (WPIterator.hasMoreElements()) 
	{ 
		// Get the next node 
		SceneNode* WPNode = (SceneNode*)WPIterator.getNext();
		if (WPNode->getName()==Name) return WPNode;
	}
	// On est arrivé au bout de tous les childs, sans en trouver un avec le nom demandé.
	return NULL;
}




/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le Waypoint appartient à la ligne
/* ------------------------------------------------------------------------- */
bool WaypointNetwork::containLineWaypoint(Ligne* line, SceneNode* node)
{
	Waypoint* p_Waypoint = line->FirstWaypoint;
	while (p_Waypoint)
	{
		if (p_Waypoint->WaypointNode == node) return true;
		p_Waypoint = p_Waypoint->Next;	
	}
	// Si on n'a pas trouvé, on renvoie false
	return false;
}


/* ------------------------------------------------------------------------- */
/// Fonction pour creer une Entity (un petit cube) à l'emplacement d'un waypoint (debug)
/* ------------------------------------------------------------------------- */
void WaypointNetwork::createWaypointEntity(String WPname, SceneNode* WPnode)
{
	DEBUG_LOG(">> createWaypointEntity("+WPname+")");

	Entity* wpEntity;

	try
	{
		// On verifie si cette entity n'existe pas déjà
		wpEntity = mSceneMgr->getEntity(WPname);
		// Si elle existe déjà, on ne fait rien
	}
	catch (Ogre::Exception) 
	{
		// S'il y a une exception, c'est que cette Entity n'existe pas encore
		// Donc on peut la créer
		wpEntity = mSceneMgr->createEntity(WPname, "wpbox.mesh");
		WPnode->attachObject(wpEntity);
	}
}
