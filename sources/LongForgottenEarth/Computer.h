/* ------------------------------------------------------------------------- */
// File       : Computer.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _COMPUTER_H_
#define _COMPUTER_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"
#include "Interface.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des Computer (tout ce qui n'est pas spécifique: mesh, etc)
/** Il s'agit d'une classe de base. Ce computer ne sait rien faire de particulier. 
Il est dérivé en toutes sortes de computers spécialisés. 
*/
/* ------------------------------------------------------------------------- */
class Computer  : public UserDefinedObject, public FrameListener, public Interface
{

public:
	Computer(String MeshFile, String DescFile, SceneNode* ParentNode, SceneManager* SceneMgr);
	virtual ~Computer();
	bool    frameStarted(const FrameEvent& evt);	// inutile
	void    addFrameListener(short User);			// inutile
	long    getTypeID (void) const;
 	void    addToScene();
 	void    removeFromScene();
	void    setPosition(Vector3 Position);
	void    setOrientation(int Orientation);
	void    setOrientation(Quaternion Orientation);
	void    setBroken(bool broken);
	void    setFocusOn();
	void    setFocusOff();
	void    requestExitInterface(Real seconds);// inutile
	bool    isExitInterfaceRequiered();// inutile
	bool    isInterfaceDisabled();
	bool    isBroken();
	void    setComputerMaterial();
	String  getName();


protected:	

	void    loadDescription(String DescFile);
	void    loadObject();

	// Variables d'environnement
	Root*           mRoot;
	SceneManager*   mSceneMgr;

	// Variable de l'objet
	String          mName;
	SceneNode*      mNode;
	SceneNode*      mParentNode;
	Entity*         mEntity;
	SubEntity*      mSubEntity_screen;
	SubEntity*      mSubEntity_visu;
	SubEntity*      mSubEntity_keyboard;

	bool            mBroken;
	bool            mHasFocus;
	int             mSelectedOrder;

	// Variables pour l'interface
	bool            mExitGui;
	Real            mExitGuiDelay;

	// Variables pour l'audio
	int             mOperatingSoundChannel;
	int             mHelloSoundChannel;
	int             mBrokenSoundChannel;

	// Variables pour les effets de particules
	SceneNode*      mParticulesNode;
	ParticleSystem*	mSmoke;

	// Donnees du fichier de configuration
	String          mMeshFile;
	String          mInterfaceFile;
	String          mHelloSoundFile;
	String          mBrokenSoundFile;
	String          mOperatingSoundFile;	
	String          mStatusFile;	

};

#endif 
