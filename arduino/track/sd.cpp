#include <SD.h>

#include "sd.h"



void SdLog::log(const char *data, bool endl) {
	File file = SD.open(filename, FILE_WRITE);
	if (!file)
		return;

	file.print(data);
	if (endl)
		file.println();
		
	file.close();
}


void SdLog::log_time(unsigned long t, bool endl) {
	static char time_buf[16];

	if (t == 0)
		t = millis();

	unsigned long seconds = t / 1000;
	sprintf(time_buf, "%02ld:%02ld:%02ld", seconds / 3600UL, seconds / 60UL % 3600UL, seconds % 60UL);
	log(time_buf, false);
}
