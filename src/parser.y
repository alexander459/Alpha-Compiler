%{
    #include <stdio.h>
    #include <string>
    #include <string.h>
    #include <stdlib.h>
    #include <iostream>
    #include <list>
    #include <array>
    #include <vector>
    #include <cstring>
    #include <stack>
    #include "utils/util.hpp"
    #include "symboltable/symboltable.hpp"
    #include "intermediate/intermediate.hpp"
    #include "expression_list/expression_list.hpp"
    #include "quads/quads.hpp"
    #include "scopespace/scopespace.hpp"
    #include "final_code/final_code.hpp"    

    using namespace std;
    void yyerror(char* s);
    extern int yylex();
    extern int yyparse();

    unsigned int symbol_scope = 0;
    string file_name;
    bool out_flag = false;
    extern FILE* yyin;
    extern int yylineno;
    unsigned int anonymous_func_num=0;
    SymbolTableEntry *e = NULL;
    string func_name="";
    bool is_func = false;
    bool is_var=false; 
    list<indexed_t*>indexed_list;
    unsigned loopcounter=0;
    SymbolTableEntry* temp_entry;
%}


%start program

%union{struct call_t* smthcall; struct SymbolTableEntry* ent; unsigned un; char *str; char *id; int intconst; struct forprefix_t *for_prefix; double doubleconst; struct expr* expression; struct indexed* indexed_el; struct stmt_t *statement;}

%token IF WHILE ELSE AND OR RETURN FOR FUNCTION BREAK CONTINUE NOT _LOCAL TRUE FALSE NIL
%token ASSIGNMENT ADD SUB MUL DIV MOD EQUAL NOTEQUAL PLUS_PLUS MINUS_MINUS GREATER LESS GREATEROREQUAL LESSOREQUAL UMINUS
%token CURLYOPENBRACKET CURLYCLOSEDBRACKET OPENBRACKET CLOSEDBRACKET OPENPARENTHESIS CLOSEDPARENTHESIS SEMICOLON COMMA
%token UPDOWNDOT UPDOWNDOUBLEDOT DOT DOUBLEDOT

%token <str> STRING
%token <intconst> INTCONST
%token <doubleconst> DOUBLECONST
%token <id> IDENTIFIER


%type <expression> lvalue
%type <expression> assignexpr
%type <expression> const
%type <expression> idlist
%type <str> funcname
%type <un> funcbody
%type <ent> funcprefix
%type <ent> funcdef
%type <un> ifprefix
%type <un> elseprefix
%type <statement> ifstmt
%type <un> whilestart
%type <un> whilecond
%type <un> N
%type <un> M
%type <statement> stmt
%type <statement> loopstmt
%type <statement> block
%type <statement> stmtlist
%type <for_prefix> forprefix
%type <expression> expr
%type <expression> term
%type <expression> objectdef
%type <expression> elist
%type <expression> member
%type <expression> primary
%type <expression> call
%type <smthcall> callsuffix
%type <smthcall> normcall
%type <smthcall> methodcall
%type <indexed_el> indexedelem


%right ASSIGNMENT
%left OR
%left AND
%nonassoc EQUAL NOTEQUAL
%nonassoc GREATER GREATEROREQUAL LESS LESSOREQUAL
%left ADD SUB
%left MUL DIV MOD
%right NOT PLUS_PLUS MINUS_MINUS UMINUS
%left DOT DOUBLEDOT
%left OPENBRACKET CLOSEDBRACKET
%left OPENPARENTHESIS CLOSEDPARENTHESIS

%%

program: stmtlist           {/*printf("stmtlist -> program\n");*/};

