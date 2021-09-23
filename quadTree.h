/*
 * octTree.h
 *
 *  Created on: Jun 17, 2021
 *      Author: Kirk Vander Ploeg
 */

#ifndef SRC_quadTree_H_
#define SRC_quadTree_H_

#include <stdbool.h>
#include <stddef.h>
#include "sensing.h"


//Forward declaration of a node (as the structure is recursive this is required)
typedef struct s_treeNode treeNode;

//need a node to be as small as possible
struct s_treeNode{
	signed char occupancy; //1byte
	treeNode *ne, *nw, *se, *sw;  //children = size of pointer * 4
	//Should change this to a list which fits four children, only needs 1 ptr then
};

typedef struct qTree{
	treeNode* treeRoot;
	int count; //#of nodes
	int height;
} quadTree;


int round10(int n);
bool inRange(int x, int y);
void initTree(quadTree* tree);
void updateTree(quadTree* tree, float pos_x, float pos_y, int point_x, int point_y);
//void getNeighbours(quadTree *tree, coord centerNode, treeNode *north, treeNode *south, treeNode *east, treeNode *west);
//void getNorthNeighbour(quadTree *tree, coord centerNode, treeNode *north);
void insertNode(quadTree* tree, treeNode* node,  int x, int y, bool occupied, int squareSize);
treeNode* getNode(int x, int y, treeNode* node, int squareSize);
bool exists(treeNode *node);
bool prune(treeNode *node);
#endif


