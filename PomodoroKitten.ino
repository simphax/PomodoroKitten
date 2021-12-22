#include <ESP8266WiFi.h>
#include <OneShotTimer.h>
#include <FrameRateCounter.h>
#include "Meow.h"
#include "Yawn.h"
#include "Purr.h"
#include "ShakeDetection.h"
#include "LeanBackDetection.h"
#include "WakeUpDetection.h"

//The unit will go into deep sleep if there has been no pomodoro session in 10 seconds
static const int DEEP_SLEEP_TIMEOUT = 10000;
//Every 5 seconds it will wake up and listen 20ms for movement in the accelerometer
static const int INITIAL_ACTIVITY_CHECK_TIMEOUT = 20;

static const int GREEN_LED_PIN = D2;
static const int YELLOW_LED_PIN = D5;
static const int ACCELEROMETER_X_PIN = A0;

static const int POMODORO_SHORT_BREAK_COUNT = 3;
static const int POMODORO_FOCUS_LENGTH = 5;
static const int POMODORO_SHORT_BREAK_LENGTH = 5;
static const int POMODORO_LONG_BREAK_LENGTH = 5;


static const int FADE_OUT_DURATION = 3000;

//Accelerometer gestures detection
ShakeDetection *shakeDetection;
LeanBackDetection *leanBackDetection;
WakeUpDetection *wakeUpDetection;

//Sounds
Meow *meow;
Yawn *yawn;
Purr *purr;

OneShotTimer pomodoroFocusTimer(POMODORO_FOCUS_LENGTH);
OneShotTimer pomodoroShortBreakTimer(POMODORO_SHORT_BREAK_LENGTH);
OneShotTimer pomodoroLongBreakTimer(POMODORO_LONG_BREAK_LENGTH);
FrameRateCounter pulsingGreenTimer(30); //30 updates per second
FrameRateCounter fadeOutTimer(30); //30 updates per second


//OFF = just booted up, no action
enum PomodoroState { OFF, SLEEP, FOCUS, SHORTBREAK, LONGBREAK };

PomodoroState pomodoroState;

int shortBreaksTaken;

int lastYellowValue;
int lastGreenValue;

unsigned long lastActiveTime;
bool halfAsleep;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);
  SPIFFS.begin();
  
  Serial.println("");
  Serial.println("Checking for activity");
  
  lastActiveTime = millis();
  halfAsleep = true;
  wakeUpDetection = new WakeUpDetection();
  wakeUpDetection->setup(analogRead(ACCELEROMETER_X_PIN));
  wakeUpDetection->onActivity([&]() {
      lastActiveTime = millis();
      halfAsleep = false;
      startUp();
  });

  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}

void loop()
{
  if(halfAsleep) {
    wakeUpDetection->loop(analogRead(ACCELEROMETER_X_PIN));
    
    unsigned long now = millis();
    if(now - lastActiveTime > INITIAL_ACTIVITY_CHECK_TIMEOUT) {
      Serial.println("Going into deep sleep for 5 seconds");
      ESP.deepSleep(5e6, RF_DISABLED);
    }
  } else {
    pomodoroLoop();

    unsigned long now = millis();
    if(pomodoroState == FOCUS || pomodoroState == SHORTBREAK || pomodoroState == LONGBREAK) {
      lastActiveTime = now;
    }
    if(now - lastActiveTime > DEEP_SLEEP_TIMEOUT) {
      Serial.println("Going into deep sleep for 5 seconds");
      ESP.deepSleep(5e6, RF_DISABLED);
    }
  }
}

