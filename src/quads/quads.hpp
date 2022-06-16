#ifndef _QUADS_H_
#define _QUADS_H_

#define EXPAND_SIZE 1024
#define CURR_SIZE (total*sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE*sizeof(quad)+CURR_SIZE)

#include "../structs.hpp"
#include "../enums.hpp"

extern quads_t* quads;
extern unsigned total;
extern unsigned int currQuad;

void expand(void);
void emit(iopcode op, expr_t* arg1, expr_t* arg2, expr_t* result, unsigned label, unsigned line);
expr_t* emit_iftableitem(expr_t* e, int line);
unsigned nextquadlabel(void);
void patchlabel(unsigned quadNo,unsigned label);
void print_quads(void);
string expr_to_string(expr_t* expr);
#endif