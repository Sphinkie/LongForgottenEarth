/* ------------------------------------------------------------------------- */
// File       : XMLParser.cpp
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


// ---------------------------------------------------------------------------
//  Includes supplémentaires pour XMLsave
// ---------------------------------------------------------------------------
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>
//#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>



#include "XMLparser.h"
#include "macros.h"

/* ------------------------------------------------------------------------- */
/// Constructor
/* ------------------------------------------------------------------------- */
XMLParser::XMLParser(String XMLfile)
{
	TRACE(">> XMLParser("+XMLfile+")");
	mDocument=NULL;
	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
	this->parseFile(XMLfile);
	TRACE("<< XMLParser");

	mTest = XMLfile;
}


/* ------------------------------------------------------------------------- */
/// Destructor
/* ------------------------------------------------------------------------- */
XMLParser::~XMLParser()
{
	XERCES_CPP_NAMESPACE_USE

	//TRACE(">> ~XMLParser");
	delete ((XercesDOMParser*)mParser);
	delete ((ErrorHandler*)mErrHandler);

	XMLPlatformUtils::Terminate();
}


/* ------------------------------------------------------------------------- */
/// Parse le fichier XML
/* ------------------------------------------------------------------------- */
void XMLParser::parseFile(String XMLfile)
{
	XERCES_CPP_NAMESPACE_USE

	// Creation du Parser
	XercesDOMParser* Parser = new XercesDOMParser();
	mParser = Parser;						// On mémorise le pointer 
	Parser->setDoNamespaces(true);			// optionnal

	// Creation du Handler d'erreurs
	ErrorHandler* ErrHandler = (ErrorHandler*) new HandlerBase();
	mErrHandler = ErrHandler;				// On mémorise le pointer 
	Parser->setErrorHandler(ErrHandler);

	mParentNode  = NULL;
	mCurrentNode = NULL;

	try
	{
		Parser->parse(XMLfile.c_str());
		mDocument = Parser->getDocument(); //(DOMDocument*)

	}
	catch (const XMLException& toCatch)
	{
		DEBUG_LOG("XML Exception raised while parsing "+XMLfile);
		char* ExMsg =  XMLString::transcode(toCatch.getMessage() );
		if (ExMsg!=NULL) DEBUG_LOG(ExMsg);
	}
	catch (const DOMException& toCatch)
	{
        XMLCh errText[2048];
		DEBUG_LOG("DOM Exception raised while parsing "+XMLfile+". Code=" + ITOA(toCatch.code) );
        if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, 2047))  
		{
			char* ExMsg =  XMLString::transcode(errText);
			if (ExMsg!=NULL) DEBUG_LOG(ExMsg);
		}
	}
	catch (...)
	{
		DEBUG_LOG("ParseFile(): Unexpected Exception.");
		DEBUG_LOG("The file "+XMLfile+" may not exist, or is invalid.");
	}

}

/* ------------------------------------------------------------------------- */
/// Indique si un document a été parsé
/* ------------------------------------------------------------------------- */
bool XMLParser::isPresent()
{
	/// On renvoie FALSE si mDocument est NULL;
	return (mDocument != NULL);
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur un élément <NodeName ...> (de preference unique).
/* ------------------------------------------------------------------------- */
bool XMLParser::gotoElement(String NodeName)
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh    tempStr[100] ;
	bool     found;
	DOMNode* node;

	// On recupère l'Element <NodeName>
	XMLString::transcode(NodeName.c_str(),tempStr,99);
	try
	{
		if (!mDocument) DEBUG_LOG("Warning: gotoElement("+NodeName+"): mDocument is NULL");
		node = ((DOMDocument*)mDocument)->getElementsByTagName(tempStr)->item(0);
	}
	catch (const DOMException& toCatch)
	{
		// Si c'est une exception DOM, on peut mettre un message détaillé.
        XMLCh errText[2048];
		DEBUG_LOG("DOM Exception raised in gotoElement("+NodeName+"). Code=" + ITOA(toCatch.code) );
        if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, 2047))  
		{
			char* ExMsg =  XMLString::transcode(errText);
			if (ExMsg!=NULL) DEBUG_LOG(ExMsg);
		}
	}
	catch (...) 
	{
		// Quelque chose a mal fonctionné....
		DEBUG_LOG("ERROR: Exception in gotoElement("+NodeName+")");
	}
	if (node) 
	{
		mParentNode  = node;
		mCurrentNode = node;
		found = true;
	}
	else
	{
		DEBUG_LOG("Warning: gotoElement("+NodeName+"): tag not found");
		found = false;
	}

	return (found);
}



