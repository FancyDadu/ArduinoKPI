#include "utils.h"

boolean HasSDBegun = false;

WiFiClient connectToSib(IPAddress server,  short port) {
  WiFiClient client;
  if (client.connect(server, port)) return client;
  return NULL;
}

//--------------------------------------------------------------------------------------------------------------

bool WiFiConnect(const char ssid[], const char pass[]) {

  int status = WL_IDLE_STATUS;

#ifdef SHIELD_STATUS_CONTROL
  if (WiFi.status() == WL_NO_SHIELD || WiFi.firmwareVersion() != "1.1.0") {

#ifdef DEBUG
    Serial.println(F("Error on WiFi Shield"));
#endif
    return false;
  }
#endif

  unsigned long startingTime = millis();

  status = WiFi.begin(ssid, pass);

  while (status != WL_CONNECTED) {


    if (millis() - startingTime > WIFI_TIMEOUT) {

#ifdef DEBUG
      Serial.println(F("WiFi timed out!"));
#endif

      return false;
    }
    status = WiFi.begin(ssid, pass);


  }

  return true;

}

//--------------------------------------------------------------------------------------------------------------

bool initializeSD() {

  if(HasSDBegun) return true;

#ifdef DEBUG
  Serial.println(F("Init SDCard..."));
#endif

  if (SD.begin(SS_SD_PIN)) {
#ifdef DEBUG
    Serial.println(F("SDCard ok."));
#endif
    HasSDBegun = true;
    return true;
  }
  
#ifdef DEBUG
  Serial.println(F("SD Error!"));
#endif
  return false;
}

