/* ------------------------------------------------------------------------- */
// File       : Body.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Body.h"
#include "NPC_Manager.h"
#include "mainApplication.h"
#include "specialFX.h"
#include "classes.h"
#include "macros.h"
#include "XMLparser_T.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
Body::Body(String name, String descfilename)
{
	TRACE(">> "+name+"::Body ("+descfilename+")");

	mName = name;
	// Par défaut, son parent est le NPC_manager
	mParentNode = NPC_Manager::getSingletonPtr()->getNode();
	mNode       = mParentNode->createChildSceneNode(mName);
	mPlayer     = false;
	mCollideObject= NULL;
	
	/// Lecture des parametres du fichier de configuration
	XMLParser_T* DescriptionFile = new XMLParser_T(descfilename);

	DescriptionFile->gotoRootElement("body");
	String MeshFile = DescriptionFile->getStringAttribute("mesh");
	
	DescriptionFile->gotoElement("body","status");
	String P ;

	P = DescriptionFile->getStringAttribute("bones_ok");
	mVitaBones = this->getVitaParameter(P);

	P = DescriptionFile->getStringAttribute("radia_ok");
	mVitaRadiated = this->getVitaParameter(P);

	P = DescriptionFile->getStringAttribute("necro_ok");
	mVitaNecrosed = this->getVitaParameter(P);

	P = DescriptionFile->getStringAttribute("wound_ok");
	mVitaWounded = this->getVitaParameter(P);

	mLastChance = DescriptionFile->getIntAttribute("lastchance");
	delete (DescriptionFile);

	// Creation de l'entity du Body
	mEntity = mainApplication::getSingletonPtr()->getSceneMgr()->createEntity(name, MeshFile);
	mEntity->setUserObject(this);
	mEntity->setQueryFlags(PNJ);
	mIsOnScene = false;

	// On gère ses collisions.
	this->initCollisionShape();

	DEBUG_LOG("   Entity created: "+mName);
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Body::~Body()
{
}

/* ------------------------------------------------------------------------- */
// Surchage de la fonction d'identification de la classe UserDefinedObject
/* ------------------------------------------------------------------------- */
long Body::getTypeID (void) const
{
	return BODY_ID;
}

/* ------------------------------------------------------------------------- */
/// Fait apparaitre le body, et le rattache eventuellement à un nouveau node parent.
/* ------------------------------------------------------------------------- */
void Body::addToScene(SceneNode* ParentNode)
{

	if (!ParentNode) 
	{
		// Si on passe NULL, on le rattache au parent existant
		mParentNode->addChild(mNode);
	}
	else
	{
		// On a passé un parent en parametre
		if (ParentNode != mParentNode)
		{
			// Si le nouveau parent est différent de l'ancien, 
			// On rattache le body au nouveau parent
			mParentNode->removeChild(mNode);
			ParentNode->addChild(mNode);
		}
		// On mémorise ce nouveau parent
		mParentNode = ParentNode;
	}
	// On verifie que l'entité n'est pas déjà attachée
	if (!mEntity->isAttached())
		mNode->attachObject(mEntity);

	// Ajout de cet objet dans le contexte
	using namespace OgreOpcode;
//	CollisionObject* collideObject = static_cast<CollisionObject*>(mCollideObject);
	if (!mIsOnScene)
		CollisionManager::getSingletonPtr()->GetDefaultContext()->AddObject(mCollideObject);
	mIsOnScene = true;
}

/* ------------------------------------------------------------------------- */
/// Change la position du Body par rapport à son node parent
/* ------------------------------------------------------------------------- */
void Body::setPosition(Vector3 Position)
{
	mNode->setPosition(Position);
}

/* ------------------------------------------------------------------------- */
/// On retire ce body de la scene, en effacçant son Entité, et en ne gérant plus ses collisions.
/* ------------------------------------------------------------------------- */
void Body::removeFromScene()
{
	// On détache l'entity
	if (mEntity->isAttached())
		mNode->detachObject(mEntity);
	// Retrait de cet objet du contexte
	using namespace OgreOpcode;
//	CollisionObject* collideObject = static_cast<CollisionObject*>(mCollideObject);
	if (mIsOnScene)
		CollisionManager::getSingletonPtr()->GetDefaultContext()->RemoveObject(mCollideObject);
	mIsOnScene = false;

}

/* ------------------------------------------------------------------------- */
/// Renvoie le nom du body.
/* ------------------------------------------------------------------------- */
String Body::getName()
{
	return mName;
}


/* ------------------------------------------------------------------------- */
/// Définit si ce body est celui du joueur.
/* ------------------------------------------------------------------------- */
void Body::setPlayerBody(bool player)
{
	mPlayer = player;
}

/* ------------------------------------------------------------------------- */
/// Renvoie VRAI si ce body est celui du joueur.
/* ------------------------------------------------------------------------- */
bool Body::isPlayerBody()
{
	return (mPlayer);
}

/* ------------------------------------------------------------------------- */
/// Renvoie le status du body (sous forme de texte)
/* ------------------------------------------------------------------------- */
String Body::getStatus()
{
	short Vitalite = getVitalite();

	if      (Vitalite == 0)	 return "Dead";        
	else if (Vitalite < 10)  return "Critical";    
	else if (Vitalite < 50)  return "Wounded";     
	else if (Vitalite < 70)  return "Weak";
	else                     return "good shape";  
}


/* ------------------------------------------------------------------------- */
/// Renvoie le status du body (sous forme de valeur numérique 0..100)
/* ------------------------------------------------------------------------- */
short Body::getVitalite()
{
	// On fait un diagnostic général
	short Vitalite;

	// Nombre de membres OK au niveau ossature:
	Vitalite  = getNbMembersOk(mVitaBones);
	// Nombre de membres OK au niveau nécrose:
	Vitalite *= getNbMembersOk(mVitaNecrosed);
	// Les damages suivants sont 2x moins grave
	// Nombre de membres OK au niveau irradiation:
	Vitalite *= getNbMembersOk(mVitaRadiated)*2;
	// Nombre de membres OK au niveau blessures:
	Vitalite *= getNbMembersOk(mVitaWounded)*2;
	Vitalite += mLastChance;

	return (Vitalite);	// entre 0 et 96
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'état du squelette du body (sous forme de valeur numérique 0..64)
/* ------------------------------------------------------------------------- */
short Body::getBoneStatus()
{
	return mVitaBones;
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'état d'irradiation du body (sous forme de valeur numérique 0..64)
/* ------------------------------------------------------------------------- */
short Body::getRadiatedStatus()
{
	return mVitaRadiated;
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'état de nécrose du body (sous forme de valeur numérique 0..64)
/* ------------------------------------------------------------------------- */
short Body::getNecrosedStatus()
{
	return mVitaNecrosed;
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'état général de la tête (sous forme de valeur numérique 0..4)
/* ------------------------------------------------------------------------- */
short Body::getHeadStatus()
{
	short HeadStatus;
	HeadStatus = (mVitaBones    & HEAD);
	HeadStatus +=(mVitaRadiated & HEAD);
	HeadStatus +=(mVitaNecrosed & HEAD);
	HeadStatus +=(mVitaWounded  & HEAD);
	
	return (HeadStatus/HEAD);
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'état général du torse (sous forme de valeur numérique 0..4)
/* ------------------------------------------------------------------------- */
short Body::getTorsoStatus()
{
	short TorsoStatus;
	TorsoStatus = (mVitaBones    & TORSO);
	TorsoStatus +=(mVitaRadiated & TORSO);
	TorsoStatus +=(mVitaNecrosed & TORSO);
	TorsoStatus +=(mVitaWounded  & TORSO);
	
	return (TorsoStatus/TORSO);
}

/* ------------------------------------------------------------------------- */
/// Renvoie le nombre de membre en bon état (0..6)
/* ------------------------------------------------------------------------- */
short Body::getNbMembersOk(short bodyparts)
{
	short diag =0;
	if (bodyparts & R_LEG) diag++;
	if (bodyparts & L_LEG) diag++;
	if (bodyparts & R_ARM) diag++;
	if (bodyparts & L_ARM) diag++;
	if (bodyparts & TORSO) diag++;
	if (bodyparts & HEAD)  diag++;
	return diag;
}


/* ------------------------------------------------------------------------- */
/// 
/* ------------------------------------------------------------------------- */
int Body::getVitaParameter(String P)
{
	int Vitality = 0;
	// Cette fonction permet de gérer des fichier de config dans un format XML simplifié (1 seul niveau)
	// On enleve le premier et le dernier caractère (qui sont des "). 	

	StringVector Vvalue= StringUtil::split(P,"+\"");

	// A propos des iterateurs STL =
	// www.msoe.edu/eecs/ce/courseinfo/stl/iterator.htm

    StringVector::iterator FirstElem, LastElem, CurrElem;
	FirstElem = Vvalue.begin();
	LastElem  = Vvalue.end();
	CurrElem  = FirstElem;

	// On transforme la liste des membres malades en une valeur numérique
	while (CurrElem!=LastElem)
	{
		//DEBUG_LOG(*CurrElem);
		if      (*CurrElem == "R_LEG")   Vitality  += R_LEG;
		else if (*CurrElem == "L_LEG")   Vitality  += L_LEG;
		else if (*CurrElem == "R_ARM")   Vitality  += R_ARM;
		else if (*CurrElem == "L_ARM")   Vitality  += L_ARM;
		else if (*CurrElem == "HEAD")    Vitality  += HEAD;
		else if (*CurrElem == "TORSO")   Vitality  += TORSO;
		else if (*CurrElem == "ALL_OK")  Vitality   = ALL_OK;
		else if (*CurrElem == "ALL_BAD") Vitality   = DEAD;
		else if (*CurrElem == "DEAD")    Vitality   = DEAD;
		else     Vitality = StringConverter::parseInt(*CurrElem);
		CurrElem++;
	}
	return Vitality;
}

/* ------------------------------------------------------------------------- */
/// Initialise les collisions du NPC.
/* ------------------------------------------------------------------------- */
void Body::initCollisionShape()
{
	using namespace OgreOpcode;

	// On recupère un pointeur sur le contexte
	CollisionContext* collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();

	// Creation de la forme englobante
	CollisionShape* collideShape = CollisionManager::getSingletonPtr()->NewShape(mName.c_str());
	// Chargement du mesh associé à cette forme
	collideShape->Load(mEntity);
	// Creation de l'objet
	mCollideObject = collideContext->NewObject();
//	CollisionObject* collideObject  = static_cast<CollisionObject*>(mCollideObject);
	mCollideObject->SetCollClass("npc");
	mCollideObject->SetShape(collideShape);

	TRACE("Collision System initialized for "+mName);
}

/* ------------------------------------------------------------------------- */
/// Gere les collisions du NPC.
/* ------------------------------------------------------------------------- */
void Body::getCollision()
{
	using namespace OgreOpcode;
	
	CollisionPair** collision_report;

	if (!mCollideObject) 
	{
		DEBUG_LOG("mCollideObject is NULL for"+mName);
		return;
	}

	int nb_collision = ((CollisionObject*)mCollideObject)->GetCollisions(collision_report);

	DEBUG_LOG(ITOA(nb_collision)+" collision found");
	if (nb_collision>0)
	{
		for (int i=0; i<nb_collision; i++)
		{
			CollisionObject* object1 = collision_report[i]->co1;
			CollisionObject* object2 = collision_report[i]->co2;
			Vector3          contact = collision_report[i]->contact;
			DEBUG_LOG("collision "+object1->GetShape()->getEntity()->getName());
			DEBUG_LOG("     with "+object2->GetShape()->getEntity()->getName());
			DEBUG_LOG("  contact "+ITOA(contact));
		}
	}
	// Liste les objets dans le contexte
//	CollisionContext* collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();
//	collideContext->Visualize();
}
