// contains some helper functions for general usage
#include <stdio.h>
#include "rtree.h"
#include "helper_functions.h"

int min(int n1, int n2)
{
    if (n1 < n2)
        return n1;
    else
        return n2;
}

int max(int n1, int n2)
{
    if (n1 > n2)
        return n1;
    else
        return n2;
}

int abs(int n)
{
    if (n < 0)
        return -1 * n;
}

/*
0, 0 = x1
0, 1 = x2
1, 0 = y1
1, 1 = y2
*/

int area(int x1, int x2, int y1, int y2)
{
    return (x2 - x1) * (y2 - y1);
}

int areaOfElementMBR(Element e)
{
    return area(e.MBR[0][0], e.MBR[0][1], e.MBR[1][0], e.MBR[1][1]);
}

int areaOfNodeMBR(node n)
{
    return area(n.MBR[0][0], n.MBR[0][1], n.MBR[1][0], n.MBR[1][1]);
}

void displayNode(node currentNode)
{
    printf("Node MBR: ((%d, %d), (%d, %d))\n", currentNode.MBR[0][0], currentNode.MBR[0][1], currentNode.MBR[1][0], currentNode.MBR[1][1]);
    printf("Node count: %d\n", currentNode.count);
    printf("Node entries: \n");
    for (int i = 0; i < currentNode.count; i++)
    {
        Element currentElement = currentNode.entries[i];
        printf("Entry %d: MBR ((%d, %d), (%d, %d))\n", i + 1, currentElement.MBR[0][0], currentElement.MBR[0][1], currentElement.MBR[1][0], currentElement.MBR[1][1]);
    }
}

void copyCoordsToElementMBR(Element *e, int coords[N][2])
{ // useful for initializing an element
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            e->MBR[i][j] = coords[i][j];
        }
    }
}

void initializeNode(node *currentNode) // used in splitnode
{
    currentNode->MBR[0][0] = INT_HIGH;
    currentNode->MBR[0][1] = INT_LOW;
    currentNode->MBR[1][0] = INT_HIGH;
    currentNode->MBR[1][1] = INT_LOW;
    currentNode->count = 0;
}

void updateMBRAfterInsert(node *currentNode, Element insertedElement) // updates MBR of node after every element insert
{
    currentNode->MBR[0][0] = min(currentNode->MBR[0][0], insertedElement.MBR[0][0]);
    currentNode->MBR[0][1] = max(currentNode->MBR[0][1], insertedElement.MBR[0][1]);
    currentNode->MBR[1][0] = min(currentNode->MBR[1][0], insertedElement.MBR[1][0]);
    currentNode->MBR[1][1] = max(currentNode->MBR[1][1], insertedElement.MBR[1][1]);
}

void insertElementIntoNode(node *currentNode, Element e)
{
    currentNode->entries[currentNode->count++] = e;
    updateMBRAfterInsert(currentNode, e);
}

rtree* createNewRtree() {
    rtree* ans = (rtree*) malloc(sizeof(rtree));
    return ans;
}

node* createNewNode() {
    node* ans = (node*) malloc(sizeof(node));
    ans->isLeaf = true;
    ans->count = 0;
    ans->parent = NULL;

    for (int i=0; i<N; ++i) {
        ans->MBR[i][0] = INT_MAX;
        ans->MBR[i][1] = INT_MIN;
    }

    return ans;
}
