//
// Created by xsauer03 on 19.11.21.
//

#include <ctype.h>

#include "code_generator.h"
#include "string.h"
#include <stdio.h>
#include "parser.h"


#define ADD_INSTRUCTION(_const)           \
    if (!strAddStr(machine_code, _const)) \
    return false
#define ADD_INSTRUCTION_CHAR(_const)       \
    _const = (char)_const + 48;            \
    if (!strAddChar(machine_code, _const)) \
    return false

#define NEW_LINE()                      \
    if (!strAddStr(machine_code, "\n")) \
    return false

tString code;
tString *machine_code = &code;
int for_count = 0;
int for_prev = 0;
int while_count = 0;
int while_prev = 0;
// build-in functions

bool code_gen_function_read(tToken *token, char *var_id) {
    ADD_INSTRUCTION("READ LF@");
    ADD_INSTRUCTION(var_id);
    switch (token->type) {
        case 5:
            ADD_INSTRUCTION(" int");
            break;
        case 6:
            ADD_INSTRUCTION(" float");
            break;
        case 7:
            ADD_INSTRUCTION(" string");
            break;
        default:
            break;
    }
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@result_value LF@");
    ADD_INSTRUCTION(var_id);
    NEW_LINE();
    return true;
}

bool code_gen_function_write(char *print) {
    char c;
    unsigned int len;
    if (print != NULL)
        len = strlen(print);
    tString str;
    tString *tmpString = &str;
    if (!strInit(tmpString))
        return false;
    for (unsigned int i = 0; i < len; i++) {
        c = print[i];
        if (c != '\0') {
            if (c == ' ') {
                strAddStr(tmpString, "\\032");
            } else if (c == '#') {
                strAddStr(tmpString, "\\035");
            } else if (c == '\n') {
                strAddStr(tmpString, "\\010");
            } else if (c == 92) {
                strAddStr(tmpString, "\\092");
            } else if (c == 9) {
                strAddStr(tmpString, "\\009");
            } else {
                strAddChar(tmpString, c);
            }
        }
    }
    ADD_INSTRUCTION("WRITE string@");
    ADD_INSTRUCTION(tmpString->str);
    NEW_LINE();
    return true;
}

bool code_gen_function_write_id(char *id) {
    ADD_INSTRUCTION("WRITE LF@");
    ADD_INSTRUCTION(id);
    NEW_LINE();
    return true;
}

bool code_gen_toint() {

    ADD_INSTRUCTION("FLOAT2INT LF@result_value LF@1");
    NEW_LINE();
    return true;
}

