/* ------------------------------------------------------------------------- */
// File       : SG_File_XML.cpp
// Project    : Star Gen
// Author C++ : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "SG_File_XML.h"

/*----------------------------------------------------------------------*/
/// Constructor
/*----------------------------------------------------------------------*/
SG_File_XML::SG_File_XML(SG_String filename, long seed) : SG_File(filename,seed)
{}

/*----------------------------------------------------------------------*/
/// Destructor. 
/*----------------------------------------------------------------------*/
SG_File_XML::~SG_File_XML()
{
	closeSection("system");
}

/*----------------------------------------------------------------------*/
/// Write a data in the file.
/*----------------------------------------------------------------------*/
void SG_File_XML::addValue(SG_String name, long double value, SG_String comment)
{
	mOutputFile << name << "='" << ITOS(value) << "' ";
}

/*----------------------------------------------------------------------*/
/// Write a data in the file (converted to integer)
/*----------------------------------------------------------------------*/
void SG_File_XML::addIntValue(SG_String name, long double value, SG_String comment)
{
	int intvalue = (int)(value);
	mOutputFile << name << "='" << ITOA(intvalue) << "' ";
}

/*----------------------------------------------------------------------*/
/// Write a data in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::addValue(SG_String name, bool value)
{
	mOutputFile << name << "='" << ITOA(value) << "' ";
}

/*----------------------------------------------------------------------*/
/// Write a data in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::addPercentage(SG_String name, long double value)
{
	long double percentage = int(value*1000);
	percentage /= 10;
	mOutputFile << name << "='" << ITOS(percentage) << "%' ";
}

/*----------------------------------------------------------------------*/
/// Write a data in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::addValue(SG_String name, char* value, SG_String comment)
{
	mOutputFile << name << "='" << value << "' ";
}

/*----------------------------------------------------------------------*/
/// Write a data in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::addValue(SG_String name, SG_String value, SG_String comment)
{
	mOutputFile << name << "='" << value << "' ";
}

/*----------------------------------------------------------------------*/
/// Add a section header in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::addSection(SG_String name)
{
	mOutputFile << "<" << name << " ";
}

/*----------------------------------------------------------------------*/
/// Add a section ender in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::closeSection(SG_String name)
{
	if (name!="") mOutputFile << "</" << name << "> \n";
	else          mOutputFile << "/>\n";
}

/*----------------------------------------------------------------------*/
/// Add a sub-section in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::subSection()
{
	mOutputFile << ">\n";
}

/*----------------------------------------------------------------------*/
/// Add a separator line in the file
/*----------------------------------------------------------------------*/
void SG_File_XML::addLine()
{
	mOutputFile << "\n<!-- ********************************************** --> \n";
}

