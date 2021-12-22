#ifdef ESP32
  #include <WiFi.h>
  #include "SPIFFS.h"
#else
  #include <ESP8266WiFi.h>
#endif
#include "Yawn.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

// To run, set your ESP8266 build to 160MHz, and include a SPIFFS of 512KB or greater.
// Use the "Tools->ESP8266/ESP32 Sketch Data Upload" menu to write the MP3 to SPIFFS
// Then upload the sketch normally.

Yawn::Yawn() {
  
}

bool Yawn::setup() {
  return true;
}

bool Yawn::stop() {
  if (mp3 && mp3->isRunning()) mp3->stop();
  return true;
}

bool Yawn::play() {
  if (mp3 && mp3->isRunning()) mp3->stop();
  file = new AudioFileSourceSPIFFS("/yawn.mp3");
  out = new AudioOutputI2SNoDAC();
  id3 = new AudioFileSourceID3(file);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  return true;
}

bool Yawn::loop() {
  if (mp3 && mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
      return false;
    }
    return true;
  }
  return false;
}
