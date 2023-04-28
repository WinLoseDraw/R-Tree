#include <stdio.h>
#include "rtree.h"
#include "helper_functions.h"
#include "splitnode.h"

int dummyCoords[N][2] = {{0, 0}, {0, 0}}; // MBR for a dummy element, used as a helper

bool isDummyElement(Element e)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (e.MBR[i][j] != dummyCoords[i][j])
                return false;
        }
    }

    return true;
}

void setAsDummyElement(Element *e)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            e->MBR[i][j] = 0;
        }
    }
}

int calculateTempArea(Element e1, Element e2) // used in splitnode
{
    return area(min(e1.MBR[0][0], e2.MBR[0][0]), max(e1.MBR[0][1], e2.MBR[0][1]), min(e1.MBR[1][0], e2.MBR[1][0]), max(e1.MBR[1][1], e2.MBR[1][1]));
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

    setAsDummyElement(elementArray + maxI);
    setAsDummyElement(elementArray + maxJ);
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
    Element returnElement;
    setAsDummyElement(&returnElement);
    int returnIndex;

    for (int i = 0; i < M + 1; i++)
    {

        Element e = originalElements[i];

        if (isDummyElement(e))
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

    setAsDummyElement(originalElements + returnIndex);

    return returnElement;
}

void quadraticSplitNode(node originalNode, Element extraElement, node *newNode1, node *newNode2)
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

    Element firstElementOfFirstNewNode;
    Element firstElementOfSecondNewNode;

    quadraticPickSeeds(elements, &firstElementOfFirstNewNode, &firstElementOfSecondNewNode);

    insertElementIntoNode(&firstNewNode, firstElementOfFirstNewNode);
    insertElementIntoNode(&secondNewNode, firstElementOfSecondNewNode);

    int remainingCount = M - 1; // after removing 2 elements from elements array using PickSeeds

    while (remainingCount > 0)
    {

        if (firstNewNode.count + remainingCount == m) // if there are too few entries in firstNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (isDummyElement(elements[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    insertElementIntoNode(&firstNewNode, elements[index]);
                    remainingCount--;
                }
            }

            break;
        }

        else if (secondNewNode.count + remainingCount == m) // if there are too few entries in secondNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (isDummyElement(elements[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    insertElementIntoNode(&secondNewNode, elements[index]);
                    remainingCount--;
                }
            }

            break;
        }

        Element nextElement = pickNext(firstNewNode, secondNewNode, elements);

        if (isDummyElement(nextElement))
            break; // entries are finished

        int cost1 = calculateCost(nextElement, firstNewNode);
        int cost2 = calculateCost(nextElement, secondNewNode);

        if (cost1 < cost2)
        {
            insertElementIntoNode(&firstNewNode, nextElement);
        }
        else if (cost2 < cost1)
        {
            insertElementIntoNode(&secondNewNode, nextElement);
        }
        else
        {
            int area1 = areaOfNodeMBR(firstNewNode);
            int area2 = areaOfNodeMBR(secondNewNode);

            if (area1 < area2)
            {
                insertElementIntoNode(&firstNewNode, nextElement);
            }
            else if (area1 > area2)
            {
                insertElementIntoNode(&secondNewNode, nextElement);
            }
            else
            {
                if (firstNewNode.count < secondNewNode.count)
                {
                    insertElementIntoNode(&firstNewNode, nextElement);
                }
                else
                {
                    insertElementIntoNode(&secondNewNode, nextElement);
                }
            }
        }

        remainingCount--;
    }

    *newNode1 = firstNewNode;
    *newNode2 = secondNewNode;
}

