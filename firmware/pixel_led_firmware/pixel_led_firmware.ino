#include "FastLED.h"
#include <SD.h>

#define COLUMNS 39 // NOTE: change the matrix column size according to your matrix.
#define ROWS  16   // NOTE: change the matrix row size according to your matrix.
#define CS_PIN  D8
#define DATA_PIN D2

byte frameData[COLUMNS * ROWS * 3];
uint16_t delayTime = 100; // NOTE: speed can be changed. Recommended min 50.
uint16_t NUM_LEDS = COLUMNS * ROWS;
unsigned long timer;
boolean isReady = false;
File dataFile;
CRGB leds[COLUMNS * ROWS];

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  //Begin SD Card
  if (!SD.begin(CS_PIN)) {
    return;
  }

  //Open the matrix designs file to read
  dataFile = SD.open("matrix_designs.bin");

  //If file read failes return
  if (!dataFile) {
    return;
  }

  //Reset timer
  timer = millis();
}

void loop() {
  if ((millis() - timer) > delayTime) { //Triggers at interval given in 'delaytime'
    if (isReady) { //Check if ready to display the frame
      timer = millis(); //Reset timer
      showFrame(); //Display the frame
      isReady = false; //Mark not ready then it can read the next frame from the file
    }
  }

  if (!isReady) {
    if (dataFile.available() > 0) { //Check if data available in the file to read
      dataFile.read(frameData, (COLUMNS * ROWS * 3)); //Read a frame. No of bytes ( columns x rows x RGB bytes = COLUMNS x ROWS x 3)
      dataFile.read(); // Read the new line charactor and ignore it
      isReady = true; //Marke as ready to display the frame
    } else {
      dataFile.seek(0); //End of the file. Return back to begining of the file
    }
  }
}

void showFrame() {
  int idxR = 1;
  int idxG = 2;
  int idxB = 3;
  int _idxLed = 0;
  delayTime = frameData[0];
  for (int i = 0; i < COLUMNS; i++) {
    for (int j = 0; j < ROWS; j++) {
      byte _r = frameData[idxR];
      byte _g = frameData[idxG];
      byte _b = frameData[idxB];

      leds[_idxLed] = CRGB(_r, _g, _b);
      idxR += 3;
      idxG += 3;
      idxB += 3;
      _idxLed++;
    }
  }
  FastLED.show();
}
