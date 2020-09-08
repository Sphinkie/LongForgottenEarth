/* ------------------------------------------------------------------------- */
//  File       : ShipElement.cpp
//  Project    : Long Forgotten Earth
//  Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "macros.h"
#include "ShipElement.h"

#undef  TRACE
#define TRACE(X)


/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
ShipElement::ShipElement(String name, Real minValue, Real maxValue, String Unit, Real HourIncrement)
{
	// On m�morise les param�tres g�n�raux
	mName         = name;
	mMinValue     = minValue;
	mMaxValue     = maxValue;
	mDayIncrement = HourIncrement * 24;
	mHourIncrement= HourIncrement;
	mUnit         = " "+Unit;

	// On initialise les membres
	mBFpercentage = 1;		// entre 0 et 1
	mCurrentValue = 0;
//	mActivity     = 0.00;	// initialement au repos = 0%	/// DEBUG
	mActivity     = 1.00;	// initialement actif = 100%
	mSpare        = NOTHING;

	for (int i=0; i<MAX_RESOURCES; i++)
	{
		mResourceQty[i] = 0;
		mResourcePtr[i] = NULL;
	}
}

/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
ShipElement::~ShipElement()
{
}


/* ------------------------------------------------------------------------- */
/// On met � jour la valeur en fonction d'un certain nombre de jours �coul�s
/* ------------------------------------------------------------------------- */
void ShipElement::updateValue_Day(int number_of_days)
{
	Real RatioQty=1;		// Ration des ressources obtenues par rapport aux ressources n�c�ssaires

	/// Si le reservoir est plein, on ne produit plus
	if (mCurrentValue>=mMaxValue) return;

	if (this->isBroken())   
	{
		// Si completement cass�, on ne consomme rien
		RatioQty=0;
	}
	else 
	{
		// Sinon on cosomme des resources autres
		for (int ResourceIndex=0; ResourceIndex<MAX_RESOURCES; ResourceIndex++)
		{
			/// La production est aussi temp�r�e par la quantit� de resources disponibles
			RatioQty *= this->consumeResource(ResourceIndex);
		}
	}

	/// Si l'�l�ment est endommag�, l'�volution de la r�serve est diminu�e.
	this->addValue(number_of_days * mBFpercentage * mDayIncrement * RatioQty * mActivity);
	TRACE("   "+mName+"::updated value is "+StringConverter::toString(mCurrentValue) + mUnit);
}



/* ------------------------------------------------------------------------- */
/// On met � jour la valeur en fonction d'un certain nombre de fraction d'heures �coul�es
/* ------------------------------------------------------------------------- */
void ShipElement::updateValue_Hour(Real ElapsedTime)
{
	Real RatioQty=1; // Ration des ressources obtenues par rapport aux ressources n�c�ssaires

	TRACE (">> "+mName+"::updateValue_Hour()");
	/// Si le reservoir est plein, on ne produit plus
	if (mCurrentValue>=mMaxValue) return;

	if (this->isBroken())   
	{
		// Si completement cass�, on ne consomme rien
		RatioQty=0;
	}
	else 
	{
		// Sinon on cosomme des resources autres
		for (int ResourceIndex=0; ResourceIndex<MAX_RESOURCES; ResourceIndex++)
		{
			/// La production est aussi temp�r�e par la quantit� de resources disponibles
			RatioQty *= this->consumeResource(ResourceIndex, ElapsedTime);
		}
	}

	/// Si l'�l�ment est endommag�, l'�volution de la r�serve est diminu�e.
	this->addValue(mHourIncrement * ElapsedTime * mBFpercentage * RatioQty * mActivity);
	// TRACE("   mHourIncrement ="+StringConverter::toString(mHourIncrement) + mUnit);
	// TRACE("   ElapsedTime    ="+StringConverter::toString(ElapsedTime) + " hours");
	TRACE("   RatioQty       ="+StringConverter::toString(100*RatioQty) + " %");
	TRACE("   mActivity      ="+StringConverter::toString(100*mActivity) + " %");
	TRACE("   "+mName+"::updated value is "+StringConverter::toString(mCurrentValue) + mUnit);
}


