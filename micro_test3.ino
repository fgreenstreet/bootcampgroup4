/*
* First RF sketch using nRF24L01+ radios and Arduino (Uno or Micro)
*/
#include <SPI.h>    // Serial Peripheral Interface bus library
#include "RF24.h"   // Library for nRF24L01s
#include <Servo.h>



/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/*Creates servos*/
Servo myservoL;  // create servo object to control a servo
Servo myservoR;  // create servo object to control a servo
float servoSpeedR = 100; 
float servoSpeedL = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus pins and pins 7 & 8 */
//RF24 radio(7,8);  // For Arduino UNO
RF24 radio(9,10);  // For Arduino Micro

/* Create a binary address for read/write pipes */
byte addresses[][6] = {"1Node","2Node"};

bool role = 0; // Local state variable that controls whether this node is sending or receiving
void setup() {
  myservoR.attach(6);  // attaches the servo on pin 9 to the servo object
  myservoL.attach(5);  // attaches the servo on pin 9 to the servo object
  
  Serial.begin(115200);
  radio.begin();

  // Set the Power Level low
  radio.setPALevel(RF24_PA_LOW);
  
  // Set radio rate and channel
  radio.setDataRate(RF24_250KBPS); 

  // Set the channel (Frequency): 108 = 2.508 GHz (above WiFi)
  radio.setChannel(124);

  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Pause briefly to let everything "settle"
  delay(1000);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Start the radio listening for data
  radio.startListening();
}

void moveWheels(char order){
   if (order == 'l'){
    //left
    servoSpeedR = 100; 
    servoSpeedL = -20;
    //Serial.println("aaaa");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL);
    delay(10);
  }else if(order == 'r'){
    //right
    servoSpeedR = 180;
    servoSpeedL = 90;  
    //Serial.println("bbbb");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(10);
   }else if(order == 'f'){
    //right
    servoSpeedR = 98.5;
    servoSpeedL = 90;  
    //Serial.println("bbbb");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(10);
  }else if(order == 'b'){
    //right
    servoSpeedR = 89.5;
    servoSpeedL = 99;  
    //Serial.println("bbbb");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(10);
  }else if(order == 'L'){
    //right
    servoSpeedR = 0;
    servoSpeedL = 99;  
    //Serial.println("bbbb");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(10);
  }else if(order == 'R'){
    //right
    servoSpeedR = 89.5;
    servoSpeedL = 120;  
    //Serial.println("bbbb");
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(10);
  }
  else{
    servoSpeedR =  89.5; // backward from 0? to 90
    servoSpeedL =  99; //backward from 96 
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(100);   
    servoSpeedR =  98.5; // backward from 0? to 90
    servoSpeedL =  90; //backward from 100 
    myservoR.write(servoSpeedR); 
    myservoL.write(servoSpeedL); 
    delay(100);   
  }
}
void loop() {
  /****************** Transmitting Role ***************************/  
  if (role == 1)
  {
    radio.stopListening();                                    // First, stop listening so we can talk.
    Serial.println(F("Now sending"));
    
    unsigned long start_time = micros();                      // Take the time, and send it.  This will block until complete
    char direction = "R";
    if (!radio.write( &start_time, sizeof(unsigned long) )){
    //if (!radio.write( &direction, sizeof(unsigned long) )){
      Serial.println(F("failed"));
    }
    radio.startListening();                                    // Now, continue listening
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while (!radio.available() ) {                            // While nothing is received
      if (micros() - started_waiting_at > 200000 ) {            // If waited longer than 200ms, indicate timeout and exit while loop
          //timeout = true;
          break;
      }      
    }
        
    if ( timeout ) {                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
    } else {
        unsigned long order;                                 // Grab the response, compare, and send to debugging spew
        //radio.read( &order, sizeof(unsigned long) );
        radio.read( &order, sizeof(unsigned long) );
        unsigned long end_time = micros();
        
        // Send to serial port
        Serial.print(F("Sent "));
        //Serial.print(start_time);
        Serial.print(start_time);
        Serial.print(F(", Got response "));
        Serial.print(order);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time-start_time);
        Serial.println(F(" microseconds"));
    }
  
    // Try again 100 ms later
    delay(100);
  }
  
  /****************** Receiving Role ***************************/  
  if ( role == 0 )
  {
    //unsigned long order;    
    char order;
    char confirmation = 'c';      
    //Serial.println(radio.available());
    if( radio.available()) {                                        // Variable for the received timestamp
      //while (radio.available()) {                                   // While there is data ready
      //radio.read( &order, sizeof(unsigned long) );             // Get the payload
      radio.read( &order, sizeof(char) );             // Get the payload
      Serial.println(order);
      moveWheels(order);
      delay(2000);
      //}
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write(&confirmation, sizeof(char) );              // Send the final one back.      
      Serial.println("Sent response");
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
   }else{
      moveWheels('0');
   }
 }

/****************** Change Roles via Serial Commands ***************************/
  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == 0 ) {      
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      role = 1;                  // Become the primary transmitter (ping out)
    } else if ( c == 'R' && role == 1 ) {
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));      
       role = 0;                // Become the primary receiver (pong back)
       radio.startListening();
    }
  }

} // End of Loop

// FIN
