#ifndef RTREE_H
#define RTREE_H

#define M 4
#define m 2
#define N 2     //Number of dimensions
#define INT_MAX 1e8;
#define INT_MIN -1e8;

#include <stdlib.h>

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

#endif
#ifndef RTREE_H
#define RTREE_H

#define M 4
#define m 2
#define N 2     //Number of dimensions
#define INT_MAX 1e8;
#define INT_MIN -1e8;

#include <stdlib.h>

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

#endif
