#ifndef _INTERMEDIATE_H
#define _INTERMEDIATE_H


#include <string>
#include <vector>
#include "../enums.hpp"
#include "../structs.hpp"
#include "../symboltable/symboltable.hpp"
#include "../expression_list/expression_list.hpp"


using namespace std;



//extern vector<struct expr*> exp_list;
extern expr_list expression_list;
extern stack<int>scopeoffsetstack;
extern stack<int>loopcounterstack;

expr_t* evalArithmetic(iopcode op, expr_t* expr1, expr_t* expr2, int line);
expr_t* evalBool(expr_t* lv,  int line);
expr_t* evalAssign(expr_t* leftvalue, expr_t* rightvalue, int line);
expr_t* evalUminus(expr_t* value, int line);
expr_t* evalLeftPlusPlus(expr_t* val, int line);
expr_t* evalRightPlusPlus(expr_t* exp, int line);
expr_t* evalLeftMinusMinus(expr_t* val, int line);
expr_t* evalRightMinusMinus(expr_t* exp, int line);
expr_t* evalArray(expr_t* lv, expr_t* rv, int line);
expr_t* evalArrayDef(expr_t* elist, int line);
expr_t* evalLvalueCallsufix(expr_t* lvalue, call_t* callsuffix, int line);
expr_t* evalIndexedObj(list<indexed_t*> inlist, int line);

call_t* evalMethodCall(string name, expr_t* e_list);
call_t* evalNormCall(expr_t* e_list);
stmt_t* evalBreak(int line, int loopcnt);
stmt_t* evalContinue(int line, int loopcnt);
stmt_t* evalStmtList(stmt_t* list, stmt_t* stmt);
void evalForStmt(forprefix_t* pref, unsigned N1, unsigned N2, unsigned N3, stmt_t* stmt);
forprefix_t* evalForPref(expr_t* expr, unsigned M, int line);
SymbolTableEntry* evalFuncDef(SymbolTableEntry* entry, int m, unsigned funcbody, int line);

expr_t* newexpr(expression_type type);
expr_t* newexpr_conststring(string s);
expr_t* newexpr_constint(int i);
expr_t* newexpr_constbool(unsigned int b);
expr_t* member_item (expr_t* lv, string name, int line);
expr_t* make_call (expr_t* lv, expr_t* reversed_elist,int line);

forprefix_t* newForPrefixJump(unsigned test, unsigned enter);
indexed_t* new_indexed(expr_t* key, expr_t* value);
SymbolTableEntry* evalFuncDef(SymbolTableEntry* entry, unsigned funcbody, int line);
unsigned evalFuncbody(unsigned int* loopcounter);
SymbolTableEntry* evalFuncprefix(char* str, int line, int symbol_scope);

void evalWhilestmt(unsigned start, unsigned cond, stmt_t* stmt, int line);
void print_explist(void);

void print_call_node(call_t* node);
stmt_t* new_statement(void);
void patchlist (int list, int label);
int new_stmt_list(int i);
int mergelist (int l1, int l2);
vector<int> merge_tf_list(vector<int>l1 ,vector<int>l2);
void backpatch_list(vector<int>list ,int quad);
expr_t* evalOr(expr_t* e1 ,expr_t* e2,unsigned m_quad, int line);
expr_t* evalAnd(expr_t* e1 ,expr_t* e2,unsigned m_quad, int line);
expr_t* evalNot(expr_t* e1, int line);
expr_t* evalrelop(expr_t* e1 ,expr_t* e2,iopcode op,unsigned line);
expr_t* convert_to_bool(expr_t* expr,int tj,int fj,int line);


#endif