stmt: expr SEMICOLON        {
                                //printf("expr SEMICOLON -> stmt\n");
                                if($1!=NULL && $1->type==constbool_e){
                                    $1 = evalBool($1,yylineno);
                                }
                                $$=NULL;
                            }

    | ifstmt                {
                               // printf("ifstmt -> stmt\n");
                                $$=$1;
                            }

    | whilestmt             {
                               // printf("whilestmt -> stmt\n");
                                $$=NULL;
                            }

    | forstmt               {
                               // printf("forstmt -> stmt\n");
                                $$=NULL;
                            }

    | returnstmt            {
                               // printf("returnstmt -> stmt\n");
                                $$=NULL;
                            }

    | BREAK SEMICOLON       {
                               // printf("BREAK SEMICOLON -> stmt\n");
                                $$=evalBreak(yylineno, loopcounter);
                            }

    | CONTINUE SEMICOLON    {
                               // printf("CONTINUE SEMICOLON -> stmt\n");
                                $$=evalContinue(yylineno, loopcounter);
                            }

    | block                 {
                               // printf("block -> stmt\n");
                                $$=$1;
                            }

    | funcdef               {
                                //printf("funcdef -> stmt\n");
                                $$=NULL;
                            }

    | SEMICOLON             {
                               // printf("SEMICOLON -> stmt\n");
                                $$=NULL;
                            }
    ;


stmtlist: stmtlist stmt         {
                                    //cout << "stmtlist stmt -> stmtlist\n";
                                    resettemp();
                                    $$=evalStmtList($1, $2);
                                }

        | stmt                  {
                                    //cout << "stmt -> stmtlist\n";
                                    resettemp();
                                    $$=$1;
                                }
        ;


expr: assignexpr                {
                                    //printf("assignexpr -> expr\n");
                                    $$=$1;
                                }

    | expr ADD expr             {
                                    //printf("expr + expr -> expr\n");
                                    $$=evalArithmetic(add, $1, $3, yylineno);
                                    if(check_arith($1, yylineno) && check_arith($3, yylineno))
                                        if($1->sym!=NULL)
                                            if($1->sym->type==USERFUNC || $1->sym->type==LIBFUNC)
                                                comperror("Cannot use function as operand", yylineno);
                                }

    | expr SUB expr             {
                                    //printf("expr - expr -> expr\n");
                                    $$=evalArithmetic(sub, $1, $3, yylineno);
                                    if(check_arith($1, yylineno) && check_arith($3, yylineno))
                                        if($1->sym!=NULL)
                                            if($1->sym->type==USERFUNC || $1->sym->type==LIBFUNC)
                                                comperror("Cannot use function as operand", yylineno);
                                }

    | expr MUL expr             {
                                   // printf("expr * expr -> expr\n");
                                    $$=evalArithmetic(mul, $1, $3, yylineno);
                                    if(check_arith($1, yylineno) && check_arith($3, yylineno))
                                        if($1->sym!=NULL)
                                            if($1->sym->type==USERFUNC || $1->sym->type==LIBFUNC)
                                                comperror("Cannot use function as operand", yylineno);
                                }

    | expr DIV expr             {
                                    //printf("expr / expr -> expr\n");
                                    $$=evalArithmetic(_div, $1, $3, yylineno);
                                    if(check_arith($1, yylineno) && check_arith($3, yylineno))
                                        if($1->sym!=NULL)
                                            if($1->sym->type==USERFUNC || $1->sym->type==LIBFUNC)
                                                comperror("Cannot use function as operand", yylineno);
                                }

    | expr MOD expr             {
                                    //printf("expr mod expr -> expr\n");
                                    $$=evalArithmetic(mod, $1, $3, yylineno);
                                    if(check_arith($1, yylineno) && check_arith($3, yylineno))
                                        if($1->sym!=NULL)
                                            if($1->sym->type==USERFUNC || $1->sym->type==LIBFUNC)
                                                comperror("Cannot use function as operand", yylineno);
                                }

    | expr GREATER expr         {
                                    //printf("expr > expr -> expr\n");
    
                                    $$ =evalrelop( $1 , $3,iopcode::if_greater, yylineno);
                                    
                                }

    | expr GREATEROREQUAL expr  {
                                    //printf("expr >= expr -> expr\n");
                                    
                                    $$ = evalrelop(  $1 , $3,iopcode::if_greatereq, yylineno);
                                }

    | expr LESS expr            {
                                    //printf("expr < expr -> expr\n");
                                    
                                     $$ = evalrelop(  $1 , $3,iopcode::if_less, yylineno);
                                     
                                }

    | expr LESSOREQUAL expr     {
                                    //printf("expr <= expr -> expr\n");
                                   
                                    $$ = evalrelop(  $1 , $3,iopcode::if_lesseq, yylineno);
                                }

    | expr EQUAL expr           {
                                    //printf("expr ==  expr -> expr\n");
                                    $$=evalBool($1, yylineno);
                                    $$=evalBool($3, yylineno);

                                    $$ = evalrelop($1 , $3, iopcode::if_eq, yylineno);
                                }

    | expr NOTEQUAL expr        {
                                    //printf("expr != expr -> expr\n");
                                    $$=evalBool($1, yylineno);
                                    $$=evalBool($3, yylineno);
                                    $$ = evalrelop(  $1 , $3,iopcode::if_noteq, yylineno);
                                }

    | expr AND {$1 = convert_to_bool($1,nextquadlabel(),nextquadlabel()+1,yylineno);} M expr           {
                                    //printf("expr && expr -> expr\n");
                                   
                                    $5 = convert_to_bool($5,nextquadlabel(),nextquadlabel()+1,yylineno);
                                    $$ = evalAnd($1 ,$5,$M, yylineno);
                                    
                                }

    | expr OR {$1 = convert_to_bool($1,nextquadlabel(),nextquadlabel()+1,yylineno);}M expr             {
                                    //cout << "expr or expr -> expr\n";
                                  
                                    $5 = convert_to_bool($5,nextquadlabel(),nextquadlabel()+1,yylineno);
                                    $$ = evalOr($1,$5,$M, yylineno);
                                }

    | term                      {
                                    //cout << "term -> expr\n";
                                    $$=$1;
                                }
    ;

