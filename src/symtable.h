//
// Created by xsauer03 on 17.11.21.
//

#ifndef IFJ_21_SYMTABLE_H
#define IFJ_21_SYMTABLE_H

#include <stdbool.h>
#include "scanner.h"

typedef enum
{
  VAR,
  FUNC,
} tType;

typedef struct
{
  tType type;
  tokenType token_type;
  bool defined;
  bool isnil;
  int numOfParams;
  char **parametersNames;
  int *parameters;
  int *returnTypes;
  int numOfReturnTypes;
  char **definedVariables;
  int numOfDefinedVariables;
} tData;

typedef struct Tree
{
  tData *data;
  char *key;
  struct Tree *left;
  struct Tree *right;
} Tree_t;

void symtableInit(Tree_t **tree);
void symtableInsert(Tree_t **tree, tData *data, char *key);
tData *symtableSearch(Tree_t **tree, char *key);
void symtableFree(Tree_t *tree);

#endif //IFJ_21_SYMTABLE_H
