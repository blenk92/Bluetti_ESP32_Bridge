#ifndef MQTT_H
#define MQTT_H
#include "Arduino.h"
#include "DeviceType.h"

extern String map_field_name(enum field_names f_name);
extern void publishTopic(enum field_names field_name, String value);
extern void publishDeviceState();
extern void handleMQTT();
extern void initMQTT();
extern bool isMQTTconnected();
extern int getPublishErrorCount();
unsigned long getLastMQTTMessageTime();
unsigned long getLastMQTDeviceStateMessageTime();

#endif
