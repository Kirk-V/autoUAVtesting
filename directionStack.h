#ifndef SRC_directionStack_H_
#define SRC_directionStack_H_


//Path
typedef struct pathStack_s{
	short top; //size in # of directions
	int D[100]; //array of directions
}pathStack;


enum direction{north, south, east, west};

enum direction peek(pathStack *stack);
void push(enum direction directionAdded, pathStack *stack);
enum direction pop(pathStack *stack);

#endif