/* ------------------------------------------------------------------------- */
/// On se positionne sur un élément <NodeName  id="id" ... >
/* ------------------------------------------------------------------------- */
bool XMLParser::gotoElement(String NodeName, int id)
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh  tempStr[100] ;
	int    cID;
	bool   found = false;

	if (!mDocument) DEBUG_LOG("Warning: gotoElement("+NodeName+", "+ITOA(id)+"): mDocument is NULL");
	/* ----------------------------------------------------- */
	// On recupere la liste des Elements <NodeName> du fichier
	/* ----------------------------------------------------- */
	XMLString::transcode(NodeName.c_str() ,tempStr,99);
	DOMNodeList* listelem = ((DOMDocument*)mDocument)->getElementsByTagName(tempStr);

	/* ----------------------------------------------------- */
	// On cherche l'élément avec l'ID demandé
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
			// On a trouvé un élement avec ce <NodeName> et id="id"
			/* ----------------------------------------------------- */
			mParentNode  = node;
			mCurrentNode = node;
			found = true;
		}
	}
	return (found);
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le Nieme sous-élément <NodeName  ... >
/* ------------------------------------------------------------------------- */
bool XMLParser::gotoSubElement(String NodeName, unsigned int index)
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh  tempStr[100] ;
	bool   found = false;

	if (!mParentNode) return (false);

	/* ----------------------------------------------------- */
	// On recupere la liste des Sous-Elements <NodeName> de l'élément courant
	/* ----------------------------------------------------- */
	XMLString::transcode(NodeName.c_str() ,tempStr,99);
	DOMNodeList* listelem = ((DOMElement*)mParentNode)->getElementsByTagName(tempStr);

	/* ----------------------------------------------------- */
	// On récupère le Nieme sous-élément
	/* ----------------------------------------------------- */
	if (index < listelem->getLength())
	{
		mCurrentNode = listelem->item(index);
		found=true;
	}
	return (found);
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le parent du Node Courant
/* ------------------------------------------------------------------------- */
bool XMLParser::gotoParentElement()
{
	XERCES_CPP_NAMESPACE_USE

	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "gotoParentElement()");
	
	DOMNode* parent = static_cast<DOMNode*>(mCurrentNode)->getParentNode();

	//  Renvoie FALSE si l'élément courant n'a pas de Parent
	if (parent==NULL) return false;

	mCurrentNode = parent;
	mParentNode  = parent;
	
	return (true);
}


/* ------------------------------------------------------------------------- */
/// Renvoie le Nom du Node Courant 
/* ------------------------------------------------------------------------- */
String XMLParser::getElementName()
{
	XERCES_CPP_NAMESPACE_USE

	String      Value = "";

	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getElementName()");

	const XMLCh* xValue = ((DOMElement*)mCurrentNode)->getTagName();
	if (xValue)   Value += XMLString::transcode(xValue);
	return (Value);
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type Integer)
//  Renvoie -1 si l'attribut n'existe pas.
/* ------------------------------------------------------------------------- */
int XMLParser::getIntAttribute(String Attribut)
{
	
	XERCES_CPP_NAMESPACE_USE

	const char* AttributeName = Attribut.c_str();
	XMLCh       tempStr[100] ;
	int         Value = -1;

	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getIntAttribute()");

	XMLString::transcode(AttributeName,tempStr,99);
	const XMLCh* xValue = ((DOMElement*)mCurrentNode)->getAttribute(tempStr);
	if (xValue)   Value = atoi(XMLString::transcode(xValue));
	return (Value);
}



/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut demandé de l'élément courant (de type Vector3)
//  Renvoie un vercteur NULL si l'attribut n'existe pas.
/* ------------------------------------------------------------------------- */
Vector3 XMLParser::getVectorAttribute(String Attribut)
{
	
	XERCES_CPP_NAMESPACE_USE

	XMLCh   tempStr[100] ;
	String  sText;
	///  On renvoie un vecteur ZERO si l'attribut n'existe pas.
	Vector3 sVector = Vector3::ZERO;

	/// On renvoie une exception en cas de probleme.
	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getVectorAttribute()");

	XMLString::transcode(Attribut.c_str(),tempStr,99);
	const XMLCh* xValue = ((DOMElement*)mCurrentNode)->getAttribute(tempStr);
			
	if (xValue) sText += XMLString::transcode(xValue);

	// On analyse le champ texte qui contient 3 valeurs séparées par un espace
	sVector = StringConverter::parseVector3(sText);
/*
	StringVector VecteurTexte = StringUtil::split(sText," ");
	Real Vx = StringUtil::toReal(VecteurTexte.at(0));
	Real Vy = StringUtil::toReal(VecteurTexte.at(1));
	Real Vz = StringUtil::toReal(VecteurTexte.at(2));
	sVector = Vector3(Vx,Vy,Vz);
*/	
	return (sVector);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type Real)
