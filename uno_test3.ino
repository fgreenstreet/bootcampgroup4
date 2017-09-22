/*
* First RF sketch using nRF24L01+ radios and Arduino (Uno or Micro)
*/
#include <SPI.h>    // Serial Peripheral Interface bus library
#include "RF24.h"   // Library for nRF24L01s

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus pins and pins 7 & 8 */
RF24 radio(7,8);  // For Arduino UNO
//RF24 radio(9,10);  // For Arduino Micro

/* Create a binary address for read/write pipes */
byte addresses[][6] = {"1Node","2Node"};

bool role = 1; // Local state variable that controls whether this node is sending or receiving
void setup() {
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
  //Serial.println(F("RF24/examples/GettingStarted"));
  //Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Start the radio listening for data
  radio.startListening();
}

void loop() {

  /****************** Transmitting Role ***************************/  
  if (role == 1)
  {
    radio.stopListening();                                    // First, stop listening so we can talk.
    //Serial.println(F("Now sending"));
    unsigned long start_time = micros();   
    // Take the time, and send it.  This will block until complete
    int randomNumber = random(1,3); 
    //unsigned long order = randomNumber;
    char order = 'l';
    char confirmation = 'c';                                 // Grab the response, compare, and send to debugging spew
    /*Send data when a key input is given*/
    //if (Serial.available()){
    if (1){
      //order = Serial.read();
      if (confirmation == 'c'){
        Serial.println(order);
        if (!radio.write( &order, sizeof(char) )){
          Serial.println(F("failed"));
        }else{
          confirmation = 'x';
          radio.startListening();                                    // Now, continue listening
          //delay(1000);
          if (radio.available()){
          //while (radio.available()){
            radio.read(&confirmation, sizeof(char) );
            // Send to serial port
            //if (confirmation == 'c'){
              //  break;
            Serial.print(confirmation);
            //}
          //}             
          }
        }
      }
    }
    // Try again 100 ms later
    delay(2000);
  }
  /****************** Receiving Role ***************************/  
  if ( role == 0 )
  {
    unsigned long order;    
    //Serial.println(radio.available());
    if( radio.available()) {                                        // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &order, sizeof(unsigned long) );             // Get the payload
      }
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write( &order, sizeof(unsigned long) );              // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Sent response "));
      Serial.println(order);  
   }
 }
  role = 1; //UNO sends data 
/****************** Change Roles via Serial Commands ***************************/
//  if ( Serial.available() )
//  {
//    char c = 'T'; //toupper(Serial.read());
//    if ( c == 'T' && role == 0 ) {      
//      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
//      role = 1;                  // Become the primary transmitter (ping out)
//    } else if ( c == 'R' && role == 1 ) {
//      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));      
//       role = 0;                // Become the primary receiver (pong back)
//       radio.startListening();
//    }
//  }

} // End of Loop

// FIN
