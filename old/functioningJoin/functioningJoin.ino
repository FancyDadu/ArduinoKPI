
// from the Web Client sketch of the WiFiClient class


#include <SPI.h>
#include <WiFi.h>

char ssid[] = "Vodafone-33346004"; //  your network SSID (name)
char pass[] = "12TREstell4";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192,168,1,7);  // IP of the PC where the SIB is currently running
//char server[] = "SS_PC";    // 

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (10010 is used by the SIB):
WiFiClient client;

void setup() {

  Serial.begin(9600);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus(); //entirely optional for our purposes

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  
  if (client.connect(server, 10010)) {
    Serial.println("connected to server");
    /* NOTE :  The following is the Join XML broken down to single fields and upper layers' opening tags
     *  it appears that the choice between using print or println doesn't affect the overall readibility
     *  for the server : this is proven by the single println of the transaction_id
     *  
     *  The response from the server is slightly confusing though ; after the first JOIN has been confirmed,
     *  if the JOIN request is repeated , the server may either reply with an XML ERROR message ( which would be 
     *  desiderable) ,or won't reply at all ( usually this happens when an ERROR message has already been received ).
     *  
     *  A further analysis will be carried out in order to discover wether this behaviour is caused by Arduino or the server.
     *  
     *  http://mssystems.emscom.net/helpdesk/knowledgebase.php?article=51
     *  The article seems to suggest that this problem may be caused by the WiFi shield , in particular because of the SPI
     *  communication , that when attempting to handle chunks of data which exceed 90 bytes will have a "silent failure and loss of data"
      */
    
    client.print("<SSAP_message>");
    client.print("<message_type>REQUEST</message_type>");
    client.print("<transaction_type>JOIN</transaction_type>");
    client.print("<transaction_id>1</transaction_id>");
    client.print("<node_id>ARDUINO-KPIo</node_id>");
    client.print("<space_id>X</space_id>");
    client.println("</SSAP_message>");

    
  }
}

void loop() {
  
  // The following enables reading of the SIB response on the Serial Console ; 
  // it is not formatted so the response will read as a single continuous line
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}





