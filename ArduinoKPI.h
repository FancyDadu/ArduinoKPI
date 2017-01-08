#ifndef ArduinoKPI.h
#define ArduinoKPI.h

#if ARDUINO>=100
	#include "Arduino.h"
#else
	#include "WProgram.h"
	#include "pins_arduino.h"
	#include "WCostants.h"
	
#endif

#include <WiFi.h>
#include <SPI.h>
#include <SD.h>

#define DEBUG 1
#define WIFI_TIMEOUT 10000
#define MAX_BUFFER_LENGTH 30
#define MAX_NAME_SIZE 50
#define MAX_CONTENT_SIZE 30

#define SS_SD_PIN 4

struct Contents;


byte processChar(const char c, byte lastState);

bool WiFiConnect(const char ssid[],const char pass[]);

WiFiClient connectToSib(IPAddress server,  short port);

bool receiveAndStore(bool mode, WiFiClient KPI);

bool composeMessage(char* type,Contents* list);

bool join(short trId, char nodeId[],WiFiClient KPI);

bool initializeSD();

#endif
