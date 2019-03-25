/* tklos
*/

#ifndef __sd_h__
#define __sd_h__



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
