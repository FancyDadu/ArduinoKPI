#include "ArduinoKPI.h"
boolean HasSDBegun = false;

struct Contents {
  char type[MAX_NAME_SIZE];
  char content[MAX_CONTENT_SIZE];
  Contents* next;

};

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
      return 0;
      ;

  }

}

//--------------------------------------------------------------------------------------------------------------

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
#ifdef DEBUG
  Serial.println(F("Initializing SDCard..."));
#endif

  if (!HasSDBegun && SD.begin(SS_SD_PIN)) {
#ifdef DEBUG
    Serial.println(F("SDCard initialized."));
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
  storage = SD.open("Store12.xml", FILE_WRITE); //gonna change this with a dynamic name

  if (storage) {

    char buffer[MAX_BUFFER_LENGTH] = {""};
    short bufCount = 0;


    byte state = 0, parentBefore = false, tagHasClosed = false, parents = 0;
    bool ignore = false;


    while (KPI.available() > 0) {

      while (bufCount < MAX_BUFFER_LENGTH && KPI.available() > 0 ) { //while there's still stuff to read and available space , store it in buffer
        buffer[bufCount++] = KPI.read();
      }

      if (bufCount > 0) { //if there's anything inside the buffer

        if (!mode) { //store without formatting
          storage.write(buffer, bufCount);
          storage.flush();
          strcpy(buffer, "");
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

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

bool composeMessage(char* type, Contents* list, WiFiClient KPI) { //type can be

  if (!HasSDBegun && !initializeSD()) {
#ifdef DEBUG
    Serial.println(F("Error on Initializing SD!"));
#endif
    return false;
  }

  File xtemp;
  //strcat(type, "template.xml");
  xtemp = SD.open(type, FILE_READ);

  if (xtemp);
  else {
#ifdef DEBUG
    Serial.print(F("Error on Opening File: "));
    Serial.println(type);
#endif
    return false;
  }

  char c;
  char tagName[MAX_NAME_SIZE] = {""};
  short i = 0;
  byte state = 0, lastState = 0;
  bool readingName = false;

  Contents* current = list;

  while (xtemp.available() > 0) {

    c = xtemp.read();
    KPI.print(c);

    if (1/*current->next != NULL*/) {

      state = processChar(c, state);

      if (lastState == B10010000 && state != B10000100) { //that is , a tag has been opened but it's not a closing one
        readingName = true;
#ifdef DEBUG
        Serial.println(F("Tag opened "));

#endif
      }

      if (state == B10001000) {
        if (readingName) { //finished reading tagName


#ifdef DEBUG
          Serial.print(F("Found tag: -"));
          Serial.print(tagName);
          Serial.print(F("- Searching for: -"));
          Serial.print(current->type);
          Serial.println("-");
#endif



          /*this is to scroll through the list to find the name we've just composed;in theory it won't be necessary , because the single function that are going to call composeMessage
             will be built so that the list given to this function is already ordered accordingly to the desired template . Should the need to change the template arise , though, then this
             snippet will improve flessibility.
          */
          //while (strcmp(tagName, current->type) != 0 || current->next != NULL) {
          //current = current->next;
          //}


          if (strncmp(tagName, current->type,i) == 0) {
#ifdef DEBUG
            Serial.println(F("Found content!: "));
            Serial.println(tagName);
#endif
            KPI.print(current->content);
            current = current->next;
          }
          for (int j = 0; j <= i; j++) tagName[j] = " ";
          i = 0;
        }
        readingName = false;
      }

    }

    if (readingName) tagName[i++] = c;

    lastState = state;
  }
  KPI.println();

}

//------------------------------------------------------------------------------------------------------------------------------------

bool join(short trId, char nodeId[], WiFiClient KPI) {

  Contents tr, id;

  strcpy(tr.type, "transaction_id");
  itoa(trId, tr.content, 10);
  tr.next = &id;

  strcpy(id.type, "node_id");
  strcpy(id.content, nodeId);
  id.next = NULL;

  composeMessage("join", &tr, KPI);

}

