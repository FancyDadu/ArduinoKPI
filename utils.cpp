#include "utils.h"

boolean HasSDBegun = false;

bool WiFiConnect(const char ssid[], const char pass[]) {

  int status = WL_IDLE_STATUS;

#ifdef SHIELD_STATUS_CONTROL
  if (WiFi.status() == WL_NO_SHIELD || WiFi.firmwareVersion() != F("1.1.0")) {

#ifdef DEBUG
    Serial.println(F("Error on WiFi Shield"));
#endif
    return WIFI_BAD_SHIELD;
  }
#endif

  unsigned long startingTime = millis();

  status = WiFi.begin(ssid, pass);

  while (status != WL_CONNECTED) {


    if (millis() - startingTime > WIFI_TIMEOUT) {

#ifdef DEBUG
      Serial.println(F("WiFi timed out!"));
#endif

      return WIFI_TIME_OUT;
    }
    status = WiFi.begin(ssid, pass);


  }

  return WIFI_OK;

}

//--------------------------------------------------------------------------------------------------------------

bool initializeSD() {

  if(HasSDBegun) return SD_OK;

#ifdef DEBUG
  Serial.println(F("Init SDCard..."));
#endif

  if (SD.begin(SS_SD_PIN)) {
#ifdef DEBUG
    Serial.println(F("SDCard ok."));
#endif
    HasSDBegun = true;
    return SD_OK;
  }
  
#ifdef DEBUG
  Serial.println(F("SD Error!"));
#endif
  return SD_ERR_INI;
}

