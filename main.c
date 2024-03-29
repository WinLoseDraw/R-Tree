#include <stdio.h>
#include <math.h>
#include "rtree.h"
#include "helper_functions.h"
#include "splitnode.h"
#include "search.h"
#include "preorder.h"
#include "insert.h"

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
    // printf("Inserting element into node %d %d %d %d with count %d\n", currentNode->MBR[0][0], currentNode->MBR[0][1], currentNode->MBR[1][0], currentNode->MBR[1][1], currentNode->count);
    currentNode->entries[currentNode->count++] = e;
    updateMBRAfterInsert(currentNode, e);
    if (currentNode->parent)
    {
        // printf("Parent has MBR %d %d %d %d\n", currentNode->parent->MBR[0][0], currentNode->parent->MBR[0][1], currentNode->parent->MBR[1][0], currentNode->parent->MBR[1][1]);
        for (int i = 0; i < currentNode->parent->count; i++)
        {
            // printf("Finding insert match %d\n", i);
            if (currentNode->parent->entries[i].childPointer == currentNode)
            {
                // printf("Insert Match found at i = %d\n", i);
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

void adjustParent(node *leaf, node *n1, node *n2)
{
    if (leaf->parent)
    {

        n1->parent = leaf->parent;
        n2->parent = leaf->parent;

        // printf("Adjusting Parent\n");
        for (int i = 0; i < leaf->parent->count; i++)
        {
            if (leaf->parent->entries[i].childPointer == leaf)
            {
                // printf("Match found at i = %d\n", i);
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

void linearPickSeeds(Element elementArray[M + 1], Element *firstElement, Element *secondElement)
{
    int highestLowX, highestLowY = INT_MIN;
    int lowestHighX, lowestHighY = INT_MAX;
    int highestLowXIndex, lowestHighXIndex, highestLowYIndex, lowestHighYIndex;

    int minX, minY = INT_MAX;
    int maxX, maxY = INT_MIN;

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

void updateChildPointer(node *currentNode, Element *e)
{
    if (e->childPointer)
    {
        e->childPointer->parent = currentNode;
    }
}

void splitNode(node *originalNode, node *newNode1, node *newNode2, bool isLinear)
{
    // printf("start split\n");
    // printf("MBR: %d %d %d %d\n", originalNode->MBR[0][0], originalNode->MBR[0][1], originalNode->MBR[1][0], originalNode->MBR[1][1]);

    node firstNewNode;
    node secondNewNode;

    initializeNode(&firstNewNode);
    initializeNode(&secondNewNode);

    firstNewNode.isLeaf = originalNode->isLeaf;
    secondNewNode.isLeaf = originalNode->isLeaf;
    // firstNewNode.parent = originalNode.parent;
    // secondNewNode.parent = originalNode.parent;

    Element firstElementOfFirstNewNode;
    Element firstElementOfSecondNewNode;

    if (isLinear)
        linearPickSeeds(originalNode->entries, &firstElementOfFirstNewNode, &firstElementOfSecondNewNode);
    else
        quadraticPickSeeds(originalNode->entries, &firstElementOfFirstNewNode, &firstElementOfSecondNewNode);

    updateChildPointer(newNode1, &firstElementOfFirstNewNode);
    updateChildPointer(newNode2, &firstElementOfSecondNewNode);

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

                if (isDummyElement(originalNode->entries[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    updateChildPointer(newNode1, originalNode->entries + index);
                    insertElementIntoNode(&firstNewNode, originalNode->entries[index]);
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

                if (isDummyElement(originalNode->entries[index]))
                {
                    index++;
                    continue;
                }
                else
                {
                    updateChildPointer(newNode2, originalNode->entries + index);
                    insertElementIntoNode(&secondNewNode, originalNode->entries[index]);
                    remainingCount--;
                }
            }

            break;
        }

        Element nextElement = pickNext(firstNewNode, secondNewNode, originalNode->entries);

        if (isDummyElement(nextElement))
            break; // entries are finished

        int cost1 = calculateCost(nextElement, firstNewNode);
        int cost2 = calculateCost(nextElement, secondNewNode);

        if (cost1 < cost2)
        {
            updateChildPointer(newNode1, &nextElement);
            insertElementIntoNode(&firstNewNode, nextElement);
        }
        else if (cost2 < cost1)
        {
            updateChildPointer(newNode2, &nextElement);
            insertElementIntoNode(&secondNewNode, nextElement);
        }
        else
        {
            int area1 = areaOfNodeMBR(firstNewNode);
            int area2 = areaOfNodeMBR(secondNewNode);

            if (area1 < area2)
            {
                updateChildPointer(newNode1, &nextElement);
                insertElementIntoNode(&firstNewNode, nextElement);
            }
            else if (area1 > area2)
            {
                updateChildPointer(newNode2, &nextElement);
                insertElementIntoNode(&secondNewNode, nextElement);
            }
            else
            {
                if (firstNewNode.count < secondNewNode.count)

                {
                    updateChildPointer(newNode1, &nextElement);
                    insertElementIntoNode(&firstNewNode, nextElement);
                }
                else
                {
                    updateChildPointer(newNode2, &nextElement);
                    insertElementIntoNode(&secondNewNode, nextElement);
                }
            }
        }

        remainingCount--;
    }
    // printf("end split\n");
    *newNode1 = firstNewNode;
    *newNode2 = secondNewNode;

    adjustParent(originalNode, newNode1, newNode2);
}

void search_rtree(node *T, int S[N][2], Element ***result, int *count)
{
    if (T == NULL)
    {
        return;
    }
    if (T->isLeaf)
    {
        for (int i = 0; i < T->count; i++)
        {
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
            if (overlaps(S, T->entries[i].MBR))
            {
                search_rtree(T->entries[i].childPointer, S, result, count);
            }
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
// check if two rectangles overlap
bool overlaps(int S[N][2], int MBR[N][2])
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

// check if one rectangle is fully contained within another
bool contains(int S[N][2], int MBR[N][2])
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

void print_internal_node(node *n)
{
    printf("Internal Node: MBR spans from ");
    printf("(%d, %d) to (%d, %d)", n->MBR[0][0], n->MBR[1][0], n->MBR[0][1], n->MBR[1][1]);
    printf("\n\n");
}

void print_leaf_node(node *n)
{
    printf("Leaf Node: contains %d %s {", n->count, (n->count > 1) ? "entries" : "entry");
    int i;
    for (i = 0; i < n->count - 1; i++)
        printf("(%d, %d) | ", n->entries[i].MBR[0][0], n->entries[i].MBR[1][0]);
    printf("(%d, %d)", n->entries[i].MBR[0][0], n->entries[i].MBR[1][0]);

    printf("}\n\n");
}

void preorder_traversal(node *n)
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

void mergeMBR(node *n1, Element ele)
{
    for (int i = 0; i < N; ++i)
    {
        n1->MBR[i][0] = min(n1->MBR[i][0], ele.MBR[i][0]);
        n1->MBR[i][1] = max(n1->MBR[i][1], ele.MBR[i][1]);
    }
}

int calcEnlargement(int r1[N][2], int r2[N][2])
{
    int newRect[N][2];

    // Calculate the MBR of the two rectangles
    for (int i = 0; i < N; i++)
    {
        newRect[i][0] = (r1[i][0] < r2[i][0]) ? r1[i][0] : r2[i][0];
        newRect[i][1] = (r1[i][1] > r2[i][1]) ? r1[i][1] : r2[i][1];
    }

    // Calculate the area of the new rectangle
    int newArea = (newRect[0][1] - newRect[0][0]) * (newRect[1][1] - newRect[1][0]);

    // Calculate the area of the two original rectangles
    int area1 = (r1[0][1] - r1[0][0]) * (r1[1][1] - r1[1][0]);
    int area2 = (r2[0][1] - r2[0][0]) * (r2[1][1] - r2[1][0]);

    // Calculate the enlargement as the difference between the area of the new rectangle and the sum of the areas of the two original rectangles
    int enlargement = newArea - area1 - area2;

    return enlargement;
}

void insert(rtree *tr, Element ele)
{
    // printf("Hello\n");
    node *leaf = choose_leaf(tr, ele);
    // printf("Chosen leaf is %d %d %d %d\n", leaf->MBR[0][0], leaf->MBR[0][1], leaf->MBR[1][0], leaf->MBR[1][1]);
    // printf("Hello2\n");

    if (leaf->count <= M)
    {
        ele.childPointer = NULL;
        insertElementIntoNode(leaf, ele);

        adjust_tree(tr, leaf, NULL);
        return;
    }

    // node *n1 = createNewNode();
    // node *n2 = createNewNode();
    // n1->isLeaf = n2->isLeaf = true;

    // insertElementIntoNode(leaf, ele);
    // splitNode(*leaf, n1, n2, true);

    // adjust_tree(tr, n1, NULL);
    // adjust_tree(tr, n2, NULL);

    return;
}

node *choose_leaf(rtree *tr, Element ele)
{
    node *n = tr->root;

    while (!n->isLeaf)
    {
        int min_enlargement = INT_MAX;
        int min_index = -1;
        int min_area = INT_MAX;

        for (int i = 0; i < n->count; i++)
        {
            int area = areaOfElementMBR(n->entries[i]);
            int enlargement = calcEnlargement(n->entries[i].MBR, ele.MBR);

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

void adjust_tree(rtree *tree, node *n, node *child)
{
    if (n->parent == NULL)
    {
        // printf("adjust count: %d\n", n->count);
        // printf("MBR: %d %d %d %d\n", n->MBR[0][0], n->MBR[0][1], n->MBR[1][0], n->MBR[1][1]);
        for (int i = 0; i < n->count; i++)
        {
            mergeMBR(n, n->entries[i]);
        }
        if (n->count <= M)
            return;
        node *n1 = createNewNode();
        node *n2 = createNewNode();
        // printf("Split the node\n");
        splitNode(n, n1, n2, false);

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

        splitNode(n, n1, n2, false);
        n1->parent = n->parent;
        n2->parent = n->parent;
        //++n->parent->count;

        adjust_tree(tree, n->parent, n1);
        // adjust_tree(tree, n->parent, n2);
    }
    else
    {
        adjust_tree(tree, n->parent, n);
    }
}

int INF = 1e9;

double center(int arr[2])
{
    return ((double)arr[0] + arr[1]) / 2;
}

void insertionSort(node **rectangles, int start, int end, int dimension)
{
    for (int i = start + 1; i < end; i++)
    {
        node *key = rectangles[i];
        int j = i - 1;
        while ((j >= start) && (center(key->MBR[dimension]) < center(rectangles[j]->MBR[dimension])))
        {
            rectangles[j + 1] = rectangles[j];
            j--;
        }
        j++;
        rectangles[j] = key;
    }
}

void insertionSort2(Element *data, int start, int end, int dimension)
{
    for (int i = start; i < end; i++)
    {
        Element key = data[i];
        int j = i - 1;
        while ((j >= start) && (center(key.MBR[dimension]) < center(data[j].MBR[dimension])))
        {
            data[j + 1] = data[j];
            j--;
        }
        j++;
        data[j] = key;
    }
}

// int min(int a, int b) {
//     if (a < b) {
//         return a;
//     }

//     return b;
// }

// int max(int a, int b) {
//     if (a > b) {
//         return a;
//     }

//     return b;
// }

node *construct(node **nodes, int n)
{
    if (n == 1)
    {
        return nodes[0];
    }
    node *nodetemp[n];
    for (int i = 0; i < n; i++)
    {
        nodetemp[i] = nodes[i];
    }
    insertionSort(nodes, 0, n, 0);

    for (int i = 0; i < n; i++)
    {
        nodetemp[i] = nodes[i];
    }

    int p = (n + M - 1) / M;
    int s = (int)(ceil(sqrt(p)));
    int number = 0;

    for (int i = 0; i < n; i += s * M)
    {
        insertionSort(nodes, i, min(i + s * M, n), 1);
        for (int j = 0; j < n; j++)
        {
            nodetemp[j] = nodes[j];
        }
        number += min(s, (n - i + M - 1) / M);
    }

    node **newNodes = (node **)malloc(sizeof(node *) * number);

    int nodeCount = 0;
    int loopNumber = 0;
    for (int i = 0; i < n; i += s * M)
    {
        for (int j = 0; j < min(s, (n - i + M - 1) / M); j++)
        {
            int index = nodeCount + j;
            newNodes[index] = createNewNode();
            newNodes[index]->isLeaf = false;
            newNodes[index]->count = min(min(M, s * M - j * M), n - i - j * M);
            newNodes[index]->MBR[0][0] = INF;
            newNodes[index]->MBR[0][1] = -INF;
            newNodes[index]->MBR[1][0] = INF;
            newNodes[index]->MBR[1][1] = -INF;
            for (int k = 0; k < min(min(M, s * M - j * M), n - i - j * M); k++)
            {
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
        nodeCount += min(s, (n - i + M - 1) / M);
    }

    for (int i = 0; i < number; i++)
    {
        nodetemp[i] = nodes[i];
    }

    node *newNodeTemp[number];
    for (int i = 0; i < number; i++)
    {
        newNodeTemp[i] = newNodes[i];
    }

    return construct(newNodes, number);
}

node **construct_leaf_pages(Element *data, int n, int *number)
{
    insertionSort2(data, 0, n, 0);
    int p = (n + M - 1) / M;
    int s = (int)(ceil(sqrt(p)));
    *number = 0;

    for (int i = 0; i < n; i += s * M)
    {
        insertionSort2(data, i, min(i + s * M, n), 1);
        *number += min(s, (n - i + M - 1) / M);
    }

    node **newNodes = (node **)malloc(sizeof(node *) * (*number));

    int nodeCount = 0;
    int loopNumber = 0;
    for (int i = 0; i < n; i += s * M)
    {
        for (int j = 0; j < min(s, (n - i + M - 1) / M); j++)
        {
            int index = nodeCount + j;
            newNodes[index] = createNewNode();
            newNodes[index]->isLeaf = true;
            newNodes[index]->count = min(min(M, s * M - j * M), n - i - j * M);
            newNodes[index]->MBR[0][0] = INF;
            newNodes[index]->MBR[0][1] = -INF;
            newNodes[index]->MBR[1][0] = INF;
            newNodes[index]->MBR[1][1] = -INF;
            for (int k = 0; k < min(min(M, s * M - j * M), n - i - j * M); k++)
            {
                newNodes[index]->entries[k] = data[loopNumber];
                newNodes[index]->MBR[0][0] = min(newNodes[index]->MBR[0][0], data[loopNumber].MBR[0][0]);
                newNodes[index]->MBR[0][1] = max(newNodes[index]->MBR[0][1], data[loopNumber].MBR[0][1]);
                newNodes[index]->MBR[1][0] = min(newNodes[index]->MBR[1][0], data[loopNumber].MBR[1][0]);
                newNodes[index]->MBR[1][1] = max(newNodes[index]->MBR[1][1], data[loopNumber].MBR[1][1]);
                loopNumber += 1;
            }
        }
        nodeCount += min(s, (n - i + M - 1) / M);
    }

    return newNodes;
}

void STR(Element *data, int n, rtree *a)
{
    int *p = (int *)malloc(sizeof(int));
    node **nodes = construct_leaf_pages(data, n, p);

    printf("Leaf nodes\n");
    node *nodetemp[(*p)];
    for (int i = 0; i < *p; i++)
    {
        nodetemp[i] = nodes[i];
        for (int j = 0; j < nodes[i]->count; j++)
        {
            printf("%d %d %d %d\n", nodes[i]->entries[j].MBR[0][0], nodes[i]->entries[j].MBR[0][1], nodes[i]->entries[j].MBR[1][0], nodes[i]->entries[j].MBR[1][1]);
        }
        printf("\n");
    }
    a->root = construct(nodes, *p);
}

void preOrder(node *a)
{
    if (!a)
    {
        return;
    }

    printf("%d %d %d %d\n", a->MBR[0][0], a->MBR[0][1], a->MBR[1][0], a->MBR[1][1]);
    for (int i = 0; i < a->count; i++)
    {
        preOrder(a->entries[i].childPointer);
    }
}

// void bfs(node* a) {
//     node* arr[50];
//     int index = 0;
//     int last = 1;
//     arr[0] = a;
//     while (arr[index]) {
//         printf("%d %d %d %d\n", arr[index]->MBR[0][0], arr[index]->MBR[0][1], arr[index]->MBR[1][0], arr[index]->MBR[1][1]);
//         for (int i = 0; i < arr[index]->count; i++) {
//             arr[last] = arr[index]->entries[i].childPointer;
//             last += 1;
//         }
//         index += 1;
//     }
// }

// int main() {
//     int n = 21;
//     FILE* fptr = fopen("data.txt", "r");
//     Element dataPoints[n];
//     for (int i = 0; i < n; i++) {
//         int a, b;
//         fscanf(fptr, "%d", &a);
//         fscanf(fptr, "%d", &b);
//         dataPoints[i].MBR[0][0] = a;
//         dataPoints[i].MBR[0][1] = a;
//         dataPoints[i].MBR[1][0] = b;
//         dataPoints[i].MBR[1][1] = b;
//         dataPoints[i].childPointer = NULL;
//     }

//     // FILE* fptr = fopen("data.txt", "r");
//     // Element dataPoints[n];
//     // for (int i = 0; i < n; i++) {
//     //     int a, b;
//     //     scanf("%d", &a);
//     //     scanf("%d", &b);
//     //     dataPoints[i].MBR[0][0] = a;
//     //     dataPoints[i].MBR[0][1] = a;
//     //     dataPoints[i].MBR[1][0] = b;
//     //     dataPoints[i].MBR[1][1] = b;
//     //     dataPoints[i].childPointer = NULL;
//     // }

//     rtree* a = createNewRtree();

//     STR(dataPoints, n, a);

//     printf("%d\n", a->root->MBR[1][1]);
//     for (int i = 0; i < a->root->count; i++) {
//         printf("%d ", a->root->entries[i].MBR[1][1]);
//     }
//     printf("\n");

//     bfs(a->root);

// }
int main()
{
    printf("Hi \n");
}