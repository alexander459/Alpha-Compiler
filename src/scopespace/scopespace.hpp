#ifndef _SCOPESPACE_H_
#define _SCOPESPACE_H_

extern unsigned scopeSpaceCounter;
extern unsigned programVarOffset;
extern unsigned functionLocalOffset;
extern unsigned formalArgOffset;

unsigned currscopeoffset(void);
void inccurrscopeoffset(void);
enum scopespace_t currscopespace(void);
void enterScopeSpace(void);
void exitScopeSpace(void);
void resetfunctionlocaloffset(void);
void restorecurrscopeoffset(unsigned n);
void resetformalargsoffset(void);
#endif