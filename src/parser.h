/**
 * @file parser.h
 *
 * @brief parser interface
 *
 * @author <xtichy31> Filip Tichy
 * @author <xkubin25> Anna Kubinova
 * @author <xkrist24> Lukas Kristek
*/

#include "scanner.h"

#ifndef IFJ_21_PARSER_H
#define IFJ_21_PARSER_H

#define INTEGER 100
#define NUMBER 101
#define STRING 102
#define NIL 103

#define EOL_CHECK if(pdata->token->type == tT_EOL){ \
                pdata->line = pdata->line + 1;}\
            
#define GET_TOKEN if (getToken(pdata->token)) {printf("line: %d lex error\n",pdata->line);return ERR_LEX;}\
            if(pdata->token->type == tT_EOL){pdata->line = pdata->line +1;}\

#define GET_TOKEN_NO_RETURN getToken(pdata->token); if(pdata->token->type == tT_EOL){pdata->line = pdata->line + 1;}

#define GE_TOKEN_EOL_IGNORE if (getToken(pdata->token)) {printf("line: %d\n",pdata->line);return ERR_LEX;}\
            if(pdata->token->type == tT_EOL){pdata->line = pdata->line +1; GET_TOKEN}\

#define  ALLOC_CHECK(a) if(a == NULL) return ERR_INTERNAL;


typedef struct {
    bool inWhile;
    bool inIf;
    bool inElse;
    int ifCounter;
    int whileCounter;
}tLabelCheck;

typedef struct
{
    tToken *token;
    Tree_t *tree;
    tLabelCheck checkLabel;
    char *var;
    int line;
    int numberOffunctions;
    char **functionName;
}tParser_data;


int addFunction(tParser_data *pdata, char *name);
char* enToStr(tToken token);

/**
 * starting function of parser
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int parse(tParser_data *pdata);

/**
 * checks the syntax of the whole program
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int start(tParser_data *pdata);

/**
 * checks syntax of the function definition
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int functionDefinition(tParser_data *pdata);

/**
 * checks the syntax of the body
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int bodySyntax(tParser_data *pdata, char *nameOfFunction);

/**
 * checks the syntax of the else statement
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int elseStatement(tParser_data *pdata);

/**
 * checks syntax of the if statement
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int ifStatement(tParser_data *pdata, char *nameOfFunction);

int whileStatement(tParser_data *pdata, char *nameOfFunction);
/**
 * checks the syntax of function definiton or declaration
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int varDefinitionOrDeclaration(tParser_data *pdata, char *nameOfFunction);

void deleteLocalVariables(tParser_data *pdata,char *nameOfFunction);

/**
 * checks the syntax of function declaration
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int functionDeclaration(tParser_data *pdata);

/**
 * checks the syntax of parameters during function declaration
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int declarationParameters(tParser_data *pdata, char *nameOfFunction);

/**
 * checks the syntax of the return types during function definition or declaration
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int returnTypesSyntax(tParser_data *pdata, char *nameOfFunction, int numOfDeclaredReturnTypes, bool declared, int *declaredReturnTypes);

/**
 * checks the syntax of the parameters during function definition
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int parametersSyntax(tParser_data *pdata, int numOfDeclaredParameters, char *nameOfFunction, bool declared, int *declaredParameters);

/**
 * checks the syntax of the prolog
 * @param token
 * @return ERR_OK - OK, otherwise - type of error
 */
int checkProlog(tParser_data *pdata);

int errorHandler(int error);

tData *initData(bool defined, int numOfParams, tType type, int numOfReturnTypes);

int insertParameter(tParser_data *pdata);

int parametersSemantic(tData *data, const int *declaredParameters);

int functionCallParams(tParser_data *pdata, tData *data);

bool checkTypes(tToken *token, tData *data, int i);

#endif /* IFJ_21_PARSER_H */
