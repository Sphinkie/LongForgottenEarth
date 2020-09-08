/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "axis.h"
#include <sstream>
#include <algorithm>

void Axis::setNewRawValue(int value)
{
    //update device axis calibration values
    if (value < minRawValue)
    {
        minRawValue = value;
    }
    if (value > maxRawValue)
    {
        maxRawValue = value;
    }
    //normalize raw value and store it. 
    if (maxRawValue != minRawValue)
    {
        this->value = ( double(value - minRawValue) / double(maxRawValue - minRawValue)); //store normalized value
    }
}
void Axis::updateValue()
{
    if (maxRawValue <= minRawValue)
    {
        value = 0.5;
    }
    //apply filters in order
    for (int i = 0; i< 10 /*axisFilterList.size()*/; i++)
    {
    //    value = axisFilterList[i]->processValue(value);
    }
}
double Axis::getValue()
{
    //return the preprocessed value
    return value;
}
Axis::Axis () : minRawValue(std::numeric_limits<int>::max()), maxRawValue(std::numeric_limits<int>::min())
{
    //empty
}
Axis::~Axis ()
{
    // delete filters
    // delete filters list
}

void inttostr (int number, std::string &destString)
{
    std::stringstream tempString;
    tempString << number;
    destString.assign (tempString.str());
}


void axisIDtoStr (int axisID, std::string& destString)
{
  std::ostringstream out;
 
/*
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
*/
      
    int rest = axisID;
    //.......................EEDDNNNEEE
    int engineType   = rest / 100000000;
    rest             = rest % 100000000;
    int deviceType   = rest /   1000000;
    rest             = rest %   1000000;
    int deviceNumber = rest /      1000;
    rest             = rest %      1000;
    int elementNumber= rest; 

    int subtype, axis, driver; 

    const char* EngineName[] = { "HID", 
				 "Replay vehicle #", 
				 "AI driver #",
				 "Unknown ID (#" };

    const char* TypeName[] = {"Keyboard #",
			    "Mouse #",
			    "Mouse #",
			    "Joystick #",
			    "Joystick #",
			    "Joystick #",
			    "Joystick #",
			    "Track IR #", 
			    "Unknown HID #" };
    
    const char* ElementName[] = { ", SDL key code #",
			   ", button #",
			   ", axis #",
			   ", button #",
			   ", axis #",
			    ", axis #",
			    ", axis #",
			    ", axis #",
			    "unknown"};

    const char* SubTypeName[] = { "","","","","", "", 
			      "hat #", ", trackball #", 
			      ""}; 
    

    // Any case fall through is intentional
    switch (engineType) {
    case 0: {
      switch (deviceType)
	{
	case 0: //kkey
	case 1: //mbut
	case 2: //maxis
	case 3: //jbut
	case 4: //jaxis
	case 7: //tir
	  out << TypeName[deviceType];
	  out << deviceNumber;
	  out << ElementName[std::min(deviceType,7)];
	  out << elementNumber;
	  break;
	case 6: //jtaxis
	case 5: //jhaxis
	  axis = axisID % 10;
	  subtype = (axisID % 1000) - axis;
	  out << TypeName[deviceType];
	  out << deviceNumber;
	  out << SubTypeName[std::min(deviceNumber, 7)];
	  out << subtype;                   // hat/trackball
	  out << ElementName[std::min(deviceType, 7)];
	  out << axis;
	  break;
	default: //other
	  out << TypeName[std::min(deviceType, 7)];
	  out << axisID;
	  out << ")";
	  break;
	}
    }
      break;

    case 1: //replay vehicle
    case 2: //ai player
      out << EngineName[engineType];
      axis = axisID % 100000;
      driver = (axisID % 100000000) - axis;
      out << driver;
      out << ", axis #";
      out << axis;
      break;

    default:
      out << "Unknown ID (#";
      out << axisID;
      out << ")";
      break;
    }
   
    destString = out.str();
}


int getIDJoyAxis (int joystickNumber, int axisNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 4;
    result +=       1000 * joystickNumber;
    result +=          1 * axisNumber;
    return result;
*/
    return 4000000 + (joystickNumber * 1000) + axisNumber;
}

int getIDJoyButton (int joystickNumber, int buttonNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 3;
    result +=       1000 * joystickNumber;
    result +=          1 * buttonNumber;
    return result;
*/
    return 3000000 + (joystickNumber * 1000) + buttonNumber;
}
int getIDMouseAxis (int axisNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 2;
    result +=       1000 * 0;
    result +=          1 * axisNumber;
    return result;
*/
    return 2000000 + axisNumber;
}
int getIDMouseButton (int buttonNumber)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 1;
    result +=       1000 * 0;
    result +=          1 * buttonNumber;
    return result;
*/
    return 1000000 + buttonNumber;
}
int getIDKeyboardKey (int keyCode)
{
/*
    int result = 0;
                DD  NNN                
        DeviceType  DeviceNumber
   EE            ^  ^               EEE
   EngineType<-. |  |  ,->ElementNumber
               | |  |  |
               | |  |  |
              EEDDNNNEEE        
    result +=  100000000 * 0;
    result +=    1000000 * 0;
    result +=       1000 * 0;
    result +=          1 * keyCode;
    return result;
*/
    return keyCode;
}