term: OPENPARENTHESIS expr CLOSEDPARENTHESIS        {
                                                        //printf("OPENPARENTHESIS expr CLOSEDPARENTHESIS -> term\n");
                                                        $$=$2;
                                                    }

    | SUB expr %prec UMINUS                         {
                                                        //printf("SUB expr prec UMINUS -> term\n");
                                                        check_arith($2, yylineno);
                                                        if($2->sym!=NULL)
                                                            if($2->sym->type==USERFUNC || $2->sym->type==LIBFUNC)
                                                                comperror("Cannot use function as operand", yylineno);
                                                        $$=evalUminus($2, yylineno);
                                                    }

    | NOT expr                                      {
                                                        //printf("NOT expr -> term\n");
                                                        $2 = convert_to_bool($2,nextquadlabel(),nextquadlabel()+1,yylineno);
                                                        $$=evalNot($2, yylineno);
                                                    }

    | PLUS_PLUS lvalue                              {
                                                        /*if (is_func)
                                                            printf(RED "Error: Cannot do operations with functions, error line: %d\n" WHITE, yylineno);*/
                                                        //cout << "PLUS_PLUS lvalue -> term\n";
                                                        check_arith($2, yylineno);
                                                        if($2->sym!=NULL)
                                                            if($2->sym->type==USERFUNC || $2->sym->type==LIBFUNC)
                                                                comperror("Cannot use function as operand", yylineno);
                                                        $$=evalLeftPlusPlus($2, yylineno);
                                                    }

    | lvalue PLUS_PLUS                              {
                                                        /*if (is_func)
                                                            printf(RED "Error: Cannot do operations with functions, error line: %d\n" WHITE, yylineno);*/
                                                         //cout <<"lvalue PLUS_PLUS -> term\n";
                                                        check_arith($1, yylineno);
                                                        if($1->sym!=NULL)
                                                            if($1->sym->type==USERFUNC || $1->sym->type==LIBFUNC)
                                                                comperror("Cannot use function as operand", yylineno);
                                                        $$=evalRightPlusPlus($1, yylineno);
                                                    }

    | MINUS_MINUS lvalue                            {
                                                        /*if (is_func)
                                                            printf(RED "Error: Cannot do operations with functions, error line: %d\n" WHITE, yylineno);*/
                                                        //cout << "MINUS_MINUS lvalue -> term\n";
                                                        check_arith($2, yylineno);
                                                        if($2->sym!=NULL)
                                                            if($2->sym->type==USERFUNC || $2->sym->type==LIBFUNC)
                                                                comperror("Cannot use function as operand", yylineno);
                                                        $$=evalLeftMinusMinus($2, yylineno);
                                                    }

    | lvalue MINUS_MINUS                            {
                                                        /*if (is_func)
                                                            printf(RED "Error: Cannot do operations with functions, error line: %d\n" WHITE, yylineno);*/
                                                        //cout << "lvalue MINUS_MINUS -> term\n";
                                                        check_arith($1, yylineno);
                                                        if($1->sym!=NULL)
                                                            if($1->sym->type==USERFUNC || $1->sym->type==LIBFUNC)
                                                                comperror("Cannot use function as operand", yylineno);
                                                        $$=evalRightMinusMinus($1, yylineno);
                                                    }

    | primary                                       {
                                                        //cout << "primary -> term\n";
                                                        $$=$1;
                                                    }
    ;

