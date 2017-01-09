#include "ArduinoKPI.h"

#define WIFI_TIMEOUT 5000
#define SS_SD_PIN 4



bool WiFiConnect(const char ssid[],const char pass[]);

WiFiClient connectToSib(IPAddress server,  short port);

bool initializeSD();
