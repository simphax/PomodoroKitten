#ifndef _YAWN_H
#define _YAWN_H

#include <Arduino.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
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
    AudioGeneratorMP3 *mp3;
    AudioFileSourceSPIFFS *file;
    AudioOutputI2SNoDAC *out;
    AudioFileSourceID3 *id3;
};

#endif
