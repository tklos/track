#include <Metro.h>

#include "settings.h"
#include "gprs.h"

#define DELAY_INITIAL 2000


Gprs gprs = Gprs(gprs_serial, 9600, apn, true);

Metro metro_gprs = Metro(20000);



void setup() {
	Serial.begin(9600);

	delay(DELAY_INITIAL);
}



void loop() {
	if (metro_gprs.check()) {
		int begin = millis();
		bool ret = gprs.send_post(post_url, api_key, "{\"lat\": 1.5554, \"long\": 6.55, \"date\": \"2212\"}");
		Serial.print("Success?: "); Serial.println(ret);
		int length = millis() - begin;

		Serial.println(length);
	}
}
