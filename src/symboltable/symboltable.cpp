#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <string>
#include <cstring>
#include <time.h>
#include <stack>
#include "symboltable.hpp"
#include <assert.h>
#include "../utils/util.hpp"
#include "../intermediate/intermediate.hpp"
#include "../scopespace/scopespace.hpp"

using namespace std;

unsigned int MAX_SIZE = 300;
unsigned int p, a, b;
unsigned int scope;
Hash_table *my_table = NULL;
unsigned temp_var_counter=0;


string newtempname(void){
	string name;
	name="^" + to_string(temp_var_counter);
	temp_var_counter++;
	return name;
}

SymbolTableEntry* newtemp(int line){
	string name=newtempname();
	SymbolTableEntry *sym=scope_look_up(name, scope);
	if(sym==NULL)
		sym=insert_temp_var(name, scope, line);

	return sym;
}

void resettemp(void){
	temp_var_counter=0;
	return;
}

int get_num_of_globals(void){
	int count=0;
	for(auto it:my_table->table){
		for(auto a:it){
			if(a!=NULL){
				if(a->type!=LIBFUNC && a->type!=USERFUNC && a->scope_space==programm_variables)
					count++;
			}
		}
	}				
	return count;
}

SymbolTableEntry* insert_temp_var(string name, unsigned int scope, int line){
    SymbolTableEntry *node = new SymbolTableEntry;
    node->value.varVal = new Variable;
    node->value.funcVal->name = name;
    node->value.funcVal->line = 0;
    node->value.funcVal->scope = scope;
    node->value.varVal->name = name;
    node->value.varVal->line = line;
    node->value.funcVal->line = line;

    node->value.varVal->scope = scope;
	node->scope_space=currscopespace();
	node->scope_offset=currscopeoffset();
	inccurrscopeoffset();
	
    node->type = SymbolType::LOCAL;
    node->isActive = true;
	node->id=name;
    my_table->table[hash_function(string_to_hash(node->id))].push_back(node);
    my_table->scope_links[scope].push_back(node);
	return node;
}



unsigned int string_to_hash(string symbol)
{
	unsigned int new_num = 0;
	for (int i = 0; symbol[i] != '\0'; i++) {
		new_num += symbol[i];
	}
	return new_num;
}

unsigned int hash_function(unsigned const int key)
{
	if(my_table->count > MAX_SIZE) 
		MAX_SIZE = my_table->count;
	
	unsigned int index = ((a * key + b) % p) % MAX_SIZE;
	return index;
}

void hide(unsigned int scope)
{
	for (auto it : my_table->scope_links[scope]) {
		it->isActive = false;
	}
}

SymbolTableEntry *scope_look_up(string symbol, unsigned int current_scope)
{
    if(current_scope > my_table->scope_links.size())
        return NULL;
    for (auto it : my_table->scope_links[current_scope]) {
        if (it->id == symbol && it->isActive == true) {
            return it;
        }
    }

    return NULL;
}

SymbolTableEntry *look_up(string symbol)
{
	unsigned int index = hash_function(string_to_hash(symbol));
	for (auto it : my_table->table[index]) {
		
		if (it->id == symbol) {
			return it;
		}
	}
	return NULL;
}

void push_to_stack(SymbolTableEntry *func_entry)
{
	my_table->func_stack.push(func_entry);
}

void pop_from_stack(void)
{
	if(my_table->func_stack.empty()) 
		return;
	my_table->func_stack.pop();
}

bool check_lib_cols(string symbol, string type, int line)
{ //true if collision exists
	for (auto it : my_table->scope_links[0]) {
		if (it->id == symbol && it->type == SymbolType::LIBFUNC) {
			comperror("Error : [" + type + "] : " + symbol + " trying to shadow libfunc: " + it->id, line);
			return true;
		}
	}
	return false;
}

