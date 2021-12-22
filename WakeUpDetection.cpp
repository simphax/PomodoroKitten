#include <Arduino.h>
#include "WakeUpDetection.h"

static const int FORCE_THRESHOLD = 40;
static const int TIME_THRESHOLD = 5;

WakeUpDetection::WakeUpDetection() {
  
}

bool WakeUpDetection::setup(int acc_x) {
  lastX = acc_x;
  return true;
}

bool WakeUpDetection::loop(int acc_x) {
  unsigned long now = millis();
  if ((now - lastTime) > TIME_THRESHOLD) {
    unsigned long diff = now - lastTime;
    int speed = abs(acc_x - lastX) / diff * 1000;
    if (speed > FORCE_THRESHOLD) {
      if(onActivityCallback) onActivityCallback();
    }
    lastTime = now;
    lastX = acc_x;
  }

  return true;
}

void WakeUpDetection::onActivity(const std::function<void(void)>& f) {
  onActivityCallback = f;
}
