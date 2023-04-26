#include <stdio.h>
#include "rtree.h"

bool isDummyElement(Element e);

void setAsDummyElement(Element *e);

int calculateTempArea(Element e1, Element e2);

void quadraticSplitNode(node originalNode, Element extraElement, node *newNode1, node *newNode2);

void linearSplitNode(node originalNode, Element extraElement, node *newNode1, node *newNode2);