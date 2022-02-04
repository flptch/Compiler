/**
 * @file parser.h
 *
 * @brief parser
 *
 * @author <xtichy31> Filip Tichy
 * @author <xkubin25> Anna Kubinova
 * @author <xkrist24> Lukas Kristek
*/

#include <stdlib.h>
#include <stdio.h>
//#include <ctype.h>
#include <string.h>

#include "scanner.h"
#include "string.h"
#include "symtable.h"
#include "error.h"
#include "parser.h"
#include "expression.c"
#include "code_generator.c"

int checkProlog(tParser_data *pdata) {
    GET_TOKEN
    if (!strcmp(pdata->token->data, "ifj21")) {
        return ERR_OK;
    } else {
        return ERR_SYNTAX;
    }
}

// <params> -> ID : <type> <params>
int parametersSyntax(tParser_data *pdata, int numOfDeclaredParameters, char *nameOfFunction, bool declared,
                     int *declaredParameters) {
    // <params> -> *empty*
    if (pdata->token->type == tT_RBRACKET) // no parameters
    {
        if (declared) {
            if (numOfDeclaredParameters == 0) // number of parameters is the same
            {
                return ERR_OK;
            } else {
                fprintf(stderr, "line %d: Function was not declared with zero parameters!\n", pdata->line);
                return ERR_FUNCTION;
            }
        } else {
            return ERR_OK;
        }
    } else {
        if (pdata->token->type != tT_IDENTIFIER) // ID
        {
            return ERR_SYNTAX;
        }
        tData *data = symtableSearch(&pdata->tree, nameOfFunction);
        data->numOfParams += 1;
        data->parametersNames[data->numOfParams - 1] = pdata->token->data;
        pdata->var = pdata->token->data;
        symtableInsert(&pdata->tree, data, nameOfFunction);
        data = symtableSearch(&pdata->tree, nameOfFunction);
        GET_TOKEN
        if (pdata->token->type != tT_COLON) // :
        {
            return ERR_SYNTAX;
        }
        GET_TOKEN
        tData *symtable_data = initData(true, 0, VAR, 0);
        if (pdata->token->type == tT_STRINGKEYWORD)
            symtable_data->token_type = tT_STRING;
        else if (pdata->token->type == tT_INTEGER)
            symtable_data->token_type = tT_INT;
        else if (pdata->token->type == tT_NUMBER)
            symtable_data->token_type = tT_DOUBLE;
        else if (pdata->token->type == tT_NIL)
            symtable_data->token_type = tT_NIL;
        else
            return ERR_SYNTAX;
        symtable_data->defined = false;
        symtableInsert(&pdata->tree, symtable_data, pdata->var);

        data->parameters[data->numOfParams - 1] = insertParameter(pdata);
        symtableInsert(&pdata->tree, data, nameOfFunction);
        GET_TOKEN
        if (pdata->token->type == tT_RBRACKET) // THE END OF PARAMETHERS
        {
            if (declared) {
                if (numOfDeclaredParameters != data->numOfParams) {
                    fprintf(stderr, "line %d: Different number of parameters than declared in function \"%s\" !\n",
                            pdata->line, nameOfFunction);
                    return ERR_FUNCTION;
                } else if (!parametersSemantic(data, declaredParameters)) {
                    fprintf(stderr, "line %d: Different type of parameters in function \"%s\" !\n", pdata->line,
                            nameOfFunction);
                    return ERR_FUNCTION;
                } else // number and type of parameters is OK
                {
                    return ERR_OK; // SYNTAX AND SEMANTICS OF PARAMETERS OK
                }
            } else {
                return ERR_OK;
            }
        } else if (pdata->token->type == tT_COMMA) // MORE PARAMETERS
        {
            GET_TOKEN
            return (parametersSyntax(pdata, numOfDeclaredParameters, nameOfFunction, declared,
                                     declaredParameters)); // RECURSIVE CALLING
        } else {
            return ERR_SYNTAX;
        }
    }
}

int parametersSemantic(tData *data, const int *declaredParameters) {
    for (int i = 0; i < data->numOfParams; i++) {
        if (data->parameters[i] != declaredParameters[i]) {
            return false;
        }
    }
    return true;
}

int returnTypesSemantic(tData *data, const int *declaredReturnTypes) {
    for (int i = 0; i < data->numOfReturnTypes; i++) {
        if (data->returnTypes[i] != declaredReturnTypes[i]) {
            return false;
        }
    }
    return true;
}

int insertReturnType(tParser_data *pdata) {
    if (pdata->token->type == tT_STRINGKEYWORD) {
        return STRING;
    } else if (pdata->token->type == tT_INTEGER) {
        return INTEGER;
    } else {
        return NUMBER;
    }
}

