#include "ArduinoKPI.h"

WiFiClient KPI;

void setup() {
  Serial.begin(9600);

  if (WiFiConnect("Xperia", "bubu1234")) {
    Serial.println("connected");
  }
  else {
    Serial.println("cant connect");
    //while (1);
  }

  IPAddress server(192, 168, 43, 52);
  KPI = connectToSib(server, 10010);
  if (KPI == NULL) {
    Serial.println("Cant connect to server");
    while (1);
  }

  KP kpi;
  kpi.client=KPI;
  strcpy(kpi.nodeID,"arduino");
  kpi.trID=1;
  
  join(kpi);
  
  Triple c;
  strcpy(c.subject,"http://ns#a");
  strcpy(c.predicate,"http://ns#b");
  strcpy(c.object,"http://ns#c");
  kpi.trID++;
  rdfInsert(kpi,c);
}

void loop() {


  while (KPI.available()) receiveAndStore(0, KPI);

}

