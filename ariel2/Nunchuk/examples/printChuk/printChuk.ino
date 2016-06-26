/******************************************************************************/
/*                               printChuk.ino                                */
/*                             Aaron Osmer, 2014                              */
/*                                                                            */
/*  This program is designed to demonstrate most important aspects of the     */
/*  Nunchuk library.  This library should allow the use of all Nintendo and   */
/*  third-party Nunchuk type game controllers, including cheap wireless ones. */
/*  This program prints all channel values to the screen and sets the LED     */
/*  based on either the accelerometer Y-axis or the Z button (settable by the */
/*  USE_ANALOG_LED flag, must have the LED on a PWM pin).  There is also a    */
/*  calibration mode which can be entered by pressing a pushbutton on a pin   */
/*  (settable by CAL_PIN).  This library may be modified or redistributed as  */
/*  you see fit.                                                              */
/*                                                                            */
/******************************************************************************/

#include <Nunchuk.h>

#define CAL_PIN 2								//Pin for a calibration button to ground
#define LED_PIN 13								//An LED to blink when you push a button

#define USE_ANALOG_LED 0							//Flag for whether to use the LED with PWM (Mega)
#define ASSUME_DEFAULTS 1							//Assume safe default axis values are halfway between the mins and maxs

#define JOY_DEFAULT 128
#define ACC_DEFAULT 512

Nunchuk myNunchuk;								//Create the Nunchuk object

void setup() {
	Serial.begin(115200);							//Class methods which print should detect whether the Serial console is running
	
	myNunchuk.initialize();							//Sets up I2C communication with the Nunchuk
	
	delay(500);								//Delay gives user a window to program the Arduino, sometimes will stall otherwise
	
	pinMode(CAL_PIN, INPUT);						//A pushbutton to enter calibration mode for the Nunchuk
	pinMode(LED_PIN, OUTPUT);
}

void loop() {
	myNunchuk.update();							//Reads in new data from Nunchuk and parses it
	
	printVals();								//Print out all channels in a line to the screen
	
	if(USE_ANALOG_LED) {
		analogWrite(LED_PIN, myNunchuk.accYNorm);			//If LED is PWM enabled, set brightness by the 0-255 normalized Y axis of the accelerometer
	}
	else {
		digitalWrite(LED_PIN, !myNunchuk.buttonZ);			//If not configured for analog control, set the LED on or off with the Z button
	}
	
	/* If the calibration button is pressed, enter into the Nunchuk calibration mode.   */
	/* This mode will print running minimum and maximum values of the accelerometer and */
	/* joystick.  The user is responsible for moving all axes to their extremes.  These */
	/* values will be temporarily saved and used while the program is running, but will */
	/* be lost when the Arduino reboots or is reset.  You must write down the printed   */
	/* values to manually enter into your program later.  Press the C button to exit.   */
	
	if(digitalRead(CAL_PIN)) {
		myNunchuk.calibrate();
		
		/* If called with no parameters, setSafeDefaults() will assume the safe defaults are */
		/* half-way between the minimum and maximum values for each axis.  If parameters are */
		/* used, it will set the specified channel.  These "safe defaults" will be used in   */
		/* the event that I2C communication is lost.  You should set these defaults to values*/
		/* that will not damage your system if anything goes wrong (e.g. shutting off the    */
		/* motors of a robot so it doesn't drive off the table).                             */
		
		if(ASSUME_DEFAULTS) {
			myNunchuk.setSafeDefaults();
		}
		else {
			for(int i=0; i<2; i++) {				//Manually set each joystick default to 128
				myNunchuk.setSafeDefaults(channel.joystick, i, JOY_DEFAULT);
			}
			for(int i=0; i<3; i++) {				//Manually set each accelerometer default to 512
				myNunchuk.setSafeDefaults(channel.accelerometer, i, ACC_DEFAULT);
			}
		}
	}
}

/* Print all axis and button values out to the console using class variables */

void printVals() {
	Serial.print(myNunchuk.joyX, DEC);
        Serial.print("\t");
        Serial.print(myNunchuk.joyY, DEC);
        Serial.print("\t");
        Serial.print(myNunchuk.accX, DEC);
        Serial.print("\t");
        Serial.print(myNunchuk.accY, DEC);
        Serial.print("\t");
        Serial.print(myNunchuk.accZ, DEC);
        Serial.print("\t");
        Serial.print(myNunchuk.buttonC, DEC);
        Serial.print("\t");
        Serial.println(myNunchuk.buttonZ, DEC);
}

/* Print all axis values out to the console using the getAxis() method, and button values by class variabls */
/* Acts the same as printVals(), just demonstrates a way to use this class with for() loops.                 */

void printValsIterative() {
	for(int i=0; i<2; i++) {
		for(int j=0; j<3; j++) {
			Serial.print(myNunchuk.getAxis(i, j), DEC);
			Serial.print("\t");
		}
	}
	Serial.print(myNunchuk.buttonC, DEC);
	Serial.print("\t");
	Serial.println(myNunchuk.buttonZ, DEC);
}