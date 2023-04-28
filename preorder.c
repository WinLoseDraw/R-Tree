#include <stdio.h>
#include "rtree.h"

void print_internal_node(node* n) 
{
    printf("Internal Node: MBR spans from ");
    printf("(%d, %d) to (%d, %d)", n->MBR[0][0], n->MBR[1][0], n->MBR[0][1], n->MBR[1][1]);
    printf("\n\n");
}

void print_leaf_node(node* n) 
{
    printf("Leaf Node: contains %d %s {", n->count, (n->count > 1)? "entries" : "entry");
    int i;
    for (i = 0; i < n->count - 1; i++) 
        printf("(%d, %d) | ", n->entries[i].MBR[0][0],n->entries[i].MBR[1][0]);
    printf("(%d, %d)", n->entries[i].MBR[0][0],n->entries[i].MBR[1][0]);
    
    printf("}\n\n");
}

void preorder_traversal(node* n) 
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