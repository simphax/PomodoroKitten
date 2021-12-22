#include <Arduino.h>
#include "ShakeDetection.h"

static const int FORCE_THRESHOLD = 1200;
static const int TIME_THRESHOLD = 100;
static const int SHAKE_TIMEOUT = 500;
static const int SHAKE_DURATION = 2000;
static const int SHAKE_COUNT = 3;

ShakeDetection::ShakeDetection() {
  
}

bool ShakeDetection::setup() {
  return true;
}

bool ShakeDetection::loop(int acc_x) {
  unsigned long now = millis();
  if ((now - lastForceTime) > SHAKE_TIMEOUT) {
    shakeCount = 0;
  }
  
  if ((now - lastTime) > TIME_THRESHOLD) {
    unsigned long diff = now - lastTime;
    int speed = abs(acc_x - lastX) / diff * 1000;
    if (speed > FORCE_THRESHOLD) {
      if ((++shakeCount >= SHAKE_COUNT) && (now - lastShakeTime > SHAKE_DURATION)) {
        lastShakeTime = now;
        shakeCount = 0;
        if(onShakeCallback) onShakeCallback();
      }
      lastForceTime = now;
    }
    lastTime = now;
    lastX = acc_x;
  }

  return true;
}

void ShakeDetection::onShake(const std::function<void(void)>& f) {
  onShakeCallback = f;
}
