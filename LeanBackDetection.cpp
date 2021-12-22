#include <Arduino.h>
#include "LeanBackDetection.h"

static const int OUT_OF_BOUNDS_THRESHOLD = 30;
static const int LEAN_BACK_X = 440;
static const int TIME_AVERAGE = 100;
static const int LEANING_DURATION_THRESHOLD = 3000;

LeanBackDetection::LeanBackDetection() {
  
}

bool LeanBackDetection::setup() {
  unsigned long now = millis();
  lastOutOfBoundsTime = now;
  lastTime = now;
  leaningBack = false;
  return true;
}

bool LeanBackDetection::loop(int acc_x) {
  unsigned long now = millis();

  accumulatedX += acc_x;
  accumulatedCount++;
  if ((now - lastTime) > TIME_AVERAGE) {
    int averageX = accumulatedX / accumulatedCount;
    accumulatedX = 0;
    accumulatedCount = 0;
    Serial.println(averageX);
    int diffFromLeanBack = abs(LEAN_BACK_X - averageX);
    if (diffFromLeanBack > OUT_OF_BOUNDS_THRESHOLD) {
      lastOutOfBoundsTime = now;
      leaningBack = false;
    }
    
    if (!leaningBack && now - lastOutOfBoundsTime  > LEANING_DURATION_THRESHOLD && now - lastTime < LEANING_DURATION_THRESHOLD) { // If it's been within bounds for the duration, and it's not an old measurement (loop has not been called for a long while)
      if(onLeanBackCallback) onLeanBackCallback();
      leaningBack = true; // The callback should only trigger once until it's raised and leaned down again
    }
    
    lastTime = now;
  }

  return true;
}

void LeanBackDetection::onLeanBack(const std::function<void(void)>& f) {
  onLeanBackCallback = f;
}
