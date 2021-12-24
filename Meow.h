#ifndef _MEOW_H
#define _MEOW_H

#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

class Meow
{
  public:
    Meow();
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
