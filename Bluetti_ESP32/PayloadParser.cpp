#include "BluettiConfig.h"
#include "MQTT.h"
#include "PayloadParser.h"
#include "DataStore.h"
#include "BWifi.h"
#include "web.h"
#include <vector>

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

std::vector<String> parse_decimal_array_field(uint8_t data[], size_t size, uint8_t scale) {
    Serial.println("Got array field. Size: " + String(size));
    std::vector<String> v;
    v.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        String s = String(parse_decimal_field(data + i*2, scale), 2);
        v.push_back(s);
        Serial.println(String(i) + ": " + s);
    }
    return v;
}

String pase_enum_field(uint8_t data[]){
    return "";
}

int process_data_field(uint8_t page, uint8_t offset, uint8_t* pData, size_t length, const device_field_data_t field, std::vector<String>* ret) {
    // filter fields not in range
    Serial.println(String("process_data_field(") + page + ", " + (int)offset + ", " + (int)pData + "," + length + ", {" 
        + map_field_name(field.f_name) + ", " +
        + field.f_page + ", " +
        + (int)field.f_offset + ", " +
        + field.f_size + "}"
    );
    
    int data_start = (2* ((int)field.f_offset - (int)offset)) + HEADER_SIZE;
    Serial.println(String("data_start: ") + data_start); 
    int data_end = (data_start + 2 * field.f_size) -1;
    Serial.println(String("data_end: ") + data_end); 
    
    Serial.println(String("cond: ") + (data_end)  + " < " + (length-2) );

    if(field.f_page == page &&
       data_start >= 0 &&
       data_start <= data_end &&
       data_end <= length -2
      ){
        Serial.println("PASSED");

        uint8_t data_payload_field[data_end - data_start +1];

        int p_index = 0;
        for (int i=data_start; i<= data_end; i++){
              data_payload_field[p_index] = pData[i-1];
              p_index++;
        }

        switch (field.f_type){

          case UINT_FIELD:
            ret->push_back(String(parse_uint_field(data_payload_field)));
            break;
          case BOOL_FIELD:
            ret->push_back(String((int)parse_bool_field(data_payload_field)));
            break;
          case DECIMAL_FIELD:
            ret->push_back(String(parse_decimal_field(data_payload_field, field.f_scale ), 2));
            break;
          case SN_FIELD:
            char sn[16];
            snprintf(sn, 15, "%lld", parse_serial_field(data_payload_field));
            sn[15] = '\0';
            ret->push_back(String(sn));
            break;
          case VERSION_FIELD:
            ret->push_back(String(parse_version_field(data_payload_field),2));
            break;
          case STRING_FIELD:
            ret->push_back(parse_string_field(data_payload_field));
            break;
          case ENUM_FIELD:
            ret->push_back(pase_enum_field(data_payload_field));
            break;
          case DECIMAL_ARRAY_FIELD:
            *ret = parse_decimal_array_field(data_payload_field, field.f_size, field.f_scale);
            break;
          default:
            Serial.println("Invalid field type: " + String(field.f_type));
            return 2;

        }
        return 0;
    }
    Serial.println("Request for " + map_field_name(field.f_name) + " seems to be out of range");
    return 1;
}

int process_data_field(uint8_t page, uint8_t offset, uint8_t* pData, size_t length, const device_field_data_t field, String* ret) {
    std::vector<String> vec;
    int r = process_data_field(page, offset, pData, length, field, &vec);
    if (!vec.empty()) {
        *ret = vec[0];
    }
    return r;
}


void parse_data_fields(uint8_t page, uint8_t offset, uint8_t* pData, size_t length, device_field_data_t* fields_to_parse, size_t array_length) {
    for(int i=0; i< array_length; i++){
        std::vector<String> vec;
        int ret = process_data_field(page, offset, pData, length, fields_to_parse[i], &vec);
        if (ret == 0) {
            ds.updateValue(fields_to_parse[i].f_name, vec);
        }
    }
}

void pase_bluetooth_data(uint8_t page, uint8_t offset, uint8_t* pData, size_t length){
    static String device_value;
    Serial.println(String("pase_bluetooth_data(") + page + ", " + offset + ", pdata, " + length + ")");
    switch(pData[1]){
      // range request

      case 0x03:
        ESPBluettiSettings settings = get_esp32_bluetti_settings();
        int d_ret = process_data_field(page, offset, pData, length, device_entry, &device_value);

        if (d_ret != 1 && device_value == "") {
            Serial.println("Got invalid data. Rebooting...");
            Serial.println("device: " + device_value);
            Serial.println("Got: " + String(settings.bluetti_device_id));
            ESP.restart();
        } else if (d_ret == 0) {
            ds.updateValue(device_entry.f_name, device_value);
        } 
        parse_data_fields(page, offset, pData, length, bluetti_device_state_common, sizeof(bluetti_device_state_common)/sizeof(device_field_data_t));

        if (device_value == "AC200M") {
            setState(PACK_NUM, 1);
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
