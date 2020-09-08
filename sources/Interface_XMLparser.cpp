/* ------------------------------------------------------------------------- */
// File       : Interface_XMLParser.cpp
// Project    : Long Forgotten Earth
// Auteur     : David de Lorenzo
/* ------------------------------------------------------------------------- */


// Attention à respecter l'ordre de ces includes
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"

#include "macros.h"
#include "Interface_XMLparser.h"


#undef  TRACE
#define TRACE(X)



/* ------------------------------------------------------------------------- */
/// Constructeur (on execute le Constructeur de la surclasse)
/* ------------------------------------------------------------------------- */
Interface_XMLParser::Interface_XMLParser(String XMLfile) : XMLParser(XMLfile)
{}


/* ------------------------------------------------------------------------- */
/// Destructeur (on execute le destructeur de la surclasse)
/* ------------------------------------------------------------------------- */
Interface_XMLParser::~Interface_XMLParser()
{}


/* ------------------------------------------------------------------------- */
/// Renvoie le "contenu" d'un item du menu 
/* ------------------------------------------------------------------------- */
String Interface_XMLParser::getItemTextContent(String sMenuName, int index)
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh  tempStr[100] ;
	String sTexte;
	const char* MenuName = sMenuName.c_str();

	// On recupère l'Element <MenuName>
	XMLString::transcode(MenuName,tempStr,99);
	DOMNode*     nMenu = ((DOMDocument*)mDocument)->getElementsByTagName(tempStr)->item(0);
	DOMElement*  eMenu = (DOMElement*)nMenu;
	if (!nMenu) {DEBUG_LOG("Error: XML Tag <"+sMenuName+"> not found");return "";}

	// On recupere la liste des Elements <item> du <menu>
	XMLString::transcode("item",tempStr,99);
	DOMNodeList* listelem= eMenu->getElementsByTagName(tempStr);

	// getLocalName donne "item"
	// getTextContent donne le contenu (à afficher dans le menu)
	DOMNode* node = listelem->item(index);
	if (!node) {return "";}
	const XMLCh* xTexte = node->getTextContent();	

	if (xTexte) sTexte += XMLString::transcode(xTexte);
	// DEBUG_LOG("Found item: "+sTexte);
	return (sTexte);
}


/* ------------------------------------------------------------------------- */
/// Renvoie le "contenu" associé à l'item demandé (cherche l'ID dans tout le document)
/* ------------------------------------------------------------------------- */
String Interface_XMLParser::getItemTextContent(int id)
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh  tempStr[100];
	String Value;
	int    cID;

	/* ----------------------------------------------------- */
	// On recupere la liste des Elements <item> du fichier
	/* ----------------------------------------------------- */
	XMLString::transcode("item"  ,tempStr,99);
	DOMNodeList* listelem = ((DOMDocument*)mDocument)->getElementsByTagName(tempStr);

	/* ----------------------------------------------------- */
	// On cherche l'item ayant cet ID
	/* ----------------------------------------------------- */
	for (unsigned int i=0; i<listelem->getLength(); i++)
	{
		DOMNode* node = listelem->item(i);

		XMLString::transcode("id",tempStr,99);
		const XMLCh* xID = ((DOMElement*)node)->getAttribute(tempStr);
		cID = atoi(XMLString::transcode(xID));

		if (cID == id)
		{
			/* ----------------------------------------------------- */
			// Une fois l'item trouvé, on récupère le contenu texte
			/* ----------------------------------------------------- */
			const XMLCh* xValue = ((DOMElement*)node)->getTextContent();	
			if (xValue) Value += XMLString::transcode(xValue);
			//  On renvoie "" si l'attribut n'existe pas.
		}
	}
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut "id" d'un item du menu 
//  Renvoie -1 si l'attribut n'existe pas.
/* ------------------------------------------------------------------------- */
int Interface_XMLParser::getMenuItemID(String sMenuName, int index)
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh       tempStr[100] ;
	const char* MenuName = sMenuName.c_str();
	int         cID = -1;

	// On recupère l'Element <MenuName>
	XMLString::transcode(MenuName,tempStr,99);
	DOMNode*     nMenu = ((DOMDocument*)mDocument)->getElementsByTagName(tempStr)->item(0);
	DOMElement*  eMenu = (DOMElement*)nMenu;
	if (!nMenu) {DEBUG_LOG("Warning: XML Tag <"+sMenuName+"> not found");return -1;}

	// On recupere la liste des Elements <item> du <menu>
	XMLString::transcode("item",tempStr,99);
	DOMNodeList* listelem= eMenu->getElementsByTagName(tempStr);

	DOMNode* node = listelem->item(index);
	if (!node) {return -1;}

	// on demande l'attribut "id"
	XMLString::transcode("id",tempStr,99);
	DOMNode* NodeAttr= ((DOMElement*)node)->getAttributes()->getNamedItem(tempStr);
	if (!NodeAttr) {DEBUG_LOG("Error: XML Tag 'id=' not found."); return -1;}

	// NodeAttr->getLocalName donne "id"
	// NodeAttr->getTextContent donne la valeur de l'ID
	// NodeAttr->getNodeValue   donne la valeur de l'ID

	const XMLCh* xID = NodeAttr->getTextContent();
	cID = atoi(XMLString::transcode(xID));
	// DEBUG_LOG("ID="+ITOA(cID));

	return cID;
}





