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

#ifdef ADV_DEBUG
  #include <MemoryFree.h>
#endif

#define MAX_BUFFER_LENGTH 30
#define MAX_NAME_SIZE 30
#define MAX_CONTENT_SIZE 30
#define MAX_URI_SIZE 15
#define MAX_ID_SIZE 20
#define MAX_SUBSCRIPTION_SIZE 30

#define MAX_PACKET_SIZE 60
#define TX_LATENCY 2000

struct KP{
  char nodeID[MAX_ID_SIZE];
  short trID;
  WiFiClient client;
  char sub[MAX_SUBSCRIPTION_SIZE];
};

struct Contents {
  char type[MAX_NAME_SIZE]="";
  char content[MAX_CONTENT_SIZE]="";
  Contents* next;
};

struct Triple {
  char subject[MAX_URI_SIZE];
  char predicate[MAX_URI_SIZE];
  char object[MAX_URI_SIZE];
};


//----------------------------------------------------FUNCTIONS-------------------------------------------

byte processChar(const char c, byte lastState);

bool receive(WiFiClient kpi);

bool store();

bool analyzeMessage();

Contents addCont(char type[MAX_NAME_SIZE], char cont[MAX_CONTENT_SIZE]);

bool composeMessage(char code, KP kpi, Triple t);


#include "utils.h"
#include "wrappers.h"

#endif