int returnTypesSyntax(tParser_data *pdata, char *nameOfFunction, int numOfDeclaredReturnTypes, bool declared,
                      int *declaredReturnTypes) {
    tData *data = symtableSearch(&pdata->tree, nameOfFunction);
    data->numOfReturnTypes += 1;
    if (pdata->token->type != tT_STRINGKEYWORD && pdata->token->type != tT_INTEGER && pdata->token->type != tT_NUMBER &&
        pdata->token->type != tT_NIL) {
        return ERR_SYNTAX;
    }
    data->returnTypes[data->numOfReturnTypes - 1] = insertReturnType(pdata);
    symtableInsert(&pdata->tree, data, nameOfFunction);
    GET_TOKEN
    if (pdata->token->type == tT_EOL || pdata->token->type == tT_EOF) {
        if (declared) {
            if (data->numOfReturnTypes != numOfDeclaredReturnTypes) {
                fprintf(stderr, "Different number of return types in function \"%s\" than declared!\n", nameOfFunction);
                return ERR_FUNCTION;
            } else if (!returnTypesSemantic(data, declaredReturnTypes)) {
                fprintf(stderr, "Return types of function \"%s\" are different than declared!\n", nameOfFunction);
                return ERR_FUNCTION;
            } else // SYNTAX AND SEMANTIC OF RETURN TYPES OK
            {
                if (!strcmp(nameOfFunction, "main")) {
                    code_gen_main_start();
                } else {
                    code_gen_function_start(nameOfFunction);
                }
                return ERR_OK;
            }
        } else {
            return ERR_OK;
        }
    } else if (pdata->token->type == tT_COMMA) {
        GET_TOKEN
        return (returnTypesSyntax(pdata, nameOfFunction, numOfDeclaredReturnTypes, declared, declaredReturnTypes));
    } else {
        return ERR_SYNTAX;
    }
}

int insertParameter(tParser_data *pdata) {
    if (pdata->token->type == tT_STRINGKEYWORD) {
        return STRING;
    } else if (pdata->token->type == tT_INTEGER) {
        return INTEGER;
    } else if (pdata->token->type == tT_NIL) {
        return NIL;
    } else {
        return NUMBER;
    }
}

int declarationParameters(tParser_data *pdata, char *nameOfFunction) {
    if (pdata->token->type == tT_RBRACKET) {
        GET_TOKEN
        return ERR_OK; // end of parameters
    }
    tData *data = symtableSearch(&pdata->tree, nameOfFunction);
    data->numOfParams += 1;
    if (pdata->token->type != tT_STRINGKEYWORD && pdata->token->type != tT_INTEGER && pdata->token->type != tT_NUMBER) {
        return ERR_SYNTAX;
    }
    data->parameters[data->numOfParams - 1] = insertParameter(pdata);
    symtableInsert(&pdata->tree, data, nameOfFunction);
    GET_TOKEN
    if (pdata->token->type == tT_COMMA) {
        GET_TOKEN
        return (declarationParameters(pdata, nameOfFunction));
    } else if (pdata->token->type == tT_RBRACKET) {
        GET_TOKEN
        return ERR_OK; // end of parameter
    } else {
        return ERR_SYNTAX;
    }
}

int functionDeclaration(tParser_data *pdata) {
    if (pdata->token->type != tT_IDENTIFIER) {
        return ERR_SYNTAX;
    }
    if (addFunction(pdata, pdata->token->data))
        return ERR_INTERNAL;
    char *nameOfFunction = pdata->token->data;
    tData *data; // (tData *)malloc(sizeof(tData));
    data = symtableSearch(&pdata->tree, pdata->token->data);
    if (data == NULL) // Function is not in the symtable
    {
        symtableInsert(&pdata->tree, initData(false, 0, FUNC, 0), pdata->token->data);
    } else // It is already in the symtable
    {
        fprintf(stderr, "line %d: Redeclaration of function \"%s\" !\n", pdata->line, pdata->token->data);
        return ERR_DEFINITION;
    }
    GET_TOKEN
    if (pdata->token->type != tT_COLON) {
        return ERR_SYNTAX;
    }
    GET_TOKEN
    if (pdata->token->type != tT_FUNCTION) {
        return ERR_SYNTAX;
    }
    GET_TOKEN
    if (pdata->token->type != tT_LBRACKET) {
        return ERR_SYNTAX;
    }
    GET_TOKEN
    if (!declarationParameters(pdata, nameOfFunction)) {
        if (pdata->token->type != tT_COLON) {
            if (pdata->token->type == tT_EOL) {
                return ERR_OK; // no return types
            } else {
                return ERR_SYNTAX;
            }
        }
        GET_TOKEN
        return (returnTypesSyntax(pdata, nameOfFunction, 0, false, NULL));
    } else {
        return ERR_SYNTAX;
    }
}

