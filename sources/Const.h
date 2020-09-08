/* ------------------------------------------------------------------------- */
// File       : const.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _CONST_H_
#define _CONST_H_

// Space constants

#define DARKER_VISIBLE_MAGNITUDE 5   // 6 avec une tres bonne vue.


#define JOUR_LUMIERE_AL          0.0027  // 1 jour.lum = 0.0027 AL
#define JOUR_LUMIERE_AL2         8e-6
//#define JOUR_LUMIERE_WU2         8e-2 // ???

#define PARSEC_TO_AL             3.26  // 1 parsec = 3.26 al
#define AL_TO_PARSEC             0.306 
#define AL2_TO_PC2               0.093636 // utile pour convertir des square-distances
#define KM_PER_PARSEC            3.0856e13
#define KM_PER_AL                1e13
#define KM_PER_AU                15e7

// Unités pour le jeu
#define SPACE_WU_PER_AL          1e5                         ///< 1al = 10.000wu
#define SPACE_WU_PER_KM          (SPACE_WU_PER_AL*KM_PER_AL)
#define SOLAR_WU_PER_KM          1000  // non
#define ORBIT_WU_PER_AU          15e4                        ///< 1ua = 15e7km = 15e4wu
#define ORBIT_KM_PER_WU          (KM_PER_AU/ORBIT_WU_PER_AU) ///< 1wu = 1000 km

// Dimensions d'un quadrant galactique
#define QUADRANT_X_SIZE          4000 // unit = AL
#define QUADRANT_Y_SIZE          1000 // unit = AL
#define QUADRANT_Z_SIZE          4000 // unit = AL
 
#endif
