#define M 4
#define m 2
#define N 2     //Number of dimensions
#define INT_HIGH 1e8;
#define INT_LOW -1e8;
#define INF 1e9

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

//----------------------------------------------------------Structs begin-----------------------------------------------------//

typedef enum { false, true } bool;

struct Element {
    int MBR[N][2];
    struct rtree_node* childPointer;
};

typedef struct Element Element;

struct rtree_node {
    bool isLeaf;
    int count;
    int MBR[N][2];
    Element entries[M+1];
    struct rtree_node* parent;
};

typedef struct rtree_node node;

struct rtree {
    node* root;
};

typedef struct rtree rtree;

//----------------------------------------------------------Structs end-----------------------------------------------------//

//----------------------------------------------------------Helper Functions begin-----------------------------------------------------//

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
Convention followed for MBR.
(x1, y1) is bottom/left, (x2,y2) is top/right.

0, 0 = x1
0, 1 = x2
1, 0 = y1
1, 1 = y2
*/

long area(int x1, int x2, int y1, int y2)
{
    return (x2 - x1) * (y2 - y1);
}

long areaOfElementMBR(Element e)
{
    return area(e.MBR[0][0], e.MBR[0][1], e.MBR[1][0], e.MBR[1][1]);
}

long areaOfNodeMBR(node n)
{
    return area(n.MBR[0][0], n.MBR[0][1], n.MBR[1][0], n.MBR[1][1]);
}

double center(int arr[2]) 
{
    return ((double)arr[0] + arr[1])/2;
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
    currentNode->MBR[0][0] = INT_MAX;
    currentNode->MBR[0][1] = INT_MIN;
    currentNode->MBR[1][0] = INT_MAX;
    currentNode->MBR[1][1] = INT_MIN;
    currentNode->count = 0;
    currentNode->parent = NULL;
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
    if (currentNode->parent)
    {
        for (int i = 0; i < currentNode->parent->count; i++)
        {
            if (currentNode->parent->entries[i].childPointer == currentNode)
            {
                currentNode->parent->entries[i].MBR[0][0] = currentNode->MBR[0][0];
                currentNode->parent->entries[i].MBR[0][1] = currentNode->MBR[0][1];
                currentNode->parent->entries[i].MBR[1][0] = currentNode->MBR[1][0];
                currentNode->parent->entries[i].MBR[1][1] = currentNode->MBR[1][1];
                break;
            }
        }
    }
}

rtree *createNewRtree()
{
    rtree *ans = (rtree *)malloc(sizeof(rtree));
    return ans;
}

node *createNewNode()
{
    node *ans = (node *)malloc(sizeof(node));
    ans->isLeaf = true;
    ans->count = 0;
    ans->parent = NULL;

    for (int i = 0; i < N; ++i)
    {
        ans->MBR[i][0] = INT_MAX;
        ans->MBR[i][1] = INT_MIN;
    }

    return ans;
}

//----------------------------------------------------------Helper Functions end-----------------------------------------------------//

//----------------------------------------------------------STR begins-----------------------------------------------------//

void insertionSort(node** rectangles, int start, int end, int dimension) {
    for (int i = start+1; i < end; i++) {
        node* key = rectangles[i];
        int j = i-1;
        while ((j >= start) && (center(key->MBR[dimension]) < center(rectangles[j]->MBR[dimension]))) {
            rectangles[j+1] = rectangles[j];
            j--;
        }
        j++;
        rectangles[j] = key;
    }
}

void insertionSort2(Element* data, int start, int end, int dimension) {
    for (int i = start; i < end; i++) {
       Element key = data[i];
       int j = i-1;
       while ((j >= start) && (center(key.MBR[dimension]) < center(data[j].MBR[dimension]))) {
            data[j+1] = data[j];
            j--;
        }
        j++;
        data[j] = key; 
    }
}

