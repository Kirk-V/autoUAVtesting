/*
 * octTree.c
 *
 *  Created on: Jun 17, 2021
 *      Author: Kirk Vander Ploeg
 */
#include "quadTree.h"
//#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "sensing.h"


#define FLOAT_TO_INT(x) ((x)>=0.0f?(int)((x)+0.5f):(int)((x)-0.5f))  //rounding floats to ints




int maxSize = 512; //size of a side for the largest possible square of the map;
int resolution = 16; //size of a side of the smallest possible square of the map

int round10(int n)
{
	//if the remainder is less than 10, return floor(n/10) * 10
	int remainder = n%10;
	if (remainder <5)
	{
		return (n/10)*10;
	}
	//if the remainder is >10, add 10 and return floor(n/10) * 10
	else return (+10/10)*10;

}


bool inRange(int x, int y)
{
	if((abs(x)- maxSize) > 0)
	{
		return false;
	}
	else return true;
}

void initTree(quadTree* tree)
{
	tree->treeRoot =  (treeNode*)malloc(sizeof(treeNode));
	tree->treeRoot->ne = 0;
	tree->treeRoot->nw = 0;
	tree->treeRoot->se = 0;
	tree->treeRoot->sw = 0;
	tree->count = 0;
	tree->height = 0;
}

void initNode(treeNode *node){
	node->nw = 0;
	node->ne = 0;
	node->sw = 0;
	node->se = 0;
	node->occupancy = 0;
}


void updateTree(quadTree* tree, float pos_x, float pos_y, int point_x, int point_y)
{
	float dy = pos_y-point_y;
	float dx = pos_x-point_x;
	float m = dy/dx;
	//insert nodes from x = pos_x to x = point_x at the appropriate y values
	float y = pos_y;
	int pos = FLOAT_TO_INT(pos_x);
	int point = FLOAT_TO_INT(point_x);
	if(dx < 0)
	{
		for (int i = pos; i < point; i++)
		{
			y = y + m;
			printf("Inserting NEW NODE: (%i, %f)\n",i, y);
			insertNode(tree, tree->treeRoot, i, y, false, maxSize);

		}
	}
	else{
		for (int i = pos; i > point; i--)
		{
			y = y - m;//c
			printf("Inserting NEW NODE: (%i,%fd)\n",i, y);
			insertNode(tree, tree->treeRoot, i, y, false, maxSize);
		}
	}
	//final node will be occupied;
	insertNode(tree, tree->treeRoot, FLOAT_TO_INT(point_x), FLOAT_TO_INT(point_y), true, maxSize);
}

void getNeighbours(quadTree *tree, coord centerNode, treeNode *north, treeNode *south, treeNode *east, treeNode *west)
{
	north = getNode(centerNode.x, centerNode.y+1, tree->treeRoot, maxSize);
	south = getNode(centerNode.x, centerNode.y-1, tree->treeRoot, maxSize);
	east = getNode(centerNode.x+1, centerNode.y, tree->treeRoot, maxSize);
	west = getNode(centerNode.x-1, centerNode.y, tree->treeRoot, maxSize);
}

void getNorthNeighbour(quadTree *tree, coord centerNode, treeNode *north)
{
	north = getNode(centerNode.x, centerNode.y+1, tree->treeRoot, maxSize);
}

