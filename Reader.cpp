#include <Arduino.h>
#include <Reader.h>
#include <ESP8266WiFi.h>

Reader::Reader(int sensorPin)
{
    Reader::sensorPin = sensorPin;
    pinMode(sensorPin, INPUT_PULLUP);
}

void Reader::setSensor(bool isOn)
{
    digitalWrite(sensorPin, isOn ? HIGH : LOW);
}

void Reader::setOnMotionDetected(voidFuncPtr onMotionDetected)
{
    attachInterrupt(digitalPinToInterrupt(sensorPin), onMotionDetected, RISING);
}