bool check_access(SymbolTableEntry *s, SymbolTableEntry *node)
{
	SymbolTableEntry *top = my_table->func_stack.top();
	if (s->value.varVal->scope == scope) {
		return true;
	} else {
		if (s->value.varVal->scope == 0) {
			return true;
		} else {
			unsigned int f_scope = top->value.funcVal->scope + 1;
			if (scope_look_up(s->id, f_scope) != NULL || s->value.varVal->scope > f_scope) {
				return true;
			}
		}
	}
	return false;
}


/*SymbolTableEntry *var_look_up(string symbol){
    unsigned int index = hash_function(string_to_hash(symbol));
	if(my_table->table[index].empty()){
		return NULL;
	}
    for(auto it : my_table->table[index]){
        if(it->id == symbol && (it->type==SymbolType::LOCAL || it->type==SymbolType::GLOBAL || it->type==SymbolType::VAR)){
            return it;
        }
    }
    return NULL;
}*/


bool is_functionn(string id){

	SymbolTableEntry *tmp = new SymbolTableEntry;

	if(( tmp = look_up(id)) != NULL){
		if(tmp->type == SymbolType::USERFUNC || tmp->type == SymbolType::LIBFUNC){
			return true;
		}
	}

	return false; 
}

SymbolTableEntry *check_definitions(SymbolTableEntry *node, int line)
{
	SymbolTableEntry *s = NULL;
	int i = 0;
	bool found = false;
	SymbolType type = node->type;

	switch (type) {
	case SymbolType::VAR: //VAR
		for (i = scope; i >= 0; i--) {
			if ((s = scope_look_up(node->id, i)) != NULL) {
				found = true;
				
				if (s->type == SymbolType::LIBFUNC) {
					return NULL;
				}
				break;
			}
		}
		if (found) {
			if (!my_table->func_stack.empty()) {
				if (s->type == SymbolType::USERFUNC)
					return NULL;
				if (check_access(s, node)) {
					return NULL;
				} else {
					comperror("Error: Can't access " + node->id, line);
					return NULL;
				}
			}
			return NULL;
		} else {
			if (scope == 0) {
				node->type = SymbolType::GLOBAL;
			}
			node->isActive = true;
			my_table->table[hash_function(string_to_hash(node->id))].push_back(node);
			my_table->scope_links[scope].push_back(node);
			return node;
		}
		break;
	case SymbolType::LOCAL: //LOCAL
		s = scope_look_up(node->id, scope);
		if (s == NULL && check_lib_cols(node->id, "variable", line) == false) {
			if (scope == 0) {
				node->type = SymbolType::GLOBAL;
			}
			node->isActive = true;
			my_table->table[hash_function(string_to_hash(node->id))].push_back(node);
			my_table->scope_links[scope].push_back(node);
			return node;
		}
		break;
	case SymbolType::GLOBAL: //GLOBAL
		s = scope_look_up(node->id, 0);
		if (s == NULL) {
			comperror("Error: " + node->id + " not found", line);
			return NULL;
		}
		break;
	case SymbolType::USERFUNC: //USRFUNC
		s = scope_look_up(node->id, scope);
		if (check_lib_cols(node->id, "function", line)) {
			return NULL;
		}
		if (s == NULL) {
			node->isActive = true;
			my_table->table[hash_function(string_to_hash(node->id))].push_back(node);
			my_table->scope_links[scope].push_back(node);
			return node;

		} else {
			if (s->isActive == true)
				comperror("Error function or variable with the same id " + node->id + " exists in current scope", node->value.funcVal->line);
			return NULL;
		}
		break;
	case SymbolType::FORMAL: //FUNC ARGS
		if (check_lib_cols(node->id, "Formal", line) == true) {
			return NULL;
		}

		for (auto it : my_table->scope_links[scope]) {
			if ((it->id == node->id) && (it->type == node->type) && (it->isActive == true)) {
				comperror("Error formal argument " + node->id + " already exists in function scope", line);
				return NULL;
			}
		}
		node->isActive = true;
		node->value.funcVal->args.push_back(node->value.varVal);
		my_table->table[hash_function(string_to_hash(node->id))].push_back(node);
		my_table->scope_links[scope].push_back(node);

		break;
	default:
		assert(0);
	}
	return node;
}

