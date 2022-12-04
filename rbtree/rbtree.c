#include "rbtree.h"

// leafはすべて同じnodeとして扱う
RBNode sentinel =
{
	.color = BLACK,
    .left = RBTNIL,
    .right = RBTNIL,
    .parent = NULL
};

// 新しい空のRB木を作成
RBTree * create_rbtree(void)
{
    RBTree * treep = malloc(sizeof(RBTree));
    treep->root = RBTNIL;
    treep->size = 0;
    return treep;
}

// RB木を削除
void delete_rbtree(RBTree * treep)
{
    void _delete_all(RBNode * );
    // 全てのノードの記憶領域を解放
    _delete_all(treep->root);
    free(treep);
}

void _delete_all(RBNode * nodep)
{
    if (nodep==RBTNIL)
        return;
    if (nodep->left!=RBTNIL)
        _delete_all(nodep->left);
    if (nodep->right!=RBTNIL)
        _delete_all(nodep->right);
    free(nodep);
    return;
}


// 昇順に値を並べる
void print_order(RBNode * nodep)
{
    if (nodep->left!=RBTNIL)
        print_order(nodep->left);
    for (int i = 0; i < nodep->count; i++)
        printf("key: %d\n", nodep->key);
    if (nodep->right!=RBTNIL)
        print_order(nodep->right);
    return;
}

// 値がkのノードを探してくる。
// 存在しなければRBTNILを返す
RBNode * search(RBTree * treep, int k)
{
    RBNode * zp = treep->root;
    while (zp != RBTNIL && k != zp->key)
    {
        if (k < zp->key)
            zp = zp->left;
        else
            zp = zp->right;
    }
    return zp;
}

// nodepをrootにしたときの木の最小値
RBNode * _get_min(RBNode * nodep)
{
    RBNode * zp = nodep;
    while (zp->left != RBTNIL)
        zp = zp->left;
    return zp;
}

// nodepをrootにしたときの木の最大値
RBNode * _get_max(RBNode * nodep)
{
    RBNode * zp = nodep;
    while (zp->right != RBTNIL)
        zp = zp->right;
    return zp;
}

// nodepの次に大きい値のノードを返す.
// 存在しないときはRBTNILを返す
// nodepはtreeに組み込まれている前提
RBNode * get_next(RBNode * nodep)
{
    RBNode * _get_min(RBNode * );

    if (nodep->right!=RBTNIL)
        return _get_min(nodep->right);

    RBNode * pp = nodep->parent;
    while (pp!=RBTNIL && nodep==pp->right)
    {
        nodep = pp;
        pp = nodep->parent;
    } 
    return pp;
}

// nodeはtreeに組み込まれている前提
RBNode * get_prev(RBNode * nodep)
{
    RBNode * _get_max(RBNode * );

    if (nodep->left!=RBTNIL)
        return _get_max(nodep->left);
    RBNode * pp = nodep->parent;
    while (pp!=RBTNIL && nodep==pp->left)
    {
        nodep = pp;
        pp = nodep->parent;
    } 
    return pp; 
}

void _left_rotate(RBTree * treep, RBNode * nodep)
{
    RBNode * yp = nodep->right;
    nodep->right = yp->left;
    if (yp->left != RBTNIL)
        yp->left->parent = nodep;
    yp->parent = nodep->parent;
    if (nodep->parent==RBTNIL)
        treep->root = yp;
    else if (nodep==nodep->parent->left)
        nodep->parent->left = yp;
    else
        nodep->parent->right = yp;
    yp->left = nodep;
    nodep->parent = yp;
}

void _right_rotate(RBTree * treep, RBNode * nodep)
{
    RBNode * yp = nodep->left;
    nodep->left = yp->right;
    if (yp->right != RBTNIL)
        yp->right->parent = nodep;
    yp->parent = nodep->parent;
    if (nodep->parent==RBTNIL)
        treep->root = yp;
    else if (nodep==nodep->parent->right)
        nodep->parent->right = yp;
    else
        nodep->parent->left = yp;
    yp->right = nodep;
    nodep->parent = yp;
}

RBNode * insert(RBTree * treep, int key)
{
    void _insert_fixup(RBTree * , RBNode * );
    RBNode * create_node(int);
   
    // nodeを生成
    RBNode * nodep = create_node(key);
    treep->size += 1;

    RBNode * yp = RBTNIL;
    RBNode * xp = treep->root;
    while (xp!=RBTNIL)
    {
        yp = xp;
        if (nodep->key < xp->key)
            xp = xp->left;
        else if(nodep->key > xp->key)
            xp = xp->right;
        else
        {
            xp->count += 1;
            free(nodep);
            return xp;
        }
    }
    nodep->parent = yp;
    if (yp==RBTNIL)
        treep->root = nodep;
    else if (nodep->key < yp->key)
        yp->left = nodep;
    else
        yp->right = nodep;
    nodep->color = RED;
    _insert_fixup(treep, nodep);
    return nodep;
}

RBNode * create_node(int key)
{
    RBNode * nodep = malloc(sizeof(RBNode));
    nodep->key = key;
    nodep->count = 1;
    nodep->parent = RBTNIL;
    nodep->left = RBTNIL;
    nodep->right = RBTNIL;
    return nodep;
}

