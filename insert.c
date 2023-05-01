#include "insert.h"
#include "helper_functions.h"
#include <stddef.h>
#include <limits.h>
#include "rtree.h"


void mergeMBR(node* n1, Element ele) {
    for (int i=0; i<N; ++i) {
        n1->MBR[i][0] = min(n1->MBR[i][0], ele.MBR[i][0]);
        n1->MBR[i][1] = max(n1->MBR[i][1], ele.MBR[i][1]);
    }
}

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

void insert(rtree *tr, Element ele) {
    node *leaf = choose_leaf(tr, ele);

    if (leaf->count < M) {
        ele.childPointer = NULL;
        insertElementIntoNode(leaf, ele);

        adjust_tree(tr, leaf, NULL);
        return;
    }

    node *n1 = createNewNode();
    node *n2 = createNewNode();
    n1->isLeaf = n2->isLeaf = true;

    linearSplitNode(*leaf, ele, n1, n2);
    
    adjust_tree(tr, n1, NULL);
    adjust_tree(tr, n2, NULL);
    
    return;
}

node *choose_leaf(rtree *tr, Element ele) {
    node *n = tr->root;

    while (!n->isLeaf) {
        int min_enlargement = INT_MAX;
        int min_index = -1;
        int min_area = INT_MAX;

        for (int i = 0; i < n->count; i++) {
            int area = areaOfElementMBR(n->entries[i]);
            int enlargement = calcEnlargement(n->entries[i].MBR, ele.MBR);

            if (enlargement < min_enlargement) {
                min_enlargement = enlargement;
                min_index = i;
                min_area = area;
            }

            if (enlargement == min_enlargement && area < min_area) {
                min_index = i;
                min_area = area;
            }
        }

        n = n->entries[min_index].childPointer;
    }
    return n;
}

void adjust_tree(rtree *tree, node *n, node *child) {
    if (n->parent == NULL) {
        printf("adjust count: %d\n", n->count);
        if (n->count <= M) return;
        node* n1 = createNewNode();
        node* n2 = createNewNode();
        printf("Split the node\n");
        linearSplitNode(*n, n->entries[0], n1, n2);
        
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

        return;
    }
    mergeMBR(n->parent, n->entries[0]);

    if (n->count > M) {
        node *n1, *n2;
        linearSplitNode(*n, n->entries[0], n1, n2);
 
        ++n->parent->count;

        adjust_tree(tree, n->parent, n1);
        adjust_tree(tree, n->parent, n2);

    } else {
       adjust_tree(tree, n->parent, n);
    }
}