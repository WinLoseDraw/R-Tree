#include <stdio.h>
#include "rtree.h"
#include "helper_functions.h"
#include "splitnode.h"

void splitNodeTest() {
    
    node testNode;
    initializeNode(&testNode);

    Element testNodeEntry1;
    int testNode1MBR[N][2] = {{1, 2}, {3, 4}};
    copyCoordsToElementMBR(&testNodeEntry1, testNode1MBR);

    Element testNodeEntry2;
    int testNode2MBR[N][2] = {{1, 4}, {4, 7}};
    copyCoordsToElementMBR(&testNodeEntry2, testNode2MBR);

    Element testNodeEntry3;
    int testNode3MBR[N][2] = {{3, 8}, {2, 10}};
    copyCoordsToElementMBR(&testNodeEntry3, testNode3MBR);

    Element testNodeEntry4;
    int testNode4MBR[N][2] = {{2, 6}, {1, 8}};
    copyCoordsToElementMBR(&testNodeEntry4, testNode4MBR);

    insertElementIntoNode(&testNode, testNodeEntry1);
    insertElementIntoNode(&testNode, testNodeEntry2);
    insertElementIntoNode(&testNode, testNodeEntry3);
    insertElementIntoNode(&testNode, testNodeEntry4);

    printf("Original Node: \n");
    displayNode(testNode);

    Element extraElement;
    int extraElementMBR[N][2] = {{5, 7}, {2, 5}};
    copyCoordsToElementMBR(&extraElement, extraElementMBR);
    printf("Extra Entry: MBR ((%d, %d), (%d, %d))\n", extraElementMBR[0][0], extraElementMBR[0][1], extraElementMBR[1][0], extraElementMBR[1][1]);

    node newNode1, newNode2;

    linearSplitNode(testNode, extraElement, &newNode1, &newNode2);

    printf("\n\nNew nodes: \n\nNode 1:\n");
    displayNode(newNode1);
    printf("\nNode 2: \n");
    displayNode(newNode2);
}

int main()
{
    splitNodeTest();
    return 0;
}