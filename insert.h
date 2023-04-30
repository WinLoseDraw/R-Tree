#ifndef INSERT_H
#define INSERT_H

#include "rtree.h"
#include "splitnode.h"

void insert(rtree *tr, Element ele);

node *choose_leaf(rtree *tr, Element ele);

void adjust_tree(rtree* tree, node* leafNode, node *child);

#endif