node* construct(node** nodes, int n) {              //constructs nodes using the lower level of nodes
    if (n == 1) {
        return nodes[0];
    }
    
    insertionSort(nodes, 0, n, 0);              //sorts all the nodes by the x-values

    int p = (n+M-1)/M;                          //ceil(n/M)
    int s = (int)(ceil(sqrt(p)));                
    int number = 0;                             //number of nodes in the next level

    for (int i = 0; i < n; i += s*M) {          //sorts ranges of s*M by the y-axis
        insertionSort(nodes, i, min(i+s*M, n), 1);
        number += min(s, (n-i+M-1)/M);          
    }

    node** newNodes = (node**) malloc(sizeof(node*)*number);            //the next level of nodes

    int nodeCount = 0;
    int loopNumber = 0;
    for (int i = 0; i < n; i += s*M) {                                 //We evaluate one vertical strip at a time
        for (int j = 0; j < min(s, (n-i+M-1)/M); j++) {                 //We will require s nodes per strip (S*M/M)
            int index = nodeCount+j;
            newNodes[index] = createNewNode();
            newNodes[index]->isLeaf = false;
            newNodes[index]->count = min(M, n-i-j*M);
            newNodes[index]->MBR[0][0] = INF;
            newNodes[index]->MBR[0][1] = -INF;
            newNodes[index]->MBR[1][0] = INF;
            newNodes[index]->MBR[1][1] = -INF;
            for (int k = 0; k < min(M, n-i-j*M); k++) {           //Each node gets M entries
                Element temp;
                temp.childPointer = nodes[loopNumber];
                nodes[loopNumber]->parent = newNodes[index];
                temp.MBR[0][0] = nodes[loopNumber]->MBR[0][0];
                temp.MBR[0][1] = nodes[loopNumber]->MBR[0][1];
                temp.MBR[1][0] = nodes[loopNumber]->MBR[1][0];
                temp.MBR[1][1] = nodes[loopNumber]->MBR[1][1];
                newNodes[index]->entries[k] = temp;
                newNodes[index]->MBR[0][0] = min(newNodes[index]->MBR[0][0], nodes[loopNumber]->MBR[0][0]);
                newNodes[index]->MBR[0][1] = max(newNodes[index]->MBR[0][1], nodes[loopNumber]->MBR[0][1]);
                newNodes[index]->MBR[1][0] = min(newNodes[index]->MBR[1][0], nodes[loopNumber]->MBR[1][0]);
                newNodes[index]->MBR[1][1] = max(newNodes[index]->MBR[1][1], nodes[loopNumber]->MBR[1][1]);
                loopNumber += 1;
            }
        }
        nodeCount += min(s, (n-i+M-1)/M);
    }

    return construct(newNodes, number);                         //recursively creating the upper nodes
       
}

node** construct_leaf_pages(Element* data, int n, int* number) {        //constructs the leaf nodes using the dataPoints
    insertionSort2(data, 0, n, 0);
    int p = (n+M-1)/M;
    int s = (int)(ceil(sqrt(p)));
    *number = 0;

    for (int i = 0; i < n; i += s*M) {
        insertionSort2(data, i, min(i+s*M, n), 1);
        *number += min(s, (n-i+M-1)/M);
    }
    
    node** newNodes = (node**) malloc(sizeof(node*)*(*number));

    int nodeCount = 0;
    int loopNumber = 0;
    for (int i = 0; i < n; i += s*M) {
        for (int j = 0; j < min(s, (n-i+M-1)/M); j++) {
            int index = nodeCount+j;
            newNodes[index] = createNewNode();
            newNodes[index]->isLeaf = true;
            newNodes[index]->count = min(M, n-i-j*M);
            newNodes[index]->MBR[0][0] = INF;
            newNodes[index]->MBR[0][1] = -INF;
            newNodes[index]->MBR[1][0] = INF;
            newNodes[index]->MBR[1][1] = -INF;
            for (int k = 0; k < min(M, n-i-j*M); k++) {
                newNodes[index]->entries[k] = data[loopNumber];
                newNodes[index]->MBR[0][0] = min(newNodes[index]->MBR[0][0], data[loopNumber].MBR[0][0]);
                newNodes[index]->MBR[0][1] = max(newNodes[index]->MBR[0][1], data[loopNumber].MBR[0][1]);
                newNodes[index]->MBR[1][0] = min(newNodes[index]->MBR[1][0], data[loopNumber].MBR[1][0]);
                newNodes[index]->MBR[1][1] = max(newNodes[index]->MBR[1][1], data[loopNumber].MBR[1][1]);
                loopNumber += 1;
            }
        }
        nodeCount += min(s, (n-i+M-1)/M);
    }

    return newNodes; 
}

