#include <stdio.h>
#include "rtree.h"
#include "helper_functions.h"
#include "splitnode.h"
#include "insert.h"

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

void bfs2(node* a, int count) {
    node* arr[2][100];
    arr[0][0] = a;
    int index = 0;
    int prevLast = 1;
    int last = 0;
    int cur = 0;
    printf("BFS: \n");
    while (prevLast < count) {
        printf("%d %d %d %d\n", arr[cur][index]->MBR[0][0], arr[cur][index]->MBR[0][1], arr[cur][index]->MBR[1][0], arr[cur][index]->MBR[1][1]);
        for (int i = 0; i < arr[cur][index]->count; i++) {
            arr[(cur+1)%2][last] = arr[cur][index]->entries[i].childPointer;
            last += 1;
        }
        index += 1;
        if (index == prevLast) {
            prevLast = last;
            last = 0;
            cur = (cur+1)%2;
            printf("\n");
            index = 0;
        }
    }

    cur = (cur+1)%2;
}

void insertTest(){
    printf("Hello");
    rtree* a = createNewRtree();
    node* root = createNewNode();
    a->root = root;
    int n = 21;
    FILE* fptr = fopen("data.txt", "r");
    Element dataPoints[n];
    for (int i = 0; i < n; i++) {
        int a, b;
        fscanf(fptr, "%d", &a);
        fscanf(fptr, "%d", &b);
        dataPoints[i].MBR[0][0] = a;
        dataPoints[i].MBR[0][1] = a;
        dataPoints[i].MBR[1][0] = b;
        dataPoints[i].MBR[1][1] = b;
        dataPoints[i].childPointer = NULL;
    }

    printf("Hello\n");
    for (int i = 0; i < n; i++) {
        printf("insert1\n");
        insert(a, dataPoints[i]);
        printf("bfs\n");
        bfs2(a->root, i);
    }

    printf("Hello");

    bfs2(a->root, n);

    
}

int main()
{
    //splitNodeTest();
    insertTest();
    return 0;
}