// nodeはREDという前提
void _insert_fixup(RBTree * treep, RBNode * nodep)
{
    void _left_rotate(RBTree * , RBNode * );
    void _left_rotate(RBTree * , RBNode * );

    while (nodep->parent->color == RED)
    {
        if (nodep->parent == nodep->parent->parent->left)
        {
            RBNode * yp = nodep->parent->parent->right;
            if (yp->color == RED)
            {
                nodep->parent->color = BLACK;
                yp->color = BLACK;
                nodep->parent->parent->color = RED;
                nodep = nodep->parent->parent;
            }
            else 
            {
                if (nodep == nodep->parent->right)
                {
                    nodep = nodep->parent;
                    _left_rotate(treep, nodep);
                }
                else
                {
                    nodep->parent->color = BLACK;
                    nodep->parent->parent->color = RED;
                    _right_rotate(treep, nodep->parent->parent);
                }
            }
        }
        else
        {
            RBNode * yp = nodep->parent->parent->left;
            if (yp->color == RED)
            {
                nodep->parent->color = BLACK;
                yp->color = BLACK;
                nodep->parent->parent->color = RED;
                nodep = nodep->parent->parent;
            }
            else 
            {
                if (nodep == nodep->parent->left)
                {
                    nodep = nodep->parent;
                    _right_rotate(treep, nodep);
                }
                else
                {
                    nodep->parent->color = BLACK;
                    nodep->parent->parent->color = RED;
                    _left_rotate(treep, nodep->parent->parent);
                }
            }
        }
    }
    treep->root->color = BLACK;
}

// uを除去してその場所にvを移植する
void _transplant(RBTree * treep, RBNode * nodeUp, RBNode * nodeVp)
{
    if (nodeUp->parent == RBTNIL)
        treep->root = nodeVp;
    else if (nodeUp == nodeUp->parent->left)
        nodeUp->parent->left = nodeVp;
    else
        nodeUp->parent->right = nodeVp;
    nodeVp->parent = nodeUp->parent;
}


//削除
void delete(RBTree * treep, RBNode * nodep)
{
    void _delete_fixup(RBTree * , RBNode * );
    void _transplant(RBTree * , RBNode * , RBNode * );
    void print_order(RBNode *);
    RBNode * _get_min(RBNode * );

    RBNode * yp = nodep;
    RBNode * xp;
    char y_original_color = yp->color;

    treep->size -= 1;
    if (nodep->count > 1)
    {
        nodep->count -= 1;
        return;
    }
    if (nodep->left == RBTNIL)
    {
        xp = nodep->right;
        _transplant(treep, nodep, xp);
    }
    else if (nodep->right == RBTNIL)
    {
        xp = nodep->left;
        _transplant(treep, nodep, xp);
    }
    else
    {
        yp = _get_min(nodep->right);
        y_original_color = yp->color;
        xp = yp->right;
        if (yp->parent == nodep)
            xp->parent = yp;
        else
        {
            _transplant(treep, yp, yp->right);
            yp->right = nodep->right;
            yp->right->parent = yp;
        }
        _transplant(treep, nodep, yp);
        yp->left = nodep->left;
        yp->left->parent = yp;
        yp->color = nodep->color;
        if (y_original_color == BLACK)
            _delete_fixup(treep, xp);
    }
    free(nodep);
}

void _delete_fixup(RBTree * treep, RBNode * nodep)
{
    void _left_rotate(RBTree * , RBNode * );
    void _left_rotate(RBTree * , RBNode * );

    while (nodep!=treep->root && nodep->color==BLACK)
    {
        if (nodep==nodep->parent->left)
        {
            RBNode * wp = nodep->parent->right;
            if (wp->color==RED)
            {
                wp->color = BLACK;
                nodep->parent->color = RED;
                _left_rotate(treep, nodep->parent);
                wp = nodep->parent->right;
            }
            if (wp->left->color==BLACK && wp->right->color==BLACK)
            {
                wp->color = RED;
                nodep = nodep->parent;
            }
            else 
            {
                if (wp->right->color==BLACK)
                {
                    wp->left->color = BLACK;
                    wp->color = RED;
                    _right_rotate(treep, wp);
                    wp = nodep->parent->right;
                }
                else
                {
                    wp->color = nodep->parent->color;
                    nodep->parent->color = BLACK;
                    wp->right->color = BLACK;
                    _left_rotate(treep, nodep->parent);
                    nodep = treep->root;
                }
            }
        }
        else
        {
            RBNode * wp = nodep->parent->left;
            if (wp->color==RED)
            {
                wp->color = BLACK;
                nodep->parent->color = RED;
                _right_rotate(treep, nodep->parent);
                wp = nodep->parent->left;
            }
            if (wp->right->color==BLACK && wp->left->color==BLACK)
            {
                wp->color = RED;
                nodep = nodep->parent;
            }
            else 
            {
                if (wp->left->color==BLACK)
                {
                    wp->right->color = BLACK;
                    wp->color = RED;
                    _left_rotate(treep, wp);
                    wp = nodep->parent->left;
                }
                else
                {
                    wp->color = nodep->parent->color;
                    nodep->parent->color = BLACK;
                    wp->left->color = BLACK;
                    _right_rotate(treep, nodep->parent);
                    nodep = treep->root;
                }
            }
        }
    }
    nodep->color = BLACK;
}
