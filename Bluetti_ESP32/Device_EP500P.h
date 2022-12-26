#ifndef DEVICE_EP500P_H
#define DEVICE_EP500P_H
#include "Arduino.h"

/* Not implemented yet
enum output_mode {
    STOP = 0,
    INVERTER_OUTPUT = 1,
    BYPASS_OUTPUT_C = 2,
    BYPASS_OUTPUT_D = 3,
    LOAD_MATCHING = 4
};

enum ups_mode {
    CUSTOMIZED = 1,
    PV_PRIORITY = 2,
    STANDARD = 3,
    TIME_CONTROl = 4  
};

enum auto_sleep_mode {
  THIRTY_SECONDS = 2,
  ONE_MINNUTE = 3,
  FIVE_MINUTES = 4,
  NEVER = 5  
};
*/

// { FIELD_NAME, PAGE, OFFSET, SIZE, SCALE (if scale is needed e.g. decimal value, defaults to 0) , ENUM (if data is enum, defaults to 0) , FIELD_TYPE }
static device_field_data_t bluetti_device_state_ep500p[] = {
 
};

#endif
