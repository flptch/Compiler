//
// Created by xsauer03 on 17.11.21.
//

#include "symtable.h"
#include <stdlib.h>

void symtableInit(Tree_t **tree)
{
    *tree = NULL;
}

void symtableInsert(Tree_t **tree, tData *data, char *key)
{
    if (!*tree)
    {
        *tree = (Tree_t *)malloc(sizeof(struct Tree));
        (*tree)->key = key;
        (*tree)->data = data;
        (*tree)->left = (*tree)->right = NULL;
        return;
    }
    int cmp = strcmp(key, (*tree)->key);
    if (cmp == 0)
    {
        (*tree)->data = data;
    }
    else if (cmp < 0)
    {
        symtableInsert(&(*tree)->left, data, key);
    }
    else
    {
        symtableInsert(&(*tree)->right, data, key);
    }
    return;
}

tData *symtableSearch(Tree_t **tree, char *key)
{
    if (*tree == NULL)
    {
        return NULL;
    }
    int cmp = strcmp(key, (*tree)->key);
    if (cmp == 0)
    {
        return (*tree)->data;
    }
    else if (cmp < 0)
        return symtableSearch(&(*tree)->left, key);
    else if (cmp > 0)
        return symtableSearch(&(*tree)->right, key);
    else
        return NULL;
}

void symtableFree(Tree_t *tree)
{
    if (tree)
    {
        symtableFree(tree->left);
        symtableFree(tree->right);
        free(tree);
        tree = NULL;
    }
}