/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut "attribut=" associé à l'item demandé (cherche l'ID dans tout le document)
/* ------------------------------------------------------------------------- */
String Interface_XMLParser::getItemStringAttribute(int id, String attribut)
{
	XERCES_CPP_NAMESPACE_USE

	const char* AttributeName = attribut.c_str();
	XMLCh  tempStr[100];
	String Value;
	int    cID;

	/* ----------------------------------------------------- */
	// On recupere la liste des Elements <item> du fichier
	/* ----------------------------------------------------- */
	XMLString::transcode("item"  ,tempStr,99);
	DOMNodeList* listelem = ((DOMDocument*)mDocument)->getElementsByTagName(tempStr);

	/* ----------------------------------------------------- */
	// On cherche l'item ayant cet ID
	/* ----------------------------------------------------- */
	for (unsigned int i=0; i<listelem->getLength(); i++)
	{
		DOMNode* node = listelem->item(i);

		XMLString::transcode("id",tempStr,99);
		const XMLCh* xID = ((DOMElement*)node)->getAttribute(tempStr);
		cID = atoi(XMLString::transcode(xID));

		if (cID == id)
		{
			/* ----------------------------------------------------- */
			// Une fois l'item trouvé, on récupère l'attribut demandé
			/* ----------------------------------------------------- */
			XMLString::transcode(AttributeName,tempStr,99);
			const XMLCh* xValue = ((DOMElement*)node)->getAttribute(tempStr);
			if (xValue) Value += XMLString::transcode(xValue);
			//  On renvoie "" si l'attribut n'existe pas.
		}
	}
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// Renvoie le titre de l'interface (à mettre en haut de la fenetre)
/* ------------------------------------------------------------------------- */
String Interface_XMLParser::getTitle()
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh  tempStr[100] ;
	String sTitleName;

	// On recupère l'Element <picture>
	XMLString::transcode("title",tempStr,99);
	DOMNode*     nNode = ((DOMDocument*)mDocument)->getElementsByTagName(tempStr)->item(0);
	if (!nNode) {DEBUG_LOG("Warning: XML Tag <title> not found");return "";}

	const XMLCh* xTitleName = nNode->getTextContent();	

	if (xTitleName) sTitleName += XMLString::transcode(xTitleName);
	// DEBUG_LOG("Found item: "+sTitleName);
	return (sTitleName);
}




/* ------------------------------------------------------------------------- */
/// Renvoie le nom du champ StaticText associé à la Listbox (grace à son attribut textref=)
/* ------------------------------------------------------------------------- */
String  Interface_XMLParser::getAssociatedTextName (String ListboxName)
{
	int    index = 0;
	String foundTextName ="";

	TRACE (">> getAssociatedTextName("+ListboxName+")");

	// On analyse le nom de la Listbox pour la retrouver dans le fichier XML
	StringVector WidgetName  = StringUtil::split(ListboxName,"/");

	// Description du vecteur:
	// WidgetName.at(0)        Nom du GUI
	// WidgetName.at(1)        Nom de la fenetre
	// WidgetName.at(2)        Type de widget (ici "listbox")
	// WidgetName.at(3)        Index du widget 

	// On récupère le textref= associé à l'item dans le fichier XML
	this->gotoElement(WidgetName.at(1));		    // = Nom de la fenetre
	this->gotoSubElement(WidgetName.at(2),StringUtil::toReal(WidgetName.at(3)));  // =("listbox",0)
	int textID = this->getIntAttribute("textID");
	TRACE ("   found textID="+ITOA(textID));

	// On recupère le champ texte ayant cet ID
	this->gotoElement("text", textID);	

	// On récupère le nom de sa fenetre
	this->gotoParentElement();
	String winName = this->getElementName();

	// On recupère l'index de ce texte dans la fenetre
	this->gotoElement(winName);	
	while (this->gotoSubElement("text",index))
	{
		if (this->getIntAttribute("id")==textID)
		{
			foundTextName=WidgetName.at(0)+"/"+winName+"/text/"+ITOA(index); 
		}
		index++;
	}

	TRACE ("<< getAssociatedTextName : returns "+foundTextName);
	if (foundTextName=="") DEBUG_LOG("WARNING: The associated StaticText designed by 'textID="+ITOA(textID)+"' of "+ListboxName+" has not been found in the XML file.");
	return foundTextName;
}

