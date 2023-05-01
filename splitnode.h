#include <stdio.h>
#include "rtree.h"

bool isDummyElement(Element e);

void setAsDummyElement(Element *e);

int calculateTempArea(Element e1, Element e2);

void splitNode(node originalNode, node *newNode1, node *newNode2, bool isLinear);
