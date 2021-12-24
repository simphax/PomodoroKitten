#ifdef ESP32
  #include <WiFi.h>
  #include "SPIFFS.h"
#else
  #include <ESP8266WiFi.h>
#endif
#include "Purr.h"
#include "purrWav.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

// To run, set your ESP8266 build to 160MHz, and include a SPIFFS of 512KB or greater.
// Use the "Tools->ESP8266/ESP32 Sketch Data Upload" menu to write the MP3 to SPIFFS
// Then upload the sketch normally.

Purr::Purr() { 
  
}

bool Purr::setup() {
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();
  return true;
}

bool Purr::stop() {
  if (wav && wav->isRunning()) wav->stop();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
  delay(10);
  return true;
}

bool Purr::play() {
  if (wav) if(wav->isRunning()) wav->stop();
  file = new AudioFileSourcePROGMEM( purrWav, sizeof(purrWav) );
  wav->begin(file, out);
  return true;
}

bool Purr::loop() {
  if (wav && wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
      return false;
    }
    return true;
  }
  return false;
}
