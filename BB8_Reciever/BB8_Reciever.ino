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
const int Y_PWM_PIN = 7;
const int Y_IN1_PIN = 5;
const int Y_IN2_PIN = 6;

const int X_PWM_PIN2 = 9;
const int X_IN1_PIN2 = 34;
const int X_IN2_PIN2 = 33;
const int Y_PWM_PIN2 = 10;
const int Y_IN1_PIN2 = 36;
const int Y_IN2_PIN2 = 35;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8, 53);
/**********************************************************/

byte addresses[][6] = {"1Node", "2Node"};

// Used to control whether this node is sending or receiving
//NOTE we will always be reading, so we don't need to use role.
//bool role = 0;

void setup() {
  pinMode(53, OUTPUT); //Required on Arduino MEGA
  Serial.begin(115200);

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

  // Sets "Previous Values" for very beginning
  int prevMotorY = 1;      // Previous Joystick 1 Y-axis value
  int prevMotorX = 2;      // Previous Joystick 1 X-axis value
  
void loop() {

  /****************** Pong Back Role ***************************/

  unsigned long payload; // payload variable 4 bytes
  //unsigned long identifier; // identifier
  int motorY;      //Joystick 1 Y-axis
  int motorX;      //Joystick 1 X-axis
  
  int dampingConstant = 30; // How fast input increases or decreases. Higher = faster acceleration.
  int outputMotorY;      // Actual Joystick 1 Y-axis output, for debugging
  int outputMotorX;      // Actual Joystick 1 X-axis output, for debugging
    
  if( radio.available()){
    // Variable for the received timestamp
    while (radio.available()) {                                   // While there is data ready
      radio.read( &payload, sizeof(unsigned long) );             // Get the payload
    }
  
    //Decode our message into individual motor signals
    // Payload structure:
    //  ---- ---- | ---- ---- | ---- ---- | ---- ---- |
    //    Other       Other     Motor X      Motor Y
    //-------------------------------------------------------
    motorY = payload & 0x000000FF; // Mask Everything except the Motor Back signal
    payload = payload;            //Recopy payload (since we masked out all the other bits)
    motorX = (payload & 0x0000FF00) >> 8; // Mask Everything except the Motor Forward signal and shift it to the lowest bits
  
    //Joystick values can range -127 and +127, set 127 to 0 by subtracting 127
    motorY -= 127;
    motorX -= 127;
    
    // UNCOMMENT TO VIEW CURRENT JOYSTICK VALUES
    /*Serial.print("Y_X: ");
    Serial.print(motorY);
    Serial.print("_");
    Serial.println(motorX);
    Serial.print("\n");*/
    
    //Encode m1 and m2 values as:
    //Bits
    //IN1 IN2 | Function   |
    //-------------------------
    // 0  0   | release    |
    // 0  1   | forward    |
    // 1  0   | backward   |
    // 1  1   | brake      |

    if (motorY > prevMotorY){         // if increasing in Y, scale up by damping constant
      prevMotorY += dampingConstant;
      Serial.print("Yes");
      if (prevMotorY > motorY){
        Serial.print("UH OH BOYO");
        prevMotorY = motorY;}
      }
    if (motorY < prevMotorY){         // if decreasing in Y, scale down by damping constant
      prevMotorY -= dampingConstant;
      if (prevMotorY < motorY){
        prevMotorY = motorY;}
      }
    if (motorX > prevMotorX){         // if increasing in X, scale up by damping constant
      prevMotorX += dampingConstant;
      if (prevMotorX > motorX){
        prevMotorX = motorX;}
      }
    if (motorX < prevMotorX){         // if decreasing in X, scale down by damping constant
      prevMotorX -= dampingConstant;
      if (prevMotorX < motorX){
        prevMotorX = motorX;}
      }
      
    // UNCOMMENT TO VIEW ACTUAL OUTPUT DURING SCALING
    /*Serial.print("outputY_outputX: ");
    Serial.print(prevMotorY);
    Serial.print("_");
    Serial.println(prevMotorX);
    Serial.println("\n");*/
    
    //Encoding m1
    if (motorY < -5) {
      //need to move backward ( IN1 IN2 = 1 0 )
      // and convert joystick angle to positive analog signal from 0 to 1024
      digitalWrite(Y_IN1_PIN, 1);
      digitalWrite(Y_IN2_PIN, 0);
      digitalWrite(Y_IN1_PIN2, 1);
      digitalWrite(Y_IN2_PIN2, 0);
      analogWrite(Y_PWM_PIN, abs(prevMotorY) * 8);
      analogWrite(Y_PWM_PIN2, abs(prevMotorY) * 8);
    }
    else if (motorY > 5) {
      //need to move forward ( IN1 IN2 = 0 1 )
      // and convert joystick angle to positive analog signal from 0 to 1024
      digitalWrite(Y_IN1_PIN, 0);
      digitalWrite(Y_IN2_PIN, 1);
      digitalWrite(Y_IN1_PIN2, 0);
      digitalWrite(Y_IN2_PIN2, 1);
      analogWrite(Y_PWM_PIN, (prevMotorY-1) * 8);
      analogWrite(Y_PWM_PIN2, (prevMotorY-1) * 8);
    }
    else {
      //need to release motor ( IN1 IN2 = 0 0 )
      // and set speed to 0
      digitalWrite(Y_IN1_PIN, 0);
      digitalWrite(Y_IN2_PIN, 0);
      analogWrite(Y_PWM_PIN, abs(prevMotorY) * 8);
      digitalWrite(Y_IN1_PIN2, 0);
      digitalWrite(Y_IN2_PIN2, 0);
      analogWrite(Y_PWM_PIN2,  abs(prevMotorY) * 8);
    }
  
    //Encoding m2
    if (motorX < -5) {
      //need to move backward ( IN1 IN2 = 1 0 )
      // and convert joystick angle to positive analog signal from 0 to 1024
      digitalWrite(X_IN1_PIN, 1);
      digitalWrite(X_IN2_PIN, 0);
      digitalWrite(X_IN1_PIN2, 1);
      digitalWrite(X_IN2_PIN2, 0);
      analogWrite(X_PWM_PIN, abs(prevMotorX) * 8);
      analogWrite(X_PWM_PIN2, abs(prevMotorX) * 8);
    }
    else if (motorX > 5) {
      //need to move forward ( IN1 IN2 = 0 1 )
      // and convert joystick angle to positive analog signal from 0 to 1024
      digitalWrite(X_IN1_PIN, 0);
      digitalWrite(X_IN2_PIN, 1);
      digitalWrite(X_IN1_PIN2, 0);
      digitalWrite(X_IN2_PIN2, 1);
      analogWrite(X_PWM_PIN, (prevMotorX-1) * 8);
      analogWrite(X_PWM_PIN2, (prevMotorX-1) * 8);
    }
    else {
      //need to release motor ( IN1 IN2 = 0 0 )
      // and set speed to 0
      digitalWrite(X_IN1_PIN, 0);
      digitalWrite(X_IN2_PIN, 0);
      analogWrite(X_PWM_PIN, abs(prevMotorX) * 8);
      digitalWrite(X_IN1_PIN2, 0);
      digitalWrite(X_IN2_PIN2, 0);
      analogWrite(X_PWM_PIN2, abs(prevMotorX) * 8 );
    }
  }
} // Loop
