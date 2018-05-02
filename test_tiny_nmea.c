#include "tiny_nmea.h"
#include "test_tiny_nmea.h"

static bool assertEqualInt(int v1, int v2) {
	if(v1 != v2) {
		printf("ERROR %d %d\n", v1, v2);
	}
	return v1 == v2;
}

static bool assertEqualInt32(int32_t v1, int32_t v2) {
	if(v1 != v2) {
		printf("ERROR %"PRIi32"!=%"PRIi32"\n", v1, v2);
	}
	return v1 == v2;
}

bool test_tiny_nmea() {
	bool result = TRUE;
	nmea_coordinates_t coords;
	char t1[] = "$GNRMC,083531.000,A,5918.3887,N,02444.8770,E,0.00,322.20,010518,,,A*7F";
	int err = nmea_parse(&coords, NULL, NULL, t1, strlen(t1));

	result &= assertEqualInt(err, 0);
	result &= assertEqualInt32(coords.latitude, 59183887L);
	result &= assertEqualInt32(coords.longitude, 24448770L);

	return result;
}
