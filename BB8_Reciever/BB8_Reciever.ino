/*
  Getting Started example sketch for nRF24L01+ radios
  This is a very basic example of how to send data from one node to another
  Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

//Pins used to control X and Y axis on motors
const int X_PWM_PIN = 2;
const int X_IN1_PIN = 3;
const int X_IN2_PIN = 4;
const int Y_PWM_PIN = 5;
const int Y_IN1_PIN = 6;
const int Y_IN2_PIN = 7;


/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
/**********************************************************/

byte addresses[][6] = {"1Node", "2Node"};

// Used to control whether this node is sending or receiving
//NOTE we will always be reading, so we don't need to use role.
//bool role = 0;

void setup() {
  pinMode(53, OUTPUT); //Required on Arduino MEGA
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);

  // Open a writing and reading pipe on each radio, with opposite addresses
  if (radioNumber) {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  } else {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }

  // Start the radio listening for data
  radio.startListening();

  //Set motor pins to output
  pinMode(X_PWM_PIN, OUTPUT);
  pinMode(X_IN1_PIN, OUTPUT);
  pinMode(X_IN2_PIN, OUTPUT);
  pinMode(Y_PWM_PIN, OUTPUT);
  pinMode(Y_IN1_PIN, OUTPUT);
  pinMode(Y_IN2_PIN, OUTPUT);
}

void loop() {

  /****************** Pong Back Role ***************************/

  unsigned long payload; // payload variable 4 bytes
  //unsigned long identifier; // identifier
  int motorY;      //Joystick 1 Y-axis
  int motorX;      //Joystick 1 X-axis

  //if( radio.available()){
  // Variable for the received timestamp
  while (radio.available()) {                                   // While there is data ready
    radio.read( &payload, sizeof(unsigned long) );             // Get the payload
  }

  //radio.stopListening();                                        // First, stop listening so we can talk
  //radio.write( &payload, sizeof(unsigned long) );              // Send the final one back.
  //radio.startListe ning();                                       // Now, resume listening so we catch the next packets.


  //Print value of our current payload and make a copy of it to work on
  Serial.print(F("Current Data (Full Payload): "));
  Serial.println(payload);
  reworkedPayload = payload;

  //Decode our message into individual motor signals
  // Payload structure:
  //  ---- ---- | ---- ---- | ---- ---- | ---- ---- |
  //    Other       Other     Motor X      Motor Y
  //-------------------------------------------------------

  motorY = reworkedPayload & 0x000000FF; // Mask Everything except the Motor Back signal
  reworkedPayload = payload;            //Recopy payload (since we masked out all the other bits)
  motorX = (reworkedPayload & 0x0000FF00) >> 8; // Mask Everything except the Motor Forward signal and shift it to the lowest bits

  //Joystick values can range -127 and +127, set 127 to 0 by subtracting 127
  motorY -= 127;
  motorX -= 127;

  //Encode m1 and m2 values as:
  //Bits
  //IN1 IN2 | Function   |
  //-------------------------
  // 0  0   | release    |
  // 0  1   | forward    |
  // 1  0   | backward   |
  // 1  1   | brake      |

  //Encoding m1
  if (motorY < 5) {
    //need to move backward ( IN1 IN2 = 1 0 )
    // and convert joystick angle to positive analog signal from 0 to 1024
    digitalWrite(Y_IN1_PIN, 1);
    digitalWrite(Y_IN2_PIN, 0);
    analogWrite(Y_PWM_PIN, abs(motorY) * 8);
  }
  else if (motorY > 5) {
    //need to move forward ( IN1 IN2 = 0 1 )
    // and convert joystick angle to positive analog signal from 0 to 1024
    digitalWrite(Y_IN1_PIN, 0);
    digitalWrite(Y_IN2_PIN, 1);
    analogWrite(Y_PWM_PIN, motorY * 8);
  }
  else {
    //need to release motor ( IN1 IN2 = 0 0 )
    // and set speed to 0
    digitalWrite(Y_IN1_PIN, 0);
    digitalWrite(Y_IN2_PIN, 0);
    analogWrite(Y_PWM_PIN, 0);
  }

  //Encoding m2
  if (motorX < 5) {
    //need to move backward ( IN1 IN2 = 1 0 )
    // and convert joystick angle to positive analog signal from 0 to 1024
    digitalWrite(X_IN1_PIN, 1);
    digitalWrite(X_IN2_PIN, 0);
    analogWrite(X_PWM_PIN, abs(motorY) * 8);
  }
  else if (motorX > 5) {
    //need to move forward ( IN1 IN2 = 0 1 )
    // and convert joystick angle to positive analog signal from 0 to 1024
    digitalWrite(X_IN1_PIN, 0);
    digitalWrite(X_IN2_PIN, 1);
    analogWrite(X_PWM_PIN, motorY * 8);
  }
  else {
    //need to release motor ( IN1 IN2 = 0 0 )
    // and set speed to 0
    digitalWrite(X_IN1_PIN, 0);
    digitalWrite(X_IN2_PIN, 0);
    analogWrite(X_PWM_PIN, 0);
  }


} // Loop
