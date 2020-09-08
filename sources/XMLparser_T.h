/* ------------------------------------------------------------------------- */
// File       : XMLParser_T.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _XML_PARSER_TINY_H_
#define _XML_PARSER_TINY_H_



#include "Ogre.h"
using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Surclasse pour la gestion des fichiers XML (basé sur la librarie TinyXML)
/**
On se déplace dans le document aec les fonction gotoElement.
On se place sur n'importe quel Element du document.
On peut alors lire ses attributs.
Au besoin on peut acceder à ses fils (gotoSubElement), et revenir à l'Element (gotoParentElement).
*/
/* ------------------------------------------------------------------------- */
class XMLParser_T
{
public:
	XMLParser_T(String XMLfile);
	~XMLParser_T();
	// Gestion du document
	void    parseFile(String XMLfile);
	bool    isPresent();
	void    addXmlData(String Data, String Data_ID, String RefNodeName= "interface");
	void    saveToFile(String filename, String rootnode="");
	// Gestion de l'arborescence
	bool    gotoRootElement(String RootName);
	bool    gotoElement(String NodeName, int id);
	bool    gotoElement(String RootName, String NodeName);
	bool    gotoSubElement(String NodeName, unsigned int index);
	bool    gotoSubElement(String ChildNodeName, String GrandChildNodeName);
	bool    gotoSubNamedElement(String NodeName, String AttribName);
	bool    gotoElementPath(String NodePath);
	void    setParentNode();
	void    returnToParentNode();
	bool    gotoParentElement();
	String  getElementName();
	void    removeAllSubElements();
	// Gestion des attributs
	int     getIntAttribute    (String Attribut);
	Vector3 getVectorAttribute (String Attribut);
	Real    getRealAttribute   (String Attribut, Real DefaultValue=0);
	Real    getRealAttribute   (String childnode, String Attribut, Real DefaultValue=0);
	bool    getBooleanAttribute(String Attribut, bool DefaultValue=false);
	bool    getBooleanAttribute(String childnode, String Attribut, bool DefaultValue=false);
	String  getStringAttribute (String Attribut);
	String  getTextContent();

protected:

	bool hasID(void* elem, int id);
	bool iterateChild(void* xmlnode, String NodeName, int id);

	void* mDocument;		
	void* mCurrentNode;		
	void* mParentNode;		

};

#endif
