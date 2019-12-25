#include <Arduino.h>
#include <Actuator.h>
#include <ESP8266HTTPClient.h>
// #include <FirebaseESP8266.h>
#include <FirebaseArduino.h>

Actuator::Actuator(int sensorPin, int lampPin, int lampOnDuration, int checkInterval)
{
  Actuator::sensorPin = sensorPin;
  Actuator::lampPin = lampPin;
  Actuator::lampOnDuration = lampOnDuration;
  Actuator::checkInterval = checkInterval;
  pinMode(Actuator::lampPin, OUTPUT);
  pinMode(Actuator::sensorPin, OUTPUT);
}

void Actuator::setOnLampEvent(statusFuncPtr onLampEvent)
{
  Actuator::onLampEvent = onLampEvent;
}

void Actuator::setLamp(bool isOn)
{
  Actuator::lastTimeOn = millis();
  Actuator::isLampOn = isOn;
  digitalWrite(lampPin, isOn ? HIGH : LOW);
  onLampEvent(isOn);
}

void Actuator::runOffTimer()
{
  long passedTime = (millis() - lastTimeOn) / (60 /*second*/ * 1000 /*miliis*/);
  if (passedTime >= lampOnDuration)
  {
    setLamp(false);
  }
}

void Actuator::setSensor(bool isOn)
{
  digitalWrite(sensorPin, isOn ? HIGH : LOW);
}

void Actuator::setupTimedCheckLamp(ETSTimerFunc *fn)
{
  os_timer_setfn(&myTimer, fn, NULL);                              // set interrupt callback
  os_timer_arm(&myTimer, checkInterval /*s*/ * 1000 /*ms*/, true); // setup timer, sample every 5s
}