/**
 * @file   Bluetooth_Car12.ino
 * @date   2023-12-14
 * @author Estiven Delgado <yeisondelgado@unicauca.edu.co>
 * @author Alejandra Benavides <vbenavides@unicauca.edu.co>
 * @brief  General control functions.
 *
 * @par Copyright
 * Information contained herein is proprietary to and constitutes valuable
 * confidential trade secrets of unicauca, and
 * is subject to restrictions on use and disclosure.
 *
 * @par
 * Copyright (c) unicauca 2023. All rights reserved.
 *
 * @par
 * The copyright notices above do not evidence any actual or
 * intended publication of this material.
 */

#include <DabbleESP32.h>

//Dabble bluetooth
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

// Motor A
int motor1Pin1 = 27;
int motor1Pin2 = 26;
int enable1Pin = 14;

// Motor B
int motor2Pin3 = 25;
int motor2Pin4 = 33;
int enable2Pin = 32;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int pwmChannel2 = 1;
const int resolution = 8;

int totalLeft = 0;
int totalRight = 0;

 /**
 * @brief setup.
 * 
 *Initial settings before entering the loop.
 * 
 * @param void.
 * @return none.
 */
void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // sets the pins as outputs:
  pinMode(motor2Pin3, OUTPUT);
  pinMode(motor2Pin4, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

  // configure LED PWM functionalities
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable2Pin, pwmChannel2);
  Dabble.begin("Bluetooth_Car12");
}

 /**
 * @brief loop.
 * 
 *Main core of the program that runs continuously.
 * 
 * @param void.
 * @return none.
 */
void loop() {
  Dabble.processInput();
  getJoystickSpeed();

  ledcWrite(pwmChannel, totalLeft);
  ledcWrite(pwmChannel2, totalRight);
}

 /**
 * @brief getJoystickSpeed.
 * 
 *Configuration to drive the car from the dabble application.
 * 
 * @param void.
 * @return none.
 */
void getJoystickSpeed() {
  float xval = GamePad.getXaxisData();
  float yval = GamePad.getYaxisData();

  // For calibration
  if (xval < -6) {
    xval = -6;
  } else if (xval > 6) {
    xval = 6;
  }
  if (yval < -6) {
    yval = -6;
  } else if (yval > 6) {
    yval = 6;
  }

  int yMapped = 0;
  if (yval < 0 ) {
    moveBackward();
    yMapped = map(yval, -6, 0, 250, 0);
  } else {
    moveForward();
    yMapped = map(yval, 0, 6, 0, 250);
  }

  int xMapped;
  if (xval < 0 ) {
    xMapped = map(xval, -6, 0, 250, 0);
  } else {
    xMapped = map(xval, 0, 6, 0, 250);
  }

  if (xval < 0 ) {
    totalLeft = yMapped + xMapped;
    if (totalLeft > 250) {
      totalLeft = 250;
    }
    totalRight = yMapped - xMapped;
    if (totalRight < 0) {
      totalRight = 0;
    }
  } else {
    totalLeft = yMapped - xMapped;
    if (totalLeft < 0) {
      totalLeft = 0;
    }

    totalRight = yMapped + xMapped;
    if (totalRight > 250) {
      totalRight = 250;
    }
  }
}

 /**
 * @brief moveForward.
 * 
 *Move the car forward.
 * 
 * @param void.
 * @return none.
 */
void moveForward() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin3, LOW);
  digitalWrite(motor2Pin4, HIGH);
}

 /**
 * @brief moveBackward.
 * 
 *Move the car backwards.
 * 
 * @param void.
 * @return none.
 */
void moveBackward() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin3, HIGH);
  digitalWrite(motor2Pin4, LOW);
}