assignexpr: lvalue ASSIGNMENT expr                  { 

                                                        check_func_assign($1->sym, yylineno);

                                                        
                                                        //cout << "lvalue ASSIGNMENT expr -> assignexpr\n";
                                                        evalBool($3, yylineno);
                                                        $$=evalAssign($1, $3, yylineno);
                                                    }
          ;

primary: lvalue                                     {
                                                        //cout << "lvalue -> primary\n";
                                                        $$=emit_iftableitem($1, yylineno);
                                                    }

       | call                                       {
                                                        //cout << "call -> primary\n";
                                                        $$=$1;
                                                    }

       | objectdef                                  {
                                                        //cout << "objectdef -> primary\n";
                                                        $$=$1;
                                                    }

       | OPENPARENTHESIS funcdef CLOSEDPARENTHESIS  {
                                                        //cout << "OPENPARENTHESIS funcdef CLOSEDPARENTHESIS -> primary\n";
                                                        $$=newexpr(programfunc_e);
                                                        $$->sym=$2;
                                                    }

       | const                                      {
                                                        //cout << "const -> primary\n";
                                                        $$=$1;
                                                    }
       ;

lvalue: IDENTIFIER                                  {
                                                        //cout << "IDENTIFIER " << $1 << " -> lvalue\n";
                                                        /*is_func=is_functionn($1);*/
                                                        $$=newexpr(var_e);
                                                        //SymbolTableEntry* tmp=scope_look_up($1, symbol_scope);
                                                        $$->sym=insert_to_table($1, SymbolType::VAR, yylineno, symbol_scope);
                                                        
                                                        //temp_entry=scope_look_up($1, symbol_scope);
                                                        if($$->sym!=NULL){
                                                            //cout << $$->sym->id << endl;
                                                            inccurrscopeoffset();
                                                        }
                                                        if($$->sym==NULL){
                                                            for(int i=symbol_scope; i>=0; i--){
                                                                $$->sym=scope_look_up($1, i);
                                                                if($$->sym!=NULL)
                                                                    break;
                                                            }
                                                        }
                                                        assert($$->sym!=NULL);
                                                       
                                                    }

      | _LOCAL IDENTIFIER                           {
                                                        //cout << "_LOCAL IDENTIFIER " << $2 << " -> lvalue\n";
                                                        /*is_func=is_functionn($2);*/
                                                        $$=newexpr(var_e);
                                                        //SymbolTableEntry* tmp=scope_look_up($2, symbol_scope);
                                                        $$->sym=insert_to_table($2, SymbolType::LOCAL, yylineno, symbol_scope);
                                                        
                                                        //temp_entry=scope_look_up($2, symbol_scope);
                                                        if($$->sym!=NULL){
                                                           // cout << $$->sym->id << endl;
                                                            inccurrscopeoffset();
                                                        }
                                                        if($$->sym==NULL){
                                                            for(int i=symbol_scope; i>=0; i--){
                                                                $$->sym=scope_look_up($2, i);
                                                                if($$->sym!=NULL)
                                                                    break;
                                                            }
                                                        }
                                                        assert($$->sym!=NULL);
                                                        /*if($$->sym==NULL)
                                                            $$->sym=temp_entry;*/
                                                    }

      | UPDOWNDOUBLEDOT IDENTIFIER                  {
                                                        //cout << "GLOBAL IDENTIFIER " << $2 << " -> lvalue\n";
                                                        /*is_func=is_functionn($2);*/
                                                        $$=newexpr(var_e);
                                                        $$->sym=insert_to_table($2, SymbolType::GLOBAL, yylineno, symbol_scope);
                                                        $$->sym=look_up($2);
                                                    }

      | member                                      {
                                                        //cout << "member -> lvalue\n";
                                                        $$=$1;
                                                    }
      ;


