#include "rtree.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "helper_functions.h"

int INF = 1e9;

double center(int arr[2]) {
    return ((double)arr[0] + arr[1])/2;
}

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

int min(int a, int b) {
    if (a < b) {
        return a;
    }

    return b;
}

int max(int a, int b) {
    if (a > b) {
        return a;
    }

    return b;
}

node* construct(node** nodes, int n) {
    if (n == 1) {
        return nodes[0];
    }
    node* nodetemp[n];
    for (int i = 0; i < n; i++) {
        nodetemp[i] = nodes[i];
    }
    insertionSort(nodes, 0, n, 0);

    for (int i = 0; i < n; i++) {
        nodetemp[i] = nodes[i];
    }

    int p = (n+M-1)/M;
    int s = (int)(ceil(sqrt(p)));
    int number = 0;

    for (int i = 0; i < n; i += s*M) {
        insertionSort(nodes, i, min(i+s*M, n), 1);
        for (int j = 0; j < n; j++) {
            nodetemp[j] = nodes[j];
        }
        number += min(s, (n-i+M-1)/M);
    }

    node** newNodes = (node**) malloc(sizeof(node*)*number);

    int nodeCount = 0;
    int loopNumber = 0;
    for (int i = 0; i < n; i += s*M) {
        for (int j = 0; j < min(s, (n-i+M-1)/M); j++) {
            int index = nodeCount+j;
            newNodes[index] = createNewNode();
            newNodes[index]->isLeaf = false;
            newNodes[index]->count = min(min(M, s*M-j*M), n-i-j*M);
            newNodes[index]->MBR[0][0] = INF;
            newNodes[index]->MBR[0][1] = -INF;
            newNodes[index]->MBR[1][0] = INF;
            newNodes[index]->MBR[1][1] = -INF;
            for (int k = 0; k < min(min(M, s*M-j*M), n-i-j*M); k++) {
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

    for (int i = 0; i < number; i++) {
        nodetemp[i] = nodes[i];
    }

    node* newNodeTemp[number];
    for (int i = 0; i < number; i++) {
        newNodeTemp[i] = newNodes[i];
    }

    return construct(newNodes, number);
       
}

node** construct_leaf_pages(Element* data, int n, int* number) {
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
            newNodes[index]->count = min(min(M, s*M-j*M), n-i-j*M);
            newNodes[index]->MBR[0][0] = INF;
            newNodes[index]->MBR[0][1] = -INF;
            newNodes[index]->MBR[1][0] = INF;
            newNodes[index]->MBR[1][1] = -INF;
            for (int k = 0; k < min(min(M, s*M-j*M), n-i-j*M); k++) {
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

    printf("Leaf nodes\n");
    node* nodetemp[(*p)];
    for (int i = 0; i < *p; i++) {
        nodetemp[i] = nodes[i];
        for (int j = 0; j < nodes[i]->count; j++) {
            printf("%d %d %d %d\n", nodes[i]->entries[j].MBR[0][0], nodes[i]->entries[j].MBR[0][1], nodes[i]->entries[j].MBR[1][0], nodes[i]->entries[j].MBR[1][1]);
        }
        printf("\n");
    }
    a->root = construct(nodes, *p);
}

void preOrder(node* a) {
    if (!a) {
        return;
    }

    printf("%d %d %d %d\n", a->MBR[0][0], a->MBR[0][1], a->MBR[1][0], a->MBR[1][1]);
    for (int i = 0; i < a->count; i++) {
        preOrder(a->entries[i].childPointer);
    }
}

void bfs(node* a) {
    node* arr[50];
    int index = 0;
    int last = 1;
    arr[0] = a;
    while (arr[index]) {
        printf("%d %d %d %d\n", arr[index]->MBR[0][0], arr[index]->MBR[0][1], arr[index]->MBR[1][0], arr[index]->MBR[1][1]);
        for (int i = 0; i < arr[index]->count; i++) {
            arr[last] = arr[index]->entries[i].childPointer;
            last += 1;
        }
        index += 1;
    }    
}

int main() {
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

    // FILE* fptr = fopen("data.txt", "r");
    // Element dataPoints[n];
    // for (int i = 0; i < n; i++) {
    //     int a, b;
    //     scanf("%d", &a);
    //     scanf("%d", &b);
    //     dataPoints[i].MBR[0][0] = a;
    //     dataPoints[i].MBR[0][1] = a;
    //     dataPoints[i].MBR[1][0] = b;
    //     dataPoints[i].MBR[1][1] = b;
    //     dataPoints[i].childPointer = NULL;
    // }

    rtree* a = createNewRtree();
    
    STR(dataPoints, n, a);

    printf("%d\n", a->root->MBR[1][1]);
    for (int i = 0; i < a->root->count; i++) {
        printf("%d ", a->root->entries[i].MBR[1][1]);
    }
    printf("\n");

    bfs(a->root);
    
}

