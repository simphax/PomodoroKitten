#ifndef _WAKEUPDETECTION_H
#define _WAKEUPDETECTION_H

#include <Arduino.h>

class WakeUpDetection
{
  public:
    WakeUpDetection();
    bool setup(int acc_x);
    void onActivity(const std::function<void(void)>& f);
    bool loop(int acc_x);

  protected:
    std::function<void(void)> onActivityCallback;
    unsigned long lastForceTime;
    unsigned long lastTime;
    int lastX;
};

#endif
