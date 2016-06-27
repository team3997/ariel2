#include <Nunchuk.h>
#include <Servo.h>
#include <Wire.h>
//#include <drive.h>

#define ASSUME_DEFAULT 1;
#define JOY_DEFAULT 128;
#define ACC_DEFAULT 512;

#define SERVO_THRESH 5.0 // Detach servo if within 5% of zero
#define LEFTPIN 4
#define RIGHTPIN 3

#define INVERTLEFT true
#define INVERTRIGHT false

Servo leftservo;
Servo rightservo;
Nunchuk nunchuk;

float fmap(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() 
{ 
  leftservo.attach(LEFTPIN);  // attaches the servo on pin 4 to the servo object 
  rightservo.attach(RIGHTPIN);
  Serial.begin(9600);
  nunchuk.initialize();
} 
 
void left(float x, float y){
  float power = x + y;
  if(power > 1)
    power=1;
  if(power<-1)
    power=-1;
  
  if(power < SERVO_THRESH/100 && power > -SERVO_THRESH/100)
    leftservo.detach();
  else
    leftservo.attach(LEFTPIN);
    
  if(INVERTLEFT)
    power= -power;
  
  leftservo.write(fmap(power,-1,1,0,180));
}

void right(float x, float y){
  float power = y-x;
  if(power > 1)
    power=1;
  if(power<-1)
    power=-1;
    
    
  if(power < SERVO_THRESH/100 && power > -SERVO_THRESH/100)
    rightservo.detach();
  else
    rightservo.attach(RIGHTPIN);  
  
  if(INVERTRIGHT)
    power= -power;  
  
  rightservo.write(fmap(power,-1,1,0,180));
}

void loop() 
{              
    nunchuk.update();
    left( fmap(nunchuk.joyX, 0, 255, -1, 1), fmap(nunchuk.joyY, 0, 255, -1, 1)  );
    right( fmap(nunchuk.joyX, 0, 255, -1, 1), fmap(nunchuk.joyY, 0, 255, -1, 1)  );
    Serial.println(fmap(nunchuk.joyX, 0, 255, -1, 1)); 
  
    
} 
