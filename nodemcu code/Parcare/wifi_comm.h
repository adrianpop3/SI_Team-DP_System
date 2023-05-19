#ifndef __WIFI_COMM_INCLUDED
#define __WIFI_COMM_INCLUDED

#include "wifi_credentials.h"
#include "matrix_led_ir.h"
/*this should contain

#define SSID "ssid"
#define PASSWORD "***"
#define MQTT_SERVER "ip.address"
#define TOPIC "topic"

*/

extern char access_license[10], verdict[5];
extern char response[100];

void wifi_init();

bool wifi_update();

void send_license(char *s);

void send_over_mqtt(char *s);


#endif
