#ifndef RTREE_H
#define RTREE_H

#define M 4
#define m 2
#define N 2     //Number of dimensions

typedef enum { false, true } bool;

struct Element {
    int MBR[N][N];
    struct rtree_node* childPointer;
};

typedef struct Element Element;

struct rtree_node {
    bool isLeaf;
    int count;
    Element children[M];
    struct rtree_node* parent;
};

typedef struct rtree_node* node;

struct rtree {
    node* root;
};

typedef struct rtree rtree;

#endif