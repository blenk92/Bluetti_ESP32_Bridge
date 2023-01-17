#pragma once

#include <vector>
#include <map>
#include <mutex>

#include "DeviceType.h"

class DataStore {
public:
    DataStore();

    void setCallback(void(*cb)(enum field_names, std::vector<String>));
    void updateValue(enum field_names field_name, const String& value);
    void updateValue(enum field_names field_name, const std::vector<String>& value);
    std::vector<String> getValue(enum field_names field_name);
    String getValueAsJson(enum field_names field_name);

private:
    void(*callback)(enum field_names, std::vector<String>) = nullptr;
    std::map<enum field_names, std::vector<String>> values;
    std::mutex mutex;
};

extern DataStore ds;
