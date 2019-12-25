#include <Arduino.h>
#include <Reader.h>
#include <FirebaseHelper.h>
#include <Actuator.h>

/** 
 * CONFIG VARIABLE
 */
const char *ssid = "Razer WiFi";
const char *password = "01454765";
const int baudRate = 9600;
const char *sensorId = "sensor1";
const int sensorPin = 13;
const int sensorRelayPin = 5;
const int lampRelayPin = 2;
const int lampOnDuration = 5;        // in minute
const int firebaseFetchInterval = 5; // in second
const int lampUpdateInterval = 5;    // in second
/// end of config

Reader reader(sensorPin);
Actuator actuator(sensorRelayPin, lampRelayPin, lampOnDuration, lampUpdateInterval);
FirebaseHelper firebaseHelper(ssid, password, sensorId, firebaseFetchInterval);

ICACHE_RAM_ATTR void onMotionDetected()
{
  Serial.println("-----------");
  Serial.println("terjadi pergerakan di " + String(sensorId));
  Serial.println("-----------");

  // then
  firebaseHelper.setNeighborSensors();
  actuator.setLamp(true);
}

// update firebase data every 5
void onFirebaseShouldFetch(void *timed)
{
  // when
  firebaseHelper.fetchNodeData();

  // then
  int bypassMode = firebaseHelper.bypassMode;
  bool isRoot = firebaseHelper.isRoot;                     // is current sensor a root (sensor is always on)
  bool isNeighborLampOn = firebaseHelper.isNeighborLampOn; // is other neighbor lamp turned on
  bool isSensorOn = firebaseHelper.isSensorOn;             // is current sensor should be On
  bool isLampOn = firebaseHelper.isLampOn;                 // is current lamp currently On

  // if root or isNeighborLampOn or isLampOn, override sensor activation
  if (isRoot || isNeighborLampOn || isLampOn)
  {
    isSensorOn = true;
    firebaseHelper.setIsSensorOn(true);
  }

  /* do things base on data */
  actuator.setSensor(isSensorOn);

  // bypass logic
  if (bypassMode == 0)
  {
    actuator.setLamp(false);
  }
  else if (bypassMode == 1)
  {
    actuator.setLamp(true);
  }
}

// check lamp inactivity every 5s, will turned off in 5 minute no activity
void onLampCheck(void *timed)
{
  actuator.runOffTimer();
}

// set firebase status to isOn
void onLampEvent(bool isOn)
{
  firebaseHelper.setIsLampOn(isOn);
}

/** initiate prerequisite */
void setup()
{
  Serial.begin(baudRate);
  Serial.println("-----------");
  Serial.println("after setup");
  Serial.println("-----------");

  // firebase setups
  firebaseHelper.firebaseConnect();
  firebaseHelper.setupTimedCheckData(onFirebaseShouldFetch);

  // reader setups
  reader.setOnMotionDetected(onMotionDetected);

  // actuator setups
  // to run automatic off timer
  actuator.setupTimedCheckLamp(onLampCheck);
  // to set listener after lamp set
  actuator.setOnLampEvent(onLampEvent);
}

/** main loops */
void loop()
{
  // check connection and try to reconnect if disconnected
  firebaseHelper.maintainConnection();
  delay(500);
}