int varDefinitonOrDeclaration(tParser_data *pdata, char *nameOfFunction) {
    int error;
    if (pdata->token->type != tT_IDENTIFIER) {
        return ERR_SYNTAX;
    }
    tData *data = symtableSearch(&pdata->tree, pdata->token->data);
    if (data == NULL) {
        symtableInsert(&pdata->tree, initData(true, 0, VAR, 0), pdata->token->data);
    } else {
        if (data->defined) {
            fprintf(stderr, "Redefinition of variable \"%s\"", pdata->token->data);
            return ERR_DEFINITION;
        } else {
            fprintf(stderr, "Redeclaration of variable \"%s\" \n", pdata->token->data);
            return ERR_DEFINITION;
        }
    }
    char *nameOfVariable = pdata->token->data;
    data = symtableSearch(&pdata->tree, nameOfFunction);
    data->numOfDefinedVariables++;
    data->definedVariables[data->numOfDefinedVariables - 1] = nameOfVariable;
    symtableInsert(&pdata->tree, data, nameOfFunction);
    pdata->var = pdata->token->data;
    GET_TOKEN
    if (pdata->token->type != tT_COLON) {
        return ERR_SYNTAX;
    }
    GET_TOKEN

    tData *symtable_data = (tData *) malloc(sizeof(tData));
    ALLOC_CHECK(symtable_data)
    if (pdata->token->type == tT_STRINGKEYWORD){
        symtable_data->token_type = tT_STRING;
        symtable_data->isnil = false;
    }
    else if (pdata->token->type == tT_INTEGER){
        symtable_data->token_type = tT_INT;
        symtable_data->isnil = false;
    }
    else if (pdata->token->type == tT_NUMBER){ //double -> number
        symtable_data->token_type = tT_DOUBLE;
        symtable_data->isnil = false;
    }
    else if (pdata->token->type == tT_NIL){
        symtable_data->token_type = tT_NIL;
        symtable_data->isnil = true;
    }
    else
        return ERR_SYNTAX;
    symtable_data->defined = false;
    int type = pdata->token->type;
    symtableInsert(&pdata->tree, symtable_data, pdata->var);
    GET_TOKEN
    //     code_gen_var_declaration(nameOfVariable);
    //     code_gen_var_definition_type(typeOfVariable,nameOfVariable);
    if (pdata->token->type == tT_EOL) {

        code_gen_var_declaration(nameOfVariable);
        code_gen_var_definition_type(type, nameOfVariable);
        //symtableInsert(&pdata->tree, initData(false, 0, VAR, 0), nameOfVariable);
        return ERR_OK;
    } else if (pdata->token->type == tT_ASSIGN) {
        symtable_data->defined = true;
        GET_TOKEN
        if(pdata->token->type == tT_NIL)
            symtable_data->isnil = true;
        symtableInsert(&pdata->tree, symtable_data, pdata->var);
            code_gen_var_declaration(nameOfVariable);
            code_gen_var_definition_type(type, nameOfVariable);

        error = expression(pdata, symtable_data->token_type);
        code_gen_pop_val(nameOfVariable);
        return error;
    } else {
        return ERR_SYNTAX;
    }
}

int ifStatement(tParser_data *pdata, char *nameOfFunction) {
    int error;
    int error_exp = expression(pdata, 0);
    if (error_exp)
        return error_exp;
    code_gen_if_start(pdata->checkLabel.ifCounter);
    //    code_gen_if_body(pdata->checkLabel.ifCounter);
    if (pdata->token->type != tT_THEN) {
        return ERR_SYNTAX; // SYNTAX ERROR
    }
    GET_TOKEN
    if (pdata->token->type != tT_EOL) {
        return ERR_SYNTAX;
    }
    GET_TOKEN

    // FIND ELSE OR ERROR
    error = bodySyntax(pdata, nameOfFunction);
    if (error != ERR_OK)
        return ERR_SYNTAX;

    GET_TOKEN
    if (pdata->token->type != tT_EOL) {
        return ERR_SYNTAX;
    }
    code_gen_if_end(pdata->checkLabel.ifCounter);
    // else <body>
    pdata->checkLabel.inIf = false;
    pdata->checkLabel.inElse = true;
    error = bodySyntax(pdata, nameOfFunction);
    if (error != ERR_OK) {
        return errorHandler(error);
    }
    pdata->checkLabel.inElse = false;
    // end of if statement, checked in bodysyntax

    // VOLANI GENERATORU PRO KONEC IFU
    code_gen_if_end_end(pdata->checkLabel.ifCounter);

    GET_TOKEN

    if (pdata->token->type != tT_EOL) {
        fprintf(stderr, "There must be end of line after end!\n");
        return ERR_SYNTAX;
    }

    GET_TOKEN

    pdata->checkLabel.ifCounter--;
    return ERR_OK;
}

int whileStatement(tParser_data *pdata, char *nameOfFunction) {
    int error;
    int error_exp = expression(pdata, 0);
    if (error_exp)
        return error_exp;

    code_gen_while(pdata->checkLabel.whileCounter);
    if (pdata->token->type != tT_DO) {
        return ERR_SYNTAX;
    }
    GET_TOKEN
    if (pdata->token->type != tT_EOL) {
        fprintf(stderr, "Expected end of line after \"do\"!\n");
        return ERR_SYNTAX;
    }
    GET_TOKEN
    error = bodySyntax(pdata, nameOfFunction);
    if (error) {
        return errorHandler(error);
    }
    GET_TOKEN
    if (pdata->token->type != tT_EOL) {
        fprintf(stderr, "There must be end of line after end!\n");
        return ERR_SYNTAX;
    }
    GET_TOKEN
    code_gen_while_end(pdata->checkLabel.whileCounter);
    pdata->checkLabel.whileCounter--;
    pdata->checkLabel.inWhile = false;
    return ERR_OK;
}

