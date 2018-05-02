
// Simple nmea parser, based on initial work by Rebane.
// Supports only RMC - coordinates, speed and time.

#include <math.h>
#include <inttypes.h>

#include "tiny_nmea.h"

static uint8_t nmea_parse_number(char *str, uint8_t len, int32_t *value);
static int32_t nmea_power(int32_t value, uint8_t from, uint8_t to);

bool nmea_checksum_ok(char* nmea_line, uint8_t nmea_length) {
	if(nmea_line[nmea_length - 3] == '*') {
		uint8_t i;
		uint8_t chk_calc = 0;
		uint8_t chk_orig = (tobin(nmea_line[nmea_length - 2]) << 4) | tobin(nmea_line[nmea_length - 1]);
		nmea_length -= 3;
		for(i = 1; i < nmea_length; i++) {
			chk_calc ^= nmea_line[i];
		}
		return chk_calc == chk_orig;
	}
	return FALSE;
}

int nmea_parse(nmea_coordinates_t* coords, struct tm* stm, int32_t* speed, char* nmea_line, uint8_t nmea_length) {
	uint8_t i;

	uint8_t field_start[32], field_size[32], field_count;

	if(nmea_length < 7) return TINY_NMEA_ERROR_LENGTH; // too short

	if(nmea_line[0] != '$') return TINY_NMEA_ERROR_MALFORMED;

	if(nmea_checksum_ok(nmea_line, nmea_length) == FALSE) {
		return TINY_NMEA_ERROR_CHECKSUM;
	}
	nmea_length -= 3; // forget about the checksum

	if(nmea_line[6] != ',') return TINY_NMEA_ERROR_SHORT;

	for(field_count = 0, i = 6; i < nmea_length; i++) {
		if(nmea_line[i] == ',') {
			if(field_count > 30) break;
			field_start[field_count] = i + 1;
			field_size[field_count] = 0;
			field_count++;
		} else {
			if(field_count)field_size[field_count - 1]++;
		}
	}

	if(!memcmp(&nmea_line[3], "RMC", 3)) { // recommended minimum
		if(field_count < 9) return TINY_NMEA_ERROR_FIELDS;

		if(!field_size[1] || ((nmea_line[field_start[1]] != 'A') && (nmea_line[field_start[1]] != 'a'))) return TINY_NMEA_ERROR_DATA;

		if(coords != NULL) {
			int32_t nmea_lat, nmea_lon;

			i = nmea_parse_number((char *)&nmea_line[field_start[2]], field_size[2], &nmea_lat);
			if((nmea_line[field_start[3]] == 'S') || (nmea_line[field_start[3]] == 's')) nmea_lat = -nmea_lat;
			nmea_lat = nmea_power(nmea_lat, i, 4);

			i = nmea_parse_number((char *)&nmea_line[field_start[4]], field_size[4], &nmea_lon);
			if((nmea_line[field_start[5]] == 'W') || (nmea_line[field_start[5]] == 'w'))nmea_lon = -nmea_lon;
			nmea_lon = nmea_power(nmea_lon, i, 4);

			coords->latitude = nmea_lat;
			coords->longitude = nmea_lon;
			if((field_count >= 12)&&(field_size[11] == 1)) {
				coords->mode = nmea_line[field_start[11]];
				if((coords->mode >= 97)&&(coords->mode <= 122)) coords->mode -= 32; // Convert to uppercase
			}
			else {
				coords->mode = '?';
			}
		}

		if(speed != NULL) {
			// 1 Knot = 1.852 Kilometers per Hour
			i = nmea_parse_number((char *)&nmea_line[field_start[6]], field_size[6], speed);
			*speed = nmea_power(*speed, i, 2);
		}

		if(stm != NULL) {
			if((field_size[0] > 5) && (field_size[8] > 5)) {
				stm->tm_hour = (((uint16_t)nmea_line[field_start[0] + 0] - '0') * 10) + (nmea_line[field_start[0] + 1] - '0');
				stm->tm_min  = (((uint16_t)nmea_line[field_start[0] + 2] - '0') * 10) + (nmea_line[field_start[0] + 3] - '0');
				stm->tm_sec  = (((uint16_t)nmea_line[field_start[0] + 4] - '0') * 10) + (nmea_line[field_start[0] + 5] - '0');
				stm->tm_mday = (((uint16_t)nmea_line[field_start[8] + 0] - '0') * 10) + (nmea_line[field_start[8] + 1] - '0');
				stm->tm_mon  = (((uint16_t)nmea_line[field_start[8] + 2] - '0') * 10) + (nmea_line[field_start[8] + 3] - '0');
				stm->tm_year = (((uint16_t)nmea_line[field_start[8] + 4] - '0') * 10) + (nmea_line[field_start[8] + 5] - '0');
				if(stm->tm_mon) stm->tm_mon--;
				stm->tm_year += 100;
			}
		}
		return 0;
	}
	return TINY_NMEA_NOT_SUPPORTED;
}

static uint8_t nmea_parse_number(char *str, uint8_t len, int32_t *value) {
	uint8_t pw, s, i;
	*value = 0;
	pw = s = 0;
	for(i = 0; i < len; i++) {
		if(isdigit(str[i])) {
			*value = (*value * 10) + (str[i] - '0');
			if(s)pw++;
		}else if(str[i] == '.') {
			s = 1;
		}
	}
	return(pw);
}

static int32_t nmea_power(int32_t value, uint8_t from, uint8_t to) {
	uint8_t i;
	if(from < to) {
		for(i = 0; i < (to - from); i++) {
			value *= 10;
		}
	} else {
		for(i = 0; i < (from - to); i++) {
			value /= 10;
		}
	}
	return(value);
}
