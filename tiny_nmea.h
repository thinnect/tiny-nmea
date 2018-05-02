#ifndef TINY_NMEA_H
#define TINY_NMEA_H

#include <stdint.h>

#ifndef NESC
#include <stdbool.h>
#endif // NESC

enum TinyNmeaErrors {
	TINY_NMEA_OK              = 0,
	TINY_NMEA_ERROR_CHECKSUM  = 1,
	TINY_NMEA_ERROR_MALFORMED = 2,
	TINY_NMEA_ERROR_LENGTH    = 3,
	TINY_NMEA_ERROR_SHORT     = 4,
	TINY_NMEA_ERROR_FIELDS    = 5,
	TINY_NMEA_ERROR_DATA      = 6,
	TINY_NMEA_ERROR_NO_FIX    = 7,
	TINY_NMEA_NOT_SUPPORTED   = 8,
};

typedef struct nmea_coordinates {
	int32_t latitude;
	int32_t longitude;
	char mode;
} nmea_coordinates_t;

#define tobin(c) ((((c) >= '0') && ((c) <= '9')) ? ((c) - '0') : ((((c) >= 'A') && ((c) <= 'F')) ? ((c) - 'A' + 10) : ((((c) >= 'a') && ((c) <= 'f')) ? ((c) - 'a' + 10) : (0))))

bool nmea_checksum_ok(char* nmea_line, uint8_t nmea_length);
int nmea_parse(nmea_coordinates_t* coords, struct tm* stm, int32_t* speed, char* nmea_line, uint8_t nmea_length);

#endif // TINY_NMEA_H
