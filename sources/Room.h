/* ------------------------------------------------------------------------- */
// File       : Room.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */
#ifndef _ROOM_H_
#define _ROOM_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <Ogre.h>
#include <OgreOpcode.h>

#include "classes.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion des pieces (et corridors) du vaisseau spacial
/* ------------------------------------------------------------------------- */
class Room : public UserDefinedObject
{
public:
	Room(String Name, String MeshFile, Vector3 Position, SceneManager* SceneMgr, SceneNode* mParentNode);
	virtual     ~Room();
	void        loadObject();
	long	    getTypeID (void) const;
	void        addToScene();
	void        setOrientation(Degree angle, Quaternion Qorient, bool dotScene=false);
	void        removeFromScene();
	void        createLight(Light::LightTypes lumtype,  String name=ROOMLIGHT, bool castShadows=false);
	void	    setLightPosition (Vector3 position,     String name=ROOMLIGHT);
	void        setLightDirection(Vector3 direction,    String name=ROOMLIGHT);
	void        setLightColour   (ColourValue newcolour,String name=ROOMLIGHT);
	void        setLightRange    (int range,            String name=ROOMLIGHT);
	void        setLight         (bool On,              String name=ROOMLIGHT);
	void        setSpotLightRange(Real inner, Real outer, Real falloff, String name);
	void	    setNightLight (bool On);
	void        setAlertLevel(short alert);
	void        setLampColour(ColourValue newcolor);
	void        setExternView(bool externview);
	void        setSoundFile(String SoundFile);
	String      getName();
	SceneNode*  getNode();
	String      getStatus();
	bool        isBroken();
	String      getChildStatus();
	Vector3     getPosition();
	void        setDepressurized();
	MaterialPtr getMaterial(String MaterialName);
	SubEntity*  getSubEntity(String MaterialName);
	void        translate(Vector3 offsetVector);
	void        adjustPitch();
	void        setPanelMaterialName(String MaterialName);
	String      getPanelMaterialName();
	void        setElevatorRoom(bool elevator);
	bool        isElevatorRoom();
	bool        hasExternView();
	void        _showLightPosition(SceneNode* N);
	void        _updateLightMarker(SceneNode* N);

protected:

	void        createRoom();
	void        initCollisionShape();

	SceneNode*	    mRoomNode;
	SceneNode*      mLightNode; 
	String          mRoomName;
	String          mMeshFile;
	Entity*		    mRoomEntity;
	bool            mElevatorRoom;
	SceneManager*   mSceneMgr;
	SceneNode*	    mParentNode;
	Entity*	        mLightEntity;
	Billboard*      mLightSprite;
	int             mLightRange;
	ColourValue     mLightColour;
	bool            mNightLight;
	bool            mNormalLight;
	ColourValue     mRedColour;
	bool            mOnScene;
	int             mRoomAlertChannel;
	int             mRoomSoundChannel;
	String          mRoomSoundFile;
	String          mRoomPanel;
	bool            mHasExternView;
	bool            mBroken;
	short           mAlertLevel;
	int             mDamage_ETR;
	Light*          mLight[4];            ///< On peut stocker 4 lampes, 0=ROOMLIGHT
	OgreOpcode::CollisionObject* mCollideObject; ///< Objet pour les collisions
};

#endif 
