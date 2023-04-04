#pragma once 
#include "DeviceType.h"

extern const device_field_data device_entry;
extern const device_field_data serial_entry;

// { FIELD_NAME, PAGE, OFFSET, SIZE, SCALE (if scale is needed e.g. decimal value, defaults to 0) , ENUM (if data is enum, defaults to 0) , FIELD_TYPE }
static device_field_data_t bluetti_device_state_common[] = {
  /*Page 0x00 Core */
  {ARM_VERSION,       0x00, 0x17, 2, 0, 0, VERSION_FIELD},
  {DSP_VERSION,       0x00, 0x19, 2, 0, 0, VERSION_FIELD},
  {DC_INPUT_POWER,    0x00, 0x24, 1, 0, 0, UINT_FIELD},
  {AC_INPUT_POWER,    0x00, 0x25, 1, 0, 0, UINT_FIELD},
  {AC_OUTPUT_POWER,   0x00, 0x26, 1, 0, 0, UINT_FIELD},
  {DC_OUTPUT_POWER,   0x00, 0x27, 1, 0, 0, UINT_FIELD},
  {POWER_GENERATION,  0x00, 0x29, 1, 1, 0, DECIMAL_FIELD},
  {TOTAL_BATTERY_PERCENT, 0x00, 0x2B, 1,0,0, UINT_FIELD},
  {AC_OUTPUT_ON,      0x00, 0x30, 1, 0, 0, BOOL_FIELD},
  {DC_OUTPUT_ON,      0x00, 0x31, 1, 0, 0, BOOL_FIELD},
  {PACK_NUM_MAX,      0x00, 0x5B, 1, 0, 0, UINT_FIELD},
  {CELL_VOLTAGES,     0x00, 0x69, 16, 2, 0, DECIMAL_ARRAY_FIELD},
  {SERIAL_NUMBER,     0x00, 0x11, 4, 0 ,0, SN_FIELD},
};

static device_field_data_t bluetti_device_command[] = {
  /*Page 0x00 Core */
  {DC_OUTPUT_ON,      0x0B, 0xC0, 1, 0, 0, BOOL_FIELD},
  {AC_OUTPUT_ON,      0x0B, 0xBF, 1, 0, 0, BOOL_FIELD},
  {PACK_NUM,          0x0B, 0xBE, 1, 0, 0, UINT_FIELD},
};

static device_field_data_t bluetti_polling_command[] = {
  {FIELD_UNDEFINED, 0x00, 0x0A, 0x28 ,0 , 0, TYPE_UNDEFINED},
  {FIELD_UNDEFINED, 0x00, 0x46, 0x15 ,0 , 0, TYPE_UNDEFINED},
  {FIELD_UNDEFINED, 0x0B, 0xB9, 0x3D ,0 , 0, TYPE_UNDEFINED},
  {FIELD_UNDEFINED, 0x00, 0x5B, 0x25 ,0 , 0, TYPE_UNDEFINED},
};

