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

#define FOLLOWER 8

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
} ledRow;


void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  trellis.begin();
  trellis.setBrightness(80);
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

void loop() {
  trellis.tick();

  while (trellis.available()){
    keypadEvent e = trellis.read();
    int key = e.bit.KEY;

    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      //update struct
      buttonPress.key = key;
      buttonPress.pressed = true;
      transmitButtonPress(buttonPress);

      // Serial.print(key); Serial.println(" pressed\n");
      trellis.setPixelColor(key, 0xFFFFFF);
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      // Serial.print(key); Serial.println(" released\n");
      trellis.setPixelColor(key, 0x0);
      buttonPress.key = key;
      buttonPress.pressed = false;
      transmitButtonPress(buttonPress);
    }
  }

  Wire.requestFrom(FOLLOWER, sizeof(LedRow));   
  while (Wire.available()) { 
    Wire.readBytes((byte*) &ledRow, sizeof ledRow);
  }
  setRow(ledRow);
  delay(1);
}

unsigned long convertRGBtoHex(int r, int g, int b) {   
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}