// recursively find the location that the node should be inserted, once located, update the nodes P(occupancy). After insertion
// check if nodes can be combined while propagating back
void insertNode(quadTree* tree, treeNode* node,  int x, int y, bool occupied, int squareSize) //recursive attempt so that pruning can be achieved
{
	printf("square size: %i\n", squareSize);

	if (!inRange(x, y)){
		// DEBUG_PRINT("out of range\n");
//		vTaskDelay(M2T(200));
		return;
	}

	//First, check if we need to continue searching down the tree to insert. If the current node's occupancy indicates
	//the same as the data measurement, we do not need to continue as the tree is already consistent with the new data.
	if(occupied)
	{
		if(node->occupancy >= 100)
		{
			//no updates needed
			return;
		}
	}
	else //data says not occupied.
	{
		if(node->occupancy <= -100)
		{

			return;
		}

	}


	// If we have reached the smallest resolution, we must insert/update the node
	if(squareSize < resolution)
	{
        printf("inserting\n");
		//space cannot be divided up further, we should update the node here
		if (occupied) //data says occupied
		{
			//add 10 to occupancy value of node
			node->occupancy = 100;
			if(node->occupancy > 100)
			{
				//cut off occupancy at 50 so over confidence does not occur
				node->occupancy = 50;
			}
		}
		if (!occupied)
		{
			node->occupancy = -100;
			if(node->occupancy < -100)
			{
				//cut off occupancy at -50 so over confidence does not occur
				node->occupancy = -50;
			}
		}
		return;
	}

	// Here we check which way down the tree we should explore to insert a new node
	if(y > 0)
	{
		//point is north
		if(x >= 0)
		{
			//point is northeast..Check to see if node exists here
			if(node->ne == 0)
			{
				// allocate mem for new node
//				treeNode new;
//				DEBUG_PRINT("Making ne node\n");
				node->ne = (treeNode*)malloc(sizeof(treeNode));
				initNode(node->ne);
				tree->count += 1;
//				DEBUG_PRINT("NE = %p\n", (void*)node->ne);
//				*(node->ne) = new;
			}
			else{
//				DEBUG_PRINT("NE = is already Made!%p\n", (void*)node->ne);
			}
			if(!exists(node->ne))
			{
//				DEBUG_PRINT("Can't make ne\n");
				return;
			}

			//now recurse down the tree with updated node and squareSize
			squareSize = squareSize/2;
//			DEBUG_PRINT("NE = %p\n", (void*)node->ne);
//			DEBUG_PRINT("recursing ne\n");
			insertNode(tree, node->ne,  x-squareSize, y-squareSize, occupied, squareSize);
		}
		else
		{
			if(node->nw == 0)
			{
				// allocate mem for new node
//				treeNode new;
				node->nw = (treeNode*)malloc(sizeof(treeNode));
				initNode(node->nw);
				tree->count += 1;
//				*(node->ne) = new;
			}
			else{
//				DEBUG_PRINT("Nw = is already Made!%p\n", (void*)node->nw);
			}
			if(!exists(node->nw))
			{
//				DEBUG_PRINT("Can't make nw\n");
				return;
			}


//			DEBUG_PRINT("recursing nw\n");
			//now recurse down the tree with updated node and squareSize
			squareSize = squareSize/2;
			insertNode(tree, node->nw,  x+squareSize, y-squareSize, occupied, squareSize);
		}
	}
	else
	{
		//south
		if(x >= 0)
		{
			//southeast..Check to see if node exists here
			if(node->se == 0)
			{
				// allocate mem for new node
//				treeNode new;
//				DEBUG_PRINT("Making se node\n");
				node->se = (treeNode*)malloc(sizeof(treeNode));
				initNode(node->se);
				tree->count += 1;
//				DEBUG_PRINT("treeSize= %i\n", tree->count);
//				*(node->ne) = new;
			}
			else{
//				DEBUG_PRINT("sE = is already Made!%p\n", (void*)node->se);
			}

				//now recurse down the tree with updated node and squareSize
			squareSize = squareSize/2;
			if(!exists(node->se))
			{
//				DEBUG_PRINT("Can't make se\n");
				return;
			}

//			DEBUG_PRINT("recursing se\n");
			insertNode(tree, node->se,  x-squareSize, y+squareSize, occupied, squareSize);
		}
		else
		{
			if(node->sw == 0)
			{
				// allocate mem for new node
//				treeNode new;
//				DEBUG_PRINT("Making sw node\n");
				node->sw = (treeNode*)malloc(sizeof(treeNode));
				initNode(node->sw);
				tree->count += 1;
//				DEBUG_PRINT("treeSize= %i\n", tree->count);
//				*(node->ne) = new;
			}
			else{
//				DEBUG_PRINT("sw = is already Made!%p\n", (void*)node->sw);
			}
			if(!exists(node->sw))
			{
//				DEBUG_PRINT("Can't make sw\n");
				return;
			}

			//now recurse down the tree with updated node and squareSize

//			DEBUG_PRINT("recursing sw\n");
			squareSize = squareSize/2;
			insertNode(tree, node->sw,  x+squareSize, y+squareSize, occupied, squareSize);
		}

	}
	//check if we can now prune
	if((node->nw) && (node->sw) && (node->ne) && (node->se))
	{
		printf("Pruning\n");
//		vTaskDelay(M2T(1000));
		bool pruned = prune(node);
		if(pruned)
		{
//			DEBUG_PRINT("Pruning\n");
			node->nw = 0;
			node->ne = 0;
			node->sw = 0;
			node->se = 0;
			tree->count -=4;
//			DEBUG_PRINT("Pruned\n");
//			vTaskDelay(M2T(1000));
		}
	}
	return;
}


//returns a pointer to the node at specified (x,y), could not be at lowest resolution if tree is not fully
//decomposed at that point returns 0 if out of range.
treeNode* getNode(int x, int y, treeNode* node, int squareSize)
{
	if (!inRange(x, y)){
		return 0; //looking up a value out of map range
	}


	// If we have reached the smallest resolution, we have found the node
	if(squareSize < resolution)
	{
		//space cannot be divided up further
		return node;
	}

	// Here we check which way down the tree we should explore to find the node
	if(y > 0)
	{
		//north
		if(x >= 0)
		{
			//northeast..Check to see if node exists here
			if(node->ne == 0)
			{
				return node; //no more children, so return node
			}
			squareSize = squareSize/2;
			return getNode(x-squareSize, y-squareSize, node->ne,squareSize);
		}
		else
		{
			if(node->nw == 0)
			{
				return node; //no more children, so return node
			}

			//now recurse down the tree with updated node and squareSize
			squareSize = squareSize/2;
			return getNode(x+squareSize, y-squareSize, node->nw, squareSize);
		}
	}
	else
	{
		//South
		if(x >= 0)
		{
			//southeast..Check to see if node exists here
			if(node->se == 0)
			{
				return node; //no more children, so return node
			}
			squareSize = squareSize/2;
			return getNode(x-squareSize, y+squareSize, node->se, squareSize);
		}
		else
		{
			if(node->sw == 0)
			{
				return node; //no more children, so return node
			}
			//now recurse down the tree with updated node and squareSize
			squareSize = squareSize/2;
			return getNode( x+squareSize, y+squareSize, node->sw, squareSize);
		}
	}
	//check if we can now prune
}

bool exists(treeNode *node)
{
	if(node == 0)
	{
		return false;
	}
	return true;
}

bool prune(treeNode *node)
{
	bool did_prune = false;
	if((node->nw->occupancy >= 50) && (node->ne->occupancy >= 50) && (node->sw->occupancy >= 50) && (node->se->occupancy >= 50))
	{
		node->occupancy = 50;
		did_prune = true;
	}
	else if((node->nw->occupancy <= -50) && (node->ne->occupancy <= -50) && (node->sw->occupancy <= -50) && (node->se->occupancy <= -50))
	{
		did_prune = true;
		node->occupancy = -50;
	}
	if(did_prune)
	{
		free(node->nw);
		free(node->ne);
		free(node->sw);
		free(node->se);
	}
	return did_prune;
}



