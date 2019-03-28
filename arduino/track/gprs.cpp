#include "gprs.h"


static char buf[128];



void Gprs::send_cmd(const char *cmd) {
	if (debug) {
		Serial.println(); Serial.print(" * Sending "); Serial.println(cmd); //Serial.flush();
	}

	while (serial->available())
		serial->read();

	serial->write(cmd);
	serial->flush();
}


bool is_whitespace(char c) {
	return c == 0 || c == 10 || c == 13 || c == ' ';
}


bool Gprs::read_ret_data(unsigned int cmdlen, char *buf, unsigned int timeout, bool cmd_resent) {
	unsigned long start_time = millis();

	cmdlen = min(cmdlen, 60);

	int state = STATE_READ_INITIAL;
	int total_read = 0, buflen = 0;
	while (1) {
		if (state == STATE_READ_RESPONSE && buflen > 0) {
			buf[buflen] = '\0';
			buflen++;

			if (debug) {
				Serial.print("Received "); Serial.println(buf);
			}

			return true;
		}

		if (millis() - start_time > timeout)
			return false;

		char c;
		bool read_new = true;
		while (!read_new || serial->available()) {
			if (read_new) {
				c = serial->read();
//				Serial.print(state); Serial.print(": "); Serial.print((int)c); Serial.print(" -> "); Serial.println(c);
			}

			read_new = true;
			switch (state) {
				case STATE_READ_INITIAL:
					if (is_whitespace(c))
						break;

					state = cmd_resent ? STATE_READ_CMD : STATE_READ_RESPONSE;
					read_new = false;
					break;

				case STATE_READ_CMD:
					total_read++;
					if (total_read <= cmdlen || !is_whitespace(c))
						break;

					state = STATE_READ_BEFORE_RESPONSE;
					read_new = false;
					break;

				case STATE_READ_BEFORE_RESPONSE:
					if (is_whitespace(c))
						break;

					state = STATE_READ_RESPONSE;
					read_new = false;
					break;

				case STATE_READ_RESPONSE:
					buf[buflen] = c;
					buflen++;
					break;
			}

			if (!serial->available())
				delay(DELAY_READ);
		}
	}
}


bool Gprs::send_cmd_return(const char *cmd, char *buf, unsigned int timeout, bool cmd_resent) {
	/* Returns:
	 *  - true: and fills buf or
	 *  - false
	*/

	send_cmd(cmd);

	return read_ret_data(strlen(cmd), buf, timeout, cmd_resent);
}


bool Gprs::send_cmd_check_ok(const char *cmd, unsigned int timeout, bool cmd_resent) {
	if (!send_cmd_return(cmd, buf, timeout, cmd_resent))
		return false;

	return !strncmp(buf, "OK", 2);
}


bool Gprs::is_registered_on_network() {
	if (!send_cmd_return(CMD_CHECK_NETWORK_REGISTRATION, buf, 5000))
		return false;

	return !strncmp(buf, "+CREG: 0,1", 10) || !strncmp(buf, "+CREG: 0,5", 10);
}


bool Gprs::set_bearer() {
//	/* AT+CREG? */
//	if (!is_registered_on_network())
//		return false;

	/* AT+SAPBR=3,1,"Contype","GPRS" */
	if (!send_cmd_check_ok(CMD_BEARER_SET_CONTYPE))
		return false;

	/* AT+SAPBR=3,1,"APN","(apn)" */
	sprintf(buf, CMD_BEARER_SET_APN, apn);
	if (!send_cmd_check_ok(buf))
		return false;

	status = STATUS_GPRS_BEARER;
	return true;
}


bool Gprs::is_bearer_open() {
	if (!send_cmd_return(CMD_BEARER_CHECK, buf))
		return false;

	return !strncmp(buf, "OK", 2);
}


void Gprs::send_cleanup_cmds() {
	send_cmd_return(CMD_HTTP_TERM, buf);
	send_cmd_check_ok(CMD_BEARER_CLOSE);
}


