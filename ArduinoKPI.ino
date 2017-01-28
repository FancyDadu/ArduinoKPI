#include "ArduinoKPI.h"


#define WIFI1 "ASUS", "miofigliomaggiore88"
#define WIFI2 "Xperia", "bubu1234"
#define WIFI3 "Vodafone-33346004", "12TREstell4"

void setup() {
  Serial.begin(57600);
  KP kpi;
  if (WiFiConnect(WIFI3)) {
    Serial.println(F("connected"));
  }
  else {
    Serial.println(F("cant connect"));
    //while (1);
  }

  IPAddress server(192, 168, 1, 5);
  kpi.client = connectToSib(server, 10010);
  if (kpi.client == NULL) {
    Serial.println(F("Cant connect to server"));
    //while (1);
  }

  strcpy_P(kpi.nodeID, PSTR("arduino"));
  kpi.trID = 1;

    join(kpi);
    kpi.trID++;
    
    delay(4000);
    while(kpi.client.available()>0) Serial.write(kpi.client.read());
    Serial.println();

    Triple c;
    strcpy(c.subject, "http://ns#a");
    strcpy(c.predicate, "http://ns#b");
    strcpy(c.object, "http://ns#c");
    

    rdfInsert(kpi, c);
    kpi.trID+=4;
    
    delay(4000);
    while(kpi.client.available()>0) Serial.write(kpi.client.read());
    Serial.println();


    leave(kpi);
    kpi.trID+=4;
    
    delay(4000);
    while(kpi.client.available()>0) Serial.write(kpi.client.read());
    Serial.println();



  Serial.println(F("FINISHED!!!"));
}

void loop() {

}