int bodySyntax(tParser_data *pdata, char *nameOfFunction) {
    int error;
    // <body> -> local ID : <type> <defvar> EOL <body>
    if (pdata->token->type == tT_LOCAL) {
        GET_TOKEN
        error = varDefinitonOrDeclaration(pdata, nameOfFunction);
        if (!error) {
            GET_TOKEN
            return (bodySyntax(pdata, nameOfFunction));
        } else {
            return errorHandler(error);
        }
    }
        // <body> -> return <expression> EOL <body>
    else if (pdata->token->type == tT_RETURN) {
//        printf("goh");
        tData *sym_var = symtableSearch(&pdata->tree, nameOfFunction);
        if (sym_var == NULL)
            return ERR_DEFINITION;
        if (sym_var->numOfReturnTypes == 0) {
//            printf("jojo");
            return ERR_OK;
        }
        for (int i = 0; i < sym_var->numOfReturnTypes; i++) {

            GET_TOKEN
            tokenType check_type;
            // printf("rerutn %d z %d : %d\n", i, sym_var->numOfReturnTypes, *sym_var->returnTypes);
            switch (sym_var->returnTypes[i]) {
                case INTEGER:
                    check_type = tT_INT;
                    break;
                case NUMBER:
                    check_type = tT_DOUBLE;
                    break;
                case STRING:
                    check_type = tT_STRING;
                    break;
                case NIL:
                    check_type = tT_NIL;
            }
            printf("return %s\n",pdata->token->data);
            int error_exp = expression(pdata, check_type);
            if (error_exp)
                return error_exp;

            if (i + 1 < sym_var->numOfReturnTypes)
                if (pdata->token->type != tT_COMMA) {
                    fprintf(stderr, "line %d : return : expected ',' in-between return values\n", pdata->line);
                    return ERR_SYNTAX;
                }
        }
        return (bodySyntax(pdata, nameOfFunction));
    }
        // <body> -> EOL <body>
    else if (pdata->token->type == tT_EOL) {
        GET_TOKEN
        return (bodySyntax(pdata, nameOfFunction));
    }
        // <body> -> ID : <defValue> EOL <body>
    else if (pdata->token->type == tT_IDENTIFIER) {
        tData *data = symtableSearch(&pdata->tree, pdata->token->data);
        if (data == NULL) {
            fprintf(stderr, "line %d : function \"%s\" not defined\n", pdata->line, pdata->token->data);
            return ERR_DEFINITION;
        }
        if (data->type == FUNC) {
            if (data->defined) {
                GET_TOKEN;
                if (pdata->token->type != tT_LBRACKET) {
                    return ERR_SYNTAX;
                }
                GET_TOKEN;
                if (pdata->token->type == tT_RBRACKET) {
                    if (data->numOfParams == 0) {
                        GET_TOKEN;
                        return bodySyntax(pdata, nameOfFunction);
                    } else {
                        return ERR_FUNCTION;
                    }
                } else {
                    code_gen_function_call_start();
                    error = functionCallParams(pdata, data);
                    if (!error) {
                        code_gen_function_call(nameOfFunction);
                        return start(pdata);
                    } else {
                        return errorHandler(error);
                    }
                }
            } else {
                return ERR_DEFINITION;
            }
        } else {

            // NOVY: process uplne stejny, jen pred zavolamin expression muze provit vic promennych
            while (pdata->token->type == tT_IDENTIFIER) {
                tData *sym_var = symtableSearch(&pdata->tree, pdata->token->data);
                if (sym_var == NULL) {
                    fprintf(stderr, "line %d : variable \"%s\" not defined\n", pdata->line, pdata->token->data);
                    return ERR_DEFINITION;
                } else if (!sym_var->defined) {
                    sym_var->defined = true;
                    symtableInsert(&pdata->tree, sym_var, pdata->var);
                }
                //printf("cyklus: %d :%s\n", pdata->token->type, pdata->token->data);
                char *nameOfVariable = pdata->token->data; //mozna pole?
                GET_TOKEN
                if (pdata->token->type == tT_COMMA) {
                } else if (pdata->token->type == tT_ASSIGN) {
                    GET_TOKEN

                    if (pdata->token->type == tT_IDENTIFIER) {
                        tData *funkce_check = symtableSearch(&pdata->tree, pdata->token->data);
                        if (sym_var == NULL) {
                            return ERR_DEFINITION;
                        } else if (funkce_check->type == FUNC) {
                            // volani funkce?   zatim token pro preskoceni ()
                            GET_TOKEN
                            GET_TOKEN
                            continue;
                        }
                        else
                        {
                            int error_exp = expression(pdata, sym_var->token_type);
                            code_gen_pop_val(nameOfVariable);
                            if (error_exp)
                                return error_exp;
                            return (bodySyntax(pdata, nameOfFunction));
                        }
                    }
                    if(pdata->token->type == tT_NIL){
                        sym_var->isnil = true;
                    }
                    int error_exp = expression(pdata, sym_var->token_type);
                    code_gen_pop_val(nameOfVariable);
                    if (error_exp)
                        return error_exp;
                    return (bodySyntax(pdata, nameOfFunction));
                }
                else
                    return ERR_SYNTAX;
                GET_TOKEN
            }

            ////  PUVODNI: UMI PRIRAZOVAT JEN DO JEDNE PROMENNE
            // tData *sym_var = symtableSearch(&pdata->tree, pdata->token->data);
            // if (sym_var == NULL){
            //     fprintf(stderr, "line %d : variable \"%s\" not defined\n", pdata->line, pdata->token->data);
            //     return ERR_DEFINITION;
            // }
            // if (!sym_var->defined) {
            //     sym_var->defined = true;
            //     symtableInsert(&pdata->tree, sym_var, pdata->var);
            // }
            // char *nameOfVariable = pdata->token->data;
            // GET_TOKEN
            // if (pdata->token->type != tT_ASSIGN) {
            //     return ERR_SYNTAX;
            // }
            // GET_TOKEN
            // int error_exp = expression(pdata, sym_var->token_type);
            // code_gen_pop_val(nameOfVariable);
            // if (error_exp)
            //     return error_exp;
            // return (bodySyntax(pdata, nameOfFunction));
        }
    }
        // <body> -> while <expression> do EOL <body> EOL end EOL <body>
    else if (pdata->token->type == tT_WHILE) {
        pdata->checkLabel.inIf = false;
        pdata->checkLabel.inWhile = true;
        pdata->checkLabel.whileCounter++;
        code_gen_while_start(pdata->checkLabel.whileCounter);
        GET_TOKEN
        error = whileStatement(pdata, nameOfFunction);
        if (!error) {
            return (bodySyntax(pdata, nameOfFunction));
        } else {
            return errorHandler(error);
        }
    }
        // <body> -> if <expression> then EOL <body> <else> EOL end
    else if (pdata->token->type == tT_IF) {
        pdata->checkLabel.inIf = true;
        pdata->checkLabel.inWhile = false;
        pdata->checkLabel.ifCounter++;
        GET_TOKEN
        error = ifStatement(pdata, nameOfFunction);
        if (!error) {
            EOL_CHECK
            return (bodySyntax(pdata, nameOfFunction));
        } else {
            return errorHandler(error);
        }
    }
        // <body> -> <else>
    else if (pdata->token->type == tT_ELSE) {
        if (!pdata->checkLabel.inIf && pdata->checkLabel.ifCounter == 0) {
            fprintf(stderr, "line %d: Expect if before else!\n", pdata->line);
            return ERR_SYNTAX;
        }
        return ERR_OK;
    } else if (pdata->token->type == tT_END) {
        if (pdata->checkLabel.inIf) {
            fprintf(stderr, "line %d: Expected else before end!\n", pdata->line);
            return ERR_SYNTAX;
        }
        return ERR_OK;
    }
        // <body> -> write <expression> EOL ?
    else if (pdata->token->type == tT_WRITE)
    {
        GET_TOKEN
        if (pdata->token->type != tT_LBRACKET)
            return ERR_LEX;
        while (pdata->token->type != tT_RBRACKET) {
            GE_TOKEN_EOL_IGNORE //GET_TOKEN
            if (pdata->token->type == tT_STRING) {
                code_gen_function_write(pdata->token->data);
            } else if (pdata->token->type == tT_IDENTIFIER) {
                tData *sym_var = symtableSearch(&pdata->tree, pdata->token->data);
                if (sym_var == NULL) {
                    fprintf(stderr, "line %d : write : variable \"%s\" not defined or declared\n", pdata->line,
                            pdata->token->data);
                    return ERR_DEFINITION;
                }
                if (!sym_var->defined) {
                    fprintf(stderr, "Variable \"%s\" not defined!\n", pdata->token->data);
                    return ERR_DEFINITION;
                }

                code_gen_function_write_id(pdata->token->data);
            } else {
                fprintf(stderr, "line %d : write : unexpected syntax\n", pdata->line);
                return ERR_SYNTAX;
            }
            GE_TOKEN_EOL_IGNORE
            if (pdata->token->type == tT_RBRACKET) {
                GE_TOKEN_EOL_IGNORE
                return (bodySyntax(pdata, nameOfFunction));
            } else if (pdata->token->type != tT_COMMA) {
                fprintf(stderr, "line %d: write : expected ',' in-between each member\n", pdata->line);
                return ERR_SYNTAX;
            }
        }
    }
        // <body> -> EOF
    else if (pdata->token->type == tT_EOF) {
        fprintf(stderr, "There can't be end of file inside of function!\n");
        return ERR_SYNTAX;
    } else {
        return ERR_SYNTAX;
    }
    return ERR_OK;
}

