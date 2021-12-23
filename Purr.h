#ifndef _PURR_H
#define _PURR_H

#include <Arduino.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

class Purr
{
  public:
    Purr();
    bool setup();
    bool stop();
    bool play();
    bool loop();

  protected:
    AudioGeneratorMP3 *mp3 = NULL;
    AudioFileSourceSPIFFS *file = NULL;
    AudioOutputI2SNoDAC *out = NULL;
    AudioFileSourceID3 *id3 = NULL;
};

#endif
