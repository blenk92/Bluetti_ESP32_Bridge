#ifndef BWIFI_H
#define BWIFI_H
#include "Arduino.h"
#include "config.h"

typedef struct{
  int  salt = EEPROM_SALT;
  char mqtt_server[40] = "127.0.0.1";
  char mqtt_port[6]  = "1883";
  char bluetti_device_id[40] = "Bluetti Blutetooth Id";
} ESPBluettiSettings;

extern ESPBluettiSettings get_esp32_bluetti_settings();
extern void initBWifi();

#endif
