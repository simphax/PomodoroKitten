#ifndef _YAWN_H
#define _YAWN_H

#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

class Yawn
{
  public:
    Yawn();
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
