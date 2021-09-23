#ifndef SRC_sensing_H_
#define SRC_sensing_H_

#include "stabilizer_types.h"
typedef struct coord_s{
	int x;
	int y;
} coord;

typedef struct cell_s{
	int x;
	int y;
}cell;


int spaceAvailable(uint16_t distance, int safetyRadius);
void get_range_coord(coord *return_coord, int pos_x, int pos_y, int yaw, int rangeDistance);
#endif



