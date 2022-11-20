
#include "DataStore.h"
#include "MQTT.h"


DataStore::DataStore() = default;

void DataStore::setCallback(void(*cb)(enum field_names, String)) {
    callback = cb;
}

void DataStore::updateValue(enum field_names field_name, const String& value) {
    Serial.println(map_field_name(field_name) + ": " + value );
    std::lock_guard<std::mutex> guard(mutex); 
    values[field_name] = value;
    (*callback)(field_name, value);
}

String DataStore::getValue(enum field_names field_name) {
    std::lock_guard<std::mutex> guard(mutex);
    if (values.count(field_name)) {
        return values[field_name];
    }
    return "";
}

DataStore ds;
