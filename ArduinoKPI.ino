#include "ArduinoKPI.h"

KP kpi;

void setup() {
  Serial.begin(57600);

  do {
    kpi.begin("SIB", "abcd1234", 10010, 192, 168, 43, 52);
  } while (kpi.getState() != 0);

  kpi.join();

  Triple t;
  strcpy(t.subject, "http://ns#a");
  strcpy(t.predicate, "http://ns#b");
  strcpy(t.object, "http://ns#c");
  kpi.rdfInsert(t);
  kpi.leave();

  if (kpi.getState() == 0) Serial.println("ok");
  else Serial.println("nok");

  Serial.println("finished");
  while (1);
}

void loop() {

}