/* ------------------------------------------------------------------------- */
/// Gestion de la consommation des resources d'autres ShipElement
/* ------------------------------------------------------------------------- */
Real ShipElement::consumeResource(int ResourceIndex, Real ElapsedHours)
{
	Real Ratio = 0;

	if(!mResourcePtr[ResourceIndex])
	{
		// Si on ne d�pend pas de cet autre element: on ne consomme rien
		Ratio =1;
	}
	else
	{
		// Sinon, on consomme une partie de cette resource
		// La consommation est proportionnelle � l'activit� de l'�l�ment
		// La consommation est proportionelle au temps �coul� depuis la derni�re consommation
		// Les dommages ne r�duisent pas la consommation.
		Real NeededResources   = mResourceQty[ResourceIndex] * mActivity * ElapsedHours;
		Real ObtainedResources = mResourcePtr[ResourceIndex]->subValue(NeededResources);

		if (NeededResources!=0) Ratio =  ObtainedResources / NeededResources;
		TRACE(">> "+mName+"::consumeResource: ask=" + StringConverter::toString(NeededResources) +" got="+StringConverter::toString(ObtainedResources) + mResourcePtr[ResourceIndex]->getUnit()+" from '"+mResourcePtr[ResourceIndex]->getName()+"'");
		TRACE("   ratio= "+StringConverter::toString(Ratio));
	}
	return Ratio;
}


/* ------------------------------------------------------------------------- */
/// On renvoie la valeur courante de cet �l�ment (en valeur absolue)
/* ------------------------------------------------------------------------- */
Real ShipElement::getValue()
{
	return mCurrentValue;
}


/* ------------------------------------------------------------------------- */
/// Initialise la valeur courante de cet �l�ment (en valeur absolue)
/* ------------------------------------------------------------------------- */
void ShipElement::setValue(Real value)
{
	mCurrentValue = value;
}

/* ------------------------------------------------------------------------- */
/// On renvoie la valeur courante de cet �l�ment (de 0 � 1)
/* ------------------------------------------------------------------------- */
Real ShipElement::getPourcentageValue()
{
	return (Real) (mCurrentValue-mMinValue)/(mMaxValue-mMinValue) ;
}

/* ------------------------------------------------------------------------- */
/// Ajoute une partie de la valeur de cet �l�ment. On renvoie la valeur r�ellement ajout�e.
/* ------------------------------------------------------------------------- */
Real ShipElement::addValue(Real addedValue)
{
	TRACE(">> "+mName+"::addValue "+StringConverter::toString(addedValue) + mUnit + " to " + StringConverter::toString(mCurrentValue));
	
	// Note: On doit faire de facon subtile en raison de la pr�cision des Real du C++
	// (il y a trop de diff�rences entre la Value, et la valeur � ajouter)
	Real PreviousValue = mCurrentValue;

	// En cas de d�passement de la limite sup�rieure
	if (mCurrentValue+addedValue > mMaxValue) 
	{
		mCurrentValue = mMaxValue;
		return (mMaxValue-PreviousValue);
	}

	// En cas de d�passement de la limite inf�rieure
	if (mCurrentValue+addedValue < mMinValue) 
	{
		mCurrentValue = mMinValue;
		return (mMinValue-PreviousValue);
	}

	// Dans les autres cas (pas de d�passement)
	mCurrentValue += addedValue;
	return (addedValue);
}


/* ------------------------------------------------------------------------- */
/// Soustrait une partie de la valeur de cet �l�ment
/* ------------------------------------------------------------------------- */
Real ShipElement::subValue(Real substractedValue)
{
	return (-addValue(-substractedValue));
}

/* ------------------------------------------------------------------------- */
/// L'�l�ment peut consommer des ressources produites par un (ou plusieurs) autres �l�ments
/* ------------------------------------------------------------------------- */
void ShipElement::setResourceElement(ShipElement* ShipResource, Real QuantityPerHour)
{
	int found = -1;

	// On trouve un emplacement libre dans le tableau pour stocker cette consommation
	for (int i=0; i<MAX_RESOURCES; i++)
	{
		// Si on a trouv� un emplacement libre, on l'utilise
		if (mResourcePtr[i]==NULL)         found=i;
		// Si on a trouv� que cette resource exitait d�j�, on la met � jour
		if (mResourcePtr[i]==ShipResource) found=i;
	}

	if (found != -1)
	{
		// Si on a trouv� un emplacement: on l'utilise
		mResourcePtr[found] = ShipResource;
		mResourceQty[found] = QuantityPerHour;
	}
	else
	{
		// Si on n'a pas trouv� d'emplacement, on met un warning
		DEBUG_LOG("Warning: "+mName+"::mResourcePtr Array is to small to store this Resource");
	}
}

