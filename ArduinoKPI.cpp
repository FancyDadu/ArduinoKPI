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

bool analyzeMessage();

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

Contents addCont(char type[MAX_NAME_SIZE], char cont[MAX_CONTENT_SIZE]) {
  Contents c;
  strcpy(c.type, type);
  strcpy(c.content, cont);
  return c;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

bool composeMessage(char code, KP kpi, Triple t) { //type can be

  if (!initializeSD()) return false;

  File xtemp;
  char name[10];
  name[0]=code;
  strcat(name,"template");
  xtemp = SD.open(name, FILE_READ);

  if (xtemp) {
#ifdef DEBUG
    Serial.println(F("File successfully opened"));

#endif

    //BUILDING CONTENT STARTS HERE---------------------------------------------

#ifdef ADV_DEBUG
    Serial.println(F("building.."));
#endif

    Contents id, node; //always necessary
    Contents* current;

    current = &id;

    strcpy(id.type, "transaction_id");
    itoa(kpi.trID, id.content, 10);

    node = addCont("node_id", kpi.nodeID);
    current->next = &node;
    current = current->next;

    if (code == 'u') { //unsubscribe
      Contents unsub;
      unsub = addCont("parameter name = \"subscription_id\"", kpi.sub);
      current->next = &unsub;
      current = current->next;
    }

    else if (code == 'i' || code == 'q' || code == 'r' || code == 's') { //insert/query/remove/subscribe -> need triple
      Contents sub, pred, obj;
      sub = addCont("subject type=\"uri\"", t.subject);
      current->next = &sub;
      current = current->next;
      pred = addCont("predicate", t.predicate);
      current->next = &pred;
      current = current->next;
      obj = addCont("object type=\"uri\"", t.object);
      current->next = &obj;
      current = current->next;
    }

    current->next = NULL;

#ifdef ADV_DEBUG
    Serial.println(F("contents :"));
    current = &id;
    while (current != NULL) {
      Serial.print(current->type);
      Serial.println(current->content);
      current = (current->next);
    }
    Serial.println(F("end"));

#endif

    //FINISHES HERE--------------------------------------------------------------------



    char c;
    char tagName[MAX_NAME_SIZE] = {""};
    short i = 0;
    byte state = 0, lastState = 0;
    bool readingName = false;

    current = &id;

    while (xtemp.available() > 0) {

      c = xtemp.read();
      (kpi.client).write(c);

      if (current != NULL) {

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

            if (strncmp(tagName, current->type, i) == 0) {

#ifdef DEBUG
              Serial.println(F("Found content!: "));
              Serial.println(tagName);
#endif
              (kpi.client).print(current->content);
              current = current->next;
            }
            //for (int j = 0; j <= i; j++) tagName[j] = " ";
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