int functionDefinition(tParser_data *pdata) {
    int error;
    GET_TOKEN

    if (pdata->token->type != tT_IDENTIFIER) // NAME OF FUNCTION
    {
        return ERR_SYNTAX;
    }
    if (addFunction(pdata, pdata->token->data))
        return ERR_INTERNAL;
    char *nameOfFunction = pdata->token->data;
    if (!strcmp(nameOfFunction, "main")) {
        tData *data;
        data = symtableSearch(&pdata->tree, nameOfFunction);
        data->defined = false;
        symtableInsert(&pdata->tree, data, nameOfFunction);
    }
    tData *data; // = (tData *)malloc(sizeof(tData));
    data = symtableSearch(&pdata->tree, pdata->token->data);
    int numOfDeclaredParams = 0;
    int numOfDeclaredReturnTypes = 0;
    int *declaredReturnTypes = (int *) malloc(10 * (sizeof(int)));
    ALLOC_CHECK(declaredReturnTypes)
    bool declared = false;
    int *declaredParameters = (int *) malloc(10 * (sizeof(int)));
    if (data == NULL || !strcmp(nameOfFunction, "main")) // function is not in the symtable
    {
        symtableInsert(&pdata->tree, initData(true, 0, FUNC, 0), pdata->token->data);
    } else // it is in the symtable
    {
        if (data->defined) // it is already defined
        {
            fprintf(stderr, "line %d: Redefinition of function \"%s\" !\n", pdata->line, pdata->token->data);
            return ERR_DEFINITION;
        } else // it is just declared, so I will define it
        {
            declared = true;
            numOfDeclaredParams = data->numOfParams;
            declaredParameters = data->parameters;
            numOfDeclaredReturnTypes = data->numOfReturnTypes;
            declaredReturnTypes = data->returnTypes;
            symtableInsert(&pdata->tree, initData(true, 0, FUNC, 0), pdata->token->data);
        }
    }
    GET_TOKEN
    if (pdata->token->type != tT_LBRACKET) // LEFT BRACKET
    {
        return ERR_SYNTAX;
    }
    GET_TOKEN
    error = parametersSyntax(pdata, numOfDeclaredParams, nameOfFunction, declared, declaredParameters);
    if (!error) // PARAMETERS
    {
        GET_TOKEN
        if (pdata->token->type != tT_COLON) {
            if (pdata->token->type == tT_EOL) // no return types
            {
                if (!strcmp(nameOfFunction, "main")) {
                    code_gen_main_start();
                } else
                    code_gen_function_start(nameOfFunction);
                data = symtableSearch(&pdata->tree, nameOfFunction);
                for (int i = 0; i < data->numOfParams; i++) {
                    code_gen_param_declaration(i, data->parametersNames[i]);
                }
                GET_TOKEN
                error = bodySyntax(pdata, nameOfFunction);
                if (!error) {
                    if (strcmp(nameOfFunction, "main") != 0) {
                        deleteLocalVariables(pdata, nameOfFunction);
                        code_gen_function_end(nameOfFunction);
                    }
                } else {
                    return errorHandler(error);
                }
            } else {
                return ERR_SYNTAX;
            }
        } else {
            GET_TOKEN
            error = returnTypesSyntax(pdata, nameOfFunction, numOfDeclaredReturnTypes, declared, declaredReturnTypes);
            if (!error) // SYNTAX OF RETURN TYPES
            {
                data = symtableSearch(&pdata->tree, nameOfFunction);
                for (int i = 0; i < data->numOfParams; i++) {
                    code_gen_param_declaration(i, data->parametersNames[i]);
                }
                GET_TOKEN
                error = bodySyntax(pdata, nameOfFunction);
                if (!error) {
                    code_gen_function_end(nameOfFunction);
                } else {
                    return errorHandler(error);
                }
            } else {
                return errorHandler(error);
            }
        }
    } else {
        return errorHandler(error);
    }
    return ERR_OK;
}

