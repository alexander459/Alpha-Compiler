#ifndef FINAL_CODE_
#define FINAL_CODE_
#include "../intermediate/intermediate.hpp"
#include "../quads/quads.hpp"
#include "../structs.hpp"
#include "../enums.hpp"
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include "../expression_list/expression_list.hpp"
#include "../scopespace/scopespace.hpp"
#include "../utils/util.hpp"
#include <cstring>
#include <vector>
using namespace std;

extern instruction *instructions;
extern vector<string> stringArray;
extern vector<double> numArray;
extern vector<string> libfuncArray;
extern vector<incomplete_jump> ijArray;
extern vector<SymbolTableEntry*> funcArray;

 
typedef void (*generator_func_t)(quads_t*);
void make_retvaloperand(vmarg *arg);
void make_booloperand(vmarg *arg, unsigned val);
void generate_relational (vmopcode op, quads_t  *q);
void add_incomplete_jump(unsigned instrNo, unsigned iaddress);


void generate (vmopcode op, quads_t  *quad);
void generate_quads(void);
void generate_ADD(quads_t  *q);
void generate_SUB(quads_t  *q);
void generate_MUL(quads_t  *q);
void generate_DIV(quads_t  *q);
void generate_MOD(quads_t  *q);
void generate_NEWTABLE(quads_t  *q);
void generate_TABLEGETELEM(quads_t  *q);
void generate_TABLESETELEM(quads_t  *q);
void generate_ASSIGN(quads_t  *q);
void generate_NOP(quads_t  *q);
void generate_JUMP(quads_t  *q);
void generate_IF_EQ(quads_t  *q);
void generate_IF_NOTEQ(quads_t  *q);
void generate_IF_GREATER(quads_t  *q);
void generate_IF_GREATEREQ(quads_t  *q);
void generate_IF_LESS(quads_t  *q);
void generate_IF_LESSEQ(quads_t  *q);
void generate_PARAM(quads_t  *q);
void generate_CALL(quads_t  *q);
void generate_GETRETVAL(quads_t  *q);
void generate_FUNCSTART(quads_t  *q);
void generate_UMINUS(quads_t *q);
void generate_RETURN(quads_t  *q);
void generate_FUNCEND(quads_t  *q);
void generate_NOP (quads_t  *q);


unsigned consts_newstring(string s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(string s);
void make_operand(expr *e, vmarg *arg);

void check_quad_funcs(quads_t* q);

void patch_incomplete_jumps(void);
void expandInstr(void);
void emit_instr(instruction t);
void backpatch_instructions(vector<unsigned>&list, unsigned addr);
void print_instructions(void);
void print_instructions_bin(string fname);
string getOpcode(vmopcode op);
string print_vmargs(vmarg arg, bool bin);
#endif