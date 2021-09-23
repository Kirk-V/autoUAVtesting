#include "debug.h"
#include "log.h"
#include "param.h"
#include "commander.h"
#include "sensing.h"
#include <math.h>



//how far forward can the drone can safely travel
//distance and safetyRadius in mm
int spaceAvailable(uint16_t distance, int safetyRadius)
{
	int available = distance - safetyRadius;
	if(available > 0)
	{
		return available;
	}
	else return 0;
}

//rangeDistance in mm, pos_x and pos_y in m, yaw in 180 to -180
void get_range_coord(coord *return_coord, int pos_x, int pos_y, int yaw, int rangeDistance)
{
	float theta = (double)yaw*M_PI/180;

	float h = (float)rangeDistance/1000;

	float opposite = (double)h*(sin(theta));

	float adjacent = (double)h*(cos(theta));

//	DEBUG_PRINT("O = %f\n", (double)opposite);
//	DEBUG_PRINT("a = %f\n", (double)adjacent);
//	DEBUG_PRINT("theta = %f\n", (double)theta);

	float newX = (pos_x + adjacent*100);
	float newY = (pos_y + opposite*100); //convert to cms

//	DEBUG_PRINT("New x %f\n", (double)newX);
//	DEBUG_PRINT("New y %f\n", (double)newY);
	return_coord->x = (int)newX;
	return_coord->y = (int)newY;
}