void STR(Element* data, int n, rtree* a) {
    int* p = (int*) malloc(sizeof(int));
    node** nodes = construct_leaf_pages(data, n, p);
    a->root = construct(nodes, *p);
}

//----------------------------------------------------------STR ends-----------------------------------------------------//

//----------------------------------------------------------Searching begins-----------------------------------------------------//

bool overlaps(int S[N][2], int MBR[N][2]) // check if two rectangles overlap
{
    for (int i = 0; i < N; i++)
    {
        if (MBR[i][0] > S[i][1] || MBR[i][1] < S[i][0])
        {
            return false;
        }
    }
    return true;
}

bool contains(int S[N][2], int MBR[N][2]) // check if one rectangle is fully contained within another
{
    for (int i = 0; i < N; i++)
    {
        if (S[i][0] < MBR[i][0] || S[i][1] > MBR[i][1])
        {
            return false;
        }
    }
    return true;
}
// Search R-tree for all entries that overlap with a given search rectangle
void search_rtree(node *T, int S[N][2], Element ***result, int *count)
{
    // Base Case
    if (T == NULL)
    {
        return;
    }
    if (T->isLeaf)
    {
        for (int i = 0; i < T->count; i++)
        {
            //If satisfies, add the entry to result
            if (overlaps(S, T->entries[i].MBR) || contains(S, T->entries[i].MBR))
            {
                *result = (Element **)realloc(*result, (*count + 1) * sizeof(Element *));
                (*result)[*count] = &(T->entries[i]);
                (*count)++;
            }
        }
    }
    else
    {
        for (int i = 0; i < T->count; i++)
        {
            //If satisfies, recursively search its child node
            if (overlaps(S, T->entries[i].MBR))
            {
                search_rtree(T->entries[i].childPointer, S, result, count);
            }
            // if satisfies, update S to be the intersection of S and the entry's MBR, and recursively search its child node
            else if (contains(S, T->entries[i].MBR))
            {
                for (int j = 0; j < N; j++)
                {
                    S[j][0] = max(S[j][0], T->entries[i].MBR[j][0]);
                    S[j][1] = min(S[j][1], T->entries[i].MBR[j][1]);
                }
                search_rtree(T->entries[i].childPointer, S, result, count);
            }
        }
    }
}

//----------------------------------------------------------Searching ends-----------------------------------------------------//

//----------------------------------------------------------SplitNode begins-----------------------------------------------------//

int dummyCoords[N][2] = {{0, 0}, {0, 0}}; // MBR for a dummy element, used as a helper

bool isDummyElement(Element e) // checks if an element is a dummy element
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

void setAsDummyElement(Element *e) // sets an element as a dummy
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            e->MBR[i][j] = 0;
        }
    }
}

long calculateTempArea(Element e1, Element e2) // used in pickseeds
{
    return area(min(e1.MBR[0][0], e2.MBR[0][0]), max(e1.MBR[0][1], e2.MBR[0][1]), min(e1.MBR[1][0], e2.MBR[1][0]), max(e1.MBR[1][1], e2.MBR[1][1]));
}