SymbolTableEntry *insert_to_table(string symbol, SymbolType type, unsigned int line, unsigned int symbol_scope)
{ //node  on success NULL on failure
	scope = symbol_scope;
	SymbolTableEntry *ptr = NULL;
	SymbolTableEntry *node = new SymbolTableEntry;
	node->value.varVal = new Variable;
	node->value.funcVal = new Function;
	node->id = symbol;
	node->value.funcVal->name = symbol;
	node->value.funcVal->line = line;
	node->value.funcVal->scope = scope;
	node->value.varVal->name = symbol;
	node->value.varVal->line = line;
	node->value.varVal->scope = scope;
	node->type = type;
	node->scope_offset = currscopeoffset();
	node->scope_space = currscopespace();
	ptr = check_definitions(node, line);
	
	if (ptr != NULL) {
		my_table->count++;
	}
	return ptr;
}

void calculate(void)
{
	p = 307;
	a = rand() % (p - 1) - 1 + 1;
	b = rand() % (p - 1) - 0 + 1;
}

void initialize(void)
{
	srand(time(0));
	string lib_funcs[12] = { "print",
				 "input",
				 "objectmemberkeys",
				 "objecttotalmembers",
				 "objectcopy",
				 "totalarguments",
				 "argument",
				 "typeof",
				 "strtonum",
				 "sqrt",
				 "cos",
				 "sin" };

	my_table = new Hash_table;
	SymbolTableEntry *lib_func_node = NULL;
	for (unsigned int i = 0; i < MAX_SIZE; i++) {
		my_table->table.push_back(list<SymbolTableEntry *>());
		my_table->scope_links.push_back(list<SymbolTableEntry *>());
	}
	calculate();
	my_table->count = 0;
	scope = 0;

	//initialize lib_funcs
	for (int i = 0; i < 12; i++) {
		lib_func_node = new SymbolTableEntry;
		lib_func_node->id = lib_funcs[i];
		lib_func_node->type = SymbolType::LIBFUNC;
		lib_func_node->value.funcVal = new Function;
		lib_func_node->isActive = true;
		lib_func_node->value.funcVal->name = lib_func_node->id;
		lib_func_node->value.funcVal->scope = 0;
		lib_func_node->value.funcVal->line = 0;
		lib_func_node->value.varVal->line = 0;
		my_table->scope_links[0].push_back(lib_func_node);
		my_table->table[hash_function(string_to_hash(lib_funcs[i]))].push_back(lib_func_node);
	}
}

void print(void)
{
	string names[] = { "VAR", "LOCAL", "GLOBAL", "FORMAL", "USERFUNC", "LIBFUNC" };
	unsigned int i = 0;
	for (i = 0; i < my_table->scope_links.size(); i++) {
		if (!my_table->scope_links[i].empty()) {
			cout << "-------SCOPE #" << i << "-----------" << endl;
			for (auto l : my_table->scope_links[i]) {
				cout << "\"" << l->id << "\""
				     << "  [" << names[l->type] << "]"
				     << "  (line: "; 

					if(l->type!=USERFUNC)
						cout << l->value.varVal->line;
					else
						cout << l->value.funcVal->line;

					cout << ")   (scope: " << i << ")"
				     << " STATUS:" << l->isActive;

					 
				if(l->type!=SymbolType::USERFUNC && l->type!=SymbolType::LIBFUNC){
					cout << " SCOPE OFFSET:" << l->scope_offset;
					cout << " SCOPE SPACE: ";
					switch(l->scope_space){
					case programm_variables:
						cout << "program vars";
						break;
					case formal_arguments:
						cout << "formal args";
						break;
					case function_locals:
						cout << "function locals";
						break;
					default:
						assert(0);
						break;
					}
				}
				cout << endl;
					 
			}
		}
	}
}

int currscope(void){
	return scope;
}


bool is_func_stack_empty(void){
	return my_table->func_stack.size()!=0;
}