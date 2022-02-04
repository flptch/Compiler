#include "expression.c"
//#include "scanner.c"

void main(){
    stdin = fopen("test.txt", "r");
    //setSourceFile(inputfile);
    tToken token;

    //while(token.type != tT_EOF){
       // getToken(&token);
        //if(token.type == tT_ASSIGN){
    expression(&token);
        //}
    //}
    //expression

    return;
}