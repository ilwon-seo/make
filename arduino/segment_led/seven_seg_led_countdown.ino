#include "SevSeg.h"
//
// https://www.circuitbasics.com/arduino-7-segment-display-tutorial/
////////////////////////////////////////////////////////////
// Parts
//----------------------------------------------------------
// 1k resister X 1
// Common Cathode Displays 7-seg with dot X 1 (PART:5161AS)
// library SevSeg (MIT License, https://github.com/DeanIsMe/SevSeg)
////////////////////////////////////////////////////////////
SevSeg sevseg;
int counter[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
unsigned long delayMs = 1000;
void setup() {
  byte numDigits = 1;
  byte digitPins[] = {};
  byte segmentPins[] = { 6, 5, 2, 3, 4, 7, 8, 9 };
  bool resistorsOnSegments = true;

  byte hardwareConfig = COMMON_CATHODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(80);
}

void loop() {
  for (int c : counter) {
    sevseg.setNumber(c);
    sevseg.refreshDisplay();
    delay(delayMs);
  }
}
