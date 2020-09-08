/* ------------------------------------------------------------------------- */
// File       : constellation.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
// Author     : Chris Laurel
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
// Copyright (C) 2001,  <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
/* ------------------------------------------------------------------------- */

#include "constellation.h"


static struct Constellation::t_constellation constellationInfo[] = 
{
    { "Aries", "Arietis", "Ari" },
    { "Taurus", "Tauri", "Tau" },
    { "Gemini", "Geminorum", "Gem" },
    { "Cancer", "Cancri", "Cnc" },
    { "Leo", "Leonis", "Leo" },
    { "Virgo", "Virginis", "Vir" },
    { "Libra", "Librae", "Lib" },
    { "Scorpius", "Scorpii", "Sco" },
    { "Sagittarius", "Sagittarii", "Sgr" },
    { "Capricornus", "Capricornii", "Cap" },
    { "Aquarius", "Aquarii", "Aqr" },
    { "Pisces", "Piscium", "Psc" },
    { "Ursa Major", "Ursae Majoris", "UMa" },
    { "Ursa Minor", "Ursae Minoris", "UMi" },
    { "Bootes", "Bootis", "Boo" },
    { "Orion", "Orionis", "Ori" },
    { "Canis Major", "Canis Majoris", "CMa" },
    { "Canis Minor", "Canis Minoris", "CMi" },
    { "Lepus", "Leporis", "Lep" },
    { "Perseus", "Persei", "Per" },
    { "Andromeda", "Andromedae", "And" },
    { "Cassiopeia", "Cassiopeiae", "Cas" },
    { "Cepheus", "Cephei", "Cep" },
    { "Cetus", "Ceti", "Cet" },
    { "Pegasus", "Pegasi", "Peg" },
    { "Carina", "Carinae", "Car" },
    { "Puppis", "Puppis", "Pup" },
    { "Vela", "Velorum", "Vel" },
    { "Hercules", "Herculis", "Her" },
    { "Hydra", "Hydrae", "Hya" },
    { "Centaurus", "Centauri", "Cen" },
    { "Lupus", "Lupi", "Lup" },
    { "Ara", "Arae", "Ara" },
    { "Ophiuchus", "Ophiuchi", "Oph" },
    { "Serpens", "Serpentis", "Ser" },
    { "Aquila", "Aquilae", "Aql" },
    { "Auriga", "Aurigae", "Aur" },
    { "Corona Australis", "Coronae Australis", "CrA" },
    { "Corona Borealis", "Coronae Borealis", "CrB" },
    { "Corvus", "Corvi", "Crv" },
    { "Crater", "Crateris", "Crt" },
    { "Cygnus", "Cygni", "Cyg" },
    { "Delphinus", "Delphini", "Del" },
    { "Draco", "Draconis", "Dra" },
    { "Equuleus", "Equulei", "Equ" },
    { "Eridanus", "Eridani", "Eri" },
    { "Lyra", "Lyrae", "Lyr" },
    { "Piscis Austrinus", "Piscis Austrini", "PsA" },
    { "Sagitta", "Sagittae", "Sge" },
    { "Triangulum", "Trianguli", "Tri" },
    { "Antlia", "Antliae", "Ant" },
    { "Apus", "Apodis", "Aps" },
    { "Caelum", "Caeli", "Cae" },
    { "Camelopardalis", "Camelopardalis", "Cam" },
    { "Canes Venatici", "Canum Venaticorum", "CVn" },
    { "Chamaeleon", "Chamaeleontis", "Cha" },
    { "Circinus", "Circini", "Cir" },
    { "Columba", "Columbae", "Col" },
    { "Coma Berenices", "Comae Berenices", "Com" },
    { "Crux", "Crucis", "Cru" },
    { "Dorado", "Doradus", "Dor" },
    { "Fornax", "Fornacis", "For" },
    { "Grus", "Gruis", "Gru" },
    { "Horologium", "Horologii", "Hor" },
    { "Hydrus", "Hydri", "Hyi" },
    { "Indus", "Indi", "Ind" },
    { "Lacerta", "Lacertae", "Lac" },
    { "Leo Minor", "Leonis Minoris", "LMi" },
    { "Lynx", "Lyncis", "Lyn" },
    { "Microscopium", "Microscopii", "Mic" },
    { "Monoceros", "Monocerotis", "Mon" },
    { "Mensa", "Mensae", "Men" },
    { "Musca", "Muscae", "Mus" },
    { "Norma", "Normae", "Nor" },
    { "Octans", "Octantis", "Oct" },
    { "Pavo", "Pavonis", "Pav" },
    { "Phoenix", "Phoenicis", "Phe" },
    { "Pictor", "Pictoris", "Pic" },
    { "Pyxis", "Pyxidis", "Pyx" },
    { "Reticulum", "Reticuli", "Ret" },
    { "Sculptor", "Sculptoris", "Scl" },
    { "Scutum", "Scuti", "Sct" },
    { "Sextans", "Sextantis", "Sex" },
    { "Telescopium", "Telescopii", "Tel" },
    { "Triangulum Australe", "Trianguli Australis", "TrA" },
    { "Tucana", "Tucanae", "Tuc" },
    { "Volans", "Volantis", "Vol" },
    { "Vulpecula", "Vulpeculae", "Vul" }
};

