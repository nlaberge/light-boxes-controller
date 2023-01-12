// Wire Controller Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI peripheral device
// Refer to the "Wire Peripheral Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_NeoTrellisM4.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SleepyDog.h>
#include "I2C_ClearBus.h"

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096), dictates how closed box is
#define SERVOMAX  300 // This is the 'maximum' pulse length count (out of 4096), dictates how open box is
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define FOLLOWER 8

// for MIDI
#define MIDI_CHANNEL     0  // default channel # is 0
// Set the value of first note, C is a good choice. Lowest C is 0.
// 36 is a good default. 48 is a high range. Set to 24 for a bass machine.
#define FIRST_MIDI_NOTE 36

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

struct ButtonPress
{
  uint8_t key;
  boolean pressed;
} buttonPress;

struct LedRow
{
  uint8_t row;
  CRGB col0;
  CRGB col1;
  CRGB col2;
  CRGB col3;
  CRGB col4;
  CRGB col5;
  CRGB col6;
  CRGB col7;
  uint8_t servo0; //servo values, which will be shifted up by SERVOMIN on trellis
  uint8_t servo1;
  uint8_t servo2;
  uint8_t servo3;
  uint8_t servo4;
  uint8_t servo5;
} ledRow;



void setup() {
  I2C_ClearBus(); //clear bus
  
  Wire.begin();        // join i2c bus (address optional for master)
  Wire.setClock(10000); // set i2c clock to 500kHz (default is 100kHz)
  Serial.begin(9600);  // start serial for output
  trellis.begin();
  trellis.setBrightness(80);

  // USB MIDI messages sent over the micro B USB port
  Serial.println("Enabling MIDI on USB");
  trellis.enableUSBMIDI(true);
  trellis.setUSBMIDIchannel(MIDI_CHANNEL);

  pwm.begin(); // pwm for servos
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  int countdownMS = Watchdog.enable(4000); //watchdog, 4s

  delay(20);
}

void transmitButtonPress(ButtonPress bp){
  Wire.beginTransmission(FOLLOWER);  // Transmit to device 
  Wire.write((byte *)&bp,2);   // Sends bytes
  Wire.endTransmission();      // Stop transmitting
}

void setRow(LedRow ledRow){
  int row = ledRow.row; //get row
  int index = row*8; //get starting index
  // set col 0
  uint8_t r = ledRow.col0.r; uint8_t g = ledRow.col0.g; uint8_t b = ledRow.col0.b;
  unsigned long c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
  index = index + 1;
  // set col 1
  r = ledRow.col1.r; g = ledRow.col1.g; b = ledRow.col1.b;
  c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
  index = index + 1;
  // set col 2
  r = ledRow.col2.r; g = ledRow.col2.g; b = ledRow.col2.b;
  c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
  index = index + 1;
  // set col 3
  r = ledRow.col3.r; g = ledRow.col3.g; b = ledRow.col3.b;
  c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
  index = index + 1;
  // set col 4
  r = ledRow.col4.r; g = ledRow.col4.g; b = ledRow.col4.b;
  c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
  index = index + 1;
  // set col 5
  r = ledRow.col5.r; g = ledRow.col5.g; b = ledRow.col5.b;
  c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
  index = index + 1;
  // set col 6
  r = ledRow.col6.r; g = ledRow.col6.g; b = ledRow.col6.b;
  c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
  index = index + 1;
  // set col 7
  r = ledRow.col7.r; g = ledRow.col7.g; b = ledRow.col7.b;
  c = convertRGBtoHex(r, g, b);
  trellis.setPixelColor(index, c);
}

void refreshLeds() {
  // update 4 times, once for each row
  for (int i = 0; i < 4; i++){
    Wire.requestFrom(FOLLOWER, sizeof(LedRow));   
    while (Wire.available()) { 
      Wire.readBytes((byte*) &ledRow, sizeof ledRow);
    }
  setRow(ledRow);
  }
}

int ensurePulselenInBounds(int pulselen){
  // ensure that pulselen is within range
  if (pulselen < SERVOMIN) {
    pulselen = SERVOMIN;
  }
  if (pulselen > SERVOMAX) {
    pulselen = SERVOMAX;
  }
  return pulselen;
}

void updateServos(LedRow ledRow){
  if (ledRow.row == 0){
    // Drive each servo one at a time using setPWM()
    int pulselen;
    pulselen = ledRow.servo0 + SERVOMIN;
    pulselen = ensurePulselenInBounds(pulselen);
    pwm.setPWM(0, 0, pulselen);

    pulselen = ledRow.servo1 + SERVOMIN;
    pulselen = ensurePulselenInBounds(pulselen);
    pwm.setPWM(1, 0, pulselen);
    
    pulselen = ledRow.servo2 + SERVOMIN;
    pulselen = ensurePulselenInBounds(pulselen);
    pwm.setPWM(2, 0, pulselen);
    
    pulselen = ledRow.servo3 + SERVOMIN;
    pulselen = ensurePulselenInBounds(pulselen);
    pwm.setPWM(3, 0, pulselen);
    
    pulselen = ledRow.servo4 + SERVOMIN;
    pulselen = ensurePulselenInBounds(pulselen);
    pwm.setPWM(4, 0, pulselen);
    
    pulselen = ledRow.servo5 + SERVOMIN;
    pulselen = ensurePulselenInBounds(pulselen);
    pwm.setPWM(5, 0, pulselen);
  }
}

void loop() {
  trellis.tick();

  while (trellis.available()){
    keypadEvent e = trellis.read();
    int key = e.bit.KEY;

    // send keypress info to arduino
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      buttonPress.key = key;
      buttonPress.pressed = true;
      transmitButtonPress(buttonPress);
      trellis.setPixelColor(key, 0xFFFFFF);
      trellis.noteOn(FIRST_MIDI_NOTE+key, 64); //midi

    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      trellis.setPixelColor(key, 0x0);
      buttonPress.key = key;
      buttonPress.pressed = false;
      transmitButtonPress(buttonPress);
      trellis.noteOff(FIRST_MIDI_NOTE+key, 64); //midi

    }
  }

  //request trellis led information from arduino
  Wire.requestFrom(FOLLOWER, sizeof(LedRow));   
  while (Wire.available()) { 
    Wire.readBytes((byte*) &ledRow, sizeof ledRow);
  }
  setRow(ledRow);
  updateServos(ledRow);
  trellis.sendMIDI(); // send any pending MIDI messages
  Watchdog.reset(); // reset watchdog timer
  delay(5);
}

unsigned long convertRGBtoHex(int r, int g, int b) {   
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}