#ifdef ESP32
  #include <WiFi.h>
  #include "SPIFFS.h"
#else
  #include <ESP8266WiFi.h>
#endif
#include "Yawn.h"
#include "yawnWav.h"
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
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();
  return true;
}

bool Yawn::stop() {
  if (wav && wav->isRunning()) wav->stop();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
  delay(10);
  return true;
}

bool Yawn::play() {
  if (wav) if(wav->isRunning()) wav->stop();
  file = new AudioFileSourcePROGMEM( yawnWav, sizeof(yawnWav) );
  wav->begin(file, out);
  return true;
}

bool Yawn::loop() {
  if (wav && wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
      return false;
    }
    return true;
  }
  return false;
}
