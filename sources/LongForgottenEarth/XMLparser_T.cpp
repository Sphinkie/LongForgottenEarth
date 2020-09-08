/* ------------------------------------------------------------------------- */
// File       : XMLParser_T.cpp
// Project    : Long Forgotten Earth
// Auteur     : David de Lorenzo
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
// Utilisation de la librairie TinyXML
/* ------------------------------------------------------------------------- */
#include "lib_tinyxml\tinyxml.h"

#ifdef TIXML_USE_STL
	#include <iostream>
	#include <sstream>
	using namespace std;
#else
	#include <stdio.h>
#endif


#if defined( WIN32 ) && defined( TUNE )
	#include <windows.h>
	// Apologies to non-windows users! But I need some good timers for
	// profiling, and these are very platform specific.
	__int64 start;
	__int64 end;
	__int64 freq;
#endif

/* ------------------------------------------------------------------------- */

#include "macros.h"
#include "XMLParser_T.h"


#undef  DEBUG_LOG
#define DEBUG_LOG(X)  // TRACE(X)

	
/* ------------------------------------------------------------------------- */
/// Constructor
/* ------------------------------------------------------------------------- */
XMLParser_T::XMLParser_T(String XMLfile)
{
	mDocument=NULL;
	this->parseFile(XMLfile);
}


/* ------------------------------------------------------------------------- */
/// Destructor
/* ------------------------------------------------------------------------- */
XMLParser_T::~XMLParser_T()
{
	delete (mDocument);
}


/* ------------------------------------------------------------------------- */
/// Parse le fichier XML
/** 
	@param XMLfile Le nom du fichier XML � parser, avec son chemin.
*/
/* ------------------------------------------------------------------------- */
void XMLParser_T::parseFile(String XMLfile)
{

	TiXmlDocument* Document = new TiXmlDocument(XMLfile);
	bool loadOkay = Document->LoadFile();

	if ( loadOkay )
	{
		mParentNode  = NULL;
		mCurrentNode = NULL;
		// On m�morise le document
		mDocument = Document;
	}
	else
	{
		TRACE( "XMLParser_T::parseFile: Could not load XML file "+XMLfile );
		TRACE( "Error description: " + String(Document->ErrorDesc()) );
		mDocument = NULL;
	}
}