/* ------------------------------------------------------------------------- */
/// On m�morise le niveau de d�gradation (entre 0 et 1) (1=100% damaged)
/* ------------------------------------------------------------------------- */
void ShipElement::setDamage (Real DamagePercentage)
{
	mBFpercentage = 1-DamagePercentage;
	if (mBFpercentage<0)  mBFpercentage=0;
	if (mBFpercentage>1)  mBFpercentage=1;
}

/* ------------------------------------------------------------------------- */
/// On augmente (ou diminue) le niveau de d�gradation (entre 0 et 1) (1=100% damaged)
/* ------------------------------------------------------------------------- */
void ShipElement::addDamage(Real DamagePercentage)
{
	mBFpercentage -= DamagePercentage;
	if (mBFpercentage<0)  mBFpercentage=0;
	if (mBFpercentage>1)  mBFpercentage=1;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le niveau de d�gradation
/* ------------------------------------------------------------------------- */
Real ShipElement::getDamage()
{
	return (1-mBFpercentage);
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si l'element est completement d�truit
/* ------------------------------------------------------------------------- */
bool ShipElement::isBroken()
{
	return (mBFpercentage == 0);
}


/* ------------------------------------------------------------------------- */
/// Memorise les spareparts n�cessaires � la r�paration de cet element quand il tombe en panne.
/* ------------------------------------------------------------------------- */
void ShipElement::defineSpare(tSpare spare)
{
	mSpare = spare;
}


/* ------------------------------------------------------------------------- */
/// TODO (Si l'�lement a un mesh associ�)
/* ------------------------------------------------------------------------- */
void ShipElement::setPosition(Vector3 pos)
{
}

/* ------------------------------------------------------------------------- */
/// Renvoie le Nom de cet Element
/* ------------------------------------------------------------------------- */
String ShipElement::getName()
{
	return mName;
}

/* ------------------------------------------------------------------------- */
/// Renvoie les Unit�s de cet Element
/* ------------------------------------------------------------------------- */
String ShipElement::getUnit()
{
	return mUnit;
}

/* ------------------------------------------------------------------------- */
/// TODO
/* ------------------------------------------------------------------------- */
String ShipElement::getStatus()
{
	TRACE (">> "+mName+"::getStatus()");
	String Xstatus = "";

	// On construit le status de l'�l�ment 
	// (si le % de Bon Fonctionnement n'est pas 100%)
	if (mBFpercentage < 100)
	{
		Xstatus  = "<item";
		Xstatus += " id='"+ITOA((int)this)  +"' ";
		Xstatus += " location='to be defined' ";
		Xstatus += " spares='"  +ITOA(mSpare)    +"' ";
		Xstatus += " ETR='"+ITOA(100-mBFpercentage)+"' ";		// "Estimated Time for Repair" selon le BF
		Xstatus += " text='This element is damaged and does not work properly. It should be repaired.' ";
		Xstatus += ">";
		Xstatus += mName;
		Xstatus += "</item> \n";
	}
	return Xstatus;
}

/* ------------------------------------------------------------------------- */
/// Change la valeur d'�volution quotidienne de l'�l�ment.
/* ------------------------------------------------------------------------- */
void ShipElement::setDayIncrementValue(Real dayIncrement)
{
	mDayIncrement = dayIncrement;
}

/* ------------------------------------------------------------------------- */
/// Change la valeur d'�volution continue de l'�l�ment.
/* ------------------------------------------------------------------------- */
void ShipElement::setHourIncrementValue(Real HourIncrement)
{
	mHourIncrement = HourIncrement;
}

/* ------------------------------------------------------------------------- */
/// D�termine l'activite de l'Element (de 0 � 1) (1=100% actif)
/* ------------------------------------------------------------------------- */
void ShipElement::setActivity(Real pourcentage)
{
	mActivity = pourcentage;
	if (mActivity >1) mActivity=1;
	if (mActivity <0) mActivity=0;
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'activit� de l'Element (de 0 � 1) (1=100% actif)
/* ------------------------------------------------------------------------- */
Real ShipElement::getActivity()
{
	return mActivity;
}

/* ------------------------------------------------------------------------- */
/// Renvoie la valeur maximum d�finie pour cet Element
/* ------------------------------------------------------------------------- */
Real ShipElement::getMaxValue()
{
	return mMaxValue;
}

/* ------------------------------------------------------------------------- */
/// Renvoie la valeur maximum que peut prendre cet Element (compte tenu des dommages)
/* ------------------------------------------------------------------------- */
Real ShipElement::getRealMaxValue()
{
	return (mMaxValue * mBFpercentage);
}

