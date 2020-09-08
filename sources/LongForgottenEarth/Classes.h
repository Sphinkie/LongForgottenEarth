/* ------------------------------------------------------------------------- */
// File       : classes.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _CLASSES_H_
#define _CLASSES_H_


/* ------------------------------------------------------------------------- */
/// Noms par defaut de quelques Elements de le Scene
/* ------------------------------------------------------------------------- */
#define ROOMLIGHT		"roomLight"
#define LEVEL_HIGH      250

/* ------------------------------------------------------------------------- */
/// Flags pour les rayons
/* ------------------------------------------------------------------------- */
#define ACTIVABLE       1<<0	/// Les elements sur lesquels on peut cliquer 
#define WALKABLE        1<<1	/// Les plans "floor" sur lesquels on peut marcher
#define ROOMS           1<<2	/// Les pieces du vaisseau
#define VOIDOBJECT      1<<3	/// Les elements inconsistants (fumees, etc)
#define PNJ             1<<4	/// Les NPC
#define IMMOBILE        1<<5	/// Les objets simples de type mobilier (chaise, etc)
#define RENDERABLE      1<<6	/// 

/* ------------------------------------------------------------------------- */
/// ID des classes UserDefineObject
/* ------------------------------------------------------------------------- */


#define NULL_ID		    0
#define ROBOT_ID		1
#define THESHIP_ID		2
#define ROOM_ID			3
#define LEVEL_ID		4
#define ARCHIPEL_ID		5
#define ASTEROIDE_ID	6
#define GALAXIE_ID		7
#define SOLARSYS_ID		8
#define PLANETE_ID		9
#define DOOR_ID 		10
#define ANIMOBJECT_ID	11	
#define HIBERNATION_ID	12
#define BODY_ID	        13
#define COMPUTER_ID	    14
#define BUTTON_ID	    15


#endif
