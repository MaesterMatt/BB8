
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(6, 10);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 0;
int state = 0;
int x1, x2, y1, y2;
void setup() {
  Serial.begin(115200);
  
  radio.begin();
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  
  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  //radio.setChannel(108);
  //Serial.println(radio.getChannel());
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data

  radio.stopListening();                                    // First, stop listening so we can talk.
}

void loop() {
  unsigned long message = 0x00;
  if (state == 0)  { // Joystick 1 X
      x1 = analogRead(A7);
      
      //x1 = constrain((x1-512)*2, -1023, 1023);
      x1 = map(x1, 0, 1023, 0, 31);
      message |= (state << 5);
      message |= x1;
      state = 1;
  }
  else if (state == 1){ // Joystick 1 Y
      y1 = analogRead(A6);
      //y1 = constrain((y1-512)*2, -1023, 1023);
      y1 = map(y1, 0, 1023, 0, 31);
      message |= (state << 5);
      message |= y1;
      state = 0;
  }
  Serial.print("Before");
  if (!radio.write( &message, sizeof(unsigned long) )){
   Serial.println(F("failed"));
  }
  Serial.println("After");
  // Try again 1s later
  delay(1000);
} // Loop

