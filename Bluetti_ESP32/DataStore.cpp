
#include "DataStore.h"
#include "MQTT.h"


DataStore::DataStore() = default;

void DataStore::setCallback(void(*cb)(enum field_names, std::vector<String>)) {
    callback = cb;
}

void DataStore::updateValue(enum field_names field_name, const std::vector<String>& value) {
    std::lock_guard<std::mutex> guard(mutex); 
    for (size_t i = 0; i < value.size(); ++i) {
        Serial.println(map_field_name(field_name) + "_" + i + ": " + value[i]);
    }
    values[field_name] = value;
    if (callback) {
        (*callback)(field_name, value);
    }
}

void DataStore::updateValue(enum field_names field_name, const String& value) {
    std::vector<String> vec;
    vec.push_back(value);
    return updateValue(field_name, vec);
}

std::vector<String> DataStore::getValue(enum field_names field_name) {
    std::lock_guard<std::mutex> guard(mutex);
    if (values.count(field_name)) {
        return values[field_name];
    }
    return {};
}

String DataStore::getValueAsJson(enum field_names field_name) {
    std::vector<String> value = getValue(field_name);
    String s;
    if (value.size() == 1) {
      s = value[0];
    } else if (value.size() != 0) {
      s = "[" + value[0];
      for (size_t i = 1; i < value.size(); ++i) {
            s += ", " + value[i];
      }
      s += "]";
    }
    return s;
}

DataStore ds;
