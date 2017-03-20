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

#include "codes.h"

#define DEBUG 1

#ifdef ADV_DEBUG
  #include <MemoryFree.h>
#endif

#define MAX_SSID_LENGTH 20
#define MAX_PSW_LENGTH 20

#define MAX_BUFFER_LENGTH 30
#define MAX_NAME_SIZE 30
#define MAX_CONTENT_SIZE 30
#define MAX_URI_SIZE 15
#define MAX_ID_SIZE 20
#define MAX_SUBSCRIPTION_SIZE 30

#define MAX_PACKET_SIZE 60
#define TX_LATENCY 2000

struct Contents {
  char type[MAX_NAME_SIZE] = "";
  char content[MAX_CONTENT_SIZE] = "";
  Contents* next;
};

struct Triple {
  char subject[MAX_URI_SIZE];
  char predicate[MAX_URI_SIZE];
  char object[MAX_URI_SIZE];
};


class KP {
  public:

    KP(char ID[] = "arduinoKP", short TR = 1);

    void begin(char SSID[MAX_SSID_LENGTH], char psw[MAX_PSW_LENGTH], short p = 10010, byte a = 0, byte b = 0, byte c = 0, byte d = 0); //Connect to WiFi network , then to

    char* getID();
    //void setID(char id[]); //random initialization at creation , unless explicitly defined in the instance call
    short getTR();
    byte getState();//returns the current state of the machine (error code)

    void join();
    void rdfInsert(Triple cont);
    void rdfQuery(Triple cont);
    void rdfSubscribe(Triple cont);
    void rdfRemove(Triple cont);
    void leave();



  private:
    char _nodeID[MAX_ID_SIZE];
    short _trID;

    short _port; //port used by the SIB server
    byte _ip[4];
    char _subID1[MAX_SUBSCRIPTION_SIZE];
    char _subID2[MAX_SUBSCRIPTION_SIZE];

    byte _status;

    WiFiClient _comm; //socket used for main communication
    //WiFiClient* _s1,_s2,_last; //two socket available for communication , plus one eventually used for the "last will"


    void receive();
    void store();
    void composeMessage(char code, Triple t);
    void analyzeMessage();
    Contents create(char type, char *state , Triple t);

    void transaction(char type, Triple c);

};

byte processChar(const char c, byte lastState);

#include "utils.h"
#include "wrappers.h"

#endif
