/* tklos
*/

#ifndef __sd_h__
#define __sd_h__



void sd_write_to_file(const char *filename, const char *data);

bool sd_copy_file(const char *in_filename, const char *out_filename);

bool sd_read_first_copy_rest(const char *in_filename, char *data, const char *out_filename);



class SdLog {
	public:

	SdLog(const char *filename) : filename(filename) {
	}

	void log(const char *data, bool endl=true);
	void log_time(unsigned long t=0, bool endl=false);


	private:

	const char *filename;
};


#endif
