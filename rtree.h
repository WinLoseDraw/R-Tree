#ifndef RTREE_H
#define RTREE_H

#define M 4
#define m 2
#define N 2     //Number of dimensions

typedef enum { false, true } bool;

struct rtree_node {
    bool isLeaf;
    int count;
    int MBR[N][2];
    struct rtree_node* childPointers[M];
    struct rtree_node* parent;
};

typedef struct rtree_node node;

struct rtree {
    node* root;
};

typedef struct rtree rtree;

#endif
