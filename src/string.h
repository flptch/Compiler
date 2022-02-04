//
// Created by xsauer03 on 25.10.21.
//

#ifndef IFJ_21_STRING_H
#define IFJ_21_STRING_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *str; // a room for string
    unsigned int length; // length of string
    unsigned int allocSize; // actual allocated size
} tString;

bool strInit(tString *string);

void strDispose(tString *string);

bool strAddChar(tString *string, char c);

bool strAddStr(tString *string, const char *str2);

void strClear(tString *string);

bool strCmpStr(tString *string, const char *str2);

//bool strCmpConstStr(tString *string);

unsigned int strGetLength(tString *string);

#endif //IFJ_21_STRING_H
