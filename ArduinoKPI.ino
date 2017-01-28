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

  store();

  /*
      join(kpi);
      (kpi.client).println();

      delay(4000);
      if((kpi.client).available()>0){
        receive(kpi.client);
        store();
        Serial.println(F("Received response!"));
      }
  */
  Serial.println(F("FINISHED!!!"));
}

void loop() {

}