void quadraticPickSeeds(Element elementArray[M + 1], Element *firstElement, Element *secondElement) // pick the first element of each new node in quadratic time
{
    int maxWastedArea = INT_MIN;
    int maxI = -1, maxJ = -1;
    long wastedArea = 0, tempArea = 0;
    Element e1, e2;

    for (int i = 0; i < M + 1; i++)
    {
        for (int j = i + 1; j < M + 1; j++)
        {

            e1 = elementArray[i];
            e2 = elementArray[j];

            tempArea = calculateTempArea(e1, e2);
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
    setAsDummyElement(elementArray + maxJ); // set the selected seeds as dummy elements
}

long calculateCost(Element e1, node group) // used in picknext
{
    long totalArea = area(min(e1.MBR[0][0], group.MBR[0][0]), max(e1.MBR[0][1], group.MBR[0][1]), min(e1.MBR[1][0], group.MBR[1][0]), max(e1.MBR[1][1], group.MBR[1][1]));
    long groupArea = areaOfNodeMBR(group);
    long cost = totalArea - groupArea;
    return cost;
}

Element pickNext(node group1, node group2, Element originalElements[M + 1]) // select the next element to add to a node from the remaining elements
{ // may return dummy element

    long d1, d2, difference;
    long maxDifference = INT_MIN;
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

    setAsDummyElement(originalElements + returnIndex); // set the picked element as a dummy

    return returnElement;
}

void adjustParent(node *leaf, node *n1, node *n2) // remove the original node from its parent and add the two new nodes instead
{
    if (leaf->parent)
    {

        n1->parent = leaf->parent;
        n2->parent = leaf->parent;

        for (int i = 0; i < leaf->parent->count; i++)
        {
            if (leaf->parent->entries[i].childPointer == leaf)
            {
                for (int j = i + 1; j < leaf->parent->count; j++)
                {
                    leaf->parent->entries[j - 1] = leaf->parent->entries[j];
                }
                break;
            }
        }
        leaf->parent->count -= 1;

        Element temp;
        temp.childPointer = n1;
        temp.MBR[0][0] = n1->MBR[0][0];
        temp.MBR[0][1] = n1->MBR[0][1];
        temp.MBR[1][0] = n1->MBR[1][0];
        temp.MBR[1][1] = n1->MBR[1][1];

        insertElementIntoNode(leaf->parent, temp);

        Element temp2;
        temp2.childPointer = n2;
        temp2.MBR[0][0] = n2->MBR[0][0];
        temp2.MBR[0][1] = n2->MBR[0][1];
        temp2.MBR[1][0] = n2->MBR[1][0];
        temp2.MBR[1][1] = n2->MBR[1][1];

        insertElementIntoNode(leaf->parent, temp2);

        free(leaf);
    }
}

void linearPickSeeds(Element elementArray[M + 1], Element *firstElement, Element *secondElement) // select the first element of each new node in linear time
{

    int highestLowX = INT_MIN, highestLowY = INT_MIN;
    int lowestHighX = INT_MAX, lowestHighY = INT_MAX;
    int highestLowXIndex, lowestHighXIndex, highestLowYIndex, lowestHighYIndex;

    int minX = INT_MAX, minY = INT_MAX;
    int maxX = INT_MIN, maxY = INT_MIN;

    for (int i = 0; i < M + 1; i++)
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
        if (high_x > maxX)
        {
            maxX = high_x;
        }
        if (low_y < minY)
        {
            minY = low_y;
        }
        if (high_y > maxY)
        {
            maxY = high_y;
        }
    }

    for (int i = 0; i < M + 1; i++)
    {
        Element e = elementArray[i];

        int low_x = e.MBR[0][0];
        int high_x = e.MBR[0][1];
        int low_y = e.MBR[1][0];
        int high_y = e.MBR[1][1];

        if (low_x > highestLowX)
        {
            highestLowX = low_x;
            highestLowXIndex = i;
        } 
    }

    for (int i = 0; i < M + 1; i++)
    {
        if (i == highestLowXIndex) continue;

        Element e = elementArray[i];

        int low_x = e.MBR[0][0];
        int high_x = e.MBR[0][1];
        int low_y = e.MBR[1][0];
        int high_y = e.MBR[1][1];

        if (high_x < lowestHighX)
        {
            lowestHighX = high_x;
            lowestHighXIndex = i;
        }
    }

    for (int i = 0; i < M + 1; i++)
    {
        Element e = elementArray[i];

        int low_x = e.MBR[0][0];
        int high_x = e.MBR[0][1];
        int low_y = e.MBR[1][0];
        int high_y = e.MBR[1][1];

        if (low_y > highestLowY)
        {
            highestLowY = low_y;
            highestLowYIndex = i;
        }
    }

    for (int i = 0; i < M + 1; i++)
    {
        if (i == highestLowYIndex) continue;

        Element e = elementArray[i];

        int low_x = e.MBR[0][0];
        int high_x = e.MBR[0][1];
        int low_y = e.MBR[1][0];
        int high_y = e.MBR[1][1];

        if (high_y < lowestHighY)
        {
            lowestHighY = high_y;
            lowestHighYIndex = i;
        }
    }
        
    int widthX = maxX - minX;
    int widthY = maxY - minY;
    int normalizedSeparationX, normalizedSeparationY;

    if (widthY > 0) {
        normalizedSeparationY = (highestLowY - lowestHighY) / widthY;
    } else {
        normalizedSeparationY = INT_MIN;
    }


    if (widthX > 0) {
        normalizedSeparationX = (highestLowX - lowestHighX) / widthX;
    } else {
        normalizedSeparationX = INT_MIN;
    }

    // After finding the normalized separation along both dimensions, we select the ones with greater separation

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

void updateChildPointer(node *currentNode, Element *e) // set the parent of the element's child as the current node
{
    if (e->childPointer)
    {
        e->childPointer->parent = currentNode;
    }
}

void splitNode(node *originalNode, node *newNode1, node *newNode2, bool isLinear) // takes in an original node and splits it into two new nodes
{

    newNode1->isLeaf = originalNode->isLeaf;
    newNode2->isLeaf = originalNode->isLeaf;

    Element firstElementOfFirstNewNode;
    Element firstElementOfSecondNewNode;

  if (isLinear) // use linearPickSeeds if isLinear is true, else use quadraticPickSeeds
        linearPickSeeds(originalNode->entries, &firstElementOfFirstNewNode, &firstElementOfSecondNewNode);
    else
        quadraticPickSeeds(originalNode->entries, &firstElementOfFirstNewNode, &firstElementOfSecondNewNode);

    updateChildPointer(newNode1, &firstElementOfFirstNewNode);
    updateChildPointer(newNode2, &firstElementOfSecondNewNode);

    insertElementIntoNode(newNode1, firstElementOfFirstNewNode);
    insertElementIntoNode(newNode2, firstElementOfSecondNewNode);

    int remainingCount = M - 1; // after removing 2 elements from elements array using PickSeeds

    while (remainingCount > 0)
    {

        if (newNode1->count + remainingCount == m) // if there are too few entries in firstNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (isDummyElement(originalNode->entries[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    updateChildPointer(newNode1, originalNode->entries + index);
                    insertElementIntoNode(newNode1, originalNode->entries[index]);
                    remainingCount--;
                }
            }

            break;
        }

        else if (newNode2->count + remainingCount == m) // if there are too few entries in secondNewNode
        {

            int index = 0;

            while (remainingCount > 0)
            {

                if (isDummyElement(originalNode->entries[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    updateChildPointer(newNode2, originalNode->entries + index);
                    insertElementIntoNode(newNode2, originalNode->entries[index]);
                    remainingCount--;
                }
            }

            break;
        }

        Element nextElement = pickNext(*newNode1, *newNode2, originalNode->entries); // pick the next element and add it to a node

        if (isDummyElement(nextElement))
            break; // entries are finished

        long cost1 = calculateCost(nextElement, *newNode1);
        long cost2 = calculateCost(nextElement, *newNode2);

        if (cost1 < cost2) // add to the one with lower cost
        {
            updateChildPointer(newNode1, &nextElement);
            insertElementIntoNode(newNode1, nextElement);
        }
        else if (cost2 < cost1)
        {
            updateChildPointer(newNode2, &nextElement);
            insertElementIntoNode(newNode2, nextElement);
        }
        else
        {
            long area1 = areaOfNodeMBR(*newNode1);
            long area2 = areaOfNodeMBR(*newNode2);

            if (area1 < area2) // if cost is equal, add to the one with smaller area
            {
                updateChildPointer(newNode1, &nextElement);
                insertElementIntoNode(newNode1, nextElement);
            }
            else if (area1 > area2)
            {
                updateChildPointer(newNode2, &nextElement);
                insertElementIntoNode(newNode2, nextElement);
            }
            else
            {
                if (newNode1->count < newNode2->count) // if areas are also equal, add to the one with smaller count

                {
                    updateChildPointer(newNode1, &nextElement);
                    insertElementIntoNode(newNode1, nextElement);
                }
                else
                {
                    updateChildPointer(newNode2, &nextElement);
                    insertElementIntoNode(newNode2, nextElement);
                }
            }
        }   

        remainingCount--;
    }

    adjustParent(originalNode, newNode1, newNode2);
}

//----------------------------------------------------------SplitNode ends-----------------------------------------------------//

//----------------------------------------------------------Insert begins-----------------------------------------------------//

void mergeMBR(node *n1, Element ele) //updates the MBR of the node n1 to include the MBR of ele
{
    for (int i = 0; i < N; ++i)
    {
        n1->MBR[i][0] = min(n1->MBR[i][0], ele.MBR[i][0]);
        n1->MBR[i][1] = max(n1->MBR[i][1], ele.MBR[i][1]);
    }
}

long calcEnlargement(int r1[N][2], int r2[N][2]) //calculate the enlargement needed to fit both MBRs in one MBR
{
    int newRect[N][2];

    // Calculate the MBR of the two rectangles
    for (int i = 0; i < N; i++)
    {
        newRect[i][0] = (r1[i][0] < r2[i][0]) ? r1[i][0] : r2[i][0];
        newRect[i][1] = (r1[i][1] > r2[i][1]) ? r1[i][1] : r2[i][1];
    }

    // Calculate the area of the new rectangle
    long newArea = (newRect[0][1] - newRect[0][0]) * (newRect[1][1] - newRect[1][0]);

    // Calculate the area of the two original rectangles
    long area1 = (r1[0][1] - r1[0][0]) * (r1[1][1] - r1[1][0]);
    long area2 = (r2[0][1] - r2[0][0]) * (r2[1][1] - r2[1][0]);

    // Calculate the enlargement as the difference between the area of the new rectangle and the sum of the areas of the two original rectangles
    long enlargement = newArea - area1 - area2;

    return enlargement;
}


node *choose_leaf(rtree *tr, Element ele) //chooses a leaf node using a greedy approach where we would insert the element
{
    node *n = tr->root;

    while (!n->isLeaf)
    {
        long min_enlargement = INT_MAX * 1e4;
        int min_index = -1;
        long min_area = INT_MAX * 1e4;

        for (int i = 0; i < n->count; i++)
        {
            long area = areaOfElementMBR(n->entries[i]);
            long enlargement = calcEnlargement(n->entries[i].MBR, ele.MBR);

            if (enlargement < min_enlargement)
            {
                min_enlargement = enlargement;
                min_index = i;
                min_area = area;
            }

            if (enlargement == min_enlargement && area < min_area)
            {
                min_index = i;
                min_area = area;
            }
        }

        n = n->entries[min_index].childPointer;
    }
    return n;
}

void adjust_tree(rtree *tree, node *n, node *child) //adjusts the tree by accounting for the update in MBRs of all the
                                                    //nodes from leaf to parents and performs splitNode wherever there is an overflow on the way up
{
    if (n->parent == NULL){
        for (int i = 0; i < n->count; i++)
        {
            mergeMBR(n, n->entries[i]);
        }
        if (n->count <= M)
            return;
        node *n1 = createNewNode();
        node *n2 = createNewNode();

        splitNode(n, n1, n2, true);

        node *root = createNewNode();
        root->isLeaf = false;
        n1->parent = n2->parent = root;
        Element temp1, temp2;
        temp1.childPointer = n1;
        temp1.MBR[0][0] = n1->MBR[0][0];
        temp1.MBR[0][1] = n1->MBR[0][1];
        temp1.MBR[1][0] = n1->MBR[1][0];
        temp1.MBR[1][1] = n1->MBR[1][1];

        temp2.childPointer = n2;
        temp2.MBR[0][0] = n2->MBR[0][0];
        temp2.MBR[0][1] = n2->MBR[0][1];
        temp2.MBR[1][0] = n2->MBR[1][0];
        temp2.MBR[1][1] = n2->MBR[1][1];

        root->entries[0] = temp1;
        root->entries[1] = temp2;

        root->MBR[0][0] = min(temp1.MBR[0][0], temp2.MBR[0][0]);
        root->MBR[0][1] = max(temp1.MBR[0][1], temp2.MBR[0][1]);
        root->MBR[1][0] = min(temp1.MBR[1][0], temp2.MBR[1][0]);
        root->MBR[1][1] = max(temp1.MBR[1][1], temp2.MBR[1][1]);

        tree->root = root;
        root->count = 2;

        return;
    }

    for (int i = 0; i < n->count; i++)
    {
        mergeMBR(n, n->entries[i]);
    }

    if (n->count > M)
    {
        node *n1, *n2;
        n1 = createNewNode();
        n2 = createNewNode();

        splitNode(n, n1, n2, true);
        n1->parent = n->parent;
        n2->parent = n->parent;
        adjust_tree(tree, n->parent, n1);
    }
    else
    {
        adjust_tree(tree, n->parent, n);
    }
}

void insert(rtree *tr, Element ele)
{
    node *leaf = choose_leaf(tr, ele);

    if (leaf->count <= M)
    {
        ele.childPointer = NULL;
        insertElementIntoNode(leaf, ele);

        adjust_tree(tr, leaf, NULL);
        return;
    }
    return;
}

//----------------------------------------------------------Insert ends-----------------------------------------------------//

//----------------------------------------------------------Preorder Traversal begins-----------------------------------------------------//

void print_internal_node(node *n) //prints the MBR bounds for an internal node
{
    printf("Internal Node: MBR spans from ");
    printf("(%d, %d) to (%d, %d)", n->MBR[0][0], n->MBR[1][0], n->MBR[0][1], n->MBR[1][1]);
    printf("\n\n");
}

void print_leaf_node(node *n) //prints the entries of a leaf node, separated by "|"
{
    printf("Leaf Node: contains %d %s {", n->count, (n->count > 1) ? "entries" : "entry");
    int i;
    for (i = 0; i < n->count - 1; i++)
        printf("(%d, %d) | ", n->entries[i].MBR[0][0], n->entries[i].MBR[1][0]);
    printf("(%d, %d)", n->entries[i].MBR[0][0], n->entries[i].MBR[1][0]);

    printf("}\n\n");
}

void preorder_traversal(node* n) //preorder traversal: visit node, then preorder(all children)
{
    if (n == NULL) 
        return;
    
    if (n->isLeaf) 
    {
        print_leaf_node(n);
    } 
    else 
    {
        print_internal_node(n);
        for (int i = 0; i < n->count; i++)
            preorder_traversal(n->entries[i].childPointer);  
    }
}

//----------------------------------------------------------Preorder Traversal ends-----------------------------------------------------//

//----------------------------------------------------------main begins-----------------------------------------------------//

int main()
{
    rtree* a = createNewRtree();
    node* root = createNewNode();
    a->root = root;

    int numDataPoints = 1e5; //number of 2D data points in data.txt
    FILE* fptr = fopen("data.txt", "r");
    Element* dataPoints = (Element *) malloc(numDataPoints * sizeof(Element));
    
    for (int i = 0; i < numDataPoints; i++) {
        int a, b;
        fscanf(fptr, "%d", &a);
        fscanf(fptr, "%d", &b);
        dataPoints[i].MBR[0][0] = a;
        dataPoints[i].MBR[0][1] = a;
        dataPoints[i].MBR[1][0] = b;
        dataPoints[i].MBR[1][1] = b;
        dataPoints[i].childPointer = NULL;
    }

    STR(dataPoints, numDataPoints, a);

    printf("\nInserted data points, tree created. Now printing pre-order traversal:\n\n");

    preorder_traversal(a->root);

    int searchSpace[N][2] = {{3, 10}, {2, 8}}; //sample search rectangle
    /*
        Convention followed for search rectangle.
        (x1, y1) is bottom/left, (x2,y2) is top/right.

        0, 0 = x1
        0, 1 = x2
        1, 0 = y1
        1, 1 = y2
    */

    printf("\nConducting a sample search on the rectangle (%d,%d) to (%d,%d).",searchSpace[0][0],searchSpace[1][0],searchSpace[0][1],searchSpace[1][1]);

    Element **result = NULL;
    int resultCount = 0;

    printf("Searching: \n");

    search_rtree(a->root, searchSpace, &result, &resultCount);

    printf("Search Results:\n");
    if (resultCount > 0)
    {
        printf("Results Found. \n");
        for (int i = 0; i < resultCount; i++)
        {
            
            printf("(%d, %d, %d, %d)\n", result[i]->MBR[0][0], result[i]->MBR[0][1], result[i]->MBR[1][0], result[i]->MBR[1][1]);
        }
    }
    else
    {
        printf("No results found.\n");
    }


    int sampleX = 3, sampleY = 14; //sample point to be inserted
    printf("\nInserting a sample point (%d,%d) in the tree.\n", sampleX, sampleY);

    
    Element samplePoint;
    samplePoint.MBR[0][0] = sampleX;
    samplePoint.MBR[0][1] = sampleX;
    samplePoint.MBR[1][0] = sampleY;
    samplePoint.MBR[1][1] = sampleY;
    samplePoint.childPointer = NULL;

    insert(a, samplePoint);

    printf("\nSample point inserted. Now printing pre-order traversal:\n\n");

    preorder_traversal(a->root);

    return 0;
}

//----------------------------------------------------------main ends-----------------------------------------------------//
