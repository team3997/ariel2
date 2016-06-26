/******************************************************************************/
/*                                Nunchuk.cpp                                 */
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

#include "Nunchuk.h"

/*  Initializes the I2C communication for the Nunchuk  */

void Nunchuk::initialize() {							//Not in a constructor because it does not actually do anything to the object itself
	Wire.begin();
        Wire.beginTransmission(0x52);						//Address of the Nunchuk
        Wire.write(0x40);							//Initializer byte
        Wire.write(0x00);							//"Go ahead" byte
        Wire.endTransmission();
        
        this->setSafeDefaults();                                                //Assume some default values that are "safe" if communication drops
}

/*  Reads in new data from the Nunchuk and parse it into useful variables  */
/*  Note:  all variables must be dereferenced because they are non-static  */
/*  and do not actually exist yet until the class is instantiated.         */

void Nunchuk::update() {
	unsigned char i;

        Wire.beginTransmission(0x52);						//Wake the Nunchuk up
        Wire.write(0x00);
        Wire.endTransmission();
        delay(10);								//Wait for 10 ms to give the Nunchuk time to yawn
        Wire.requestFrom(0x52, 6);						//Ask for 6 bytes from the Nunchuk

        for(i=0; Wire.available(); i++) this->rawData[i] = Wire.read();		//Read in as many bytes as are available on the I2C bus
        if(i==6) {								//If we read in 6 bytes (as we're supposed to), continue
 /*               for(i=0; i<6; i++) {                                          //XOR decryption loop for old Nunchuks
                        this->rawData[i] ^= 0x17;
                        this->rawData[i] += 0x17;
                }*/
                
                this->joyX = this->rawData[0];                                  //Parse out information from raw data into individual axes
                this->joyY = this->rawData[1];
                this->accX = (this->rawData[2] << 2);
                this->accX += (this->rawData[5] >> 2) & 0x03;
                this->accY = (this->rawData[3] << 2);
                this->accY += (this->rawData[5] >> 4) & 0x03;
                this->accZ = (this->rawData[4] << 2);
                this->accZ += (this->rawData[5] >> 6) & 0x03;
                this->buttonC = (this->rawData[5] & 0x02) ? true : false;
                this->buttonZ = (this->rawData[5] & 0x01) ? true : false;
        }
        else {                                                                  //If we didn't receive the right number of bytes, go to safe mode
                this->joyX = this->joySafeDefault[channel.x];
                this->joyY = this->joySafeDefault[channel.y];
                
                this->accX = this->accSafeDefault[channel.x];
                this->accY = this->accSafeDefault[channel.y];
                this->accZ = this->accSafeDefault[channel.z];
        }
        
        this->accXNorm = map(this->accX, this->accMins[channel.x], this->accMaxs[channel.x], 0, 255);        //Create a 0-255 version of the accelerometer axes
        this->accXNorm = constrain(this->accXNorm, 0, 255);
        this->accYNorm = map(this->accY, this->accMins[channel.y], this->accMaxs[channel.y], 0, 255);
        this->accYNorm = constrain(this->accYNorm, 0, 255);
        this->accZNorm = map(this->accZ, this->accMins[channel.z], this->accMaxs[channel.z], 0, 255);
        this->accZNorm = constrain(this->accZNorm, 0, 255);
}

short Nunchuk::getAxis(unsigned char chanType, unsigned char chan) {            //An easier way to retrieve the axis values if you're using a for() loop
        if(chanType == channel.joystick) {
                if(chan == channel.x) return this->joyX;
                if(chan == channel.y) return this->joyY;
                return -1;
        }
        if(chanType == channel.accelerometer) {
                if(chan == channel.x) return this->accX;
                if(chan == channel.y) return this->accY;
                if(chan == channel.z) return this->accZ;
                return -1;
        }
        return -1;
}

void Nunchuk::calibrate() {                                                     //Calibrate and report minimum and maximum axis values (only for current session)
        short joyMin[2] = {255, 255};                                           //Initialize min and max arrays to extreme values
        short joyMax[2] = {0, 0};
        short accMin[3] = {255, 255, 255};
        short accMax[3] = {0, 0, 0};
        
        while(this->buttonC) {                                                  //Runs until C button is pressed
                for(unsigned char i=0; i<2; i++) {                              //Compare recorded mins and maxes to current values, adjust accordingly
                        if(this->getAxis(channel.joystick, i) < joyMin[i]) joyMin[i] = this->getAxis(channel.joystick, i);
                        else if(this->getAxis(channel.joystick, i) > joyMax[i]) joyMax[i] = this->getAxis(channel.joystick, i);
                }
                for(unsigned char i=0; i<3; i++) {
                        if(this->getAxis(channel.accelerometer, i) < accMin[i]) accMin[i] = this->getAxis(channel.accelerometer, i);
                        else if(this->getAxis(channel.accelerometer, i) > accMax[i]) accMax[i] = this->getAxis(channel.accelerometer, i);
                }
                
                if(Serial) {                                                    //Use operator implementation of Serial object to see if it's been initialized
                        Serial.print("joyXMin = ");                             //Report recorded mins and maxs to the user
                        Serial.print(joyMin[channel.x], DEC);
                        Serial.print(", joyYMin = ");
                        Serial.print(joyMin[channel.y], DEC);
                        
                        Serial.print("accXMin = ");
                        Serial.print(accMin[channel.x], DEC);
                        Serial.print(", accYMin = ");
                        Serial.print(accMin[channel.y], DEC);
                        Serial.print(", accYMin = ");
                        Serial.print(accMin[channel.z], DEC);
                        
                        Serial.print(";  joyXMax = ");
                        Serial.print(joyMax[channel.x], DEC);
                        Serial.print(", joyYMax = ");
                        Serial.print(joyMax[channel.y], DEC);
                        
                        Serial.print("accXMax = ");
                        Serial.print(accMax[channel.x], DEC);
                        Serial.print(", accYMax = ");
                        Serial.print(accMax[channel.y], DEC);
                        Serial.print(", accYMax = ");
                        Serial.print(accMax[channel.z], DEC);
                        Serial.println();
                }
        }                                                                       //C button was pressed, time to exit loop
        
        for(unsigned char i=0; i<2; i++) {                                      //Save new mins and maxs (will not save between reboots)
                this->joyMins[i] = joyMin[i];
        }
        
        for(unsigned char i=0; i<3; i++) {
                this->accMins[i] = accMin[i];
        }
}

void Nunchuk::setSafeDefaults() {	                                        //Set the default "safe" values for if communication drops.
	for(unsigned char i=0; i<2; i++) {                                      //Since unspecified, assume half-way between configured min and max for each channel
		this->joySafeDefault[i] = (this->joyMins[i] - this->joyMins[i]) / 2 + this->joyMins[i];
	}
	
	for(unsigned char i=0; i<3; i++) {
		this->accSafeDefault[i] = (this->accMaxs[i] - this->accMins[i]) / 2 + this->accMins[i];
	}
}

void Nunchuk::setSafeDefaults(unsigned char chanType, unsigned char chan, short val) {
	if(chanType == channel.joystick && chan <= 2) {                         //Since exact channel and value is specified, configure manually
		this->joySafeDefault[chan] = val;
	}
	else if(chanType == channel.accelerometer && chan <= 3) {
		this->accSafeDefault[chan] = val;
	}
}
