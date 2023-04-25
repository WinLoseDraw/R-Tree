#include <stdio.h>
#include "rtree.h"

Element dummy = {NULL, NULL};

#define DUMMY_ELEMENT dummy;

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

int calculateTempArea(Element e1, Element e2)
{
    return area(min(e1.MBR[0][0], e2.MBR[0][0]), max(e1.MBR[0][1], e2.MBR[0][1]), min(e1.MBR[1][0], e2.MBR[1][0]), max(e1.MBR[1][1], e2.MBR[1][1]));
}

int areaOfElementMBR(Element e)
{
    return area(e.MBR[0][0], e.MBR[0][1], e.MBR[1][0], e.MBR[1][1]);
}

int areaOfNodeMBR(node n)
{
    return area(n.MBR[0][0], n.MBR[0][1], n.MBR[1][0], n.MBR[1][1]);
}

void quadraticPickSeeds(Element elementArray[M + 1], Element *firstElement, Element *secondElement)
{

    int maxWastedArea = -1;
    int maxI, maxJ;
    int wastedArea, tempArea;
    Element e1, e2;

    for (int i = 0; i < M; i++)
    {
        for (int j = i + 1; j < M + 1; j++)
        {

            e1 = elementArray[i];
            e2 = elementArray[j];

            int tempArea = calculateTempArea(e1, e2);
            wastedArea = tempArea - areaOfElementMBR(e1) - areaOfElementMBR(e2);

            if (wastedArea > maxWastedArea)
            {
                maxWastedArea = wastedArea;
                maxI = i;
                maxJ = j;
                *firstElement = e1;
                *secondElement = e2;
            }
        }
    }

    elementArray[maxI], elementArray[maxJ] = DUMMY_ELEMENT;
}

int calculateCost(Element e1, node group)
{
    int totalArea = area(min(e1.MBR[0][0], group.MBR[0][0]), max(e1.MBR[0][1], group.MBR[0][1]), min(e1.MBR[1][0], group.MBR[1][0]), max(e1.MBR[1][1], group.MBR[1][1]));
    int groupArea = areaOfNodeMBR(group);
    int cost = totalArea - groupArea;
    return cost;
}

Element pickNext(node group1, node group2, Element originalElements[M + 1])
{ // may return dummy element

    int d1, d2, difference;
    int maxDifference = -1;
    Element returnElement = DUMMY_ELEMENT;
    int returnIndex;

    for (int i = 0; i < M + 1; i++)
    {

        Element e = originalElements[i];

        if (e.MBR == NULL)
            continue;

        d1 = calculateCost(e, group1);
        d2 = calculateCost(e, group2);

        difference = abs(d1 - d2);

        if (difference > maxDifference)
        {
            maxDifference = difference;
            returnElement = e;
            returnIndex = i;
        }
    }

    originalElements[returnIndex] = DUMMY_ELEMENT;

    return returnElement;
}

void initializeNode(node *currentNode)
{
    currentNode->MBR[0][0] = 1000000;
    currentNode->MBR[0][1] = -1000000;
    currentNode->MBR[1][0] = 1000000;
    currentNode->MBR[1][1] = -1000000;
    currentNode->count = 0;
}

void updateMBRAfterInsert(node *currentNode, Element insertedElement)
{
    currentNode->MBR[0][0] = min(currentNode->MBR[0][0], insertedElement.MBR[0][0]);
    currentNode->MBR[0][1] = max(currentNode->MBR[0][1], insertedElement.MBR[0][1]);
    currentNode->MBR[1][0] = min(currentNode->MBR[1][0], insertedElement.MBR[1][0]);
    currentNode->MBR[1][1] = max(currentNode->MBR[1][1], insertedElement.MBR[1][1]);
}

void quadraticSplitNode(node originalNode, Element extraElement)
{

    Element elements[M + 1];

    for (int i = 0; i < M; i++)
    {
        elements[i] = originalNode.entries[i];
    }

    elements[M] = extraElement;

    node firstNewNode;
    node secondNewNode;

    initializeNode(&firstNewNode);
    initializeNode(&secondNewNode);

    quadraticPickSeeds(elements, &(firstNewNode.entries[0]), &(secondNewNode.entries[0]));
    firstNewNode.count++;
    secondNewNode.count++;

    int remainingCount = M - 1; // after removing 2 elements from elements array using PickSeeds

    while (remainingCount > 0)
    {

        if (firstNewNode.count + remainingCount == m) // if there are too few entries in firstNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (elements[index].MBR == NULL)
                    continue;

                firstNewNode.entries[firstNewNode.count++] = elements[index++];
                remainingCount--;
            }

            break;
        }

        else if (secondNewNode.count + remainingCount == m) // if there are too few entries in secondNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (elements[index].MBR == NULL)
                    continue;

                secondNewNode.entries[secondNewNode.count++] = elements[index++];
                remainingCount--;
            }

            break;
        }

        Element nextElement = pickNext(firstNewNode, secondNewNode, elements);

        if (nextElement.MBR == NULL)
            break; // entries are finished

        int cost1 = calculateCost(nextElement, firstNewNode);
        int cost2 = calculateCost(nextElement, secondNewNode);

        if (cost1 < cost2)
        {
            firstNewNode.entries[firstNewNode.count++] = nextElement;
            updateMBRAfterInsert(&firstNewNode, nextElement);
        }
        else if (cost2 < cost1)
        {
            secondNewNode.entries[secondNewNode.count++] = nextElement;
            updateMBRAfterInsert(&secondNewNode, nextElement);
        }
        else
        {
            int area1 = areaOfNodeMBR(firstNewNode);
            int area2 = areaOfNodeMBR(secondNewNode);

            if (area1 < area2)
            {
                firstNewNode.entries[firstNewNode.count++] = nextElement;
                updateMBRAfterInsert(&firstNewNode, nextElement);
            }
            else if (area1 > area2)
            {
                secondNewNode.entries[secondNewNode.count++] = nextElement;
                updateMBRAfterInsert(&secondNewNode, nextElement);
            }
            else
            {
                if (firstNewNode.count < secondNewNode.count)
                {
                    firstNewNode.entries[firstNewNode.count++] = nextElement;
                    updateMBRAfterInsert(&firstNewNode, nextElement);
                }
                else
                {
                    secondNewNode.entries[secondNewNode.count++] = nextElement;
                    updateMBRAfterInsert(&secondNewNode, nextElement);
                }
            }
        }

        remainingCount--;
    }
}

void linearPickSeeds()
{
}

void linearSplitNode()
{
}