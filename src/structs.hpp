#ifndef _STRUCTS_HPP_
#define _STRUCTS_HPP_
#include <string>
#include "enums.hpp"
#include <list>
#include <stack>
#include <vector>
#include <map>

using namespace std;

struct expression_list{
	struct expr* head;
};

typedef struct Variable {
	string name;
	unsigned int scope;
	unsigned int line;
} Variable;

typedef struct Function {
	string name;
	unsigned int scope;
	list<Variable *> args;
	unsigned totalLocals;
	unsigned int line;
	unsigned iaddress;
	vector<unsigned> returnList;
} Function;

typedef struct SymbolTableEntry {
	bool isActive;
	unsigned int token_num;
	string id;
	union {
		Variable *varVal;
		Function *funcVal;
	} value;
	enum SymbolType type;
	enum scopespace_t scope_space;
	int scope_offset;
	unsigned taddress;
} SymbolTableEntry;

typedef struct Hash_table {
	size_t count;
	stack<SymbolTableEntry*> func_stack;
	vector<list<SymbolTableEntry *> > scope_links;
	vector<list<SymbolTableEntry *> > table; //hash_table
} Hash_table;



typedef struct expr{
    expression_type type;
    SymbolTableEntry *sym;
    struct expr* index;
    int intConst;
    double doubleConst;
    string strConst;
    unsigned char boolConst;
	vector<int>truelist;
	vector<int>falselist;
    struct expr* next;
}expr_t;


typedef struct call_t {
	struct expr* e_list;
    bool method;
    string name;
}call_t;


typedef struct quad{
    iopcode op;
    expr_t* result;
    expr_t* arg1;
    expr_t* arg2;
    unsigned label;
    unsigned line;
	unsigned taddress;
}quads_t;


typedef struct indexed{
	expr_t* key;
	expr_t* value;
}indexed_t;

typedef struct forprefix_t{
    unsigned test;
    unsigned enter;
} forprefix_t;

typedef struct stmt_t{
    int breaklist;
    int contlist;
} stmt_t;


//--------VM structs-------------
#define AVM_TABLE_HASHSIZE 211

typedef struct vmarg{
	vmarg_e type;
	unsigned val;
}vmarg;

typedef struct instruction{
	vmopcode opcode;
	vmarg result;
	vmarg arg1;
	vmarg arg2;
	unsigned srcLine;
}instruction;

typedef struct program_function{
    string id;
    int address;
    int totalLocals;
}program_func;

struct avm_table; // for use in avm_table_bucket

typedef struct avm_memcell{
	avm_memcell_e type;
	struct{
		double numVal;
		string strVal;
		bool boolVal;
		avm_table *tableVal;
		unsigned funcVal;	/* ############ PERIEXEI AYSTHRA(!!!) INDEXEIS STON PINAKA, POTE DIEYTHINSI SYNARTISIS! */
		string libfuncVal;
	}data;
}avm_memcell;


typedef struct avm_table{
	unsigned refCounter=0;
	//map<avm_memcell::data*,avm_memcell*> elements;
	map<string,avm_memcell*> strIndexed;
	map<double,avm_memcell*> numIndexed;
    map<unsigned,avm_memcell*> userfuncIndexed;
    map<string,avm_memcell*> libFuncIndexed;
    map<avm_table*,avm_memcell*>tableIndexed;
    map<bool,avm_memcell*>boolIndexed;
	unsigned total;
}avm_table;

typedef struct incomplete_jump{
	unsigned instrNo;
	unsigned iaddress;
	struct incomplete_jump *next;
}incomplete_jump;
#endif