bool Gprs::extract_http_response(char *buf, int *status_code, int *datalen) {
	/* Format: x,xxx,x.. */
	if (strlen(buf) < 7 || buf[1] != ',' || buf[5] != ',')
		return false;

	buf[5] = '\0';

	*status_code = atoi(buf+2);
	*datalen = atoi(buf+6);
	return true;
}


bool Gprs::send_post(const char *post_url, const char *api_key, const char *data) {
	bool ret;

	if (!is_registered_on_network())
		return false;

	if (status != STATUS_GPRS_BEARER && !set_bearer())
		return false;

	/* AT+SAPBR=1,1 */
	if (!send_cmd_check_ok(CMD_BEARER_OPEN)) {
		/* Maybe bearer is already open? */
		if (!send_cmd_return(CMD_BEARER_CHECK, buf))
			return false;
		if (strncmp(buf, "+SAPBR: 1,1,", 12)) {
			status = 0;
			return false;
		}
	}

	/* AT+HTTPINIT */
	if (!send_cmd_check_ok(CMD_HTTP_INIT)) {
		send_cmd_return(CMD_HTTP_TERM, buf);
		return false;
	}

	/* AT+HTTPPARA="CID",1 */
	if (!send_cmd_check_ok(CMD_HTTP_SET_PARA_CID)) {
		send_cleanup_cmds();
		return false;
	}

	/* AT+HTTPPARA="URL","(url)" */
	sprintf(buf, CMD_HTTP_SET_PARA_URL, post_url);
	if (!send_cmd_check_ok(buf)) {
		send_cleanup_cmds();
		return false;
	}

	/* AT+HTTPPARA="CONTENT","application/json" */
	if (!send_cmd_check_ok(CMD_HTTP_SET_PARA_CONTENT)) {
		send_cleanup_cmds();
		return false;
	}

	/* AT+HTTPPARA="USERDATA","API-Key: (api_key)" */
	sprintf(buf, CMD_HTTP_SET_PARA_API_KEY, api_key);
	if (!send_cmd_check_ok(buf)) {
		send_cleanup_cmds();
		return false;
	}

	/* AT+HTTPDATA=(bodylen),(timeout) */
	sprintf(buf, CMD_HTTP_SET_DATA, strlen(data), TIMEOUT_CMD_HTTP_SET_PARA_CONTENT);
	ret = send_cmd_return(buf, buf, TIMEOUT_CMD_HTTP_SET_PARA_CONTENT);
	if (!ret) {
		send_cleanup_cmds();
		return false;
	}
	if (strncmp(buf, "DOWNLOAD", 8)) {
		send_cleanup_cmds();
		return false;
	}

	if (!send_cmd_check_ok(data, TIMEOUT_CMD_HTTP_SET_PARA_CONTENT)) {
		send_cleanup_cmds();
		return false;
	}

	/* AT+HTTPACTION=1 */
	if (!send_cmd_check_ok(CMD_HTTP_SET_ACTION, buf, TIMEOUT_CMD_HTTP_SET_ACTION)) {
		send_cleanup_cmds();
		return false;
	}
	const char *http_action_response_prefix = "+HTTPACTION:";
	if (strstr(buf, http_action_response_prefix) == NULL)
		if (!read_ret_data(strlen(http_action_response_prefix), buf, TIMEOUT_CMD_HTTP_SET_ACTION, true)) {
			send_cleanup_cmds();
			return false;
		}

	int status_code, datalen;
	if (!extract_http_response(buf, &status_code, &datalen)) {
		send_cleanup_cmds();
		return false;
	}

	if (status_code != 201) {
		send_cleanup_cmds();
		return false;
	}

	/* AT+HTTPREAD */
	/* Note: We are not reading the data sent by server;
	   It is enough to check the html status code */

	/* AT+HTTPTERM */
	if (!send_cmd_check_ok(CMD_HTTP_TERM)) {
		send_cmd_check_ok(CMD_BEARER_CLOSE);
		return false;
	}

	/* AT+SAPBR=0,1 */
	if (!send_cmd_check_ok(CMD_BEARER_CLOSE))
		return false;

	return true;
}
