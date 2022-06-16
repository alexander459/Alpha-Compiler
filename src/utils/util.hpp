#include <string>
#include <fstream>
#include <assert.h>
#include "../structs.hpp"
#define RED "\033[31m"
#define WHITE "\x1B[37m"

#ifndef __util_h__
#define __util_h__

using namespace std;
extern bool out_flag;
extern string file_name;
string str_append(string _str, char character);
string upperCase(string str);

/*Takes a func name. returns false if the name belongs in the lib_functions union, true if not*/
bool check_func(string name);

/*takes a string from lex and checks if its valid (valid escape characters)*/
bool is_valid_string(char* str);
expr_t* stringBoolToBool(expr_t* exp);
/*takes a string and formats all the escape characters*/
string format_str(char* str);

string isolate_func_name(string name);

bool check_arith (struct expr* e, int line);

void comperror (string msg, int line);

string remove_quotes(string s);

bool is_temp_var(string name);

bool is_temp_expr(expr_t* e);

int get_errors(void);
void incr_error(void);

void check_func_assign(SymbolTableEntry* sym, int line);
#endif