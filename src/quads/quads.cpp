#include "../structs.hpp"
#include "quads.hpp"
#include <assert.h>
#include <cstring>
#include <iostream>
#include "../intermediate/intermediate.hpp"
using namespace std;

quads_t* quads=NULL;
unsigned total=0;
unsigned int currQuad=0;

void expand(void){
    assert(total==currQuad);

    quads_t *p=(quads_t*)malloc(NEW_SIZE);

    if(quads){
        memcpy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads=p;
    total+=EXPAND_SIZE;
}

void emit(iopcode op, expr_t* arg1, expr_t* arg2, expr_t* result, unsigned label, unsigned line){
    if(currQuad==total)
        expand();
    quads_t *p=quads+currQuad;
    p->op=op;
    p->arg1=arg1;
    p->arg2=arg2;
    p->result=result;
    p->label=label;
    p->line=line;
    currQuad++;
    return;
}

expr_t* emit_iftableitem(expr_t* e, int line){
    expr_t* result;
    if(e->type!=tableitem_e)
        return e;
    result=newexpr(var_e);
    result->sym=newtemp(line);
    emit(tablegetelem, e, e->index, result, 0, line);
    return result;
}

unsigned nextquadlabel(void){ return currQuad;}

void patchlabel(unsigned quadNo,unsigned label){
    assert(quadNo < currQuad);
    quads[quadNo].label = label;
}

void print_quads(void){
    bool p_lbl=false;
    unsigned int i;
    cout << "----------------------------------------------------------------------------------------------------------\n";
    for(i=0; i<currQuad; i++){
        cout << "Quad " << i << ": ";
        switch(quads[i].op){
            case assign:
                cout << "ASSIGN";
                break;
            case add:
                cout << "ADD";
                break;
            case sub:
                cout << "SUB";
                break;
            case mul:
                cout << "MUL";
                break;
            case _div:
                cout << "DIV";
                break;
            case mod:
                cout << "MOD";
                break;
            case uminus:
                cout << "UMINUS";
                break;
            case if_eq:
                cout << "IFEQ";
                p_lbl=true;
                break;
            case if_noteq:
                cout << "IFNEQ";
                p_lbl=true;
                break;
            case if_lesseq:
                cout << "IFLESSQE";
                p_lbl=true;
                break;
            case if_greatereq:
                cout << "IFGREATEREQ";
                p_lbl=true;
                break;
            case if_less:
                cout << "IFLESS";
                p_lbl=true;
                break;
            case if_greater:
                cout << "IFGREATER";
                p_lbl=true;
                break;
            case call:
                cout << "CALL";
                break;
            case param:
                cout << "PARAM";
                break;
            case ret:
                cout << "RET";
                break;
            case getretval:
                cout << "GETRETVAL";
                break;
            case funcstart:
                cout << "FUNCSTART";
                break;
            case funcend:
                cout << "FUNCEND";
                break;
            case tablecreate:
                cout << "TABLECREATE";
                break;
            case tablegetelem:
                cout << "TABLEGETELEM";
                break;
            case tablesetelem:
                cout << "TABLESETELEM";
                break;
            case jump:
                cout << "JUMP";
                p_lbl=true;
                break;
            default:
                assert(0);
        }

        if(quads[i].result!=NULL)
            cout << " " << expr_to_string(quads[i].result);
        if(quads[i].arg1!=NULL)
            cout << " " << expr_to_string(quads[i].arg1);
        if(quads[i].arg2!=NULL)
            cout << " " << expr_to_string(quads[i].arg2);
        if(p_lbl){
            cout << " " << quads[i].label;
        }
        cout << " (line: " << quads[i].line << ")\n";
        p_lbl=false;
    }

    cout << "----------------------------------------------------------------------------------------------------------\n";
    return;
}


string expr_to_string(expr_t* expr){
    string str="";
    switch (expr->type){
        case constint_e:
            str+=to_string(expr->intConst);
            break;
        case constdouble_e:
            str+=to_string(expr->doubleConst);
            break;
        case conststring_e:
            str+=expr->strConst;
            break;
        case constbool_e:
            if(expr->boolConst==1)
                str+="true";
            else if (expr->boolConst==0)
                str+="false";
            else{
                assert(0);
            }

            break;
        case nil_e:
            str+="nil";
            break;
        case var_e:
        case programfunc_e:
        case arithexpr_e:
        case assignexpr_e:
        case tableitem_e:
        case boolexpr_e:
        case newtable_e:
            str+=expr->sym->id;
            break;
        default:
            //cout << endl << "ASSERTION: " << expr->type << endl;
            assert(0);
            break;
    }
    return str;
}

