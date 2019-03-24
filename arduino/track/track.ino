#include <Metro.h>
#include <TinyGPS++.h>

#include "settings.h"
#include "gprs.h"

#define DELAY_INITIAL 2000


Gprs gprs = Gprs(serial_gprs, 9600, apn, true);

Metro metro_gps = Metro(INTERVAL_GPS);
Metro metro_gprs = Metro(30000);



void setup() {
	Serial.begin(9600);

	serial_gps->begin(9600);

	delay(DELAY_INITIAL);
}


void metro_loop_gps() {
	unsigned long start_time = millis();

	TinyGPSPlus gps;

	while (1) {
		if (gps.date.isValid() && gps.time.isValid() && gps.location.isValid())
			break;

		if (millis() - start_time > TIMEOUT_GPS_GET_DATA)
			return;

		if (!serial_gps->available())
			continue;

		char c = serial_gps->read();
		Serial.print(c);
		gps.encode(c);
	}

	double lat = gps.location.lat();
	double lng = gps.location.lng();

	char data[90];
	sprintf(data, "{\"lat\": %d.%06ld, \"long\": %d.%06ld, \"date\": \"%04d:%02d:%02dT%02d:%02d:%02dZ\"}",
			int(lat), (long)((lat - (int)lat) * 1000000),
			int(lng), (long)((lng - (int)lng) * 1000000),
			gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());

	Serial.println(data);

	bool ret = gprs.send_post(post_url, api_key, data);
	Serial.print("Success?: "); Serial.println(ret);
}


void loop() {
	if (metro_gps.check())
		metro_loop_gps();

//	if (metro_gprs.check()) {
//		int begin = millis();
//		bool ret = gprs.send_post(post_url, api_key, "{\"lat\": 1.5554, \"long\": 6.55, \"date\": \"2212\"}");
//		Serial.print("Success?: "); Serial.println(ret);
//		int length = millis() - begin;
//
//		Serial.println(length);
//	}
}