bool builtin_ord() {
    ADD_INSTRUCTION("LABEL ord");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("DEFFVAR LF@result_value");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@result_value nil@nil");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@str");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@str LF@1");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@i LF@2");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@len");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@len int@0");
    NEW_LINE();
    ADD_INSTRUCTION("STRLEN LF@len LF@str");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@len");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("LTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFEQS chr_end");
    NEW_LINE();
    ADD_INSTRUCTION("GETCHAR LF@result_value LF@str LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("JUMP chr_true_end");
    NEW_LINE();
    ADD_INSTRUCTION("LABEL chr_end");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@result_value nil@nil");
    NEW_LINE();
    ADD_INSTRUCTION("LABEL chr_true_end");
    NEW_LINE();
    ADD_INSTRUCTION("POPFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("RETURN");
    NEW_LINE();
    return true;
}

bool builtin_chr() {
    ADD_INSTRUCTION("LABEL chr");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@result_value");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@result_value nil@nil");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@i LF@1");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@top");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@top int@255");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@bottom");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@bottom int@255");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@top");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("LTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSH bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFEQS chr_end");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@bottom");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("GTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFEQS chr_end");
    NEW_LINE();
    ADD_INSTRUCTION("INT2CHR LF@result_value LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("LABEL chr_end");
    NEW_LINE();
    ADD_INSTRUCTION("POPFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("RETURN");
    NEW_LINE();
    return true;
}

bool builtin_substr() {
    ADD_INSTRUCTION("LABEL substr");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@result_str");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@result_str string@");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@str");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@str LF@1");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@i LF@2");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@j");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@j LF@3");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@len");
    NEW_LINE();
    ADD_INSTRUCTION("STRLEN LF@len LF@str");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@j");
    NEW_LINE();
    ADD_INSTRUCTION("GTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFNEQS substr_end");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS int@0");
    NEW_LINE();
    ADD_INSTRUCTION("LTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFEQS substr_end");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@j");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@len");
    NEW_LINE();
    ADD_INSTRUCTION("GTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFEQ substr_end");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR LF@str1");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@str1 string@");
    NEW_LINE();
    ADD_INSTRUCTION("LABEL loop");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS LF@j");
    NEW_LINE();
    ADD_INSTRUCTION("GTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFNEQS substr_true_end");
    NEW_LINE();
    ADD_INSTRUCTION("GETCHAR LF@str1 LF@str LF@i");
    NEW_LINE();
    ADD_INSTRUCTION("CONCAT LF@result_str LF@result_str LF@str1");
    NEW_LINE();
    ADD_INSTRUCTION("ADD LF@i LF@i int@1");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@result_value LF@result_str");
    NEW_LINE();
    ADD_INSTRUCTION("JUMP loop");
    NEW_LINE();
    ADD_INSTRUCTION("LABEL substr_end");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE LF@result_value nil@nil");
    NEW_LINE();
    ADD_INSTRUCTION("LABEL substr_true_end");
    NEW_LINE();
    ADD_INSTRUCTION("POPFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("RETURN");
    NEW_LINE();
    return true;
}

// start

bool code_gen_header() {
    ADD_INSTRUCTION(".IFJcode21");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR GF@res");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR GF@tmp1");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR GF@tmp2");
    NEW_LINE();
    ADD_INSTRUCTION("DEFVAR GF@tmp3");
    NEW_LINE();
    ADD_INSTRUCTION("JUMP main");
    NEW_LINE();

    return true;
}

bool code_gen_start() {
    if (!strInit(machine_code))
        return false;
    if (!code_gen_header())
        return false;
//    if (!builtin_chr()) return false;
//    NEW_LINE();
//    if (!builtin_ord()) return false;
//    NEW_LINE();
//    if (!builtin_substr()) return false;
//    NEW_LINE();
    return true;
}

void code_gen_free() {
    strDispose(machine_code);
}

void code_gen_end() {
    printf("%s", machine_code->str);
    code_gen_free();
}

// main

bool code_gen_main_start() {

    ADD_INSTRUCTION("LABEL main");
    NEW_LINE();
    ADD_INSTRUCTION("CREATEFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHFRAME");
    NEW_LINE();

    return true;
}

