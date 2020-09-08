/* ------------------------------------------------------------------------- */
// File       : specialFX.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "specialFX.h"

#include "macros.h"

/* ------------------------------------------------------------------------- */
/// Ajoute des nuages à la scene
// TODO: AJOUTER DES PARAMETRES
/* ------------------------------------------------------------------------- */
void SFX_addClouds(SceneManager* SceneMgr)
{

	Plane   Clouds_Plane;
	Entity* Clouds_Entity;

	Clouds_Plane.d = 800;					// Nb of world units above the camera
	Clouds_Plane.normal= -Vector3::UNIT_Y;	// above the camera, facing down
	MeshManager::getSingleton().createPlane("nuagesPlane","nuagesGroup",Clouds_Plane,20000,20000,1,1,true,1,5,5,Vector3::UNIT_Z);
	// largeur, hauteur, 

	Clouds_Entity = SceneMgr->createEntity("nuages", "nuagesPlane");
	Clouds_Entity->setMaterialName("Examples/CloudySky");		// (le script est dans Example.material)
	SceneMgr->getRootSceneNode()->attachObject(Clouds_Entity);
	//ParentNode->attachObject(Clouds_Entity);
}


/* ------------------------------------------------------------------------- */
/// Ajoute ds particules à la scene (fumée, vapeur, étincelles, etc)
/* ------------------------------------------------------------------------- */
ParticleSystem* SFX_addSmoke (String Name, SceneNode* SourceNode)
{
	ParticleSystem* pParticle;

	// (le script est dans smoke.material et smoke.particle)
	pParticle  = ParticleSystemManager::getSingleton().createSystem(Name, "Effect/Smoke");	// 
	SourceNode->attachObject(pParticle);
	return pParticle; 
}

/* ------------------------------------------------------------------------- */
/// Ajoute des etincelles
/* ------------------------------------------------------------------------- */
ParticleSystem* SFX_addSparks (String Name, SceneNode* SourceNode)
{
	ParticleSystem* pParticle;

	// (le script est dans smoke.material et smoke.particle)
	pParticle = ParticleSystemManager::getSingleton().createSystem(Name, "Effect/Sparks");
	SourceNode->attachObject(pParticle);
	return pParticle; 
}


/* ------------------------------------------------------------------------- */
/// Arret (progressif) des particules
/* ------------------------------------------------------------------------- */
void SFX_die (ParticleSystem* pParticle)
{
	pParticle->getEmitter(0)->setEnabled(false);
}

/* ------------------------------------------------------------------------- */
/// Reprise progressive des particules
/* ------------------------------------------------------------------------- */
void SFX_rebirth(ParticleSystem* pParticle)
{
	pParticle->getEmitter(0)->setEnabled(true);
}



/* *********************************************************************************** */
/// Lecture d'une ligne dans un fichier texte (lgmax est la taille du buffer)
/* *********************************************************************************** */
bool readln(FILE* stream, char* Buffer, int lgmax)
// Reads a \n-terminated line from the stream.  
// Converts \n to \0.    
// Returns string read or NULL when EOF reached in parametrer Buffer. 
// If (LG_MAX-1) chars are read before a \n is found,
// the last element of the buffer is set to \0, and return is as normal.
{
	size_t read;
	char   Letter;
	int    Position;

	read = 0;
	Position = 0;
	Letter = '\0';
	Buffer[0] = Letter;
	while ((Position < lgmax) && (Letter != '\n'))
	{
		read = fread(&Letter, 1, sizeof(char), stream);
		Buffer[Position++] = Letter;
		if (Letter == '\n')
		--Position;
	}
	Buffer[Position] = 0;

	if (read == 0)    return false;
	else              return true;
}




/* ------------------------------------------------------------------------- */
/// Renvoie un parametre entier du fichier de config tiny-XML: attrib="40"
/* ------------------------------------------------------------------------- */
int _getIntParameter(String P)
{
	// Cette fonction permet de gérer des fichier de config dans un format XML simplifié (1 seul niveau)
	// On enleve le premier et le dernier caractère (qui sont des " ou des '). et on renvoie un entier
	P = P.substr(1,P.length()-2);
	return StringConverter::parseInt(P);
}

/* ------------------------------------------------------------------------- */
/// Renvoie un parametre String du fichier de config tiny-XML: attrib="example"
/* ------------------------------------------------------------------------- */
String getStringParameter(String P)
{
	// Cette fonction permet de gérer des fichier de config dans un format XML simplifié (1 seul niveau)
	// On enleve le premier et le dernier caractère (qui sont des " ou des '). et on renvoie un String	
	P = P.substr(1,P.length()-2);
	return P;
}


/* ------------------------------------------------------------------------- */
/// Renvoie un parametre Vector3 du fichier de config tiny-XML: attrib="0 0 0"
/* ------------------------------------------------------------------------- */
Vector3 getVectorParameter(String P)
{
	// Cette fonction permet de gérer des fichier de config dans un format XML simplifié (1 seul niveau)
	// On enleve le premier et le dernier caractère (qui sont des " ou des '). et on renvoie un Vector3
	P = P.substr(1,P.length()-2);
	return StringConverter::parseVector3(P);
}

/* ------------------------------------------------------------------------- */
/// Renvoie un parametre Boolean du fichier de config tiny-XML: attrib="false" || "true"
/* ------------------------------------------------------------------------- */
bool getBooleanParameter(String P)
{
	// Cette fonction permet de gérer des fichier de config dans un format XML simplifié (1 seul niveau)
	// On enleve le premier et le dernier caractère (qui sont des " ou des '). et on renvoie un Booleen
	P = P.substr(1,P.length()-2);
	return StringConverter::parseBool(P);
}



/* ------------------------------------------------------------------------- */
/// Lecture d'un fichier texte
/* ------------------------------------------------------------------------- */
String readFile(String filename)
{
using namespace std;

	String s, FileContent;

	// On ouvre le fichier en lecture
	ifstream inputfile(filename.c_str()); 

	// On le lit ligne à ligne
	while(getline(inputfile, s)) 
	{
		// On ignore les lignes de commentaires commençant par #
		if (!StringUtil::startsWith(s,"#"))
		{
			if (s.length()==0) s=" ";
			// On le stocke dans la String
			FileContent += s;
			FileContent += "\n";
		}
	} 
	inputfile.close();
	return (FileContent);
}


/* ------------------------------------------------------------------------- */
/// Ecriture d'un fichier texte
/* ------------------------------------------------------------------------- */
void writeFile(String filename, String data, bool append)
{
using namespace std;

	fstream::openmode mode;

	// On ouvre le fichier en ecriture
	if (append)  mode = (fstream::out|fstream::app); 
	else         mode = (fstream::out|fstream::trunc); 

	ofstream outputfile(filename.c_str(), mode); 
	data += "\n";
	outputfile << data;	// On ecrit les données 
	outputfile.close();
}


