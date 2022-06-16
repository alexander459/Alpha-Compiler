#include <assert.h>
#include "../enums.hpp"
#include "scopespace.hpp"


unsigned scopeSpaceCounter=1;
unsigned programVarOffset=0;
unsigned functionLocalOffset=0;
unsigned formalArgOffset=0;

unsigned currscopeoffset(void){
    switch(currscopespace()){
        case programm_variables:
            return programVarOffset;
        case function_locals:
            return functionLocalOffset;
        case formal_arguments:
            return formalArgOffset;
        default:
            assert(0);
    }
    return 5;
}

void inccurrscopeoffset(void){
    switch(currscopespace()){
        case programm_variables:
            programVarOffset++;
            break;
        case function_locals:
            functionLocalOffset++;
            break;
        case formal_arguments:
            formalArgOffset++;
            break;
    }
}

enum scopespace_t currscopespace(void){
    if(scopeSpaceCounter==1){
        return programm_variables;
    }else{
        if(scopeSpaceCounter%2==0)
            return formal_arguments;
        else
            return function_locals;
    }
}

void enterScopeSpace(void){
    scopeSpaceCounter++;
    return;
}

void exitScopeSpace(void){
    assert(scopeSpaceCounter>1);
    scopeSpaceCounter--;
    return;
}

void  resetfunctionlocaloffset(void){
    functionLocalOffset = 0;
}

void resetformalargsoffset(void){
    formalArgOffset = 0;
}
void restorecurrscopeoffset(unsigned n){
    switch (currscopespace())
    {
    case programm_variables: programVarOffset = n;
        break;
    case function_locals: functionLocalOffset = n; 
        break;
    case formal_arguments: formalArgOffset = n;
        break;    
    default: assert(0);
    }
}