void deleteLocalVariables(tParser_data *pdata, char *nameOfFunction) {
    char *nameOfVariable;
    tData *data = symtableSearch(&pdata->tree, nameOfFunction);
    for (int i = 0; i < data->numOfDefinedVariables; i++) {
        nameOfVariable = data->definedVariables[i];
        data = symtableSearch(&pdata->tree, data->definedVariables[i]);
        data->defined = false;
        symtableInsert(&pdata->tree, data, nameOfVariable);
    }
}

int start(tParser_data *pdata) {
    int error;
    // <prog> -> GLOBAL ID : (<params>) : <type> EOL <prog>
    if (pdata->token->type == tT_GLOBAL) {
        GET_TOKEN
        error = functionDeclaration(pdata);
        if (!error) {
            GET_TOKEN
            return (start(pdata));
        } else {
            return (errorHandler(error));
        }
    }
        // <prog> -> FUNCTION ID : (<params>) : <type> EOL <body> end <prog>
    else if (pdata->token->type == tT_FUNCTION) {
        error = functionDefinition(pdata);
        if (!error) {
            GET_TOKEN
            return (start(pdata));
        } else {
            return errorHandler(error);
        }
    } else if (pdata->token->type == tT_IDENTIFIER) {
        char *nameOfFunction = pdata->token->data;
        tData *data; // = (tData *)malloc(sizeof(tData));
        data = symtableSearch(&pdata->tree, pdata->token->data);
        if (data != NULL) // function is defined or declared
        {
            if (data->defined) // function is defined
            {
                GET_TOKEN
                if (pdata->token->type != tT_LBRACKET) // checking the parameters
                {
                    return ERR_SYNTAX;
                }
                GET_TOKEN
                if (pdata->token->type == tT_RBRACKET) // function is called with zero pameters
                {
                    if (data->numOfParams == 0) {
                        GET_TOKEN
                        return start(pdata); // function is called properly
                    } else {
                        fprintf(stderr, "line %d: Function \"%s\" has more parameters than none!\n", pdata->line,
                                nameOfFunction);
                        return ERR_FUNCTION;
                    }
                    //                    return ERR_OK;
                } else {
                    code_gen_function_call_start();
                    error = functionCallParams(pdata, data);
                    if (!error) {
                        code_gen_function_call(nameOfFunction);
                        return start(pdata); // Calling function properly
                    } else {
                        return errorHandler(error);
                    }
                }
                //                if (getToken(pdata->token))
                //                    return ERR_LEX;
                //EOL_CHECK
                //                return start(pdata);
            } else // function is not defined
            {
                fprintf(stderr, "line %d: \"%s\" hasn't been defined yet!\n", pdata->line, pdata->token->data);
                return ERR_DEFINITION;
            }
        } else {
            fprintf(stderr, "line %d: \"%s\" hasn't been declared or defined!\n", pdata->line, pdata->token->data);
            return ERR_DEFINITION;
        }
    }
        // <prog> -> EOL <prog>
    else if (pdata->token->type == tT_EOL) {

        GET_TOKEN

        return (start(pdata));
    }
        // <prog> -> EOF
    else if (pdata->token->type == tT_EOF) {
        for (int i = 0; i < pdata->numberOffunctions; i++) {
            tData *data = symtableSearch(&pdata->tree, pdata->functionName[i]);
            if (data->defined == false) {
                fprintf(stderr, "function \"%s\" hasn't been defined!\n", pdata->functionName[i]);
                return ERR_DEFINITION;
            }
        }
        code_gen_main_end();
        code_gen_end();
        return ERR_OK;
    } else if (pdata->token->type == tT_END) {
        code_gen_main_end();
        code_gen_end();
        if (pdata->checkLabel.inIf) {
            return ERR_SYNTAX;
        }
        return ERR_OK;
    }
    {
        return ERR_SYNTAX;
    }
}