member: lvalue DOT IDENTIFIER                           {
                                                            //cout << "lvalue DOT IDENTIFIER ( << $3 <<) -> tableitem\n";
                                                            $$=member_item($1, $3, yylineno);
                                                        }

        | lvalue OPENBRACKET expr CLOSEDBRACKET         {
                                                            //cout << "lvalue OPENBRACKET expr CLOSEDBRACKET -> member\n";
                                                            $3=stringBoolToBool($3);
                                                            $$=evalArray($1, $3, yylineno);
                                                        }

        | call DOT IDENTIFIER                           {
                                                            //cout << "call DOT IDENTIFIER -> member\n";
                                                            string name($3);
                                                            $$=member_item($1, name, yylineno);
                                                        }
        | call OPENBRACKET expr CLOSEDBRACKET           {
                                                            //cout << "call OPENBRACKET expr CLOSEDBRACKET -> member\n";
                                                            evalBool($3, yylineno);
                                                            $1=emit_iftableitem($1, yylineno);
                                                            $$=newexpr(tableitem_e);
                                                            $$->sym=$1->sym;
                                                            $$->index=$3;
                                                        }
        ;


call: call OPENPARENTHESIS elist CLOSEDPARENTHESIS      {
                                                            //cout << "call OPENPARENTHESIS elist CLOSEDPARENTHESIS -> call\n";
                                                            $$=make_call($$, $3, yylineno);
                                                        }

    | lvalue callsuffix                                 {
                                                            //cout << "lvalue callsuffix -> call\n";
                                                            if($1->sym==NULL)
                                                                $1->sym=temp_entry;
                                                            $$=evalLvalueCallsufix($1, $2, yylineno);
                                                            
                                                        }
    | OPENPARENTHESIS funcdef CLOSEDPARENTHESIS OPENPARENTHESIS elist CLOSEDPARENTHESIS {
                                                             //cout << "OPENPARENTHESIS funcdef CLOSEDPARENTHESIS OPENPARENTHESIS elist CLOSEDPARENTHESIS -> call\n";
                                                            expr* func=newexpr(programfunc_e);
                                                            func->sym=$2;
                                                            $$=make_call(func, $5, yylineno);
                                                        }

    | OPENPARENTHESIS funcdef CLOSEDPARENTHESIS OPENPARENTHESIS CLOSEDPARENTHESIS       {
                                                                                             //cout << "OPENPARENTHESIS funcdef CLOSEDPARENTHESIS OPENPARENTHESIS CLOSEDPARENTHESIS -> call\n";
                                                                                            expr* func=newexpr(programfunc_e);
                                                                                            func->sym=$2;
                                                                                            $$=make_call(func, NULL, yylineno);
                                                                                        }

    | call OPENPARENTHESIS CLOSEDPARENTHESIS                                            {
                                                                                             //cout << "call OPENPARENTHESIS CLOSEDPARENTHESIS -> call\n";
                                                                                            $$=make_call($$, NULL, yylineno);
                                                                                        }
    ;

callsuffix: normcall                                {
                                                         //cout << "normcall -> callsuffix\n";
                                                        $$ = $1;    
                                                    }

          | methodcall                              {
                                                         //cout << "methodcall -> callsuffix\n";
                                                        $$ = $1;
                                                    }
          ;

normcall: OPENPARENTHESIS elist CLOSEDPARENTHESIS   {
                                                         //cout << "OPENPARENTHESIS elist CLOSEDPARENTHESIS -> normcall\n";
                                                        $$=evalNormCall($2);
                                                    }

        | OPENPARENTHESIS CLOSEDPARENTHESIS         {
                                                         //cout << "OPENPARENTHESIS CLOSEDPARENTHESIS -> normcall\n";
                                                        $$=evalNormCall(NULL);
                                                    }
        ;               