static struct Constellation::t_abbrev GreekAlphabet[] = 
{
    { "Alpha",   "ALF"},
    { "Beta",    "BET"}, 
    { "Gamma",   "GAM"}, 
    { "Delta",   "DEL"}, 
    { "Epsilon", "EPS"}, 
    { "Zeta",    "ZET"}, 
    { "Eta",     "ETA"}, 
    { "Theta",   "TET"},
    { "Iota",    "IOT"},
    { "Kappa",   "KAP"}, 
    { "Lambda",  "LAM"}, 
    { "Mu",      "MU" }, 
    { "Nu",      "NU" }, 
    { "Xi",      "XI" }, 
    { "Omicron", "OMI"}, 
    { "Pi",      "PI" },
    { "Rho",     "RHO"}, 
    { "Sigma",   "SIG"}, 
    { "Tau",     "TAU"}, 
    { "Upsilon", "UPS"}, 
    { "Phi",     "PHI"}, 
    { "Chi",     "CHI"}, 
    { "Psi",     "PSI"}, 
    { "Omega",   "OME"}
};

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
Constellation::Constellation()
{}


/* ------------------------------------------------------------------------- */
/// Renvoie l'ID d'une constellation
/** Le nom doit correspondre exactement à l'un des trois champs. */
/* ------------------------------------------------------------------------- */
int Constellation::getConstellation(String name)
{
	int NumberOfConstellations = sizeof(constellationInfo) / sizeof(constellationInfo[0]);
    for (int i=0; i<NumberOfConstellations; i++)
    {
        if ((name == constellationInfo[i].abbr)  ||
            (name == constellationInfo[i].genitive)||
            (name == constellationInfo[i].name) )
        {
            return i;
        }
    }
    return -1;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le nom latin d'une constellation.
/* ------------------------------------------------------------------------- */
String Constellation::getName(int index)
{
    return constellationInfo[index].name;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le nom latin (au genitif) d'une constellation.
/* ------------------------------------------------------------------------- */
String Constellation::getGenitive(int index)
{
    return constellationInfo[index].genitive;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le nom abbrégé d'une constellation.
/* ------------------------------------------------------------------------- */
String Constellation::getAbbreviation(int index)
{
    return constellationInfo[index].abbr;
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'ID d'une constellation
/* ------------------------------------------------------------------------- */
String Constellation::getGreekLetter(String abbrev)
{
	StringUtil::toUpperCase(abbrev);

    for (int i=0; i<23; i++)
    {
        if (abbrev == GreekAlphabet[i].abbr)
        {
            return GreekAlphabet[i].name;
        }
    }
    return "";
}