int functionCallParams(tParser_data *pdata, tData *data) {
    int i;
    for (i = 0; i < data->numOfParams; i++) {
        if (!checkTypes(pdata->token, data, i)) {
            fprintf(stderr, "line %d: Different types of parameters in function calling!\n", pdata->line);
            return ERR_FUNCTION;
        }
        code_gen_functions_param(i, pdata->token);
        GET_TOKEN
        if (i == data->numOfParams - 1) {
            if (pdata->token->type != tT_RBRACKET) {
                fprintf(stderr, "line %d: Wrong number of parameters in function calling!\n", pdata->line);
                return ERR_FUNCTION;
            }
        }
        GET_TOKEN
    }
    if (i == 0) {
        fprintf(stderr, "line %d: Wrong number of parameters in function calling!\n", pdata->line);
        return ERR_FUNCTION;
    }
    return ERR_OK;
}

bool checkTypes(tToken *token, tData *data, int i) {
    if (token->type == tT_STRING) {
        if (data->parameters[i] != STRING) {
            return false;
        } else {
            return true;
        }
    } else if (token->type == tT_INT) {
        if (data->parameters[i] != INTEGER) {
            return false;
        } else {
            return true;
        }
    } else if (token->type == tT_DOUBLE) {
        if (data->parameters[i] != NUMBER) {
            return false;
        } else {
            return true;
        }
    } else if (token->type == tT_NIL) {
        if (data->parameters[i] != NIL) {
            return false;
        } else {
            return true;
        }
    } else {
        return ERR_FUNCTION;
    }
}

tData *initData(bool defined, int numOfParams, tType type, int numOfReturnTypes) {
    tData *data = (tData *) malloc(sizeof(tData));
    data->defined = defined;
    data->numOfParams = numOfParams;
    data->numOfReturnTypes = numOfReturnTypes;
    data->type = type;
    data->parameters = (int *) malloc(10 * (sizeof(int)));
    data->returnTypes = (int *) malloc(10 * (sizeof(int)));
    data->parametersNames = (char **) malloc(10 * (sizeof(char)));
    data->definedVariables = (char **) malloc(10 * (sizeof(char)));
    return data;
}

//tData *basicFunctionsParameters(tData *data, int *parameters)

tData *initBasicFunctionsData(bool defined, int numOfParams, tType type, int *parameters, int *returnTypes) {
    tData *data = (tData *) malloc(sizeof(tData));
    data->defined = defined;
    data->numOfParams = numOfParams;
    data->type = type;
    data->parameters = (int *) malloc(10 * (sizeof(int)));
    data->parameters = parameters;
    data->returnTypes = (int *) malloc(10 * (sizeof(int)));
    data->returnTypes = returnTypes;
    return data;
}

