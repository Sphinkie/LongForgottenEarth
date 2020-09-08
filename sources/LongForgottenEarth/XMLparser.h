/* ------------------------------------------------------------------------- */
// File       : XMLParser.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_


#include "Ogre.h"
using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Surclasse pour la gestion des fichiers XML
/* ------------------------------------------------------------------------- */
class XMLParser
{
public:
	XMLParser(String XMLfile);
	~XMLParser();
	void    parseFile(String XMLfile);
	bool    isPresent();
	bool    gotoElement(String NodeName);
	bool    gotoElement(String NodeName, int id);
	bool    gotoSubElement(String NodeName, unsigned int index);
	bool    gotoParentElement();
	String  getElementName();
	int     getIntAttribute    (String Attribut);
	Vector3 getVectorAttribute (String Attribut);
	Real    getRealAttribute   (String Attribut, Real DefaultValue=0);
	bool    getBooleanAttribute(String Attribut, bool DefaultValue=false);
	String  getStringAttribute (String Attribut);
	String  getTextContent();
	void    addXmlData(String Data, String Data_ID);
	void    saveToFile(String filename, String rootnode="");

protected:

	void* mDocument;		// pointeur de type XERCES_CPP_NAMESPACE::DOMDocument*
	void* mParser;			// pointeur de type XercesDOMParser*
	void* mErrHandler;		// pointeur de type ErrorHandler*
	void* mCurrentNode;		// pointeur de type DOMElement*
	void* mParentNode;		// pointeur de type DOMElement*

	String mTest;
};

#endif
