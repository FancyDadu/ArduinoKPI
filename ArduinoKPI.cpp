#include "ArduinoKPI.h"

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

bool receiveAndStore(bool mode, WiFiClient KPI) {
  /*
     this function handles the reception of the message from the server and consequently stores it in the SDCard,accordingly to the mode selected
  */
  if (!initializeSD()) return false;

  File storage;
  storage = SD.open(F("store.xml"), FILE_WRITE); //gonna change this with a dynamic name

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

        /*
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
        */
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

bool analyzeMessage();

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

Contents subs(char type, char *state , KP kpi , Triple t) {

  Contents c;

  /*3 bits -> 8 combinations:
     trId,nodeId,unsub,sub,obj,pred
     the state coupled with the type of message that is being built will determine whether there's still content to be built;
     otherwise the returning struct will have a field indicating that no more content is going to be added / searched for
  */

  switch (*state) {

    case 'i':
      strcpy_P(c.type, PSTR("transaction_id"));
      itoa(kpi.trID, c.content, 10);
      *state = 'n';
      break;

    case 'n':
      strcpy_P(c.type, PSTR("node_id"));
      strcpy(c.content, kpi.nodeID);
      if (type == 'j' || type == 'l') *state = 'f'; //"finished"
      else if (type == 'u') *state = 'u';
      else *state = 's';
      break;

    case 'u':
      strcpy_P(c.type, PSTR("parameter name = \"subscription_id\""));
      strcpy(c.content, kpi.sub);
      *state = 'f';

      break;

    case 's':
      strcpy_P(c.type, PSTR("subject type=\"uri\""));
      strcpy(c.content, t.subject);
      *state = 'p';
      break;

    case 'p':
      strcpy_P(c.type, PSTR("predicate"));
      strcpy(c.content, t.predicate);
      *state = 'o';
      break;

    case 'o':
      strcpy_P(c.type, PSTR("object type=\"uri\""));
      strcpy(c.content, t.object);
      *state = 'f';
      break;

    case 'f':
    case 'z':
      *state = 'z';

  }
  return c;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

bool composeMessage(char code, KP kpi, Triple t) { //type can be

  if (!initializeSD()) return false;

  File xtemp;
  char name[5] = "";
  name[0] = code;

  strcat(name, "temp");
  Serial.println(name);
  xtemp = SD.open(name, FILE_READ);

  if (xtemp) {
#ifdef DEBUG
    Serial.println(F("File successfully opened"));

#endif

    char c, cState = 'i';
    char tagName[MAX_NAME_SIZE] = {""};

    byte i = 0;
    byte state = 0, lastState = 0 ;

    bool readingName = false, last = true;

    Contents curr = subs(code, &cState, kpi, t);

    while (xtemp.available() > 0) {

      c = xtemp.read();
      (kpi.client).write(c);

      if (cState != 'z') {

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
            Serial.print(curr.type);
            Serial.println("-");
#endif

            if (strncmp(tagName, curr.type, i) == 0) {

#ifdef DEBUG
              Serial.println(F("Found content!: "));
              Serial.println(tagName);
#endif
              (kpi.client).print(curr.content);
              curr = subs(code, &cState, kpi, t);

            }
            strncpy(tagName, "", sizeof(tagName));
            i = 0;
          }
          readingName = false;
        }

      }

      if (readingName) tagName[i++] = c;

      lastState = state;
    }

    (kpi.client).println();
    xtemp.close();
  }

  else {
#ifdef DEBUG
    Serial.print(F("Error File: "));
    Serial.println(code);
#endif
    return false;
  }
}

//------------------------------------------------------------------------------------------------------------------------------------


