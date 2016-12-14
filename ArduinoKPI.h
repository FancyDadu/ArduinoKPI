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
#define WIFI_TIMEOUT 5000
#define MAX_BUFFER_LENGTH 30

#define SS_SD_PIN 4




byte processChar(const char c, byte lastState);

bool WiFiconnect(const char ssid[],const char pass[]);

WiFiClient connectToSib(const byte ip[4],const short port,bool* conn);

bool receiveAndStore(bool mode, WiFiClient KPI);

char join(short trId,char nodeId[]);

bool initializeSD();

#endif
