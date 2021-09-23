#include "directionStack.h"
//#include "debug.h"
#include <stdio.h>

//Note little endian is used

enum direction peek(pathStack *stack){
	short top = stack->top - 1;
//	printf("top of stack at: %i \n ", top);

	int insertionPlace = (top%16)*2; //remainder using 16 directions per block
	insertionPlace = insertionPlace < 0 ? 0: insertionPlace;

	int insertionBlock = top/16; //block to insert into


//	printf("stackBlock: %x \n ", stack->D[insertionBlock]);
//	printf("insertion place : %i \n", insertionPlace);
	unsigned int flag = 3; //0000..00000011
	flag = flag << insertionPlace;
//	printf("flag: %x \n ", flag);
	//test bits
	int dir = stack->D[insertionBlock] & flag;
//	DEBUG_PRINT("dir: %i\n", dir);
	dir = dir >> insertionPlace;
//	printf("dir: %x \n ", dir);
	enum direction d;
	d = dir;
	return d;
}

void push(enum direction directionAdded, pathStack *stack)
{
	short top = stack->top;
	int insertionPlace = (top%16)*2; //remainder using 16 directions per block
	int insertionBlock = top/16; //block to insert into
	if(insertionPlace ==0)
	{
		//if a new block is being added, we must initialize the int memory to 0;
//		printf("adding new block\n");
		stack->D[insertionBlock] = 0;
	}
	int d = directionAdded;
//	printf("direciton added: %i \n", directionAdded);
	unsigned int flag;
	switch(d)
	{
		case north:
//			printf("pushingNorth\n");
			flag = 0;
			flag = flag << insertionPlace; //
//			printf("pushflag: %x \n", flag);
			stack->D[insertionBlock] = stack->D[insertionBlock] | flag;
			break;
		case south:
//			printf("pushingSouth\n");
			flag = 1;
			flag = flag << insertionPlace; //
			stack->D[insertionBlock] = stack->D[insertionBlock] | flag;
			break;

		case east:
			flag = 2;
			flag = flag << insertionPlace; //
//			printf("pushingEast\n");
//			printf("pushflag: %x \n ", flag);
			stack->D[insertionBlock] = stack->D[insertionBlock] | flag;
			break;
		case west:
//			printf("pushingwest\n");
			flag = 3;
			flag = flag << insertionPlace; //
			stack->D[insertionBlock] = stack->D[insertionBlock] | flag;
			break;
	}
	 stack->top += 1;

	//insertion
}

enum direction pop(pathStack *stack)
{
	enum direction returnD = peek(stack);
	//pop off and return top of stack, must set back to 0
	short top = stack->top -1;
	int index = (top%16)*2;
	int block = top/16; //block to insert into
//	printf("block before pop: %x \n", stack->D[block]);
	int flag = 3;
	flag = flag << index;
	flag = ~flag;
//	printf("pop flag: %x\n", flag);
	stack->D[block] = stack->D[block] & flag;
//	printf("block after pop: %x \n", stack->D[block]);

	stack->top -=1;
	return returnD;

}

