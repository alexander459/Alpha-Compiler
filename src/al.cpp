#include <stdio.h>
#include <string>
#include "tokenlist/tokenlist.hpp"
#include "utils/util.hpp"
#include "parser.hpp"
extern int yylex();
extern int yyparse();
using namespace std;

string file_name;
bool out_flag=false;
extern FILE* yyin;
int main(int argc, char **argv){
    if(argc == 3){
       out_flag = true;
       file_name = argv[2];
    }

    yyin=fopen(argv[1],"r");
    
    //token_t *node=(token_t*)malloc(sizeof(token_t));

    //alpha_yylex((token_t*)node);
    //yyparse();
    yyparse();
    fclose(yyin);
}