void startUp() {
  Serial.println("Awake");
  lastYellowValue = 0;
  lastGreenValue = 0;
  
  meow = new Meow();
  meow->setup();
  yawn = new Yawn();
  yawn->setup();
  purr = new Purr();
  purr->setup();
  
  shakeDetection = new ShakeDetection();
  shakeDetection->setup();
  shakeDetection->onShake([&]() {
    pomodoroState = FOCUS;
    pomodoroStateBegin();
  });

  leanBackDetection = new LeanBackDetection();
  leanBackDetection->setup();
  leanBackDetection->onLeanBack([&]() {
      pomodoroState = SLEEP;
      pomodoroStateBegin();
  });

  pulsingGreenTimer.onUpdate([&]() {
    int value = abs((int) (pulsingGreenTimer.frame()*16) % (22 * 128) - (11 * 128)); //22*22/2 = 242
    value = (value*value*2)/(128*128)+1;//Goes from 243-0-243
    analogWrite(GREEN_LED_PIN, value);
    lastGreenValue = value;
  });

  fadeOutTimer.onUpdate([&]() {
    int newGreenValue = lastGreenValue - (fadeOutTimer.msec() / FADE_OUT_DURATION) * lastGreenValue;
    if(newGreenValue < 0) newGreenValue = 0;
    analogWrite(GREEN_LED_PIN, newGreenValue);
    
    int newYellowValue = lastYellowValue - (fadeOutTimer.msec() / FADE_OUT_DURATION) * lastYellowValue;
    if(newYellowValue < 0) newYellowValue = 0;
    analogWrite(YELLOW_LED_PIN, newYellowValue);

    if(newGreenValue <= 0 && newYellowValue <= 0) {
      fadeOutTimer.stop();
    }
  });
  
  pomodoroFocusTimer.onUpdate([&]() {
    if(shortBreaksTaken >= 3) {
      pomodoroState = LONGBREAK;
      shortBreaksTaken = 0;
    } else {
      pomodoroState = SHORTBREAK;
      shortBreaksTaken++;
    }
    pomodoroStateBegin();
  });  
  pomodoroShortBreakTimer.onUpdate([&]() {
    pomodoroState = FOCUS;
    pomodoroStateBegin();
  });  
  pomodoroLongBreakTimer.onUpdate([&]() {
    pomodoroState = FOCUS;
    pomodoroStateBegin();
  });
  
  pomodoroState = OFF;
  pomodoroStateBegin();
}

void pomodoroStateBegin() {
  
  switch(pomodoroState) {
    case OFF:
      Serial.println("OFF");
      break;
    case SLEEP:
      Serial.println("ZZZZZZZ");
      meow->stop();
      purr->stop();
      yawn->play();
      fadeOutTimer.start();
      break;
    case FOCUS:
      digitalWrite(YELLOW_LED_PIN, HIGH);
      lastYellowValue = 255;
      digitalWrite(GREEN_LED_PIN, LOW);
      lastGreenValue = 0;
      Serial.println("MEOW");
      yawn->stop();
      purr->stop();
      meow->play();
      pomodoroFocusTimer.start();
      break;
    case SHORTBREAK:
      digitalWrite(YELLOW_LED_PIN, LOW);
      lastYellowValue = 0;
      digitalWrite(GREEN_LED_PIN, LOW);
      lastGreenValue = 0;
      Serial.println("SHORTBREAK MEOW");
      pulsingGreenTimer.start();
      yawn->stop();
      meow->stop();
      purr->play();
      pomodoroShortBreakTimer.start();
      break;
    case LONGBREAK:
      digitalWrite(YELLOW_LED_PIN, HIGH);
      lastYellowValue = 255;
      digitalWrite(GREEN_LED_PIN, LOW);
      lastGreenValue = 0;
      Serial.println("LONGBREAK MEOW");
      yawn->stop();
      meow->stop();
      purr->play();
      pomodoroLongBreakTimer.start();
      break;
  }
}

//Returns false if it's ok to delay the next loop
bool pomodoroLoop() {
  switch(pomodoroState) {
    case OFF:
      shakeDetection->loop(analogRead(ACCELEROMETER_X_PIN));
      return false;
    case SLEEP:
      shakeDetection->loop(analogRead(ACCELEROMETER_X_PIN));
      if(!yawn->loop()) {
        delay(10);
      }
      fadeOutTimer.update();
      return false;
    case FOCUS:
      leanBackDetection->loop(analogRead(ACCELEROMETER_X_PIN));
      if(!meow->loop()) {
        delay(10);
      }
      pomodoroFocusTimer.update();
      return true;
    case SHORTBREAK:
      leanBackDetection->loop(analogRead(ACCELEROMETER_X_PIN));
      pulsingGreenTimer.update();
      if(!purr->loop()) {
        delay(10);
      }
      pomodoroShortBreakTimer.update();
      return true;
    case LONGBREAK:
      leanBackDetection->loop(analogRead(ACCELEROMETER_X_PIN));
      if(!purr->loop()) {
        delay(10);
      }
      pomodoroLongBreakTimer.update();
      return true;
    default:
      return false;
  }
}
