/* ------------------------------------------------------------------------- */
// File       : Button.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _BUTTON_H_
#define _BUTTON_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des Button (boutons poussoirs en 3D clicables dans les Room)
/* ------------------------------------------------------------------------- */
class Button    : public UserDefinedObject
{
public:
	Button(String Name, String MeshFile, String DescFile, SceneNode* ParentNode, SceneManager* mSceneMgr);
	virtual ~Button();
	long     getTypeID (void) const;
 	void     setPosition(Vector3 Position);
	void     setOrientation(int Orientation);
	void     setOrientation(Quaternion Orientation);
 	void     addToScene();
	void     removeFromScene();
	void     loadObject (String DescFile);
	void     setMaterial(String MaterialName);
	void     setFocusOn();
	void     setFocusOff();

	void     actionButton();


protected:
	void     actionPlayerUp();
	void     actionPlayerDown();

	// Variables d'environnement
	Root*           mRoot;
	SceneManager*   mSceneMgr;

	// Variable de l'objet
	String          mName;
	SceneNode*      mNode;
	SceneNode*      mParentNode;
	Entity*         mEntity;
	SubEntity*      mSubEntity_screen;
	bool            mBroken;
	bool            mHasFocus;

	// Donnees du fichier de configuration
	String          mMeshFile;
	String          mAction;
	String          mIdleMaterial;
	String          mActiveMaterial;


};

#endif 
