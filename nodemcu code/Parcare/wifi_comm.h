#ifndef __WIFI_COMM_INCLUDED
#define __WIFI_COMM_INCLUDED

#include "wifi_credentials.h"
/*this should contain

#define SSID "ssid"
#define PASSWORD "***"
#define MQTT_SERVER "ip.address"
#define TOPIC "topic"

*/

void wifi_init();

bool wifi_update();

void send_over_mqtt(char *s);


#endif
