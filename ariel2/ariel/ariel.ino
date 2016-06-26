//. Motor driver shield- 2012 Copyright (c) Seeed Technology Inc.
// 
//  Original Author: Jimbo.we
//  Contribution: LG
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this       
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <Nunchuk.h>
             
#define ASSUME_DEFAULTS 1

#define JOY_DEFAULT 128
#define ACC_DEFAULT 512

Nunchuk myNunchuk;                //Create the Nunchuk object





/* Print all axis and button values out to the console using class variables */


 
void setup()
{
  Serial.begin(115200);             //Class methods which print should detect whether the Serial console is running
  
  myNunchuk.initialize();             //Sets up I2C communication with the Nunchuk
  
  delay(500);    
}
 
bool isForward()
{
     if((myNunchuk.joyY, DEC) > 128) {
        return true; 
      
     } else {
        return false; 
     }
}
bool isBackward()//
{
     if((myNunchuk.joyY, DEC) < 128) {
        return true; 
      
     } else {
        return false; 
     }
}
bool isLeft()//
{
     if((myNunchuk.joyX, DEC) > 128) {
        return true; 
      
     } else {
        return false; 
     }
}
bool isRight()//
{
     if((myNunchuk.joyX, DEC) < 128) {
        return true; 
      
     } else {
        return false; 
     }
}
bool isStop()//
{
     if((myNunchuk.joyX, DEC) == 128 && (myNunchuk.joyY, DEC) == 128) {
        return true; 
      
     } else {
        return false; 
     }
 
}

void loop() {
  myNunchuk.update();             //Reads in new data from Nunchuk and parses it
  
  
  
  
 
  /* If the calibration button is pressed, enter into the Nunchuk calibration mode.   */
  /* This mode will print running minimum and maximum values of the accelerometer and */
  /* joystick.  The user is responsible for moving all axes to their extremes.  These */
  /* values will be temporarily saved and used while the program is running, but will */
  /* be lost when the Arduino reboots or is reset.  You must write down the printed   */
  /* values to manually enter into your program later.  Press the C button to exit.   */
  
  
    if(ASSUME_DEFAULTS) {
      myNunchuk.setSafeDefaults();
    }
    else {
      for(int i=0; i<2; i++) {        //Manually set each joystick default to 128
        myNunchuk.setSafeDefaults(channel.joystick, i, JOY_DEFAULT);
      }
      for(int i=0; i<3; i++) {        //Manually set each accelerometer default to 512
        myNunchuk.setSafeDefaults(channel.accelerometer, i, ACC_DEFAULT);
      }
    }
  
}
