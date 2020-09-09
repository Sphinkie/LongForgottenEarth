/***********************************************************************/
// File   : crater.h
// Author : David de Lorenzo
// Project: Addon for libnoise
// Date   : July 19 2005
/***********************************************************************/
#ifndef NOISE_MODULE_CRATER_H
#define NOISE_MODULE_CRATER_H

#include "modulebase.h"

namespace noise
{
  namespace module
  {

    /// @addtogroup libnoise
    /// @{

    /// @addtogroup modules
    /// @{

    /// @defgroup generatormodules Generator Modules
    /// @addtogroup generatormodules
    /// @{

	/// Default noise seed for the Crater noise module.
	const int DEFAULT_CRATER_SEED = 0;      
	/// Default map X size
	const int DEFAULT_X_SIZE = 512;         
	/// Default map Y size
	const int DEFAULT_Y_SIZE = 256;         

	/***********************************************************************/
	/// Noise module that outputs a map filled with lunar craters.
	///
	/// @image html modulecrater.png
	///
	/// You can set the dimensions of the map with SetXsize() and SetYsize() methods.
	/// You need to call the Generate() method once, before any call of GetValue().
	/// This noise module does not require any source modules.
	///
	/// Credits: John Walker(1989)  David de Lorenzo(2005)
	/***********************************************************************/
	class Crater: public Module
	{
	public:
		Crater();
		~Crater();       
		virtual int    GetSourceModuleCount () const;
		virtual double GetValue (double x, double y, double z) const;
		void   SetSeed(int seed);
		int    GetSeed() const;
		void   SetXsize(int Xsize);
		int    GetXsize() const;
		void   SetYsize(int Ysize);
		int    GetYsize() const;
		void   Generate(long NbCraters);

	protected:
		double RangeRandom(double fLow, double fHigh);
		void   GenerateSmallCrater  (int cx, int cy);
		void   GenerateRegularCrater(double g, int cx, int cy);
		int    MaxInt (double value1, double value2);
		double MaxReal(double value1, double value2);
		int    MinInt (double value1, double value2);
		double MinReal(double value1, double value2);

		int  m_Xsize;                ///< Map width size	
		int  m_Ysize;                ///< Map height size
		int  m_Seed;                 ///< Seed value used by the Crater functions.
		long m_MapLength;            ///< Total nomber of item of the map array
		short* mCraterMap;           ///< Dynamic Array for the crater map
    };

    /// @}
    /// @}
    /// @}
  }
}

#endif