void linearPickSeeds(Element elementArray[M + 1], Element *firstElement, Element *secondElement)
{
    int highestLowX, highestLowY = INT_LOW;
    int lowestHighX, lowestHighY = INT_HIGH;
    int highestLowXIndex, lowestHighXIndex, highestLowYIndex, lowestHighYIndex;

    int minX, minY = INT_HIGH;
    int maxX, maxY = INT_LOW;

    for (int i = 0; i < M; i++)
    {
        Element e = elementArray[i];

        int low_x = e.MBR[0][0];
        int high_x = e.MBR[0][1];
        int low_y = e.MBR[1][0];
        int high_y = e.MBR[1][1];

        if (low_x < minX)
        {
            minX = low_x;
        }
        else if (low_x > highestLowX)
        {
            highestLowX = low_x;
            highestLowXIndex = i;
        }

        if (low_y < minY)
        {
            minY = low_y;
        }
        else if (low_y > highestLowY)
        {
            highestLowY = low_y;
            highestLowYIndex = i;
        }

        if (high_x > maxX)
        {
            maxX = high_x;
        }
        else if (high_x < lowestHighX)
        {
            lowestHighX = high_x;
            lowestHighXIndex = i;
        }

        if (high_y > maxY)
        {
            maxY = high_y;
        }
        else if (high_y < lowestHighY)
        {
            lowestHighY = high_y;
            lowestHighYIndex = i;
        }
    }

    int widthX = maxX - minX;
    int widthY = maxY - minY;

    int normalizedSeparationX = (highestLowX - lowestHighX) / widthX;
    int normalizedSeparationY = (highestLowY - lowestHighY) / widthY;

    if (normalizedSeparationX > normalizedSeparationY)
    {
        *firstElement = elementArray[lowestHighXIndex];
        *secondElement = elementArray[highestLowXIndex];
        setAsDummyElement(elementArray + highestLowXIndex);
        setAsDummyElement(elementArray + lowestHighXIndex);
    }
    else
    {
        *firstElement = elementArray[lowestHighYIndex];
        *secondElement = elementArray[highestLowYIndex];
        setAsDummyElement(elementArray + highestLowYIndex);
        setAsDummyElement(elementArray + lowestHighYIndex);
    }
}

void linearSplitNode(node originalNode, Element extraElement, node *newNode1, node *newNode2)
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

    Element firstElementOfFirstNewNode;
    Element firstElementOfSecondNewNode;

    linearPickSeeds(elements, &firstElementOfFirstNewNode, &firstElementOfSecondNewNode);

    insertElementIntoNode(&firstNewNode, firstElementOfFirstNewNode);
    insertElementIntoNode(&secondNewNode, firstElementOfSecondNewNode);

    int remainingCount = M - 1; // after removing 2 elements from elements array using PickSeeds

    while (remainingCount > 0)
    {

        if (firstNewNode.count + remainingCount == m) // if there are too few entries in firstNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (isDummyElement(elements[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    insertElementIntoNode(&firstNewNode, elements[index]);
                    remainingCount--;
                }
            }

            break;
        }

        else if (secondNewNode.count + remainingCount == m) // if there are too few entries in secondNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (isDummyElement(elements[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    insertElementIntoNode(&secondNewNode, elements[index]);
                    remainingCount--;
                }
            }

            break;
        }

        Element nextElement = pickNext(firstNewNode, secondNewNode, elements);

        if (isDummyElement(nextElement))
            break; // entries are finished

        int cost1 = calculateCost(nextElement, firstNewNode);
        int cost2 = calculateCost(nextElement, secondNewNode);

        if (cost1 < cost2)
        {
            insertElementIntoNode(&firstNewNode, nextElement);
        }
        else if (cost2 < cost1)
        {
            insertElementIntoNode(&secondNewNode, nextElement);
        }
        else
        {
            int area1 = areaOfNodeMBR(firstNewNode);
            int area2 = areaOfNodeMBR(secondNewNode);

            if (area1 < area2)
            {
                insertElementIntoNode(&firstNewNode, nextElement);
            }
            else if (area1 > area2)
            {
                insertElementIntoNode(&secondNewNode, nextElement);
            }
            else
            {
                if (firstNewNode.count < secondNewNode.count)
                {
                    insertElementIntoNode(&firstNewNode, nextElement);
                }
                else
                {
                    insertElementIntoNode(&secondNewNode, nextElement);
                }
            }
        }

        remainingCount--;
    }

    *newNode1 = firstNewNode;
    *newNode2 = secondNewNode;
}