#pragma once

#include <vector>
#include <map>
#include <mutex>

#include "DeviceType.h"

class DataStore {
public:
    DataStore();

    void setCallback(void(*cb)(enum field_names, String));
    void updateValue(enum field_names field_name, const String& value);
    String getValue(enum field_names field_name);

private:
    void(*callback)(enum field_names, String) = nullptr;
    std::map<enum field_names, String> values;
    std::mutex mutex;
};

extern DataStore ds;
