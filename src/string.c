//
// Created by xsauer03 on 25.10.21.
//

#include <string.h>

#include "string.h"

#define STR_LEN_INC 8 // initial bit length for allocation

// function for clearing string, just the elements in it
void strClear(tString *string)
{
    string->length = 0;
    string->str[string->length] = '\0';
}

// initialization of string data type
bool strInit(tString *string)
{
    if (!(string->str = (char *)malloc(STR_LEN_INC)))
    {
        return false;
    }
    strClear(string);
    string->allocSize = 0;
    return true;
}

// deleting allocated string
void strDispose(tString *string)
{
    free(string->str);
}

// adding character c to string
bool strAddChar(tString *string, char c)
{
    if (string->allocSize + 1 >= string->length)
    {
        if (!(string->str = (char *)realloc(string->str, string->length + STR_LEN_INC)))
        {
            return false;
        }
        string->allocSize = string->length + STR_LEN_INC;
    }

    string->str[string->length++] = c;
    string->str[string->length] = '\0';
    return true;
}

bool strAddStr(tString *str, const char *str2) {
    unsigned int str2Len = (unsigned int) strlen(str2);

    if (str->length + str2Len + 1 >= str->allocSize) {
        unsigned int strNewLen = str->length + strlen(str2) + 1;
        if (!(str->str = (char *) realloc(str->str, strNewLen))) {
            return false;
        }
        str->allocSize = strNewLen;
    }
    str->length += str2Len;
    strcat(str->str, str2);
    str->str[str->length] = '\0';
    return true;
}

// comparing string with str2 (string given by program)
bool strCmpStr(tString *string, const char *str2)
{
    return !strcmp(string->str, str2);
}

// function that returns length of string
unsigned int strGetLength(tString *string)
{
    return string->length;
}