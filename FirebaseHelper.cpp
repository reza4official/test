#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
#include <FirebaseHelper.h>

#define FIREBASE_HOST "light-control-fe6f8.firebaseio.com"
#define FIREBASE_AUTH "VrkBz0o2te1yZFSy7AaoHKMJcbtFLKsd832G0T9y"
const String ROOT = "configuration/";

FirebaseHelper::FirebaseHelper(const String ssid, const String password, String sensorId, int firebaseFetchInterval)
{
    FirebaseHelper::ssid = ssid;
    FirebaseHelper::password = password;
    FirebaseHelper::sensorId = sensorId;
    FirebaseHelper::firebaseFetchInterval = firebaseFetchInterval;
}

bool FirebaseHelper::queryNeighborLamp()
{
    for (int i = 0; i < neighborsCount; i++)
    {
        String neighborKey = Firebase.getString(ROOT + sensorId + "/neighbors/" + i);
        bool isNeighborLampOn = Firebase.getBool(ROOT + neighborKey + "/isLampOn");
        if (isNeighborLampOn)
        {
            return true;
        }
    }

    return false;
}

/** get node state from firebase */
void FirebaseHelper::fetchNodeData()
{
    // neighbor info
    neighborsCount = Firebase.getInt(ROOT + sensorId + "/neighbors_count");
    otherCount = Firebase.getInt(ROOT + sensorId + "/others_count");
    isNeighborLampOn = queryNeighborLamp();

    // current node info
    bypassMode = Firebase.getInt(ROOT + sensorId + "/bypassMode");
    isRoot = Firebase.getBool(ROOT + sensorId + "/isRoot");
    isSensorOn = Firebase.getBool(ROOT + sensorId + "/isSensorOn");
    isLampOn = Firebase.getBool(ROOT + sensorId + "/isLampOn");
}

void FirebaseHelper::setIsLampOn(bool isLampOn)
{
    Firebase.setBool(ROOT + sensorId + "/isLampOn", isLampOn);
}

void FirebaseHelper::setIsSensorOn(bool isSensorOn)
{
    Firebase.setBool(ROOT + sensorId + "/isSensorOn", isSensorOn);
}

void FirebaseHelper::setNeighborSensors()
{
    for (int i = 0; i < neighborsCount; i++)
    {
        String neighborKey = Firebase.getString(ROOT + sensorId + "/neighbors/" + i);
        Firebase.setBool(ROOT + neighborKey + "/isSensorOn", true);
    }
    for (int i = 0; i < otherCount; i++)
    {
        String otherKey = Firebase.getString(ROOT + sensorId + "/others/" + i);
        Firebase.setBool(ROOT + otherKey + "/isSensorOn", false);
    }
}

void FirebaseHelper::setupTimedCheckData(ETSTimerFunc *fn)
{
    os_timer_setfn(&myTimer, fn, NULL);                                      // set interrupt callback
    os_timer_arm(&myTimer, firebaseFetchInterval /*s*/ * 1000 /*ms*/, true); // setup timer, sample every 5s
}

void FirebaseHelper::firebaseConnect()
{
    Serial.print("connecting to WIFI");
    WiFi.begin(FirebaseHelper::ssid, FirebaseHelper::password); // Begin WiFi connection
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("connecting to firebase");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Serial.println("connected to firebase");
}

void FirebaseHelper::maintainConnection()
{
    if (WiFi.status() != WL_CONNECTED || Firebase.failed())
    {
        Serial.println("Disconnected: Re-Try on 3s");
        firebaseConnect();
        delay(3 /*second*/ * 1000 /*ms*/); // reconnecting after waiting 3 s
    }
    else
    {
        Serial.println("Still Connected...");
    }
}