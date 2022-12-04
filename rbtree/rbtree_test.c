#include "rbtree.h"

int main(void)
{
    int L, Q;
    scanf("%d %d", &L, &Q);
    static int c[200001];
    static int x[200001];
    for (int i = 0; i < Q; i++)
        scanf("%d %d", c+i, x+i);

    RBTree * treep = create_rbtree();
    for (int i = 0; i < Q; i++)
    {
        if (*(c+i) == 1)
            insert(treep, *(x+i));
        else
        {
            int key = *(x+i);
            RBNode * nodep = insert(treep, key);

            int maxKey = L;
            int minKey = 0;
            if (get_next(nodep)!=RBTNIL)
                maxKey = get_next(nodep)->key;
            
            if (get_prev(nodep)!=RBTNIL)
                minKey = get_prev(nodep)->key;
            delete(treep, nodep);
            printf("%d\n", maxKey-minKey);
        }
    }
    // print_order(treep->root);
    delete_rbtree(treep);
    return 0;
}