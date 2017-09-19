/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservoL;  // create servo object to control a servo
Servo myservoR;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int sensorPinR = A0;
int sensorPinL = A1; 
int sensorValueR = 0;
int sensorValueL = 0;
const int threshold = 100; 
int servoSpeedR = 0;
int servoSpeedL = 0;
int sensitivityLevel = 10;
int offset = 300; 
int accArray[10];
int accCnt = 0;
const int varThreshold = 10;

const int xpin = A4;                  // x-axis of the accelerometer
const int ypin = A3;                  // y-axis
const int zpin = A2;                  // z-axis (only on 3-axis models)
float relativeSpeedR = 1.0;
float relativeSpeedL = 1.0;
unsigned long timeArduino;
unsigned long timeThreshold = 10000;
  
void setup() {
  myservoR.attach(9);  // attaches the servo on pin 9 to the servo object
  myservoL.attach(8);  // attaches the servo on pin 9 to the servo object
  Serial.begin (9600);
  randomSeed(analogRead(sensorPinR));
}

void loop() {
  //count the number of milliseconds
  unsigned long currentTime = millis(); 
  
  //Serial.println(timeArduino);
  // read the value from the sensor:
  sensorValueR = analogRead(sensorPinR)*sensitivityLevel;
  sensorValueL = analogRead(sensorPinL)*sensitivityLevel;
  
  // read from the accelermetor 
  if (accCnt < 5){
    accArray[accCnt] = analogRead(ypin);
    accCnt += 1; 
  }else{
    float mean = 0;
    for (int i=0; i < 5; i+=1){
      mean += accArray[i];
    }
    mean /= 5;
    float var = 0; 
    for (int i=0; i < 5; i+=1){
      var += (accArray[i] - mean) * (accArray[i] - mean); 
    }
    var /= 5;
    //Serial.println(var); 
    if (var <= varThreshold){
      servoSpeedR =  80;
      servoSpeedL =  120;  
      //Serial.println("vvvv");
      for (int i = 0; i < 20; i+=1){
        myservoR.write(servoSpeedR);
        myservoL.write(servoSpeedL);
        delay(100); 
      } 
    }
    accCnt = 0;
  }
 
  Serial.println(sensorValueR);
  Serial.println(sensorValueL);
  if (sensorValueR - sensorValueL>offset){
    servoSpeedR = 100; 
    servoSpeedL = 0;
    //Serial.println("aaaa");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL);
    delay(100);
  }else if(sensorValueR- sensorValueL<-offset){
    servoSpeedR = 180;
    servoSpeedL = 90;  
    //Serial.println("bbbb");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(100);
  }else{
    int randomNumber = random(1000); 
    int random1 = random(100);
    int random2 = random(100,200);
    int signL = random1%2==0 ? 1 : -1; 
    int signR = random1%2==0 ? 1 : -1; 
    timeArduino = currentTime; 
    if (randomNumber % 2 == 0){
      servoSpeedR = signR * 98;
      servoSpeedL = signL * 20;
    }else{
      servoSpeedR = signR * 120;
      servoSpeedL = signL * 90;
    }
   myservoR.write(servoSpeedR); 
   myservoL.write(servoSpeedL); 
   delay(100);   
   if (timeArduino % timeThreshold == 0){
      sensitivityLevel *= 2;
   }
  }
  delay(10);

}

