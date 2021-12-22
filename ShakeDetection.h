#ifndef _SHAKEDETECTION_H
#define _SHAKEDETECTION_H

#include <Arduino.h>

class ShakeDetection
{
  public:
    ShakeDetection();
    bool setup();
    void onShake(const std::function<void(void)>& f);
    bool loop(int acc_x);

  protected:
    std::function<void(void)> onShakeCallback;
    int shakeCount;
    unsigned long lastForceTime;
    unsigned long lastShakeTime;
    unsigned long lastTime;
    int lastX;
};

#endif