methodcall: DOUBLEDOT IDENTIFIER OPENPARENTHESIS elist CLOSEDPARENTHESIS  {
                                                                    string n($2);
                                                                    n=isolate_func_name(n);
                                                                     //cout << "DOUBLEDOT IDENTIFIER " << n << " OPENPARENTHESIS elist CLOSEDPARENTHESIS\n";
                                                                    $$=evalMethodCall(n, $4);
                                                                }
          | DOUBLEDOT IDENTIFIER OPENPARENTHESIS CLOSEDPARENTHESIS       {

                                                                    string n($2);
                                                                    n=isolate_func_name(n);
                                                                     //cout << "DOUBLEDOT IDENTIFIER " << n << " OPENPARENTHESIS CLOSEDPARENTHESIS -> methodcall\n";
                                                                    $$=evalMethodCall(n, NULL);
                                                                }
          ;

elist: expr                                         {
                                                         //cout << "expr -> elist\n";
                                                      
                                                        evalBool($1, yylineno);
                                                        $$=$1;
                                                    }

     | elist COMMA expr                             {
                                                         //cout << "expr COMMA elist -> elist\n";
                                                      
                                                        evalBool($3, yylineno);
                                                        $3->next=$1;
                                                        $$=$3;
                                                    }
     ;

objectdef: OPENBRACKET elist CLOSEDBRACKET          {
                                                         //cout << "OPENBRACKET elist CLOSEDBRACKET -> objectdef\n";
                                                        $2=stringBoolToBool($2);
                                                        $$=evalArrayDef($2, yylineno);
                                                    }

         | OPENBRACKET indexed CLOSEDBRACKET        {
                                                         //cout << "OPENBRACKET indexed CLOSEDBRACKET -> objectdef\n";
                                                        $$=evalIndexedObj(indexed_list, yylineno);
                                                    }

         | OPENBRACKET CLOSEDBRACKET                {
                                                         //cout << "OPENBRACKET CLOSEDBRACKET -> objectdef\n";
                                                        $$=evalArrayDef(NULL, yylineno);
                                                    }
         ;  

indexed: indexedelem                                {
                                                         //cout << "indexedelem -> indexed\n";
                                                        indexed_list.clear();
                                                        indexed_list.push_front($1);
                                                    }

       | indexedelem COMMA indexed                  {
                                                         //cout << "indexdelement COMMA indexed -> indexed\n";
                                                        indexed_list.push_front($1);
                                                    }
       ;    

indexedelem: CURLYOPENBRACKET expr UPDOWNDOT expr CURLYCLOSEDBRACKET {
                                                        evalBool($2,yylineno);
                                                        evalBool($4,yylineno);
                                                         //cout << "CURLYOPENBRACKET expr UPDOWNDOT expr CURLYCLOSEDBRACKET -> indexdelement\n";
                                                        $4=stringBoolToBool($4);
                                                        $$=new_indexed($2, $4);
                                                    }
           ;        


block: CURLYOPENBRACKET {symbol_scope++;} stmtlist CURLYCLOSEDBRACKET   {
                                                                            hide(symbol_scope);
                                                                            symbol_scope--;
                                                                             //cout << "CURLYOPENBRACKET stmtlist CURLYCLOSEDBRACKET -> block\n";
                                                                            $$=$3;
                                                                        }

     | CURLYOPENBRACKET {symbol_scope++;} CURLYCLOSEDBRACKET            {
                                                                            hide(symbol_scope);
                                                                            symbol_scope--;
                                                                             //cout << "CURLYOPENBRACKET CURLYCLOSEDBRACKET -> block\n";
                                                                            $$=new_statement();
                                                                        }
     ;


funcdef: funcprefix M funcargs funcbody {
                                            //cout << "funcprefix M funcargs funcbody -> funcdef\n";
                                            $$=evalFuncDef($1, $2, $4, yylineno);
                                        };


funcargs: OPENPARENTHESIS {symbol_scope++; } idlist {enterScopeSpace(); resetfunctionlocaloffset(); symbol_scope--;} CLOSEDPARENTHESIS {/*cout << "OPENPARENTHESIS idlist CLOSEDPARENTHESIS -> funcargs\n";*/};


funcbody: {loopcounterstack.push(loopcounter); loopcounter=0;} block {
                                                                                             //cout << "FUNCTION IDENTIFIER OPENPARENTHESIS idlist CLOSEDPARENTHESIS block -> funcdef\n";
                                                                                            $$=evalFuncbody(&loopcounter);    
                                                                                        };

funcprefix: FUNCTION funcname   {
                                    //cout << "FUNCTION funcname -> funcprefix\n";
                                    $$=evalFuncprefix($2, yylineno, symbol_scope);
                                };