void initBasicFunctions(tParser_data *pdata) {
    int *parameters = (int *) malloc(10 * (sizeof(int)));
    int *returnTypes = (int *) malloc(10 * (sizeof(int)));
    returnTypes[0] = STRING;
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 0, FUNC, NULL, returnTypes), "reads");
    returnTypes = NULL;
    returnTypes = (int *) malloc(10 * (sizeof(int)));
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 0, FUNC, NULL, NULL), "main");
    returnTypes[0] = INTEGER;
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 0, FUNC, NULL, returnTypes), "readi");
    returnTypes = NULL;
    returnTypes = (int *) malloc(10 * (sizeof(int)));
    returnTypes[0] = NUMBER;
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 0, FUNC, NULL, returnTypes), "readn");
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 0, FUNC, NULL, NULL), "write");
    returnTypes = NULL;
    returnTypes = (int *) malloc(10 * (sizeof(int)));
    returnTypes[0] = INTEGER;
    parameters[0] = NUMBER;
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 1, FUNC, parameters, returnTypes), "tointeger");
    returnTypes = NULL;
    returnTypes = (int *) malloc(10 * (sizeof(int)));
    parameters = NULL;
    parameters = (int *) malloc(10 * (sizeof(int)));
    returnTypes[0] = STRING;
    parameters[0] = STRING;
    parameters[1] = NUMBER;
    parameters[2] = NUMBER;
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 3, FUNC, parameters, returnTypes), "substr");
    returnTypes = NULL;
    returnTypes = (int *) malloc(10 * (sizeof(int)));
    parameters = NULL;
    parameters = (int *) malloc(10 * (sizeof(int)));
    returnTypes[0] = INTEGER;
    parameters[0] = STRING;
    parameters[1] = INTEGER;
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 2, FUNC, parameters, returnTypes), "ord");
    returnTypes = NULL;
    returnTypes = (int *) malloc(10 * (sizeof(int)));
    parameters = NULL;
    parameters = (int *) malloc(10 * (sizeof(int)));
    returnTypes[0] = STRING;
    parameters[0] = INTEGER;
    symtableInsert(&pdata->tree, initBasicFunctionsData(true, 1, FUNC, parameters, returnTypes), "chr");
}

int addFunction(tParser_data *pdata, char *name) {
    for (int i = 0; i < pdata->numberOffunctions; i++)
        if (strcmp(pdata->functionName[i], name) == 0)
            return 0;
    pdata->numberOffunctions++;
    if (pdata->functionName == NULL) {
        if (!(pdata->functionName = (char **) malloc(1)))
            return ERR_INTERNAL;
    } else if (!(pdata->functionName = (char **) realloc(pdata->functionName, pdata->numberOffunctions + 1)))
        return ERR_INTERNAL;
    if (!(pdata->functionName[pdata->numberOffunctions - 1] = malloc(strlen(name) + 1)))
        return ERR_INTERNAL;
    pdata->functionName[pdata->numberOffunctions - 1] = name;
    return 0;
}

char *enToStr(tToken token) {
    tString string;
    tString *str = &string;
    strInit(str);
    if (token.type == tT_STRING) {
        strAddStr(str, "string");
        return str->str;
    }
    if (token.type == tT_IDENTIFIER) {
        strAddStr(str, "variable");
        return str->str;
    }
    if (token.type == tT_INT) {
        strAddStr(str, "int");
        return str->str;
    }
    if (token.type == tT_DOUBLE || token.type == tT_NUMBER) {
        strAddStr(str, "number");
        return str->str;
    }
    if (token.type == tT_NIL) {
        strAddStr(str, "nil");
        return str->str;
    } else {
        return token.data;
    }
}

int errorHandler(int error) {
    if (error == ERR_OK) {
        return ERR_OK;
    } else if (error == ERR_SYNTAX) {
        return ERR_SYNTAX;
    } else if (error == ERR_LEX) {
        return ERR_LEX;
    } else if (error == ERR_DEFINITION) {
        return ERR_DEFINITION;
    }
    return error;
}

// <prog> -> REQUIRE "ifj21" EOL <prog>
int prolog(tParser_data *pdata) {
    if (pdata->token->type == tT_REQUIRE) {
        if (!checkProlog(pdata)) {
            code_gen_start();
            GET_TOKEN
            return (start(pdata));
        } else {
            return ERR_SEM_OTHER;
        }
    } else {
        return ERR_SYNTAX;
    }
}

int parse(tParser_data *pdata) {
    initBasicFunctions(pdata);
    pdata->checkLabel.whileCounter = 0;
    pdata->checkLabel.ifCounter = 0;
    pdata->checkLabel.inElse = false;
    pdata->checkLabel.inIf = false;
    pdata->checkLabel.inWhile = false;
    pdata->numberOffunctions = 0;
    pdata->token->type = tT_UNKNOWN;
    pdata->token->data = NULL;
    pdata->line = 1;
    GET_TOKEN
    int error = prolog(pdata);
    return (error);
}
