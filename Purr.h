#ifndef _PURR_H
#define _PURR_H

#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
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
    AudioGeneratorWAV *wav = NULL;
    AudioFileSourcePROGMEM *file = NULL;
    AudioOutputI2SNoDAC *out = NULL;
};

#endif