funcname: IDENTIFIER {  
                        //cout << "IDENTIFIER -> funcname\n";
                        int n = strlen($1);
                        $$=(char*)malloc(n*sizeof(char)+1);
                        strcpy($$,$1);
                     }

        |            {
                        //cout << "(no id) -> funcname\n";
                        string res = "$"+to_string(anonymous_func_num);
                        int n = res.length();
                        char *func_name = (char*)malloc(n*sizeof(char)+1);
                        strcpy(func_name,res.c_str()); 
                        $$=(char*)malloc(n*sizeof(char));
                        strcpy($$,func_name);
                        anonymous_func_num++;
                    };


const: INTCONST                 {
                                     //cout << "intconst (%d) -> const\n", $1;
                                    $$=newexpr(constint_e);
                                    $$->intConst=$1;
                                }
                                
      | DOUBLECONST             {
                                     //cout << "doubleconst (%f) -> const\n", $1;
                                    $$=newexpr(constdouble_e);
                                    $$->doubleConst=$1;
                                }

      | STRING                  {
                                     //cout << "string (%s) -> const\n", $1;
                                    $$=newexpr(conststring_e);
                                    string temp($1);
                                    $$->strConst=temp;
                                }

      | NIL                     {
                                     //cout << "nil -> const\n";
                                    $$=newexpr(nil_e);
                                }

      | TRUE                    {
                                     //cout << "true -> const\n";
                                    $$=newexpr_conststring("true");
                                    
                                }

      | FALSE                   {
                                     //cout << "false -> const\n";
                                    $$=newexpr_conststring("false");
  
                                }
      ;

idlist: IDENTIFIER                  {
                                        //cout << "IDENTIFIER -> idlist\n";
                                        $$=newexpr(var_e);
                                        $$->sym=insert_to_table($1, SymbolType::FORMAL, yylineno, symbol_scope);
                                        inccurrscopeoffset();
                                    }

      | idlist COMMA IDENTIFIER     {
                                         //cout << "idlist COMMA idlist -> idlist\n";
                                        $$=newexpr(var_e);
                                        $$->sym=insert_to_table($3, SymbolType::FORMAL, yylineno, symbol_scope);
                                        inccurrscopeoffset();
                                    }

      |                             {
                                         //cout << "EMPTY id list\n";
                                    }
      ;
ifprefix: IF OPENPARENTHESIS expr CLOSEDPARENTHESIS {
                                                         //cout << "IF OPENPARENTHESIS expr CLOSEDPARENTHESIS -> ifprefix\n";
                                                        evalBool($3,yylineno);
                                                        emit(iopcode::if_eq, $3, newexpr_constbool(1), NULL, nextquadlabel() + 2, yylineno);
                                                        $$ = nextquadlabel();
                                                        emit(iopcode::jump, NULL, NULL, NULL, 0, yylineno);
                                                    }
        ;

elseprefix: ELSE                                    {
                                                         //cout << "elseprefix\n";
                                                        $$ = nextquadlabel();
                                                        emit(iopcode::jump, NULL, NULL, 0, 0, yylineno);
                                                    }
        ;
                                                    
ifstmt: ifprefix stmt                               {
                                                         //cout << "ifprefix stmt -> ifstmt\n";
                                                        patchlabel($1, nextquadlabel());
                                                        $$=$2;
                                                    }

      | ifprefix stmt elseprefix stmt               {
                                                         //cout << "ifprefix stmt ELSE -> ifstmt\n";
                                                        int tmp_bl1=0, tmp_bl2=0, tmp_cl1=0, tmp_cl2=0;
                                                        patchlabel($1, $3 + 1);
                                                        patchlabel($3, nextquadlabel());
                                                        $$=new_statement();
                                                        if($2!=NULL){
                                                            tmp_bl1=$2->breaklist;
                                                            tmp_cl1=$2->contlist;
                                                        }
                                                
                                                        if($4!=NULL){
                                                            tmp_bl2 = $4->breaklist;
                                                            tmp_cl2 = $4->contlist;
                                                        }

                                                        $$->breaklist=mergelist(tmp_bl1, tmp_bl2);
                                                        $$->contlist=mergelist(tmp_cl1, tmp_cl2);
                                                    }
      ;

