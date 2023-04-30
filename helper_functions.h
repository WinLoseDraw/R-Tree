// header file for some general helper functions
#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include "rtree.h"

int min(int n1, int n2);

int max(int n1, int n2);

int abs(int n);

int area(int x1, int x2, int y1, int y2);

int areaOfElementMBR(Element e);

int areaOfNodeMBR(node n);

void displayNode(node currentNode);

void copyCoordsToElementMBR(Element *e, int coords[N][2]);

void initializeNode(node *currentNode);

void updateMBRAfterInsert(node *currentNode, Element insertedElement);

void insertElementIntoNode(node *currentNode, Element e);

rtree* createNewRtree();

node* createNewNode();

#endif
