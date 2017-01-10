#include "ArduinoKPI.h"

void setup() {
  Serial.begin(57600);
  KP kpi;
  if (WiFiConnect("SSID","psw")) {
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

  strcpy_P(kpi.nodeID, PSTR("arduino")); //future versions will change the name dinamically
  kpi.trID = 1;

  join(kpi);

  Triple c;
  strcpy_P(c.subject, PSTR("http://ns#a")); //these
  strcpy_P(c.predicate, PSTR("http://ns#b")); //as well
  strcpy_P(c.object, PSTR("http://ns#c")); //will be changed dinamically
  kpi.trID++; 
  rdfInsert(kpi, c);
  leave(kpi);
}

void loop() {

  /*
  code below has been commented out because declaring the kpi globally would take too much space in SRAM , and for now
  the situation is slightly critical 
  */
  //while (kpi.client.available()) receiveAndStore(0, kpi.client);

}

