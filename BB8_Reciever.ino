
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8,53);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 1;

void setup() {
  pinMode(53, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
}

void loop() {
  
    
  /****************** Pong Back Role ***************************/
  
  
  unsigned long got_time;                                          // Variable for the received timestamp
  unsigned long identifier;
     
  if(radio.available())
  {                                                 
    radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
    identifier = got_time >> 5;                                 // Extracts identification bits
    Serial.println(identifier);
    switch(identifier)
    {
      case 0:
        Serial.println("Right X");
        break;
      case 1:
        Serial.println("Right Y");
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      case 6:
        break;
      case 7:
        break;
      default:
        break;
      }
    
    Serial.print(F("Current Data: "));
    Serial.println(got_time);  
  }
  else
   {
    Serial.println("Data not available.");
   }
   
  delay(1000);
} // Loop