/* ------------------------------------------------------------------------- */
Real XMLParser::getRealAttribute(String Attribut, Real DefaultValue)
{
	
	XERCES_CPP_NAMESPACE_USE

	const char* AttributeName = Attribut.c_str();
	XMLCh       tempStr[100] ;
	Real        Value = DefaultValue;
	///  Renvoie DefaultValue (fournie en parametre) si l'attribut n'existe pas.

	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getRealAttribute()");

	XMLString::transcode(AttributeName,tempStr,99);
	const XMLCh* xValue = ((DOMElement*)mCurrentNode)->getAttribute(tempStr);
	if (String(XMLString::transcode(xValue))!="")   Value = StringUtil::toReal(XMLString::transcode(xValue));
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type Boolean)
/* ------------------------------------------------------------------------- */
bool XMLParser::getBooleanAttribute(String Attribut, bool DefaultValue)
{
	
	XERCES_CPP_NAMESPACE_USE

	const char* AttributeName = Attribut.c_str();
	XMLCh       tempStr[100] ;
	String      sValue;
	bool        Value = DefaultValue;
	/// Renvoie "DefaultValue" si l'attribut demandé n'existe pas, ou a une valeur incorrecte 
	/// Valeurs acceptées (true | on | yes) ou (false | off | no)

	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getBooleanAttribute()");

	XMLString::transcode(AttributeName,tempStr,99);
	const XMLCh* xValue = ((DOMElement*)mCurrentNode)->getAttribute(tempStr);
	if (xValue)   
	{
		sValue = XMLString::transcode(xValue);
		// On passe en minuscules 
		StringUtil::toLowerCase(sValue);
		if ((sValue=="false") || (sValue=="off") || (sValue=="no" ))   Value=false;
		if ((sValue=="true")  || (sValue=="on")  || (sValue=="yes"))   Value=true;
	}
	return (Value);
}




/* ------------------------------------------------------------------------- */
/// Renvoie l'attribut du Node Courant (de type String)
//  Renvoie "" si l'attribut n'existe pas.
/* ------------------------------------------------------------------------- */
String XMLParser::getStringAttribute(String Attribut)
{
	XERCES_CPP_NAMESPACE_USE

	const char* AttributeName = Attribut.c_str();
	XMLCh       tempStr[100] ;
	String      Value = "";

	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getStringAttribute()");

	XMLString::transcode(AttributeName,tempStr,99);
	const XMLCh* xValue = ((DOMElement*)mCurrentNode)->getAttribute(tempStr);
	if (xValue)   Value += XMLString::transcode(xValue);
	return (Value);
}



/* ------------------------------------------------------------------------- */
/// Renvoie le "Contenu" du node courant
/* ------------------------------------------------------------------------- */
String XMLParser::getTextContent()
{
	XERCES_CPP_NAMESPACE_USE

	String Value;

	if (!mCurrentNode) OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,"mCurrentNode undefined", "getTextContent()");

	const XMLCh* xValue = ((DOMElement*)mCurrentNode)->getTextContent();	
	if (xValue) Value += XMLString::transcode(xValue);
	return (Value);
}


