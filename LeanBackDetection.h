#ifndef _LEANBACKDETECTION_H
#define _LEANBACKDETECTION_H

#include <Arduino.h>

class LeanBackDetection
{
  public:
    LeanBackDetection();
    bool setup();
    void onLeanBack(const std::function<void(void)>& f);
    bool loop();

  protected:
    std::function<void(void)> onLeanBackCallback;
    unsigned long lastTime;
    unsigned long lastOutOfBoundsTime;
    unsigned long lastWithinBoundsTime;
    long accumulatedX;
    int accumulatedCount;
    bool leaningBack;
};

#endif
