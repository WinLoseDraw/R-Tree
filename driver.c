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

void bfs(node* a) {
    node* arr[50];
    int index = 0;
    int last = 1;
    arr[0] = a;
    while (arr[index]) {
        printf("MBR: \n");
        printf("%d %d %d %d\n", arr[index]->MBR[0][0], arr[index]->MBR[0][1], arr[index]->MBR[1][0], arr[index]->MBR[1][1]);
        printf("count: %d\n", arr[index]->count);

        for (int i = 0; i < arr[index]->count; i++) {
            printf("%d %d %d %d\n", arr[index]->entries[i].MBR[0][0], arr[index]->entries[i].MBR[0][1], arr[index]->entries[i].MBR[1][0], arr[index]->entries[i].MBR[1][1]);
            arr[last] = arr[index]->entries[i].childPointer;
            last += 1;
            printf("\n");
        }

        index += 1;
    }    
}

void insertTest(){
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

    for (int i = 0; i < n; i++) {
        printf("i: %d\n", i);
        insert(a, dataPoints[i]);
        bfs(a->root);
    }

}

int main()
{
    splitNodeTest();
    //insertTest();
    return 0;
}