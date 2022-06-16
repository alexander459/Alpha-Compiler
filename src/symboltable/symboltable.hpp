
#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H
#include "../enums.hpp"
#include "../structs.hpp"
#include <list>
#include <array>
#include <stack>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;



string newtempname(void);
SymbolTableEntry* newtemp(int line);
void resettemp(void);
SymbolTableEntry* insert_temp_var(string name, unsigned int scope, int line);

//function definitions
unsigned int string_to_hash(string symbol);
unsigned int hash_function(unsigned const int key);
void hide(unsigned int scope);
SymbolTableEntry *look_up(string symbol);
SymbolTableEntry *scope_look_up(string symbol, unsigned int current_scope);
bool is_functionn(string id);
SymbolTableEntry *check_definitions(SymbolTableEntry *node, int line);
SymbolTableEntry *insert_to_table(string symbol, SymbolType type, unsigned int line, unsigned int symbol_scope);
//SymbolTableEntry *var_look_up(string symbol);
void push_to_stack(SymbolTableEntry* func_entry);
void pop_from_stack(void);
void calculate(void);
void initialize(void);
void print(void);
int currscope(void);
int get_num_of_globals(void);

unsigned currscopeoffset(void);
//void inccurrscopeoffset(void);

enum scopespace_t currscopespace(void);
void enterScopeSpace(void);
void exitScopeSpace(void);
bool check_lib_cols(string symbol, string type, int line);
bool is_func_stack_empty(void);

#endif

