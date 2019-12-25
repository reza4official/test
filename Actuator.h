#include <ESP8266WiFi.h>

typedef void (*statusFuncPtr)(bool);
class Actuator
{
private:
    int lampPin;
    int sensorPin;
    int lampOnDuration;
    bool isLampOn;
    long lastTimeOn;
    int checkInterval;
    os_timer_t myTimer; // timer, software interrupt
    statusFuncPtr onLampEvent;

public:
    Actuator(int sensorPin, int lampPin, int lampOnDuration, int checkInterval);

    void setLamp(bool isOn);
    void setSensor(bool isOn);
    void runOffTimer();
    void setupTimedCheckLamp(ETSTimerFunc *fn);
    void setOnLampEvent(statusFuncPtr onLampEvent);
};