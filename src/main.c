/**
 * @file main.c
 *
 * Scanner
 *
 * IFJ Project 2021, team 91
 *
 * @author <xsauer03> Matej Sauer
 * @author <xtichy31> Filip Tichy
 * @author <xkubin25> Anna Kubinova
 * @author <xkrist24> Lukas Kristek
*/

#include <stdio.h>
#include <stdbool.h>
#include "scanner.c"
#include "string.h"
#include "symtable.c"
#include "parser.c"


int main()
{
    tParser_data *pdata = (tParser_data*)malloc(sizeof(tParser_data));
    pdata->token = (tToken*)malloc(sizeof(tToken));
    symtableInit(&pdata->tree);
    int error = parse(pdata);
    symtableFree(pdata->tree);
    if(error)
        printf("error %d\n", error);
    return error;
}
