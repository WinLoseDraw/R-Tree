
// struct rtree {
//     size_t count;
//     int height;
//     struct rect rect;
//     struct node *root; 
//     void *(*malloc)(size_t);
//     void (*free)(void *);
// };

#include "rtree.h"
#include "limits.h"
#include <stddef.h>
#include "helper_functions.h"
// #define M 4
// #define m 2
// #define N 2     //Number of dimensions
// #define INT_MAX 1e8;
// #define INT_MIN -1e8;

typedef enum { false, true } bool;

int calcEnlargement(int r1[N][2], int r2[N][2]) {
    int newRect[N][2];
    
    // Calculate the MBR of the two rectangles
    for (int i = 0; i < N; i++) {
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

void updateMBR(Element *ele, int rect[N][2]) {
    for (int i=0; i<N; ++i) {
        ele->MBR[i][0] = min(ele->MBR[i][0], rect[i][0]);
        ele->MBR[i][1] = max(ele->MBR[i][1], rect[i][1]);
    }
}

void insert(node *T, int rect[N][2]) {
    updateMBR(T->MBR, rect);

    if (T->isLeaf) {
        if (T->count < M) {
            Element *newEntry = malloc(sizeof(*newEntry));
            updateMBR(newEntry, rect);

            newEntry->childPointer = malloc(sizeof(*newEntry->childPointer));
            newEntry->childPointer->isLeaf = true;
            newEntry->childPointer->count = 0;
            updateMBR(newEntry->childPointer->MBR, rect);
            newEntry->childPointer->parent = T;

            T->entries[T->count] = *newEntry;
            T->count++;

        } else {
            // TODO
            
        }
        
        return;
    } 

    int minEnlargement = INT_MAX;
    int minIndex = -1;
    int enlargement;

    for (int i = 0; i < T->count; ++i) {
        enlargement = calcEnlargement(T->entries[i].MBR, rect);
        if (enlargement < minEnlargement) {
            minEnlargement = enlargement;
            minIndex = i;
        }
    }

    insert(T->entries[minIndex].childPointer, rect);
}

node* adjustTree(node *T, int rect[N][2]){

}
