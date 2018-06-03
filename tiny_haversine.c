#include <math.h>
#include <inttypes.h>

#include "tiny_haversine.h"

uint32_t tiny_haversine(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2) {
	float phi_1 = lat1*M_PI/180/1000000;
	float phi_2 = lat2*M_PI/180/1000000;

	float delta_phi = (lat2 - lat1)*M_PI/180/1000000;
	float delta_lambda = (lon2 - lon1)*M_PI/180/1000000;

	float a = pow(sin(delta_phi / 2.0), 2) + cos(phi_1) * cos(phi_2) * pow(sin(delta_lambda / 2.0), 2);

	float c = 2 * atan2(sqrt(a), sqrt(1 - a));

	return (uint32_t)(6371000 * c); // 6371000 radius of the Earth in meters
}
