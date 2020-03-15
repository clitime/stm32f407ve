// файл создан автоматически semver.py
// не изменяйте данные, потому что они могут быть утеряны

#include "version.h"

const char device_version[] = "v{{SEMVER}} ({{HASH_DATA}})";
const char device_type[] = "notype";
const char device_name[] = "stm32f407ve";

const char* get_device_name(void) {
    return device_name;
}

const char* get_device_version(void) {
    return device_version;
}

const char* get_device_type(void) {
    return device_type;
}
