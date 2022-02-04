//  error.h
//  xkrist24

#ifndef IFJ_21_ERROR_H
#define IFJ_21_ERROR_H

#define ERR_OK                  0   //OK
#define ERR_LEX                 1   //chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
#define ERR_SYNTAX              2   //chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu)
#define ERR_DEFINITION          3   //sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp
#define ERR_ASSIGN_TYPE         4   //sémantická chyba v příkazu přiřazení (typová nekompatibilita)
#define ERR_FUNCTION            5   //sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot u volání funkce či návratu z funkce
#define ERR_TYPE_COMPATIBILITY  6   //sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
#define ERR_SEM_OTHER           7   //ostatní sémantické chyby
#define ERR_UNEXPECTED_NIL      8   //běhová chyba při práci s neočekávanou hodnotou nil
#define ERR_ZERO_DIVISION       9   //běhová chyba celočíselného dělení nulovou konstantou
#define ERR_INTERNAL            99  //interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd

#endif //IFJ_21_ERROR_H