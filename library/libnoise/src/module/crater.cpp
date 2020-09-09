/***********************************************************************/
// File   : Crater.cpp
// Author : David de Lorenzo
// Project: Addon module for libnoise
// Date   : July 19 2005
/***********************************************************************/

#include "crater.h"
#include "..\misc.h"
#include "..\mathconsts.h"

#include <conio.h>			// debug console 

#define INDEX(x,y)    (x+y*m_Xsize)
#define ZERO_LEVEL    0         // Central value of a short 
#define MAX_LEVEL     32767     // max value of a short 


using namespace noise::module;

/***********************************************************************/
/// Contructor
/***********************************************************************/
Crater::Crater() :
	Module (GetSourceModuleCount ())
{
	m_Xsize  = DEFAULT_X_SIZE;
	m_Ysize  = DEFAULT_Y_SIZE;
	this->SetSeed(DEFAULT_CRATER_SEED);
	this->SetXsize(DEFAULT_X_SIZE);
	this->SetYsize(DEFAULT_Y_SIZE);
}


/***********************************************************************/
/// Destructor
/***********************************************************************/
Crater::~Crater()
{
	// Free the Elevation array
	delete [] mCraterMap;
}

/***********************************************************************/
/// Sets the seed value used by the Crater function.
/// @param seed The seed value, to initialize the randomizer.
/***********************************************************************/
void Crater::SetSeed (int seed)
{
	m_Seed = seed;
	srand(m_Seed);
}


/***********************************************************************/
/// Return a random number between low and high.
/**
	@param fLow The minimum value requested for the random real number (included).
	@param fHigh The maximum value requested for the random real number (excluded).
*/
/***********************************************************************/
double Crater::RangeRandom(double fLow, double fHigh)
{
	// r is a random floating point value in the range [0,1[ {including 0, not including 1}. 
	double r = ( (double)rand() / ((double)(RAND_MAX)+(double)(1)) ); 
	double range = fHigh - fLow;
	double value = (r * range); //  random floating point value in the range [0..range[ 
	return (fLow + value);
	// Note that we must convert rand() and RAND_MAX+1 to floating point values 
	// to avoid integer division. 
	// In addition, it is possible that RAND_MAX may be the largest positive 
	// integer the architecture can represent, 
	// so (RAND_MAX+1) may result in an overflow, or more likely the value will 
	// end up being the largest negative integer the architecture can represent, 
	// so to avoid this we convert RAND_MAX and 1 to doubles before adding.
}

/***********************************************************************/
/// Returns the seed value used by the crater generating function.
/** @return The seed value. */
/***********************************************************************/
int Crater::GetSeed() const
{
	return m_Seed;
}

/***********************************************************************/
/// Sets the width of the map where the craters will be generated.
/** @param Xsize The width value. Big value vill need large amount of memory. */
/***********************************************************************/
void Crater::SetXsize (int Xsize)
{
	m_Xsize = Xsize;
	m_MapLength = m_Xsize*m_Ysize;
}

/***********************************************************************/
/// Returns the width of the map.
/** @return The width value. */
/***********************************************************************/
int Crater::GetXsize () const
{
	return m_Xsize;
}

/***********************************************************************/
/// Sets the height of the map where the craters will be generated
/** @param Ysize The height value. Big value vill need large amount of memory. */
/***********************************************************************/
void Crater::SetYsize (int Ysize)
{
	m_Ysize = Ysize;
	m_MapLength = m_Xsize*m_Ysize;
}

/***********************************************************************/
/// Returns the height of the map.
/** @return The height value. */
/***********************************************************************/
int Crater::GetYsize () const
{
	return m_Ysize;
}


/***********************************************************************/
/// Generate cratered terrain with N craters.
/** @param NbCraters The number of craters on the map. */
/***********************************************************************/
void Crater::Generate(long NbCraters)
{
    /// Allocate the elevation array
	mCraterMap = new short[m_MapLength];

	this->SetSeed(m_Seed);

	/* Initialise the map to a mean elevation. */
	for (int index=0; index<m_MapLength; index++)
	{
		mCraterMap[index] = ZERO_LEVEL;
	}

	/* ************************************************************************ */
	/* Every time we go around this loop we plop another crater on the surface.	*/
	/* ************************************************************************ */
	double gmax = m_Xsize/42;  // the max crater size authorized

	for (long n=0; n<NbCraters; n++)
	{
		/* We assume the impact area is a fraction of the total crater size. */
		/// Repartition law between small and large craters.
		/// (based on astronomical observation . Equation from Rudy.)
		double g = sqrt(1 / (PI * (1 - RangeRandom(0, 0.9999))));
		if (g>gmax) g=gmax-8;   // we dont want too big craters

		// Random position for the center of the crater
		// (not too close of the edge of the map)
		int cx = (int)this->RangeRandom(g, m_Xsize-g);
		int cy = (int)this->RangeRandom(g, m_Ysize-g);

		// _cprintf("Plop a crater at %d %d with g=%f \n",cx,cy,g);	// debug console

		if (g < 1)	
		{ /* no crater */	}
		else if (g < 3)		
		{
			// tiny crater.
			this->GenerateSmallCrater(cx, cy);
		}
		else
		{
			// Regular crater.
			this->GenerateRegularCrater(g, cx, cy);
		}
	}
}




