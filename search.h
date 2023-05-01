#include "rtree.h"

void search_rtree(node *T, int S[N][2], Element ***result, int *count);

bool overlaps(int S[N][2], int MBR[N][2]);

bool contains(int S[N][2], int MBR[N][2]);
