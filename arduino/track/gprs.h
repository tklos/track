/* tklos
 * Based on https://github.com/carrascoacd/ArduinoSIM800L
*/

#ifndef __gprs_h__
#define __gprs_h__

#include <Arduino.h>


#define BUFLEN 64
#define DELAY_READ 2  // Time to send 1 byte is 10/9600s ~= 1.04ms

#define TIMEOUT_CMD_DEFAULT 5000
#define TIMEOUT_CMD_HTTP_SET_PARA_CONTENT TIMEOUT_CMD_DEFAULT
#define TIMEOUT_CMD_HTTP_SET_ACTION 15000  // Max time to wait for response from server

#define STATUS_GPRS_NONE 0
#define STATUS_GPRS_BEARER 1  // bearer set

#define STATE_READ_INITIAL 0
#define STATE_READ_CMD 1
#define STATE_READ_BEFORE_RESPONSE 2
#define STATE_READ_RESPONSE 3

#define CMD_CHECK_NETWORK_REGISTRATION "AT+CREG?\r\n"
#define CMD_BEARER_SET_CONTYPE "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"
#define CMD_BEARER_SET_APN "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n"
#define CMD_BEARER_OPEN "AT+SAPBR=1,1\r\n"
#define CMD_BEARER_CLOSE "AT+SAPBR=0,1\r\n"
#define CMD_BEARER_CHECK "AT+SAPBR=2,1\r\n"
#define CMD_HTTP_INIT "AT+HTTPINIT\r\n"
#define CMD_HTTP_SET_PARA_CID "AT+HTTPPARA=\"CID\",1\r\n"
#define CMD_HTTP_SET_PARA_URL "AT+HTTPPARA=\"URL\",\"%s\"\r\n"
#define CMD_HTTP_SET_PARA_CONTENT "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n"
#define CMD_HTTP_SET_PARA_API_KEY "AT+HTTPPARA=\"USERDATA\",\"API-Key: %s\"\r\n"
#define CMD_HTTP_SET_DATA "AT+HTTPDATA=%d,%d\r\n"
#define CMD_HTTP_SET_ACTION "AT+HTTPACTION=1\r\n"
#define CMD_HTTP_READ "AT+HTTPREAD\r\n"
#define CMD_HTTP_TERM "AT+HTTPTERM\r\n"



class Gprs {
	public:

	Gprs(HardwareSerial *serial, unsigned int baud, const char *apn, bool debug=false) : serial(serial), apn(strdup(apn)), debug(debug) {
		status = STATUS_GPRS_NONE;
		serial->begin(baud);
	}


	void send_cmd(const char *cmd);
	bool send_cmd_return(const char *cmd, char *buf, unsigned int timeout=TIMEOUT_CMD_DEFAULT, bool cmd_resent=true);
	bool send_cmd_check_ok(const char *cmd, unsigned int timeout=TIMEOUT_CMD_DEFAULT, bool cmd_resent=true);

	bool is_registered_on_network();

	bool send_post(const char *post_url, const char *api_key, const char *data);


	private:

	bool debug;

	HardwareSerial *serial;
	int status;
	const char *apn;

	bool read_ret_data(unsigned int cmdlen, char *buf, unsigned int timeout, bool cmd_resent);

	void send_cleanup_cmds();

	bool set_bearer();
	bool is_bearer_open();

	bool extract_http_response(char *buf, int *status_code, int *datalen);
};


#endif
