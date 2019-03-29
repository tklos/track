/*
 * https://github.com/tklos/track
*/

#include <Metro.h>
#include <TinyGPS++.h>
#include <SD.h>

#include "settings.h"
#include "gprs.h"
#include "sd.h"

#define DELAY_INITIAL 2000


Gprs gprs = Gprs(serial_gprs, 9600, apn, true);

SdLog sdlog = SdLog(sd_log_filename);

Metro metro_gps = Metro(interval_gps);
Metro metro_gprs = Metro(interval_gprs);



void setup() {
	delay(DELAY_INITIAL);

	Serial.begin(9600);

	serial_gps->begin(9600);

	if (!SD.begin(sd_chip_select)) {
		Serial.println("Can't initialise SD card");
		while (1) ;
	}

	metro_gps.reset();
	delay(1);  // make sure GPRS is run after GPS
	metro_gprs.reset();
}



void metro_loop_gps() {
	unsigned long start_time = millis();

	Serial.println("\nGPS"); sdlog.log("\n"); sdlog.log_time(start_time); sdlog.log(" GPS");

	TinyGPSPlus gps;

	while (1) {
		if (gps.date.isValid() && gps.time.isValid() && gps.location.isValid())
			break;

		if (millis() - start_time > timeout_gps_get_data)
			return;

		if (!serial_gps->available())
			continue;

		char c = serial_gps->read();
//		Serial.print(c);
		gps.encode(c);
	}

	double lat = gps.location.lat();
	double lng = gps.location.lng();

	char data[90];
	sprintf(data, "{\"lat\": %d.%06ld, \"long\": %d.%06ld, \"date\": \"%04d-%02d-%02dT%02d:%02d:%02dZ\"}",
			int(lat), (long)((lat - (int)lat) * 1000000),
			int(lng), (long)((lng - (int)lng) * 1000000),
			gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());

	Serial.println(data); sdlog.log(data);


	/* Save to SD card */
	sd_write_to_file(sd_measurements_filename, data);
	sd_write_to_file(sd_to_send_filename, data);
}



bool gprs_send_single_record() {
	bool ret;
	unsigned long start_time = millis();

	Serial.println("\nSEND"); sdlog.log("\n"); sdlog.log_time(start_time); sdlog.log(" SEND");


	/* Read data to send */
	char data[90];
	ret = sd_read_first_copy_rest(sd_to_send_filename, data, sd_tmp_filename);
	if (!ret) {
		Serial.println("Nothing to send"); sdlog.log("Nothing to send");
		return false;
	}


	/* Send data */
	Serial.println(data);
	sdlog.log("Sending ", false); sdlog.log(data);

	ret = gprs.send_post(post_url, api_key, data);
	if (!ret) {
		Serial.println("Failed"); sdlog.log("Failed");
		return false;
	}


	/* Copy file with records to send */
	ret = sd_copy_file(sd_tmp_filename, sd_to_send_filename);
	if (!ret) {
		Serial.println("Copy failed"); sdlog.log("Copy failed");
		return false;
	}


	char buf[50];
	int length = millis() - start_time;
	sprintf(buf, "Finished %d", length);
	Serial.println(buf); sdlog.log(buf);

	return true;
}



void metro_loop_gprs() {
	for (int num_sent = 0; num_sent < gprs_max_records_to_send; num_sent++) {
		bool ret = gprs_send_single_record();
		if (!ret)
			break;
	}
}



void loop() {
	if (metro_gps.check())
		metro_loop_gps();

	if (metro_gprs.check())
		metro_loop_gprs();
}
