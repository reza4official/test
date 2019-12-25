#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <list>

typedef void (*voidFuncPtr)(void);

class Reader
{
private:
    int sensorPin;
   
public:
    Reader(int sensorPin);

    void setSensor(bool isOn);
    void setOnMotionDetected(voidFuncPtr onMotionDetected);
};