/***********************************************************************/
///  Generate a regular crater
/**
	@param g	Factor of the crater. Usually > 3.
	@param cx	X position of the crater center.
	@param cy	Y position of the crater center.
*/
/***********************************************************************/
#define rollmin        0.4     // the slope of the crater: 0.2(slow) to 0.9(steep)
#define CdepthPower    1.4
#define DepthBias      0.7071   // = sqrt(0.5)
#define CraterHeigth   400
void Crater::GenerateRegularCrater(double g, int cx, int cy)
{
	long cumuled_elevation = 0;
	long mean_elevation = 0;
	int  nb_points = 0;
	int  x, y;

	//_cprintf("GenerateRegularCrater \n");	// debug console

   	/// Determine the mean elevation around the crater center.
	int area = MaxInt(2, (double)(g/3));	

	for (y=cy-area; y<=cy+area; y++)
	{
		for (x=cx-area; x<=cx+area; x++)
		{
			cumuled_elevation += mCraterMap[INDEX(x, y)];
			nb_points++;
		}
   	}
	mean_elevation = cumuled_elevation / nb_points;
	//_cprintf("mean_elevation= %u \n",mean_elevation);	// debug console

	/// Calculate and set the elevation for every point of the crater
	int radius = MaxInt(2, g);

	for (y = cy-radius; y <= cy+radius; y++)
	{
		double dy = (cy - y) / (double)radius;
		double dysq = dy * dy;
		for (x = cx-radius; x <= cx+radius; x++)
		{
			short old_elevation  = mCraterMap[INDEX(x, y)];
			//_cprintf("old elevation(%i,%i)= %u \n",x,y,old_elevation);	// debug console
			double dx  = (cx - x) / (double)radius;
			double dxsq= dx * dx;
			double cd  = dxsq + dysq;
			double cd2 = 2.25 * cd;
			double cz  = 0;
			if (cd2 < 1.5) 
			{
				double tcz = DepthBias - sqrt(fabs(1 - cd2));
				cz = tcz * pow(radius, CdepthPower);
			}

			// Add a small bump in the middle of the crater
			if (dy==0 && dx==0 && cz<1)	
			{
				cz = (cz<0)? -0.1 : 0.1;
			}

			cz *= CraterHeigth;  

			//_cprintf("delta elevation cz= %lf \n",cz);	// debug console
			double roll  = (((1 / (1-MinReal(rollmin, cd))) / (1 / (1-rollmin))) - (1-rollmin)) / rollmin;
			double iroll = 1 - roll;
			double aw    = cz + old_elevation*roll + mean_elevation*iroll;
			short new_elevation = static_cast<short>(aw);
			if (new_elevation>MAX_LEVEL)   new_elevation=MAX_LEVEL;
			if (new_elevation<-MAX_LEVEL)  new_elevation=-MAX_LEVEL;
			mCraterMap[INDEX(x, y)] = new_elevation;
			//_cprintf("new elevation= %u \n",new_elevation);	// debug console
		}
	}
}


/***********************************************************************/
/// This function generate a small crater on the map (1 pixel radius).
/**
	@param cx	X position of the crater center.
	@param cy	Y position of the crater center.
*/
/***********************************************************************/
void Crater::GenerateSmallCrater(int cx, int cy)
{
	int   x, y;
	long  cumuled_elevation = 0;
	short mean_elevation  = 0;
	short delta_elevation = 0;
	short nb_points       = 0;

	// _cprintf("GenerateSmallCrater \n");	// debug console

	// calculate the average elevation of the center's Moore neighbourhood.
	for (y = cy-1; y <= cy+1; y++)
	{
		for (x = cx-1; x <= cx+1; x++)
		{
			cumuled_elevation += mCraterMap[INDEX(x, y)];
			nb_points++;
		}
	}
	mean_elevation = (short)(cumuled_elevation/nb_points);
	//_cprintf("mean_elevation= %u \n",mean_elevation);	// debug console

	// The crater depth vary between -2000 and +500.
	delta_elevation = (short)this->RangeRandom(-2000,500);	
	//_cprintf("delta_elevation= %i \n",delta_elevation);	// debug console

	// Set the new center elevation
	mCraterMap[INDEX(cx, cy)] = mean_elevation + delta_elevation;
}


/***********************************************************************/
/// This Generator Module dont need any source
/***********************************************************************/
int Crater::GetSourceModuleCount () const
{
	return 0;
}


/***********************************************************************/
/// Return a value from the map
/***********************************************************************/
double Crater::GetValue(double x, double z, double y) const
{
	/// y parameter is ignored.
	// We transform x [-1..1] into [0..m_Xsize]
	int X = (int) ((x+1)*(m_Xsize/2));
	int Y = (int) ((y+1)*(m_Ysize/2));	
	int Z = (int) ((z+1)*(m_Xsize/2));	

	X = X % m_Xsize;
	Y = Y % m_Ysize;
	Z = Z % m_Xsize;

	short elevation = mCraterMap[INDEX(X,Y)];

	/// We convert from -32767...32767 to -1...+1
	double value = (double)elevation / MAX_LEVEL;

//	if (x==17) _cprintf("value(%lf,%lf) = %u = %lf \n",x,z,value,elevation);	// debug console
	return value;
}

/***********************************************************************/
/// Return the lower value
/***********************************************************************/
int Crater::MinInt(double value1, double value2)
{
	if (value1<value2) return (int)value1; 
	else return (int)value2;
}

/***********************************************************************/
/// Return the bigger value
/***********************************************************************/
int Crater::MaxInt(double value1, double value2)
{
	if (value1>value2) return (int)value1; 
	else return (int)value2;
}


/***********************************************************************/
/// Return the bigger value
/***********************************************************************/
double Crater::MaxReal(double value1, double value2)
{
	if (value1>value2) return value1; 
	else               return value2;
}


/***********************************************************************/
/// Return the lower value
/***********************************************************************/
double Crater::MinReal(double value1, double value2)
{
	if (value1<value2) return value1; 
	else               return value2;
}
