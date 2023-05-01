#include <stdio.h>
#include "rtree.h"
#include "search.h"
#include "helper_functions.h"

void search_rtree(node *T, int S[N][2], Element ***result, int *count)
{
    if (T == NULL)
    {
        return;
    }
    if (T->isLeaf)
    {
        for (int i = 0; i < T->count; i++)
        {
            if (overlaps(S, T->entries[i].MBR) || contains(S, T->entries[i].MBR))
            {
                *result = (Element **)realloc(*result, (*count + 1) * sizeof(Element *));
                (*result)[*count] = &(T->entries[i]);
                (*count)++;
            }
        }
    }
    else
    {
        for (int i = 0; i < T->count; i++)
        {
            if (overlaps(S, T->entries[i].MBR))
            {
                search_rtree(T->entries[i].childPointer, S, result, count);
            }
            else if (contains(S, T->entries[i].MBR))
            {
                for (int j = 0; j < N; j++)
                {
                    S[j][0] = max(S[j][0], T->entries[i].MBR[j][0]);
                    S[j][1] = min(S[j][1], T->entries[i].MBR[j][1]);
                }
                search_rtree(T->entries[i].childPointer, S, result, count);
            }
        }
    }
}
// check if two rectangles overlap
bool overlaps(int S[N][2], int MBR[N][2])
{
    for (int i = 0; i < N; i++)
    {
        if (MBR[i][0] > S[i][1] || MBR[i][1] < S[i][0])
        {
            return false;
        }
    }
    return true;
}

// check if one rectangle is fully contained within another
bool contains(int S[N][2], int MBR[N][2])
{
    for (int i = 0; i < N; i++)
    {
        if (S[i][0] < MBR[i][0] || S[i][1] > MBR[i][1])
        {
            return false;
        }
    }
    return true;
}