loopstart: {++loopcounter;};

loopend: {--loopcounter;};

loopstmt: loopstart stmt loopend {$$=$2;};

whilestart: WHILE                       {
                                             //cout << "while -> whilestart\n";
                                            $$ = nextquadlabel();
                                        }
        ;

whilecond: OPENPARENTHESIS expr CLOSEDPARENTHESIS   {
                                                        evalBool($2,yylineno);
                                                        $2=stringBoolToBool($2);
                                                        
                                                         //cout << "OPEN PARENTHESIS expr CLOSEDPARENTHESIS -> whilecond\n";
                                                        emit(iopcode::if_eq, $2, newexpr_constbool(1), NULL, nextquadlabel() + 2, yylineno);
                                                        $$ = nextquadlabel();
                                                        emit(iopcode::jump, NULL, NULL, NULL, 0, yylineno);
                                                    }
        ;
whilestmt: whilestart whilecond loopstmt            {      
                                                         //cout << "whilestart whilecond stmt -> whilestmt\n";
                                                        evalWhilestmt($1, $2, $3, yylineno);
                                                    }
         ;


N:                                                  {
                                                         //cout << "N (for next quad)\n";
                                                        $$=nextquadlabel();
                                                        emit(jump, NULL, NULL, NULL, 0, yylineno);
                                                    }
 ;

M:                                                  { 
                                                         //cout << "M (for next quad)\n";                                                        
                                                        $$=nextquadlabel();
                                                    }
 ;

forprefix: FOR OPENPARENTHESIS elist SEMICOLON M expr SEMICOLON                {
                                                                                     //cout << "forprefix for: for stmt found\n";
                                                                                    evalBool($6, yylineno);
                                                                                    $$=evalForPref($6, $5, yylineno);
                                                                                }

         | FOR OPENPARENTHESIS SEMICOLON M expr SEMICOLON                       {
                                                                                     //cout << "forprefix for: for stmt found ver 2\n";
                                                                                    evalBool($5, yylineno);
                                                                                    $$=evalForPref($5, $4, yylineno);
                                                                                }
                                                                  
                                                                              
        ;


forstmt:  forprefix N elist CLOSEDPARENTHESIS N {++loopcounter;} stmt {--loopcounter;} N    {
                                                                                                //cout << "for stmt found\n";
                                                                                                evalForStmt($1, $2, $5, $9, $7); 
                                                                                            }

        | forprefix N CLOSEDPARENTHESIS N {++loopcounter;} stmt {--loopcounter;} N      {
                                                                                            //cout << "for stmt found ver 2\n";
                                                                                            evalForStmt($1, $2, $4, $8, $6);
                                                                                        }
        ;


returnstmt: RETURN SEMICOLON            {
                                             //cout << "RETURN -> returnstmt found\n";
                                            if(!is_func_stack_empty()){
                                                comperror("Return not in function error", yylineno);
                                            }
                                            emit(ret, NULL, NULL, NULL, 0, yylineno);
                                        }

          | RETURN expr SEMICOLON       {
                                             //cout << "RETURN expr SEMICOLON -> returnstmt found\n";
                                            if(!is_func_stack_empty()){
                                                comperror("Return not in function error", yylineno);
                                            }
                                            $2=evalBool($2, yylineno);
                                            emit(ret, $2, NULL, NULL, 0, yylineno);
                                        }
          ;     


%%

void yyerror(char* msg){
    printf(RED "%s, error line: %d\n" WHITE, msg, yylineno);
    incr_error();
    return;
}


int main(int argc, char **argv){
    extern FILE *yyin;
	cout << "############ FINAL ############\n";
    if(argc == 3){
       out_flag = true;
       file_name = argv[2];
    }

    yyin=fopen(argv[1],"r");

    initialize();
    
    yyparse();
    int errors=get_errors();
    if(errors>0){
        if(errors==1)
            printf(RED "%d error\n" WHITE, get_errors());
        else
            printf(RED "%d errors\n" WHITE, get_errors());
        exit(0);
    }
    print();
    
    print_quads();

    generate_quads();

    print_instructions();

    string name(argv[1]);
    print_instructions_bin(name);


    fclose(yyin);
}
