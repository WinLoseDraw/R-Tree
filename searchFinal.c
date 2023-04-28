#include <stdio.h>
#include "rtree.h"

// Searches for elements that overlap with the given range
void search(node *T, int input_rect[N][2], Element *result, int *count)
{
    if (T == NULL)
    {
        return;
    }

    // Check if T is a leaf node
    if (T->isLeaf)
    {
        // Check each entry in T to see if it overlaps with input_rect
        for (int i = 0; i < T->count; i++)
        {
            bool overlaps = true;
            for (int j = 0; j < N; j++)
            {
                if (T->entries[i].MBR[j][1] < input_rect[j][0] || T->entries[i].MBR[j][0] > input_rect[j][1])
                {
                    overlaps = false;
                    break;
                }
            }
            // If the entry overlaps with input_rect, add it to the result array
            if (overlaps)
            {
                result[*count] = T->entries[i];
                (*count)++;
            }
        }
    }
    else
    {
        // Check each entry in T to see if it overlaps with input_rect
        for (int i = 0; i < T->count; i++)
        {
            bool overlaps = true;
            for (int j = 0; j < N; j++)
            {
                if (T->entries[i].MBR[j][1] < input_rect[j][0] || T->entries[i].MBR[j][0] > input_rect[j][1])
                {
                    overlaps = false;
                    break;
                }
            }
            // If the entry overlaps with input_rect, recursively search the subtree rooted at the child pointer
            if (overlaps)
            {
                search(T->entries[i].childPointer, input_rect, result, count);
            }
        }
    }
}
