#include "ArduinoKPI.h"


#define WIFI1 "ASUS", "miofigliomaggiore88"
#define WIFI2 "Xperia", "bubu1234"



void setup() {
  Serial.begin(57600);
  KP kpi;
  if (WiFiConnect(WIFI2)) {
    Serial.println(F("connected"));
  }
  else {
    Serial.println(F("cant connect"));
    //while (1);
  }

  IPAddress server(192, 168, 43, 52);
  kpi.client = connectToSib(server, 10010);
  if (kpi.client == NULL) {
    Serial.println(F("Cant connect to server"));
    //while (1);
  }

  strcpy_P(kpi.nodeID, PSTR("arduino"));
  kpi.trID = 1;

  join(kpi);

  Triple c;
  strcpy(c.subject, "http://ns#a");
  strcpy(c.predicate, "http://ns#b");
  strcpy(c.object, "http://ns#c");
  kpi.trID++;
  rdfInsert(kpi, c);
  leave(kpi);
}

void loop() {


  //while (kpi.client.available()) receiveAndStore(0, kpi.client);

}

