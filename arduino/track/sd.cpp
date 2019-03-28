#include <SD.h>

#include "sd.h"


#define BUFSIZE 256

static char buf[BUFSIZE];



void sd_write_to_file(const char *filename, const char *data) {
	File file = SD.open(filename, O_WRITE | O_CREAT | O_APPEND);
	if (!file) {
		Serial.println("Can't open file..");
		return;
	}

    file.println(data);
    file.close();
}


bool sd_copy_file(File &in_file, File &out_file) {
	while (1) {
		int n = in_file.read(buf, BUFSIZE);

		if (n == 0)
			break;

		if (n == -1) {
			out_file.close();
			in_file.close();
			return false;
		}

		out_file.write(buf, n);
	}

	return true;
}


bool sd_copy_file(const char *in_filename, const char *out_filename) {
	File in_file = SD.open(in_filename, O_READ);
	File out_file = SD.open(out_filename, O_WRITE | O_CREAT | O_TRUNC);

	if (!in_file || !out_file)
		return false;

	if (!sd_copy_file(in_file, out_file))
		return false;

	out_file.close();
	in_file.close();

	return true;
}


bool sd_read_first_copy_rest(const char *in_filename, char *data, const char *out_filename) {
	File in_file = SD.open(in_filename, O_READ);
	if (!in_file)
		return false;


	/* Read first line */
	int datalen = 0;
	bool correct_line = false;
	while (in_file.available()) {
		char c = in_file.read();

		if (c == 13)
			continue;
		if (c == 10) {
			correct_line = true;
			data[datalen] = '\0';
			datalen++;
			break;
		}

		data[datalen] = c;
		datalen++;
	}

	if (!correct_line) {
		in_file.close();
		return false;
	}


	/* Copy the rest of the file */
	File out_file = SD.open(out_filename, O_WRITE | O_CREAT | O_TRUNC);
	if (!out_file) {
		in_file.close();
		return false;
	}

	if (!sd_copy_file(in_file, out_file))
		return false;


	out_file.close();
	in_file.close();

	return true;
}



void SdLog::log(const char *data, bool endl) {
	File file = SD.open(filename, O_WRITE | O_CREAT | O_APPEND);
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
