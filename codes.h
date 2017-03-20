#define OK 0x00
//error codes relative to WiFi conditions are 0x1*
#define WIFI_OK 0x10
#define WIFI_CONN_ERR 0x11
#define WIFI_BAD_SHIELD 0x12
#define WIFI_TIME_OUT 0x13

//error codes relative to SD conditions are 0x2*
#define SD_OK 0x20
#define SD_ERR_INI 0x21
#define SD_ERR_FILE 0x22

//error codes relative to messages integrity are 0x3*
#define TOO_SHORT 0x31  //if the received message characters count is below a certain threshold
#define BAD_STRUCT 0x32  //happens when certain expected xml tags aren't found inside the file