bool code_gen_main_end() {
    ADD_INSTRUCTION("POPFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("CLEARS");
    NEW_LINE();
    return true;
}

bool code_gen_main_call() {
    ADD_INSTRUCTION("LABEL main_call");
    NEW_LINE();
    return true;
}

//functions

bool code_gen_function_label(char *function_id) {
    ADD_INSTRUCTION("LABEL ");
    ADD_INSTRUCTION(function_id);
    return true;
}

//generates function start
bool code_gen_function_start(char *function_id) {
    code_gen_function_label(function_id);
    NEW_LINE();
    ADD_INSTRUCTION("PUSHFRAME");
    NEW_LINE();
    return true;
}

//generates function end
bool code_gen_function_end(char *function_id) {
    ADD_INSTRUCTION("LABEL ");
    ADD_INSTRUCTION(function_id);
    ADD_INSTRUCTION("_end");
    NEW_LINE();

    ADD_INSTRUCTION("POPFRAME");
    NEW_LINE();
    ADD_INSTRUCTION("RETURN");
    NEW_LINE();
    return true;
}

bool code_gen_function_call_start() {
    ADD_INSTRUCTION("CREATEFRAME");
    NEW_LINE();
    return true;
}

//call function using CALL instruction
bool code_gen_function_call(char *function_id) {
    ADD_INSTRUCTION("CALL ");
    ADD_INSTRUCTION(function_id);
    NEW_LINE();
    return true;
}

//na zacatku fce... (cislo navratove hodnoty)
//bool code_gen_function_return_value(int token, int i) {
//    ADD_INSTRUCTION("DEFVAR LF@return_value_");
//    ADD_INSTRUCTION_CHAR(i);
//    NEW_LINE();
//    ADD_INSTRUCTION("MOVE LF@return_value_");
//    ADD_INSTRUCTION_CHAR(i);
//    printf("%d",token);
//    switch (token) {
//        case 5:
//            ADD_INSTRUCTION(" int@0");
//            break;
//        case 6:
//            ADD_INSTRUCTION(" float@0x0p+0");
//            break;
//        case 7:
//            ADD_INSTRUCTION(" string@");
//            break;
//        case 38:
//            ADD_INSTRUCTION(" nil@nil");
//            break;
//        default:
//            return false;
//    }
//    return true;
//}

bool code_gen_var_value(tToken *token) {
    char c;
    unsigned int len;
    if (token->data != NULL)
        len = strlen(token->data);
    char ahoj[100];
    tString str;
    tString *tmpString = &str;
    if (!strInit(tmpString))
        return false;
    switch (token->type) {
        case 5:
            ADD_INSTRUCTION("int@");
            ADD_INSTRUCTION(token->data);
            break;
        case 6:
            sprintf(ahoj, "%a", atof(token->data));
            ADD_INSTRUCTION("float@");
            ADD_INSTRUCTION(ahoj);
            break;
        case 7:
            for (unsigned int i = 0; i < len; i++) {
                c = token->data[i];
                if (c != '\0') {
                    if (c == ' ') {
                        strAddStr(tmpString, "\\032");
                    } else if (c == '\t') {
                        strAddStr(tmpString, "\\009");
                    } else if (c == '#') {
                        strAddStr(tmpString, "\\035");
                    } else if (c == '\n') {
                        strAddStr(tmpString, "\\010");
                    } else if (c == 92) {
                        strAddStr(tmpString, "\\092");
                    } else {
                        strAddChar(tmpString, c);
                    }
                }
            }
            ADD_INSTRUCTION("string@");
            ADD_INSTRUCTION(tmpString->str);
            break;
        case 3:
            ADD_INSTRUCTION("LF@");
            ADD_INSTRUCTION(token->data);
            break;
        case 38:
            ADD_INSTRUCTION("nil@nil");
            break;
        default:
            strDispose(tmpString);
            return false;
    }

    strDispose(tmpString);
    return true;
}

//declerations of param
bool code_gen_param_declaration(int i, char *param_id) {
    ADD_INSTRUCTION("DEFVAR LF@");
    ADD_INSTRUCTION(param_id);
    NEW_LINE();
    i++;
    ADD_INSTRUCTION("MOVE LF@");
    ADD_INSTRUCTION(param_id);
    ADD_INSTRUCTION(" LF@");
    ADD_INSTRUCTION_CHAR(i);
    NEW_LINE();

    return true;
}

//send param to funtion
bool code_gen_functions_param(int i, tToken *token) {
    i++;
    ADD_INSTRUCTION("DEFVAR TF@");
    ADD_INSTRUCTION_CHAR(i);
    NEW_LINE();

    ADD_INSTRUCTION("MOVE TF@");
    ADD_INSTRUCTION_CHAR(i);
    ADD_INSTRUCTION(" ");
    if (!code_gen_var_value(token))
        return false;
    NEW_LINE();
    return true;
}
bool print_me(char *a) {
    ADD_INSTRUCTION(a);
    NEW_LINE();
    return true;
}
// moves result to return value and jump to function end
bool code_gen_function_return(tParser_data *pdata, tToken *token) {
    ADD_INSTRUCTION("MOVE LF@return_value ");
    if (!code_gen_var_value(token)) return false;
    NEW_LINE();
    ADD_INSTRUCTION("JUMP ");
    ADD_INSTRUCTION(pdata->token->data);
    ADD_INSTRUCTION("_end");
    NEW_LINE();

    return true;
}

//moves return_value to var where it actually should be
bool code_gen_return2var(char *var_id) {
    ADD_INSTRUCTION("MOVE LF@");
    ADD_INSTRUCTION(var_id);
    ADD_INSTRUCTION(" TF@return_value");
    NEW_LINE();
    return true;
}

//var functions

bool code_gen_convert_top2_int2float() {
    ADD_INSTRUCTION("INT2FLOATS");
    NEW_LINE();
    return true;
}

bool code_gen_convert_top1_float2int() {
    ADD_INSTRUCTION("FLOAT2INTS");
    NEW_LINE();
    return true;
}

bool code_gen_convert_top1_int2float() {
    ADD_INSTRUCTION("POPS GF@tmp1");
    NEW_LINE();
    ADD_INSTRUCTION("INT2FLOATS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS GF@tmp1");
    NEW_LINE();
    return true;
}

bool code_gen_convert_top2_float2int() {
    ADD_INSTRUCTION("POPS GF@tmp1");
    NEW_LINE();
    ADD_INSTRUCTION("FLOATS2INTS");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS GF@tmp1");
    NEW_LINE();
    return true;
}

bool code_gen_concat_str() {
    ADD_INSTRUCTION("POPS GF@tmp3");
    NEW_LINE();
    ADD_INSTRUCTION("POPS GF@tmp2");
    NEW_LINE();
    ADD_INSTRUCTION("CONCAT GF@tmp1 GF@tmp2 GF@tmp3");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS GF@tmp1");
    NEW_LINE();
    return true;
}

bool code_gen_operation_nil_eq() {
    ADD_INSTRUCTION("POPS GF@tmp1");
    NEW_LINE();
    ADD_INSTRUCTION("POPS GF@tmp2");
    NEW_LINE();
    ADD_INSTRUCTION("EQ GF@res GF@tmp1 GF@tmp2");
    NEW_LINE();
    ADD_INSTRUCTION("MOVE GF@res bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("PUSHS GF@res");
    NEW_LINE();
    return true;
}

//generates operation
bool code_gen_operation(char op) {
    switch (op) {
        case '+':
            ADD_INSTRUCTION("ADDS");
            NEW_LINE();
            break;
        case '-':
            ADD_INSTRUCTION("SUBS");
            NEW_LINE();
            break;
        case '*':
            ADD_INSTRUCTION("MULS");
            NEW_LINE();
            break;
        case '/':
            ADD_INSTRUCTION("DIVS");
            NEW_LINE();
            break;
        case 'i':
            ADD_INSTRUCTION("IDIVS");
            NEW_LINE();
            break;
        case '<':
            ADD_INSTRUCTION("LTS");
            NEW_LINE();
            break;
        case 'l':
            ADD_INSTRUCTION("POPS GF@tmp1");
            NEW_LINE();
            ADD_INSTRUCTION("POPS GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp1");
            NEW_LINE();
            ADD_INSTRUCTION("LTS");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp1");
            NEW_LINE();
            ADD_INSTRUCTION("EQS");
            NEW_LINE();
            ADD_INSTRUCTION("ORS");
            NEW_LINE();
            break;
        case '>':
            ADD_INSTRUCTION("GTS");
            NEW_LINE();
            break;
            //            greater or equal >=
        case 'g':
            ADD_INSTRUCTION("POPS GF@tmp1");
            NEW_LINE();
            ADD_INSTRUCTION("POPS GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp1");
            NEW_LINE();
            ADD_INSTRUCTION("GTS");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@tmp1");
            NEW_LINE();
            ADD_INSTRUCTION("EQS");
            NEW_LINE();
            ADD_INSTRUCTION("ORS");
            NEW_LINE();
            break;
        case 'e':
            ADD_INSTRUCTION("POPS GF@tmp1");
            NEW_LINE();
            ADD_INSTRUCTION("POPS GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("EQ GF@res GF@tmp1 GF@tmp2");
            NEW_LINE();
            ADD_INSTRUCTION("PUSHS GF@res");
            NEW_LINE();
            break;
        case 'n':
            ADD_INSTRUCTION("EQS");
            NEW_LINE();
            ADD_INSTRUCTION("NOTS");
            NEW_LINE();
            break;
        default:
            break;
    }
    return true;
}

//declares new var
bool code_gen_var_declaration(char *var_id) {
    ADD_INSTRUCTION("DEFVAR ");
    ADD_INSTRUCTION("LF@");
    ADD_INSTRUCTION(var_id);
    NEW_LINE();
    return true;
}

bool code_gen_var_definition_type(int type, char *var_id) {
    ADD_INSTRUCTION("MOVE ");
    ADD_INSTRUCTION("LF@");
    ADD_INSTRUCTION(var_id);
    ADD_INSTRUCTION(" ");
    switch (type) {
        case 36:
            ADD_INSTRUCTION("int@0");
            break;
        case 39:
            ADD_INSTRUCTION("float@0x0p+0");
            break;
        case 42:
            ADD_INSTRUCTION("string@");
            break;
        case 38:
            ADD_INSTRUCTION("nil@nil");
            break;
        default:
            return false;
    }
    NEW_LINE();
    return true;
}

//push value or identifier on stack
bool code_gen_push_val(tToken *token) {
    ADD_INSTRUCTION("PUSHS ");
    if (!code_gen_var_value(token))
        return false;
    NEW_LINE();
    return true;
}

//saves whats on stack to var
bool code_gen_pop_val(char *var_id) {
    ADD_INSTRUCTION("POPS LF@");
    ADD_INSTRUCTION(var_id);
    NEW_LINE();
    return true;
}

// statement dependent instructions

bool code_gen_if_start(int label_index) {
    char tmp[40];
    sprintf(tmp, "%d", for_count);
    ADD_INSTRUCTION("PUSHS ");
    ADD_INSTRUCTION("bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFNEQS $IF");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    return true;
}

//bool code_gen_if_body(int label_index)
//{
//    ADD_INSTRUCTION("LABEL_")
//
//    NEW_LINE();
//    return true;
//}

bool code_gen_if_end(int label_index) {
//    ADD_INSTRUCTION("JUMP IF_END");
//    NEW_LINE();
    char tmp[40];
    sprintf(tmp, "%d", for_count);
    ADD_INSTRUCTION("JUMP $IF");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_END");
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    label_index -= 48;
    ADD_INSTRUCTION("LABEL ");
    ADD_INSTRUCTION("$IF");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    return true;
}

bool code_gen_if_end_end(int label_index) {
    char tmp[40];
    sprintf(tmp, "%d", for_count);
    for_count++;
    ADD_INSTRUCTION("LABEL ");
    ADD_INSTRUCTION("$IF");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_END");
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    return true;
}

bool code_gen_while_start(int label_index) {
    if (while_prev == label_index) {
        while_prev = label_index;
    }
    char tmp[40];
    sprintf(tmp, "%d", while_count);
    ADD_INSTRUCTION("LABEL ");
    ADD_INSTRUCTION("WHILE");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    return true;
}

bool code_gen_while(int label_index) {
    char tmp[40];
    sprintf(tmp, "%d", while_count);
    ADD_INSTRUCTION("PUSHS ");
    ADD_INSTRUCTION(" bool@true");
    NEW_LINE();
    ADD_INSTRUCTION("JUMPIFNEQS ");
    ADD_INSTRUCTION("WHILE");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_END");
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    return true;
}

bool code_gen_while_end(int label_index) {
    char tmp[40];
    sprintf(tmp, "%d", while_count);
    if (while_prev == label_index) {
        while_count++;
    }
    ADD_INSTRUCTION("JUMP WHILE");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    label_index -= 48;
    ADD_INSTRUCTION("LABEL ");
    ADD_INSTRUCTION("WHILE");
    ADD_INSTRUCTION_CHAR(label_index);
    ADD_INSTRUCTION("_END");
    ADD_INSTRUCTION("_");
    ADD_INSTRUCTION(tmp);
    NEW_LINE();
    return true;
}