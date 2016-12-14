#include "ArduinoKPI.h"
boolean HasSDBegun = false;

//--------------------------------------------------------------------------------------------------------------

byte processChar(const char c, byte lastState) {
  byte state;
  /*
    state:byte -> Bb7b6....b0
    b7 : is a special character
    b6 : is a terminating character -> /0
    b5 : is an entity reference -> &
    b4 : a tag is opened -> <
    b3 : a tag is closed -> >
    b2 : is a closing tag-> </
    b1 : whitespace
    b0 : ERROR



  */

  switch (c) {
    case '<'://a tag has opened
      if (bitRead(lastState, 4)) return 1;
      return B10010000;
      break;

    case '>'://tag has closed
      if (bitRead(lastState, 3)) return 1;
      else if (bitRead(lastState, 2)) return B10001010;
      return B10001000;
      break;

    case '/'://if a tag is opened, then it's a closing tag
      if (bitRead(lastState, 4))return B10000100;
      return 0;
      break;

    case '\0'://terminating character
      return B11000000;
      break;

    case ' '://ignore it if it's in a tag
      return B00000010;
      break;

    case '&': //entity reference? -> need to analyze those after
      break;

    default: //whatever else,either it's part of the tag name, or an entity reference,or parameter,or
      ;

  }

}

//--------------------------------------------------------------------------------------------------------------

WiFiClient connectToSib(IPAddress server,  short port) {
  WiFiClient client;
  if (client.connect(server, 10010)) return client;
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

  do  {

    status = WiFi.begin(ssid, pass);

    if (millis() - startingTime > WIFI_TIMEOUT) {

#ifdef DEBUG
      Serial.println(F("WiFi timed out!"));
#endif

      return false;
    }

  } while (status != WL_CONNECTED);

  return true;

}

//--------------------------------------------------------------------------------------------------------------

bool initializeSD() {
  if (!HasSDBegun && SD.begin(SS_SD_PIN)) {
#ifdef DEBUG
    Serial.println(F("Initializing SDCard..."));
#endif
    HasSDBegun = true;
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------------------

bool receiveAndStore(bool mode, WiFiClient KPI) {
  /*
     this function handles the reception of the message from the server and consequently stores it in the SDCard,accordingly to the mode selected
  */
  if (!HasSDBegun && !initializeSD()) {
#ifdef DEBUG
    Serial.println(F("Error on Initializing SD!"));
#endif
    return false;
  }

  File storage;
  storage = SD.open("Store.xml", FILE_WRITE); //gonna change this with a dynamic name

  if (storage) {

    char buffer[MAX_BUFFER_LENGTH] = {""}, element[MAX_BUFFER_LENGTH], tag[MAX_BUFFER_LENGTH];
    short bufCount = 0;


    byte state = 0, parentBefore = false, tagHasClosed = false, parents = 0;
    bool ignore = false;


    while (KPI.available()) {

      while (bufCount < MAX_BUFFER_LENGTH && KPI.available() ) { //while there's still stuff to read and available space , store it in buffer
        buffer[bufCount] = KPI.read();
        bufCount++;
      }

      if (bufCount > 0) { //if there's anything inside the buffer

        if (!mode) { //store without formatting
          storage.write(buffer, bufCount);

          for (int j = 0; j < bufCount; j++) {
            buffer[j] = "";
          }
          bufCount = 0;
        }


        else {
          for (int i = 0; i < bufCount ; i++) {
            state = processChar(buffer[i], state);

            switch (state) {
              case 1:
                ;//error!

              case B10010000: //<
                if (!parentBefore) {
                  parentBefore = true;
                  parents++;
                }
                if (tagHasClosed) {
                  storage.println();
                }
                break;

              case B10001000: // >
                tagHasClosed = true;
                break;

              case B10001010: // > + closingTag
                if (parentBefore) storage.println();

              case B00000010: //whitespace
                ignore = true;
                break;


            }

            if (!ignore) storage.print(buffer[i]);

          }

        }
      }



    }

#ifdef DEBUG
    Serial.println(F("Communication Ended. Closing file now.."));
#endif

    storage.close();
  }

  else {
#ifdef DEBUG
    Serial.println(F("Error on Opening File!"));
#endif
    return false;
  }

}



