/* ------------------------------------------------------------------------- */
// File       : TheShip.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */
#ifndef _THE_SHIP_H_
#define _THE_SHIP_H_

#if _MSC_VER > 1000
#pragma once
#endif

// Pour OIS  
#include "OISInputManager.h"
#include "OISEvents.h"
#include "OISMouse.h"
#include "OISJoyStick.h"

#include "Ogre.h"
#include "OgreSingleton.h"	
#include "SpaceShip.h"
 
#include "ShipLevel.h"
#include "ShipElement.h"
#include "ShipEngine.h"
#include "ShipLight.h"
#include "WaypointNetwork.h"
#include "TextList.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Le vaisseau spatial principal ("Le Sphinx"). C'est un Singleton.
/* ------------------------------------------------------------------------- */
/** Le vaisseau est un objet complet qui contient:
 - Sa propre description (il est composé de plusieurs Level, et Door)
 - Un plancher (floor) par Level, qui sert au deplacement des PNJ
 - La gestion d'un réseau de Waypoints (toujours pour les déplacements des PNJ)
 - La gestion de l'état du vaisseau, et de différents éléments
 - La gestion des mouvements du vaisseau

  Quand un nouveau ShipLevel est ajouté au Ship, il faut modifier:
  - TheShip::TheShip()
  - TheShip::openDoor()
  - TheShip::frameStarted()
  - TheShip::setAlertLevel()
  - TheShip::getStatus()
*/
/* ------------------------------------------------------------------------- */

#define MAX_LEVELS 7	// Les levels vont de 0 à 6

class TheShip  : public SpaceShip, public FrameListener, public OIS::JoyStickListener, public Singleton <TheShip>
{

public:
	TheShip(SceneManager* SceneMgr);
	virtual    ~TheShip();
	long       getTypeID(void) const;
	void       addToScene(Vector3 Position);
	bool       frameStarted(const FrameEvent& evt);
	void       loadInitialSituation();

	// Gestion des Niveaux
	ShipLevel* getShipLevel(int Number);

	// Gestion des planchers
//	Real       detectGround(Vector3 Origin, bool RoomDetection = true);
	Real       detectGround(Vector3 Origin);				/* unused */
	Real       detectFloorAltitude(Vector3 Origin);
//	void       detectRoom(Vector3 Origin);
	void       detectRoom(SceneNode* Origin);
	void       createFloor(int Level);
	void       setFloorLevel(int Level);
	Real       getFloorYPosition();

	// Gestion des Portes
	bool       openDoor(Door* Porte);
	bool       openDoor(String DoorName);

	// Gestion des waypoints
	SceneNode* findNextWaypointTo(SceneNode* FromNode,SceneNode* ToNode);
	SceneNode* getClosestWaypoint(Vector3 CurrentPos);
	bool       doClosestWaypointAction(Vector3 CurrentPos);

	// Gestion des pièces
	Room*      getActiveRoom();
	void       showActiveRoom(SceneNode* Origin);
	void       showActiveRoom(String RoomName);
	Room*      getRoom(String RoomName);

	// Gestion de l'état du vaisseau
	String     getStatus();
	short      getAlertLevel();
	void       setRedLight(bool ON);
	void       setAlertLevel(short alert);
	int        getAmbiantTemperature();
	Real       getCurRadiation();
	Real       getMaxRadiation();

	// Gestion des différents elements du vaisseau
	void       loadObject();
	void       updateElements_Day();
	void       updateElements_Hour(Real ElapsedHours);
	void       addPassenger();

	// Gestion des mouvements du vaisseau
	void       initOISSystem();
	void       fly(Real ElapsedHours);
	void       _setPosition(Real X, Real Y, Real Z);
	Vector3    _getPosition();

	// Fonctions de debug
	void       _showWPnetwork();
	Vector3    _getCamPosition(int Level);
	void       _showAllLightPosition();

	// Pour le singleton
	static TheShip* getSingletonPtr(void);

	// Les éléments producteurs
	ShipElement* mHydProdSystem1;     ///< Fabrication de plasma a partir de l'Hydrogène de l'espace
	ShipElement* mHydProdSystem2;     ///< Fabrication de plasma a partir de l'Hydrogène de l'espace
	ShipElement* mEnergyProdSystem;   ///< Système de production d'Energie à partir du Plasma
	ShipElement* mAirProdSystem;      ///< Système de fabrication d'air respirable
	ShipElement* mFoodProdSystem;     ///< Système de fabrication de provisions alimentaire
	ShipElement* mWaterProdSystem;    ///< Système de fabrication de  d'eau douce
	ShipElement* mHybernationSystem;  ///< TODO
	ShipElement* mAntigravitySystem;  ///< TODO
	ShipEngine*  mEngine1;            ///< Produit de la poussée à partir de plasma
	ShipEngine*  mEngine2;            ///< Produit de la poussée à partir de plasma
	ShipElement* mReactor;            ///< Utile pour transformer la poussée en accélation ???

	Real         mCurRadiation;       ///< Radiation Ambiante (en rad/jour)
	Real         mMaxRadiation;       ///< Radiation Ambiante Max (en rad/jour)
	Real         mExternalTemp;       ///< TODO: Température Exterieure

	TextList*    mAlertBook;          ///< Journal des Alertes
	TextList*    mLogBook;            ///< Journal de Bord
	TextList*    mCrewBook;           ///< Journal d'équipage
	TextList*    mMissionBook;        ///< Journal des missions

	ShipLight*   mSpot1;              ///< Les projecteurs exterieurs
	ShipLight*   mSpot2;              ///< Les projecteurs exterieurs

protected:

	/* -------------------------- */
	// Input Library
	/* -------------------------- */
	OIS::JoyStick* mJoyStick;
	OIS::JoyStick* getJoyStick();
	void           readJoyStick(Real elapsedTime);
	bool buttonPressed (const OIS::JoyStickEvent &e, int buttonId);
	bool buttonReleased(const OIS::JoyStickEvent &e, int buttonId);
	bool axisMoved     (const OIS::JoyStickEvent &e, int AxisId);
	bool povMoved      (const OIS::JoyStickEvent &e, int PovId);
//	Real minX, maxX, minY, maxY, minZ, maxZ, rX, rY, rZ, sX, sY, sZ;
	bool mJoyDisabled;

	SceneNode*       mFloorNode;           ///< Le niveau du sol pour la camera
	ShipLevel*       mLevels[MAX_LEVELS];  ///< Le vaisseau comporte MAX_LEVELS étages
	Room*            mActiveRoom;          ///< Piece dans laquelle se trouve la camera
	WaypointNetwork* mShipNetwork ;        ///< Le réseau de waypoints du vaisseau
	ParticleSystem*  mSpaceDust;           ///< Source de particules (SpaceDust)
	SceneNode*       mParticleNode;        ///< Node pour la sources des particules

	// Membres
	Real     mDelayDoorClosed;          ///< Delai restant avant l'ordre de fermeture des portes

	// Gestion du vaisseau
	/// Niveaux d'alerte. 0=normal. 2=collision 4=captain 5=evacuation
	short    mAlertLevel;               ///< Degre d'alerte. 
	int      mAmbiantTemperature;       ///< Temperature ambiante dans le vaisseau (Kelvin)
	bool     mPiloting;                 ///< VRAI si le joystick est actif

	// Les éléments consomateurs
	ShipElement* mPassenger1;
	ShipElement* mPassenger2;
};

#endif 
