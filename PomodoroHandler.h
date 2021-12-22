#ifndef _POMODOROHANDLER_H
#define _POMODOROHANDLER_H

#include <Arduino.h>

class PomodoroHandler
{
  public:
    PomodoroHandler();
    bool setup();
    bool play();
    bool loop();

  protected:
    AudioGeneratorMP3 *mp3;
    AudioFileSourceSPIFFS *file;
    AudioOutputI2SNoDAC *out;
    AudioFileSourceID3 *id3;
};

#endif
