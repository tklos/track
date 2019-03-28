#ifndef __settings_h__
#define __settings_h__

#include <Arduino.h>


extern HardwareSerial *serial_gprs;
extern HardwareSerial *serial_gps;

extern const unsigned long interval_gps;

extern const unsigned long timeout_gps_get_data;

extern const char *apn;

extern const char *post_url;
extern const char *api_key;

extern const int sd_chip_select;
extern const char *sd_measurements_filename;
extern const char *sd_log_filename;


#endif