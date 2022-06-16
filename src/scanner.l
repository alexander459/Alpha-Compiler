%option noyywrap
%option yylineno
%option outfile= "scanner.cpp"
%{
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <bits/stdc++.h>
#include "utils/util.hpp"
#include "parser.hpp"
#include "tokenlist/tokenlist.hpp"

using namespace std;

extern int yylex();


unsigned int num_of_comment = 0;
unsigned int numtoken = 0;
unsigned int nesting = 0;

unsigned int beginning_of_comment = 0;

unsigned int string_line=1;

bool is_valid = true;

string str = "";
%}

%x MULTI_COMMENT
%x STRINGS

newline \n
integer [0-9]*
double [0-9]*"."[0-9]*
identifier  [a-zA-Z][a-zA-Z_0-9]*
comment "//".*


%%

"if"        {return IF;}
"while"     {return WHILE;}
"else"      {return ELSE;}
"and"       {return AND;}
"or"        {return OR;}
"return"    {return RETURN;}
"for"       {return FOR;}
"function"  {return FUNCTION;}
"break"     {return BREAK;}
"continue"  {return CONTINUE;}
"not"       {return NOT;}
"local"     {return _LOCAL;}
"true"      {return TRUE;}
"false"     {return FALSE;}
"nil"       {return NIL;}

"$"|"_d"|"~"|"?"|"#"|"&&"|"||"|"!" {string s(yytext); comperror("Error: Invalid character " + s, yylineno);}


[\"]([^"]|\\.)*[\"] {
            if(!is_valid_string(yytext)){
              comperror("Error: Invalid escape sequence in string", string_line);
            }
            str=format_str(yytext);
  					yylval.str = (char*)str.c_str();
						return STRING;
					}
         
[\"][^"]* {comperror("Error: Not terminating string", string_line);}

{integer} {insert(yytext,"INTCONST", yylineno); yylval.intconst=atoi(yytext); return INTCONST;}

{identifier} {insert(yytext,"IDENTIFIER", yylineno); yylval.id=yytext; return IDENTIFIER;}

{comment} {insert("", "COMMENT", yylineno);}

"/*"             {numtoken++; num_of_comment = 0; nesting = 0; update_token_num(numtoken); beginning_of_comment = yylineno; BEGIN(MULTI_COMMENT);}
<MULTI_COMMENT>{
  "/*"           {numtoken++; update_token_num(numtoken); nesting++; num_of_comment++;}
  <<EOF>>        {comperror("Error: End of file with open comment", beginning_of_comment);}
  "*/"           {if(nesting) nesting--;
                  else {
                    if(num_of_comment) {
                      insert("", "MULTI LINE COMMENT", yylineno);
                      if(num_of_comment>=1){
                        for(unsigned int i=0; i<num_of_comment; ++i)
                          insert("", "NESTED COMMENT", yylineno);
                      }
                    }else insert("", "MULTI LINE COMMENT", yylineno);
                    BEGIN(INITIAL);
                  }
                }

  "\n"           {}
   .             {}
}


"="  {insert(yytext, "OPERATOR", yylineno); return ASSIGNMENT;}
"+"  {insert(yytext, "OPERATOR", yylineno); return ADD;}
"-"  {insert(yytext, "OPERATOR", yylineno); return SUB;}
"*"  {insert(yytext, "OPERATOR", yylineno); return MUL;}
"/"  {insert(yytext, "OPERATOR", yylineno); return DIV;}
"%"  {insert(yytext, "OPERATOR", yylineno); return MOD;}
"==" {insert(yytext, "OPERATOR", yylineno); return EQUAL;}
"!=" {insert(yytext, "OPERATOR", yylineno); return NOTEQUAL;}
"++" {insert(yytext, "OPERATOR", yylineno); return PLUS_PLUS;}
"--" {insert(yytext, "OPERATOR", yylineno); return MINUS_MINUS;}
">"  {insert(yytext, "OPERATOR", yylineno); return GREATER;}
"<"  {insert(yytext, "OPERATOR", yylineno); return LESS;}
">=" {insert(yytext, "OPERATOR", yylineno); return GREATEROREQUAL;}
"<=" {insert(yytext, "OPERATOR", yylineno); return LESSOREQUAL;}

"{"  {insert(yytext, "PUNCTUATIONMARK", yylineno); return CURLYOPENBRACKET;}
"}"  {insert(yytext, "PUNCTUATIONMARK", yylineno); return CURLYCLOSEDBRACKET;}
"["  {insert(yytext, "PUNCTUATIONMARK", yylineno); return OPENBRACKET;}
"]"  {insert(yytext, "PUNCTUATIONMARK", yylineno); return CLOSEDBRACKET;}
"("  {insert(yytext, "PUNCTUATIONMARK", yylineno); return OPENPARENTHESIS;}
")"  {insert(yytext, "PUNCTUATIONMARK", yylineno); return CLOSEDPARENTHESIS;}
";"  {insert(yytext, "PUNCTUATIONMARK", yylineno); return SEMICOLON;}
","  {insert(yytext, "PUNCTUATIONMARK", yylineno); return COMMA;}
":"  {insert(yytext, "PUNCTUATIONMARK", yylineno); return UPDOWNDOT;}
"::" {insert(yytext, "PUNCTUATIONMARK", yylineno); return UPDOWNDOUBLEDOT;}
"."  {insert(yytext, "PUNCTUATIONMARK", yylineno); return DOT;}
".." {insert(yytext, "PUNCTUATIONMARK", yylineno); return DOUBLEDOT;}


"\n" {string_line++;}

. {}

{double} {insert(yytext,"DOUBLECONST", yylineno); yylval.doubleconst=strtod(yytext, NULL); return DOUBLECONST;}

<<EOF>> return EOF;
%%