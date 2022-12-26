#include "BluettiConfig.h"
#include "MQTT.h"
#include "PayloadParser.h"
#include "DataStore.h"
#include "BWifi.h"

uint16_t parse_uint_field(uint8_t data[]){
    return ((uint16_t) data[0] << 8 ) | (uint16_t) data[1];
}

bool parse_bool_field(uint8_t data[]){
    return (data[1]) == 1;
}

float parse_decimal_field(uint8_t data[], uint8_t scale){
    uint16_t raw_value = ((uint16_t) data[0] << 8 ) | (uint16_t) data[1];
    return (raw_value) / pow(10, scale);
}

float parse_version_field(uint8_t data[]){

   uint16_t low = ((uint16_t) data[0] << 8 ) | (uint16_t) data[1];
   uint16_t high = ((uint16_t) data[2] << 8) | (uint16_t) data[3];
   long val = (low ) | (high << 16) ;

   return (float) val/100;
}

uint64_t parse_serial_field(uint8_t data[]){

   uint16_t val1 = ((uint16_t) data[0] << 8 ) | (uint16_t) data[1];
   uint16_t val2 = ((uint16_t) data[2] << 8 ) | (uint16_t) data[3];
   uint16_t val3 = ((uint16_t) data[4] << 8 ) | (uint16_t) data[5];
   uint16_t val4 = ((uint16_t) data[6] << 8 ) | (uint16_t) data[7];

   uint64_t sn =  ((((uint64_t) val1) | ((uint64_t) val2 << 16)) | ((uint64_t) val3 << 32)) | ((uint64_t) val4 << 48);

   return  sn;
}

String parse_string_field(uint8_t data[]){
    return String((char*) data);
}

String pase_enum_field(uint8_t data[]){
    return "";
}

String process_data_field(uint8_t page, uint8_t offset, uint8_t* pData, size_t length, const device_field_data_t field) {
    // filter fields not in range
    if(field.f_page == page &&
       field.f_offset >= offset &&
       field.f_offset <= (offset + length)/2 &&
       field.f_offset + field.f_size-1 >= offset &&
       field.f_offset + field.f_size-1 <= (offset + length)/2
      ){

        uint8_t data_start = (2* ((int)field.f_offset - (int)offset)) + HEADER_SIZE;
        uint8_t data_end = (data_start + 2 * field.f_size);
        uint8_t data_payload_field[data_end - data_start];

        int p_index = 0;
        for (int i=data_start; i<= data_end; i++){
              data_payload_field[p_index] = pData[i-1];
              p_index++;
        }

        switch (field.f_type){

          case UINT_FIELD:
            return String(parse_uint_field(data_payload_field));

          case BOOL_FIELD:
            return String((int)parse_bool_field(data_payload_field));

          case DECIMAL_FIELD:
            return String(parse_decimal_field(data_payload_field, field.f_scale ), 2);

          case SN_FIELD:
            char sn[16];
            snprintf(sn, 15, "%lld", parse_serial_field(data_payload_field));
            sn[15] = '\0';
            return String(sn);

          case VERSION_FIELD:
            return String(parse_version_field(data_payload_field),2);

          case STRING_FIELD:
            return parse_string_field(data_payload_field);

          case ENUM_FIELD:
            return pase_enum_field(data_payload_field);
          default:
            return "";

        }

    }
    return "";
}

void parse_data_fields(uint8_t page, uint8_t offset, uint8_t* pData, size_t length, device_field_data_t* fields_to_parse, size_t array_length) {
    for(int i=0; i< array_length; i++){
        String s = process_data_field(page, offset, pData, length, bluetti_device_state_common[i]);
        if (s != "") {
            ds.updateValue(bluetti_device_state_common[i].f_name, s);
        }
    }
}

void pase_bluetooth_data(uint8_t page, uint8_t offset, uint8_t* pData, size_t length){
    switch(pData[1]){
      // range request

      case 0x03:
        ESPBluettiSettings settings = get_esp32_bluetti_settings();
        String device_value = process_data_field(page, offset, pData, length, device_entry);
        String serial_value = process_data_field(page, offset, pData, length, serial_entry);

        if (String(settings.bluetti_device_id) != (device_value + serial_value)) {
            Serial.println("Got invalid data. Rebooting...");
            ESP.restart();
        } else {
            ds.updateValue(device_entry.f_name, device_value);
            ds.updateValue(serial_entry.f_name, serial_value);
        }
        parse_data_fields(page, offset, pData, length, bluetti_device_state_common, sizeof(bluetti_device_state_common)/sizeof(device_field_data_t));

        if (device_value == "AC200M") {
            parse_data_fields(page, offset, pData, length, bluetti_device_state_ac200m, sizeof(bluetti_device_state_ac200m)/sizeof(device_field_data_t));
        } else if (device_value == "AC300") {
            parse_data_fields(page, offset, pData, length, bluetti_device_state_ac300, sizeof(bluetti_device_state_ac300)/sizeof(device_field_data_t));
        } else if (device_value == "AC500") {
            parse_data_fields(page, offset, pData, length, bluetti_device_state_ac500, sizeof(bluetti_device_state_ac500)/sizeof(device_field_data_t));
        } else if (device_value == "EP500") {
            parse_data_fields(page, offset, pData, length, bluetti_device_state_ep500, sizeof(bluetti_device_state_ep500)/sizeof(device_field_data_t));
        } else if (device_value == "EP500P") {
            parse_data_fields(page, offset, pData, length, bluetti_device_state_ep500p, sizeof(bluetti_device_state_ep500p)/sizeof(device_field_data_t));
        } else if (device_value == "EB3A") {
            parse_data_fields(page, offset, pData, length, bluetti_device_state_eb3a, sizeof(bluetti_device_state_eb3a)/sizeof(device_field_data_t));
        } else {
            Serial.println("Got unkown device string: " + device_value);
        }


      break;

    }

}
