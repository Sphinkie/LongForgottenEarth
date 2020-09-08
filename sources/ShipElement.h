/* ------------------------------------------------------------------------- */
//  File       : ShipElement.h
//  Project    : Long Forgotten Earth
//  Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SHIP_ELEMENT_H_
#define _SHIP_ELEMENT_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"
#include "spares.h"

using namespace Ogre;


#define MAX_RESOURCES 3		// Nombre max de resources que peut consommer un ShipElement

/* ------------------------------------------------------------------------- */
/// Objets du Vaisseau ayant un contenu variable avec le temps (par exemple: réserve d'energie)
/* ------------------------------------------------------------------------- */
/**
	Cet objet est un peu complexe car il peut servir à simuler des éléments variés 
du vaisseau spacial. 

  L'utilisation basique est le générateur d'energie. L'élément a:
 - une valeur courante qui représente le niveau d'énergie présent dans la réserve (les batteries)
 - des ressources exterieures, (par exemple, de l'hydrogène de l'espace)
 - une valeur d'increment: normalement, on emmagazine 300Watt par jour.
 - Pour cela, on a besoin d'une certaine quantité de ressources, par exemple: 300 moles d'H2.

  Si l'Element est endommagé, ou s'il ne trouve pas assez de resources, ou si on lmui demande de travailler à 50%
  (activity), son rendement baisse.

*/
/* ------------------------------------------------------------------------- */
class ShipElement  
{
public:
	ShipElement(String name, Real minValue, Real maxValue, String Unit, Real HourIncrement);
	virtual ~ShipElement();
	void    setValue(Real value);
	Real    getValue();
	String  getName();
	String  getUnit();
	Real    getPourcentageValue();
	void    updateValue_Day(int number_of_days = 1);
	void    updateValue_Hour(Real ElapsedTime);
	Real    addValue(Real addedValue);
	Real    subValue(Real subtractedValue);
	void    setResourceElement(ShipElement* ShipResource, Real QuantityPerHour);
	void    setDamage(Real DamagePercentage);
	void    addDamage(Real DamagePercentage);
	Real    getDamage();
	bool    isBroken();
	void    defineSpare(tSpare spare);
	void    setPosition(Vector3 pos);
	String  getStatus();
	void    setDayIncrementValue (Real dayIncrement);
	void    setHourIncrementValue(Real HourIncrement);
	void    setActivity(Real pourcentage);
	Real    getActivity();
	Real    getMaxValue();
	Real    getRealMaxValue();

protected:
	Real    consumeResource(int ResourceIndex, Real ElapsedHours = 24);

	String mName;                 /// Nom de la réserve
	String mUnit;                 /// Unité de la réserve
	Real   mActivity;             /// Pourcentage d'activité demandé: entre 0 et 1
	Real   mMinValue;             /// Valeur minimale de la réserve
	Real   mMaxValue;             /// Valeur maximale de la réserve
	Real   mCurrentValue;         /// Valeur courante de la réserve
	Real   mDayIncrement;         /// Augmentation normale de la réserve durant 1 journée
	Real   mHourIncrement;        /// Augmentation normale de la réserve durant 1 heure
	Real   mBFpercentage;         /// Pourcentage de bon fonctionnement: entre 0 et 1
	tSpare mSpare;                /// liste des spareparts nécéssaires en cas de panne

	ShipElement* mResourcePtr[MAX_RESOURCES]; /// Tableau des ressources nécéssaires à cette réserve
	Real         mResourceQty[MAX_RESOURCES]; /// Tableau des quantités de ressources nécessaires

};

#endif 