/* ------------------------------------------------------------------------- */
/// Ajoute des données XML au document \<interface\>.
/** @param Data Les données à ajouter au document.
    Data Example: "<status><item call='repair(wpOUT_nav,20,wires)'  text='five minutes' >Navigation Door </item></status>" 
    @param Data_ID     Le nom du Node racine à transferer. Ex: "status"; Sert aussi de system-ID pour le Memory-Buffer. (Identification en cas d'erreur).
*/
/* ------------------------------------------------------------------------- */
void XMLParser::addXmlData(String Data, String Data_ID)
{
	XERCES_CPP_NAMESPACE_USE
	XMLCh  tempStr[100] ;


	
	TRACE(">> addXmlData(Data, "+Data_ID+")");
	TRACE(">> Data= " + Data);

	DOMDocument* MainDocument = static_cast<DOMDocument*>(mDocument);

    //  Create MemBufferInputSource from the buffer containing the XML  statements.
    MemBufInputSource* memBufIS = new MemBufInputSource
    (
		 (const XMLByte*)Data.c_str(),	
		 Data.length(),
         Data_ID.c_str(),
         false
    );

	try
	{
		if (!memBufIS) DEBUG_LOG("Warning memBufIS is NULL"); 
		// On parse les données reçues en parametre
		static_cast<XercesDOMParser*>(mParser)->parse(*memBufIS);
		// On met ces données dans un nouveau document
		DOMDocument* NewDoc  = static_cast<XercesDOMParser*>(mParser)->getDocument();
		// On recupere le Node <interface> du document principal
		XMLString::transcode("interface",tempStr,99);
		DOMNode*     RefNode = MainDocument->getElementsByTagName(tempStr)->item(0);
		// On supprime un eventuel Node <Data_ID>, existant dans le document principal
		XMLString::transcode(Data_ID.c_str(),tempStr,99);
		DOMNode* 	OldNode = MainDocument->getElementsByTagName(tempStr)->item(0);
		if (OldNode) 
		{
			DEBUG_LOG("<"+Data_ID+"> already exist: it will be removed"); 
			RefNode->removeChild(OldNode);
		}
		// On recupère le Node <Data_ID> des données parsées
		XMLString::transcode(Data_ID.c_str(),tempStr,99);
		DOMNode*     NewNode = NewDoc->getElementsByTagName(tempStr)->item(0);
		DOMElement*  NewElem = static_cast<DOMElement*>(NewNode);
		if (!NewNode) DEBUG_LOG("Warning addXmlData: <"+Data_ID+"> not found"); 
		
		// On importe le node, et on le place dans <interface> 
		DOMNode*     NewMainNode = MainDocument->importNode(NewNode,true);
		RefNode->appendChild(NewMainNode);
	}
    catch (const XMLException& toCatch)
    {	
		DEBUG_LOG("XML Exception raised in addXmlData("+Data_ID+")");
		char* ExMsg =  XMLString::transcode(toCatch.getMessage() );
		if (ExMsg!=NULL) DEBUG_LOG(ExMsg);
	}
	catch (const DOMException& toCatch)
	{
        XMLCh errText[2048];
		DEBUG_LOG("DOM Exception raised in addXmlData("+Data_ID+") - code " + ITOA(toCatch.code) );
        if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, 2047))  
		{
			char* ExMsg =  XMLString::transcode(errText);
			if (ExMsg!=NULL) DEBUG_LOG(ExMsg);
		}
	}
	catch (...)
	{
		DEBUG_LOG("addXmlData(Data, "+Data_ID+"): Unexpected Exception" );
	}
}




/* ------------------------------------------------------------------------- */
/// Enregistre le contenu du document XML dans un fichier, en entier, ou à partir du Node indiqué.
/* ------------------------------------------------------------------------- */
void XMLParser::saveToFile(String filename, String rootnode)
{

	XERCES_CPP_NAMESPACE_USE
	XMLCh tempStr[100];
	DOMDocument* MainDocument = static_cast<DOMDocument*>(mDocument);
	DOMNode*     FirstNode;

    try
    {
		// On cree une implementation
        XMLString::transcode("LS", tempStr, 99);
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

		// On cree le DOMwriter
        DOMWriter* theSerializer = impl->createDOMWriter();
        theSerializer->setEncoding(NULL);		// default is UTF-8
		theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

		// On lui asocie un gestionnaire d'erreur: A faire éventuellement...
		
		// On cree le fichier de destination
        XMLFormatTarget* myFormTarget = new LocalFileFormatTarget(filename.c_str());

        // On se positionne sur le Node à écrire dans le fichier
		if (rootnode!=StringUtil::BLANK)
		{
			XMLString::transcode(rootnode.c_str(),tempStr,99);
			FirstNode = MainDocument->getElementsByTagName(tempStr)->item(0);
		}
		else
		{
			// Si le firstNode n'est pas précisé, on écrit le DOMdocument entier
			FirstNode = MainDocument;
		}

		// On remplit le fichier XML 
        if (FirstNode) theSerializer->writeNode(myFormTarget, *FirstNode); 

        delete theSerializer;
        delete myFormTarget;
	}
    catch (const OutOfMemoryException&)
    {
		DEBUG_LOG("OutOfMemoryException");
	}
	catch (const XMLException& toCatch)
	{
		DEBUG_LOG("An error occurred during creation of output transcoder for "+filename);
		char* ExMsg =  XMLString::transcode(toCatch.getMessage() );
		if (ExMsg!=NULL) DEBUG_LOG(ExMsg);
	}
}



