/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef AXIS_HPP
#   define AXIS_HPP
#   include <limits>
#   include <string>  

class Axis
{
  private:
    int minRawValue; //calibration data (automatically updated)
    int maxRawValue; //calibration data (automatically updated)
    double value; //stores the normalized value
    //std::list < AxisFilter * > axisFilterList; //deadzones, linearity, etc... the AxisFilter class hasn't been coded yet
    
  public:
    Axis();
    ~Axis();

    //management of axis values
    void setNewRawValue(int value); //updates internal normalized value, done once per input event
    void updateValue();      //applies the filters, always done once per input engine step
    double getValue();            //returns normalized value, done once per physics engine step
    
    //management of filters list
    //void appendFilter (AxisFilter * filter);
    //void removeFilter (int n);
    //void insertFilter (AxisFilter * filter);
    // etc...
};

void axisIDtoStr(int axisID, std::string& destString);

int getIDKeyboardKey (int keyCode);
int getIDMouseButton (int buttonNumber);
int getIDMouseAxis (int axisNumber);
int getIDJoyButton (int joystickNumber, int buttonNumber);
int getIDJoyAxis (int joystickNumber, int axisNumber);

#endif
