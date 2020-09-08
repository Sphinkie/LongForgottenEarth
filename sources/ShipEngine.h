/* ------------------------------------------------------------------------- */
// File       : ShipEngine.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SHIP_ENGINE_H_
#define _SHIP_ENGINE_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ShipElement.h"


#define MAX_ENGINE_TEMP 10000	/// Au dela de cette temperature, les moteurs s'abiment
#define MIN_ENGINE_TEMP 800	    /// Au dessous de cette temperature, les moteurs ne démarrent pas

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// ShipEngine est un ShipElement particulier du vaisseau: le Moteur 
/* ------------------------------------------------------------------------- */
/** Il a quelques propriétés en plus par rapport aux elements habituels: 
 - la gestion de la température, et du préchauffage,
 - la gestion d'un coefficient modérateur de l'activité: le mode
 - La possibilité d'être arrêté (=en mode STOPPED)
 - La possibilité de valeurs négatives (retro poussée)
 - Pour ce qui est des propriétes héritées de ShipElement:
 - Son Activity correspond à la puissance demandée au moteur
 - Il consomme des moles d'hydrogène
 - Sa Value correspond à de la poussée (kN) qui est convertie en accélération par le Ship lui-même. */
/* ------------------------------------------------------------------------- */
class ShipEngine : public ShipElement  
{
public:
	enum tMode {STOPPED_ENGINE, IDLE_ENGINE, HEATING_ENGINE, ASTEROID_MODE, PLANETARY_MODE, SOLAR_MODE, FULL_ENGINE };

	ShipEngine(String name, Real minValue, Real maxValue, String Unit, Real HourIncrement);
	virtual ~ShipEngine();
	void    updateTemperature(Real ElapsedHours);
	void    updateValue_Hour(Real ElapsedTime);	
	void    updatePower(Real ElapsedTime);
	void    setTemperature(Real temperature);
	void    setHeatingEngine(bool On);
	void    setActivity(Real pourcentage);
	void    setPushDirection(int dir);
	bool    isEngineHeating();
	bool    isEngineStopped();
	Real    getTemperature();
	Real    getValue();
	void    setMode(tMode mode);
	tMode   getMode();
	String  getModeName();
	String  getModeName(tMode mode);
	Real    getModeCoeff();
	Real    getModeCoeff(tMode mode);
	Real    getRealActivity();
	void    switchActivity();
	void    switchHeating();

protected:
	Real    mEngineTemperature;      /// Temperature du Moteur  en degres K
	tMode   mMode;                   /// Mode général du Moteur (short range travel, long range travel, etc)
	Real    mModeCoeff;              /// Coefficient de réduction de la puissance du moteur en fonction de son mode.
	int     mEngineSound;            /// Channel du son du moteur
	int     mForward;                /// Pousée=1  Retropoussée=-1
};

#endif 
