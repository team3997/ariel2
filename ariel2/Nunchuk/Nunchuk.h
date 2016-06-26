/******************************************************************************/
/*                                 Nunchuk.h                                  */
/*                             Aaron Osmer, 2014                              */
/*                                                                            */
/*  The Nunchuk library should allow the use of any Nintendo or thrid-party   */
/*  Wii Nunchuk game controller, including cheap wireless ones.  This class   */
/*  implements functions for I2C initialization, reading values from the      */
/*  nunchuk, calibration, and setting safe defaults so your system will not   */
/*  damage itself if I2C communication is lost.  Data may be retrieved either */
/*  directily from the appropriately named variables, or by the getAxis()     */
/*  method.  The variables are easier to read in the code, but getAxis() can  */
/*  facilitate easier use of for() loops if many values need to be accessed   */
/*  the same way.  This library also includes a channel struct to make some   */
/*  method calls easier to understand by specifying the indexes for each      */
/*  channel type and axis.  You may modify or redistribute this library as    */
/*  you see fit.                                                              */
/*                                                                            */
/******************************************************************************/

#include <Arduino.h>
#include <Wire.h>

#ifndef Nunchuk_h								//Only define this class once
#define Nunchuk_h

struct {
	const unsigned char joystick = 0;
	const unsigned char accelerometer = 1;
	const unsigned char x = 0;
	const unsigned char y = 1;
	const unsigned char z = 2;
} channel;

class Nunchuk {
        private:								//Keep raw data and calibration points private
        	unsigned char rawData[6];					//Raw data could be corrupted if public
                unsigned short accMins[3] = {320, 320, 296};			//Accelerometer ranges should get locked in unless explicitly calibrated
                unsigned short accMaxs[3] = {708, 708, 700};
                unsigned short joyMins[2] = {0, 0};
                unsigned short joyMaxs[2] = {255, 255};
		unsigned char joySafeDefault[3];				//Safe default values for the Nunchuk sensors if communication is lost
		unsigned short accSafeDefault[3];
		boolean buttonSafeDefault[2];
	
	public:									//User-access values
		unsigned char joyX, joyY;					//Joystick axes
		unsigned short accX, accY, accZ;				//Raw accelerometer values
		short accXNorm, accYNorm, accZNorm;				//Accelerometer values normalized to 0-255
		boolean buttonC, buttonZ;					//Button values (an unpushed button returns high)
		void initialize();						//Set up I2C communications
                short getAxis(unsigned char chanType, unsigned char chan);      //Returns channel value given the channel number
		void update();							//Read in new values from Nunchuk
		void calibrate();						//Calibrate axes
		void setSafeDefaults();
		void setSafeDefaults(unsigned char chanType, unsigned char chan, short val);
};

#endif
