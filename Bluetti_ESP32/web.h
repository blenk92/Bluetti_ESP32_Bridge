#pragma once

#include <WebServer.h>
#include <DeviceType.h>
void initWeb(WebServer& httpServer);
void setState(enum field_names field_name, int enabled);