/* ------------------------------------------------------------------------- */
/// Indique si un document a �t� pars�
/* ------------------------------------------------------------------------- */
bool XMLParser_T::isPresent()
{
	/// On renvoie FALSE si mDocument est NULL;
	return (mDocument != NULL);
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur un �l�ment \<RootName\> (de premier niveau = racine).
/** 
@param RootName Nom du Node XML qui va devenir le node courant.
@return FALSE si l'element n'a pas �t� trouv�.
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoRootElement(String RootName)
{

	bool       found;
	TiXmlNode* node = NULL;

	// Controle de securit�
	if (!mDocument) 
	{
		TRACE("Warning: gotoRootElement("+RootName+"): mDocument is NULL");
		return false;
	}

	node = static_cast<TiXmlDocument*>(mDocument)->FirstChildElement(RootName);

	if (node) 
	{
		mParentNode  = node;
		mCurrentNode = node;
		found = true;
	}
	else
	{
		TRACE("Warning: gotoElement("+RootName+"): Node not found");
		found = false;
	}

	return (found);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur un �l�ment \<NodeName\> (de second niveau).
/**
	@param RootName Nom du Racine que l'on va commencer � parcourir.
	@param NodeName Nom du Node XML qui va devenir le node courant.
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoElement(String RootName, String NodeName)
{

	bool       found;
	TiXmlNode* node = NULL;
//	TiXmlHandle* node = NULL;

	// Controle de securit�
	if (!mDocument) 
	{
		TRACE("Warning: gotoElement("+RootName+"): mDocument is NULL");
		return false;
	}

	node = static_cast<TiXmlDocument*>(mDocument)->FirstChild(RootName);
//	node = static_cast<TiXmlHandle>(mDocument).FirstChild(RootName);

	if (node) 
	{
		node = node->FirstChild(NodeName);
		if (node)
		{
			mParentNode  = node;
			mCurrentNode = node;
			found = true;
		}
		else
		{
			TRACE("Warning: gotoElement("+RootName+","+NodeName+"): Element not found");
			found = false;
		}
	}
	else
	{
		TRACE("Warning: gotoElement("+RootName+","+NodeName+"): Root Node not found");
		found = false;
	}

	return (found);
}



/* ------------------------------------------------------------------------- */
/// On se positionne sur un �l�ment dont on donne le chemin.
/**
	@param NodePath Liste de Nodes, s�par�s par des slashs "/"
	@return Renvoie TRUE si l'�l�ment demand� a �t� trouv�.
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoElementPath(String NodePath)
{

	bool         found      = false;
	StringVector NodeVector = StringUtil::split(NodePath,"/\\");
	int          index      = 0;
	int          lastIndex  = NodeVector.size();

	// Controles de securit�
	if (!mDocument) 
	{
		TRACE("Warning: gotoElementPath("+NodePath+"): mDocument is NULL");
		return false;
	}
	if (NodeVector.at(0) == "") return false;

	TiXmlHandle nodeHandle((TiXmlDocument*)mDocument);

	// On partout les elements du Path, tant qu'il y en a...
	while (index<lastIndex)
	{
		DEBUG_LOG ("   gotoElementPath::scanning for "+NodeVector.at(index) );
		if (nodeHandle.Node()) nodeHandle = nodeHandle.FirstChild(NodeVector.at(index++));
	}
	
	if (nodeHandle.Node()) 
	{
		// Quand il n'y en a plus, on m�morise le Node courant
		mParentNode  = nodeHandle.Node();
		mCurrentNode = nodeHandle.Node();
		found = true;
	}
	else
	{
		// Si on n'a pas trouv� l'�l�ment, on met un warning, et on renvoie FALSE
		TRACE("*** WARNING: gotoElementPath("+NodePath+"): Node not found");
	}
	
	return (found);
}



/* ------------------------------------------------------------------------- */
/// On se positionne sur un �l�ment \<NodeName  id="id" ... \>
/** 
	@param NodeName Nom du Node XML qui va devenir le node courant.
	@param id       Occurence de ce node, � utiliser dans le cas o� il y plusieurs nodes avec ce m�me nom. Commence � 0.
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoElement(String NodeName, int id)
{

	int    cID;
	bool   found   = false;
	int    errcode = 0;

	if (!mDocument) 
	{
		TRACE("Warning: gotoElement("+NodeName+", "+ITOA(id)+"): mDocument is NULL");
		return false;
	}

	/* ----------------------------------------------------- */
	// On parcourt tous les <NodeName> du document
	/* ----------------------------------------------------- */
	TiXmlNode* node = static_cast<TiXmlDocument*>(mDocument)->FirstChild(NodeName);
	while (node)
	{
		TiXmlElement* element = node->ToElement();

		errcode = element->QueryIntAttribute("id",&cID);
		if (errcode == TIXML_WRONG_TYPE)
		{
			TRACE("Warning: gotoElement("+NodeName+") : The attribute ID found is not an integer ");   
		}
		// TIXML_NO_ATTRIBUTE: The attribute ID does not exist (pas de warning dans ce cas)

		if ((errcode == TIXML_SUCCESS) && (cID == id))
		{
			/* ----------------------------------------------------- */
			// On a trouv� un �lement avec ce <NodeName> et id="id"
			/* ----------------------------------------------------- */
			mParentNode  = node;
			mCurrentNode = node;
			found = true;
		}
		node = static_cast<TiXmlDocument*>(mDocument)->IterateChildren(NodeName,node);
	}
	return (found);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le Nieme sous-�l�ment \<NodeName\> en commen�ant par l'index 0.
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoSubElement(String NodeName, unsigned int index)
{
	bool   found = false;
	DEBUG_LOG(">> gotoSubElement('"+NodeName+"',"+ITOA(index)+")");

	if (!mParentNode) return(false);

	/* ----------------------------------------------------- */
	// On parcourt les <NodeName> de l'�l�ment courant.
	/* ----------------------------------------------------- */
	TiXmlNode* node = static_cast<TiXmlNode*>(mParentNode)->FirstChild(NodeName);
	for (unsigned int i=0; i<index; i++)
	{
		node = static_cast<TiXmlNode*>(mParentNode)->IterateChildren(NodeName,node);
	}
	
	/* ----------------------------------------------------- */
	// On r�cup�re le Nieme sous-�l�ment.
	/* ----------------------------------------------------- */
	if (node)
	{
		mCurrentNode = node;
		found        = true;
	}
	return (found);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur un sous-�l�ment \<NodeName\> possedant l'attribut "name" demand�.
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoSubNamedElement(String NodeName, String AttribName)
{
	bool found = false;
	int  index = 0;

	if (!mParentNode) return(false);

	/* ----------------------------------------------------- */
	// On parcourt les <NodeName> de l'�l�ment courant.
	/* ----------------------------------------------------- */
	TiXmlHandle nodeHandle((TiXmlNode*)mParentNode);
	TiXmlElement* elem = nodeHandle.ChildElement(NodeName,index).Element();
	
	while (elem && !found)
	{
		if (elem->Attribute("name") == AttribName) found=true;
		index++;
		elem = nodeHandle.ChildElement(NodeName,index).Element();
	} 	


	
	/* ----------------------------------------------------- */
	// On r�cup�re le Nieme sous-�l�ment.
	/* ----------------------------------------------------- */
	mCurrentNode = nodeHandle.ChildElement(NodeName,index-1).Node();
	return (found);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur un sous-sous-�l�ment du node courant, en pr�cisant le nom du Child direct et du Child suivant.
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoSubElement(String ChildNodeName, String GrandChildNodeName)
{
	bool   found = false;
	DEBUG_LOG(">> gotoSubElement('"+ChildNodeName+"','"+GrandChildNodeName+"')");

	if (!mParentNode) return(false);
	DEBUG_LOG("  mParentNode exist" ); 

	/* ----------------------------------------------------- */
	// On parcourt les <NodeName> de l'�l�ment courant.
	/* ----------------------------------------------------- */
	TiXmlNode* node = static_cast<TiXmlNode*>(mParentNode);

	DEBUG_LOG("  start from " + String(node->Value()));

	TiXmlHandle nodeHandle(node);
	
	node = nodeHandle.FirstChild(ChildNodeName).FirstChild(GrandChildNodeName).Node();

	if (node)
	{
		DEBUG_LOG("   reaching <"+String(node->Value())+">");
		mCurrentNode = node;
		found        = true;
	}
	return (found);
}



/* ------------------------------------------------------------------------- */
/// On supprime tous les Childs du Node Courant
/* ------------------------------------------------------------------------- */
void XMLParser_T::removeAllSubElements()
{
	if (mParentNode) static_cast<TiXmlNode*>(mParentNode)->Clear();
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le parent du Node Courant
/** 
	@return Renvoie TRUE si on est bien revenu au parent, ou FALSE si l'�l�ment courant n'a pas de Parent
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::gotoParentElement()
{
	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "gotoParentElement()");
	
	TiXmlNode* parent = static_cast<TiXmlNode*>(mCurrentNode)->Parent();

	if (parent)
	{
		mCurrentNode = parent;
		mParentNode  = parent;
		return (true);
	}
	else
	{
		//  Renvoie FALSE si l'�l�ment courant n'a pas de Parent
		return (false);
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie le Nom du Node Courant 
/* ------------------------------------------------------------------------- */
String XMLParser_T::getElementName()
{
	String Value = "";

	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getElementName()");

	Value = static_cast<TiXmlNode*>(mCurrentNode)->Value();
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type Integer)
/**  
	@param Attribut Nom de l'attribut � lire.
	@return Renvoie la valeur lue, ou -1 si l'attribut n'existe pas.
*/
/* ------------------------------------------------------------------------- */
int XMLParser_T::getIntAttribute(String Attribut)
{
	int Value = -1;

	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getIntAttribute()");

	int errcode = static_cast<TiXmlElement*>(mCurrentNode)->QueryIntAttribute(Attribut, &Value);

	switch (errcode)
	{
	case TIXML_WRONG_TYPE  : TRACE("Warning: getIntAttribute("+Attribut+"): The attribute found is not an integer"); Value=-1; break;
	case TIXML_NO_ATTRIBUTE: TRACE("Warning: getIntAttribute("+Attribut+"): The attribute is not found"); Value=-1; break;
	case TIXML_SUCCESS     : break;
	}
	
	return (Value);
}



/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut demand� de l'�l�ment courant (de type Vector3)
/** 
	@param Attribut  Nom de l'attribut XML � lire.
	@return Renvoie le vecteur lu, ou un vecteur NULL si l'attribut n'existe pas.
*/
/* ------------------------------------------------------------------------- */
Vector3 XMLParser_T::getVectorAttribute(String Attribut)
{
	//  On renvoie un vecteur ZERO si l'attribut n'existe pas.
	Vector3 sVector = Vector3::ZERO;

	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getVectorAttribute()");

	const char* cValue = static_cast<TiXmlElement*>(mCurrentNode)->Attribute(Attribut);
			
	// On analyse le champ texte qui contient 3 valeurs s�par�es par un espace
	if (cValue) sVector = StringConverter::parseVector3(cValue);

	return (sVector);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type Real)
/**
@param Attribut     Le nom de l'attribut demand�.
@param DefaultValue La valeur � retourner si cet attribut n'existe pas.
@return La valeur de l'attribut (Real). 
*/
/* ------------------------------------------------------------------------- */
Real XMLParser_T::getRealAttribute(String Attribut, Real DefaultValue)
{
	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getRealAttribute()");

	const char* cValue = static_cast<TiXmlElement*>(mCurrentNode)->Attribute(Attribut);

	/// On renvoie la valeur par defaut si l'attribut n'existe pas.
	if (!cValue) return (DefaultValue);
	else         return (StringConverter::parseReal(cValue));
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut demand� du node child.
/** 
@param childnode Le nom du node fils du node courant.
@param Attribut  Le nom de l'attribut demand�.
@param DefaultValue La valeur � retourner si cet attribut n'existe pas.
@return La valeur de l'attribut (Real).
*/
/* ------------------------------------------------------------------------- */
Real XMLParser_T::getRealAttribute(String childnode, String Attribut, Real DefaultValue)
{
	Real   rvalue= DefaultValue;
	bool   found = this->gotoSubElement(childnode,0);
	if (found) 
	{
		rvalue  = this->getRealAttribute(Attribut, DefaultValue);
	}
	return (rvalue);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type Boolean)
/** 
@param Attribut     Le nom de l'attribut demand�.
@param DefaultValue La valeur � retourner si cet attribut n'existe pas.
@return La valeur de l'attribut (boolean).
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::getBooleanAttribute(String Attribut, bool DefaultValue)
{
	/// Renvoie "DefaultValue" si l'attribut demand� n'existe pas, ou a une valeur incorrecte 
	/// Valeurs accept�es (true | on | yes) ou (false | off | no)
	bool   Value = DefaultValue;

	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getBooleanAttribute()");

	const char* cValue = static_cast<TiXmlElement*>(mCurrentNode)->Attribute(Attribut);

	if (cValue)   
	{
		String sValue = cValue;
		// On passe en minuscules 
		StringUtil::toLowerCase(sValue);
		if ((sValue=="false") || (sValue=="off") || (sValue=="no" ))   Value=false;
		if ((sValue=="true")  || (sValue=="on")  || (sValue=="yes"))   Value=true;
	}
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut demand� du node child.
/** 
@param childnode    Le nom du node fils du node courant.
@param Attribut     Le nom de l'attribut demand�.
@param DefaultValue La valeur � retourner si cet attribut n'existe pas.
@return La valeur de l'attribut (boolean).
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::getBooleanAttribute(String childnode, String Attribut, bool DefaultValue)
{
	bool   rvalue= DefaultValue;
	bool   found = this->gotoSubElement(childnode,0);
	if (found) 
	{
		rvalue = this->getBooleanAttribute(Attribut, DefaultValue);
	}
	return (rvalue);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type String)
/* ------------------------------------------------------------------------- */
String XMLParser_T::getStringAttribute(String Attribut)
{
	DEBUG_LOG(">> getStringAttribute('"+Attribut+"')");
	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getStringAttribute()");

	const char* cValue = static_cast<TiXmlElement*>(mCurrentNode)->Attribute(Attribut);

	/// On renvoie une chaine vide si l'attribut n'existe pas.
	if (!cValue) return("");	
	else         return(String(cValue));

}



/* ------------------------------------------------------------------------- */
/// Renvoie le "Contenu" du node courant
/* ------------------------------------------------------------------------- */
String XMLParser_T::getTextContent()
{

	/// On gen�re une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getTextContent()");

	const char* Value = static_cast<TiXmlNode*>(mCurrentNode)->FirstChild()->Value(); 

	/// On renvoie une chaine vide si le champ texte n'existe pas.
	if (!Value) Value="";
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// Ajoute des donn�es XML au document \<interface\>.
/** @param Data        Les donn�es � ajouter au document.
    <i>Data Example: "<status><item call='repair(wpOUT_nav,20,wires)'  text='five minutes' >Navigation Door </item></status>" </i>
    @param Data_ID     Le nom du Node racine � transferer. Ex: "status";
    @param RefNodeName Le nom du Node du document principal auquel seront rattach�es les donn�es.
*/
/* ------------------------------------------------------------------------- */
void XMLParser_T::addXmlData(String Data, String Data_ID, String RefNodeName)
{
	DEBUG_LOG(">> addXmlData(Data, "+Data_ID+","+RefNodeName+")");
	DEBUG_LOG(">> Data= " + Data);

	// On parse les donn�es re�ues en parametre
	// On met ces donn�es dans un nouveau document
	TiXmlDocument* NewDoc = new TiXmlDocument();
	NewDoc->Parse (Data.c_str());

	if (NewDoc->Error())
	{
		TRACE( "ERROR addXmlData: cannot parse "+Data);
		TRACE( NewDoc->ErrorDesc() );
		return;
	}

	// On recupere le Node Parent (ex: <interface>) du document principal
	TiXmlNode*  RefNode = static_cast<TiXmlDocument*>(mDocument)->FirstChild(RefNodeName);
	if (!RefNode) TRACE("ERROR addXmlData: RefNode <"+RefNodeName+"> does not exist in the XML document"); 

	// On supprime un eventuel Node <Data_ID>, existant dans le document principal
	TiXmlNode* 	OldNode = static_cast<TiXmlDocument*>(mDocument)->FirstChild(Data_ID);
	if (OldNode) 
	{
		DEBUG_LOG("<"+Data_ID+"> already exist: it will be removed"); 
		RefNode->RemoveChild(OldNode);
	}

	// On recup�re le Node <Data_ID> des donn�es pars�es
	TiXmlNode*    NewNode = NewDoc->FirstChild(Data_ID);
	if (!NewNode) TRACE("*** WARNING : addXmlData(): <"+Data_ID+"> not found in Data"); 
	// On importe le node, et on le place dans <interface> 
	 RefNode->InsertEndChild(*NewNode); // Cette methode oblige � conserver le NewDoc

	// On peut detruire le document temporaire...
	if (!NewDoc) TRACE("*** ERROR : addXmlData(): NewDoc is NULL"); 
	NewDoc->Clear();
	delete(NewDoc);
}




/* ------------------------------------------------------------------------- */
/// Enregistre le contenu du document XML dans un fichier, en entier, ou � partir du Node indiqu�.
/* ------------------------------------------------------------------------- */
void XMLParser_T::saveToFile(String filename, String rootnode)
{
	TiXmlNode*  FirstNode;


        // On se positionne sur le Node � �crire dans le fichier
		if (rootnode == "")
		{
			// Si le firstNode n'est pas pr�cis�, on �crit le DOMdocument entier
			static_cast<TiXmlDocument*>(mDocument)->SaveFile(filename);
		}
		else
		{
			// Si le firstNode est pr�cis�, on �crit le Node dans le fichier
			FirstNode = static_cast<TiXmlDocument*>(mDocument)->FirstChild(rootnode);
			if (FirstNode) 
			{
				FILE* outfile;
				outfile = fopen(filename.c_str(), "w");
				FirstNode->Print(outfile,0);
				fclose(outfile);
			}
		}
}




/* ------------------------------------------------------------------------- */
/// Le noeud courant devient le Parent. Ce qui permet de refaire des appels a getSubElement().
/* ------------------------------------------------------------------------- */
void XMLParser_T::setParentNode()
{
	mParentNode = mCurrentNode ;
}


/* ------------------------------------------------------------------------- */
/// 
/* ------------------------------------------------------------------------- */
void XMLParser_T::returnToParentNode()
{
	mCurrentNode = mParentNode;
}



/* ------------------------------------------------------------------------- */
/// Cette fonction indique si le node concern� a un attribut "id" avec la valeur d�sir�e.
/** 
@param elem Un element XML
@param id   La valeur de l'ID recherch�
@return     La fonction renvoie TRUE si l'�l�ment a bien l'attribut id="id".
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::hasID(void* elem, int id)
{
	int  cID;
	bool found = false;
	TiXmlElement* element = static_cast<TiXmlElement*>(elem);

	if (!elem) return false;

	if (element->QueryIntAttribute("id",&cID) == TIXML_SUCCESS)
	{
		if (cID==id) found = true;
	}
	return found;
}


/* ------------------------------------------------------------------------- */
/// Cette fonction parcourt une arborescence � la recherche de d'un node particulier.
/** 
       INCOMPLETE  ! ! ! ! ! 
On parcourt les enfants, les enfants des enfants, etc.
@param node     Un node XML, d�but de l'arbre de recherche.
@param NodeName Le nom du node recherch�.
@param id       La valeur de l'attribut ID recherch�.
@return         La fonction renvoie TRUE si l'�l�ment a bien l'attribut id="id".
*/
/* ------------------------------------------------------------------------- */
bool XMLParser_T::iterateChild(void* xmlnode, String NodeName, int id)
{
	TiXmlNode* child = NULL;
	bool       found = false;

	TiXmlElement* node = static_cast<TiXmlElement*>(xmlnode);
	child = node->IterateChildren(NodeName,node);
	while (0)
	{
		if (hasID(child, id)) { found=true; } /// TODO
		else this->iterateChild(child, NodeName,id);
	}
	/// TODO : g�rer la sortie
	return false;
}


