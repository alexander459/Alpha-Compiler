#include "intermediate.hpp"
#include "../quads/quads.hpp"
#include "../structs.hpp"
#include "../enums.hpp"
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include "../expression_list/expression_list.hpp"
#include "../scopespace/scopespace.hpp"
#include "../utils/util.hpp"

using namespace std;

stack<int>scopeoffsetstack;
stack<int>loopcounterstack;

expr_t* evalArithmetic(iopcode op, expr_t* expr1, expr* expr2, int line){
    expr_t* new_expr=newexpr(arithexpr_e);
    new_expr->sym=newtemp(line);
    emit(op, expr1, expr2, new_expr, 0, line);
    return new_expr;
}

expr_t* evalBool( expr_t* lv,int line){
    if(lv->type == boolexpr_e || lv->type == constbool_e){
       // expr_t* new_expr=newexpr(boolexpr_e);
        //new_expr->sym=newtemp(line);
        backpatch_list(lv->truelist,nextquadlabel());
        emit(assign, newexpr_constbool(1), NULL, lv, 0, line);
        emit(jump, NULL, NULL, NULL, nextquadlabel()+2, line);
        backpatch_list(lv->falselist,nextquadlabel());
        emit(assign, newexpr_constbool(0), NULL, lv, 0, line);
        //return new_expr;
    }
    return lv;
}

expr_t* evalAssign(expr_t* leftvalue, expr_t* rightvalue, int line){
    expr_t* new_expr;
    rightvalue=stringBoolToBool(rightvalue);
    if(leftvalue->type == tableitem_e){
        emit(tablesetelem, leftvalue->index, rightvalue, leftvalue, 0, line);
        new_expr=emit_iftableitem(leftvalue, line);
        new_expr->type=assignexpr_e;
    }else{
        emit(assign, rightvalue, NULL, leftvalue, 0, line);
        new_expr=newexpr(assignexpr_e);
        new_expr->sym=newtemp(line);
        emit(assign, leftvalue, NULL, new_expr, 0, line);
    }
    return new_expr;
}

expr_t* evalUminus(expr_t* value, int line){
    expr_t* new_expr;
    new_expr=newexpr(arithexpr_e);
    if(is_temp_expr(value))
        new_expr->sym=value->sym;
    else
        new_expr->sym=newtemp(line);
    emit(iopcode::uminus, value, NULL, new_expr, 0, line);
    return new_expr;
}



expr_t* evalLeftPlusPlus(expr_t* val, int line){
    expr_t* new_expr;
    if (val->type == tableitem_e){
        new_expr=emit_iftableitem(val, line);
        emit(iopcode::add, new_expr, newexpr_constint(1), new_expr, 0, line);
        emit(iopcode::tablesetelem, val->index, new_expr, val, 0, line);
    }else{
        emit(iopcode::add, val, newexpr_constint(1), val, 0, line);
        new_expr=newexpr(arithexpr_e);
        new_expr->sym=newtemp(line);
        emit(iopcode::assign, val, NULL, new_expr, 0, line);
    }
    return new_expr;
}

expr_t* evalRightPlusPlus(expr_t* exp, int line){
    expr_t* new_expr;
    new_expr=newexpr(var_e);
    new_expr->sym=newtemp(line);
    if (exp->type == tableitem_e) {
        expr_t *val = emit_iftableitem(exp, line);
        emit(iopcode::assign, val, NULL, new_expr, 0, line);
        emit(iopcode::add, val, newexpr_constint(1), val, 0, line);
        emit(iopcode::tablesetelem, exp->index, val, exp, 0, line);
    }else{
        emit(iopcode::assign, exp, NULL, new_expr, 0, line);
        emit(iopcode::add, exp, newexpr_constint(1), exp, 0, line);
    }
    return new_expr;
}

expr_t* evalLeftMinusMinus(expr_t* val, int line){
    expr_t* new_expr;
    if (val->type == tableitem_e){
        new_expr = emit_iftableitem(val, line);
        emit(iopcode::sub, new_expr, newexpr_constint(1), new_expr, 0, line);
        emit(iopcode::tablesetelem, val->index, new_expr, val, 0, line);
    }else{
        emit(iopcode::sub, val, newexpr_constint(1), val, 0, line);
        new_expr = newexpr(arithexpr_e);
        new_expr->sym = newtemp(line);
        emit(iopcode::assign, val, NULL, new_expr, 0, line);
    }
    return new_expr;
}

expr_t* evalRightMinusMinus(expr_t* exp, int line){
    expr_t* new_expr;
    new_expr = newexpr(var_e);
    new_expr->sym = newtemp(line);
    if (exp->type == tableitem_e) {
        expr_t *val = emit_iftableitem(exp, line);
        emit(iopcode::assign, val, NULL, new_expr, 0, line);
        emit(iopcode::sub, val, newexpr_constint(1), val, 0, line);
        emit(iopcode::tablesetelem, exp->index, val, exp, 0, line);
    }else{
        emit(iopcode::assign, exp, NULL, new_expr, 0, line);
        emit(iopcode::sub, exp, newexpr_constint(1), exp, 0, line);
    }
    return new_expr;
}

expr_t* evalArray(expr_t* lv, expr_t* rv, int line){
    expr_t* new_expr;
    
    lv=emit_iftableitem(lv, line);
    new_expr=newexpr(tableitem_e);
    new_expr->sym=lv->sym;
    new_expr->index=rv;
    return new_expr;
}

expr_t* evalArrayDef(expr_t* elist, int line){
    expr_t* t=newexpr(newtable_e);
    expr_t* temp;
    int i;

    elist=reverse_list(elist);

    t->sym=newtemp(line);
    emit(tablecreate, t, NULL, NULL, 0, line);
    i=0;
    temp=elist;
    while(temp!=NULL){
        temp=stringBoolToBool(temp);
        emit(tablesetelem, newexpr_constint(i), temp, t, 0, line);
        temp=temp->next;
        i++;
    }
    return t;
}

call_t* evalMethodCall(string name, expr_t* e_list){
    call_t* call=new call_t;
    assert(call!=NULL);
    call->e_list=e_list;
    call->method=true;
    call->name=name;
    return call;
}

call_t* evalNormCall(expr_t* e_list){
    call_t* call=new call_t;
    assert(call!=NULL);
    call->e_list=e_list;
    call->method=false;
    call->name="";
    return call;
}

expr_t* evalLvalueCallsufix(expr_t* lvalue, call_t* callsuffix, int line){
    lvalue=emit_iftableitem(lvalue, line);
    if(callsuffix->method==true){
        expr_t* t=lvalue;
        lvalue=emit_iftableitem(member_item(t, callsuffix->name, line), line);
        if(callsuffix->e_list!=NULL){
            t->next=callsuffix->e_list;
        }else{
            t->next=NULL;
        }
        callsuffix->e_list=t;
    }
    return make_call(lvalue, callsuffix->e_list, line);
}

expr_t* evalIndexedObj(list<indexed_t*> inlist, int line){
    expr* t=newexpr(newtable_e);
    t->sym=newtemp(line);
    
    emit(tablecreate, t, NULL, NULL, 0, line);
    for(auto it:inlist){
        it->key=stringBoolToBool(it->key);
        emit(tablesetelem, it->key, it->value, t, 0, line);
    }
    return t;
}

stmt_t* evalBreak(int line, int loopcnt){
    stmt_t* new_stmt=new_statement();
    assert(new_stmt!=NULL);
    if(loopcnt==0){
        comperror("Break not in loop error", line);
    }
    emit(jump, NULL, NULL, NULL, 0, line);
    new_stmt->breaklist=new_stmt_list(nextquadlabel()-1); //maybe not minus 1....
    return new_stmt;
}

stmt_t* evalContinue(int line, int loopcnt){
    stmt_t* new_stmt=new_statement();
    assert(new_stmt!=NULL);
    if(loopcnt==0){
        comperror("Continue not in loop error", line);
    }
    emit(jump, NULL, NULL, NULL, 0, line);
    new_stmt->contlist=new_stmt_list(nextquadlabel()-1); //maybe not minus 1....
    return new_stmt;
}

void evalForStmt(forprefix_t* pref, unsigned N1, unsigned N2, unsigned N3, stmt_t* stmt){
    int bl = 0, cl = 0;

    patchlabel(pref->enter, N2+1);
    patchlabel(N1, nextquadlabel());
    patchlabel(N2, pref->test);
    patchlabel(N3, N1+1);

    if(stmt!=NULL){
        bl=stmt->breaklist;
        cl=stmt->contlist;
    }

    patchlist(bl, nextquadlabel());
    patchlist(cl, N1+1);
}

forprefix_t* evalForPref(expr_t* expr, unsigned M, int line){
    forprefix_t* prefix;
    prefix=newForPrefixJump(M, nextquadlabel());
    emit(if_eq, expr, newexpr_constbool(1), NULL, 0, line);

    return prefix;
}

forprefix_t* newForPrefixJump(unsigned test, unsigned enter){
    forprefix_t* pref=new struct forprefix_t;
    pref->enter = enter;
    pref->test = test;
    return pref;
}

stmt_t* evalStmtList(stmt_t* list, stmt_t* stmt){
    stmt_t* new_stmt;
    int tmp_bl1=0, tmp_bl2=0, tmp_cl1=0, tmp_cl2=0; 
                                    
    if(list!=NULL){
        tmp_bl1=list->breaklist;
        tmp_cl1=list->contlist;
    }

    if(stmt!=NULL){
        tmp_bl2=stmt->breaklist;
        tmp_cl2=stmt->contlist;
    }

    new_stmt=new_statement();
    new_stmt->breaklist=mergelist(tmp_bl1, tmp_bl2);
    new_stmt->contlist=mergelist(tmp_cl1, tmp_cl2);
    return new_stmt;
}



SymbolTableEntry* evalFuncDef(SymbolTableEntry* entry, int m, unsigned funcbody, int line){
    if(entry==NULL)
        return NULL;
    expr* new_exp=newexpr(programfunc_e);
    exitScopeSpace();
    entry->value.funcVal->totalLocals=funcbody;
    int offset=scopeoffsetstack.top();
    scopeoffsetstack.pop();
    restorecurrscopeoffset(offset);
    new_exp->sym=entry;
    emit(funcend, new_exp, NULL, NULL, 0, line);
    patchlabel(m-2, nextquadlabel());
    return entry;

}


unsigned evalFuncbody(unsigned int* loopcounter){
    unsigned temp;
    pop_from_stack();
    temp=currscopeoffset();
    exitScopeSpace();
    if(loopcounterstack.size()!=0){
        *loopcounter=loopcounterstack.top();
        loopcounterstack.pop();
    }
    return temp;
}

SymbolTableEntry* evalFuncprefix(char* str, int line, int symbol_scope){
    SymbolTableEntry* entry;
    string name(str);
    entry=insert_to_table(name, SymbolType::USERFUNC, line, symbol_scope);
    emit(jump, NULL, NULL, NULL, 0, line);
    if(entry!=NULL){
        entry->taddress=nextquadlabel();
        push_to_stack(entry);
        entry->value.funcVal->iaddress = nextquadlabel();
        expr* new_exp = newexpr(programfunc_e);
        new_exp -> sym = entry;
        emit(iopcode::funcstart,new_exp,NULL,NULL,0,line);
        scopeoffsetstack.push(currscopeoffset());
        enterScopeSpace();
        resetformalargsoffset();
    }

    return entry;
}

void evalWhilestmt(unsigned start, unsigned cond, stmt_t* stmt, int line){
    int tmp_blist=0;
    int tmp_contlist=0;
    emit(iopcode::jump, NULL, NULL, NULL, start, line);
    patchlabel(cond, nextquadlabel());
    if(stmt!=NULL){
        tmp_blist=stmt->breaklist;
        tmp_contlist=stmt->contlist;
    }
    patchlist(tmp_blist, nextquadlabel());
    patchlist(tmp_contlist, start);
    return;
}

void print_call_node(call_t* node){
    assert(node!=NULL);
    if(node->method==true)
        cout << "method call\n";
    else
        cout << "Is normal call\n";

    cout << "call name: " << node->name << endl;
    if(node->e_list==NULL)
        cout << "No arguments...\n";
    else{
        cout << "Args:\n";
        expr_list_print_from_node(node->e_list);
    }
    return;
}

expr_t* newexpr(expression_type type){
    expr_t* expr_node=new expr_t;
    assert(expr_node!=NULL);
    expr_node->sym=NULL;
    expr_node->type=type;    
    expr_node->boolConst = -1;
    expr_node->next=NULL;
    return expr_node;
}

expr_t* newexpr_conststring(string s){
    expr_t* e=newexpr(conststring_e);
    string str;
    str="\"" + s + "\"";
    e->strConst=s;
    return e;
}

expr_t* newexpr_constint(int i){
    expr_t* e=newexpr(constint_e);
    e->intConst=i;
    return e;
}

expr_t* newexpr_constbool(unsigned int b){
    expr_t* e = newexpr(constbool_e);
    e->boolConst = b;
    return e;
}

indexed_t* new_indexed(expr_t* key, expr_t* value){
    indexed_t* elem;
    assert(key!=NULL);
    assert(value!=NULL);
    elem=new struct indexed;
    elem->key=key;
    elem->value=value;
    return elem;
}

stmt_t* new_statement(void){
    stmt_t* new_stmt=new struct stmt_t;
    assert(new_stmt!=NULL);
    new_stmt->breaklist=0;
    new_stmt->contlist=0;
    return new_stmt;
}

void patchlist (int list, int label) {
    while (list) {
        int next = quads[list].label;
        quads[list].label = label;
        list = next;
    }
}

int new_stmt_list(int i){
    quads[i].label=0;
    return i;
}

expr_t* member_item(expr_t* lv, string name, int line){
    lv = emit_iftableitem(lv, line); // Emit code if r-value use of table item
    expr_t* ti = newexpr(tableitem_e); // Make a new expression
    ti->sym = lv->sym;
    ti->index = newexpr_conststring(name); // Const string index
    return ti;
}


expr_t* make_call (expr_t* lv, expr_t* reversed_elist, int line) {
    expr_t* func = emit_iftableitem(lv, line);
    while(reversed_elist!=NULL){
        emit(param, reversed_elist, NULL, NULL, 0, line);
        reversed_elist = reversed_elist->next;
    }

    emit(call, func, NULL, NULL, 0, line);
    expr_t* result = newexpr(var_e);
    result->sym = newtemp(line);
    emit(getretval, NULL, NULL, result, 0, line);

    return result;
}


int mergelist (int l1, int l2) {
    if (!l1)
        return l2;
    else if (!l2)
        return l1;
    else {
    int i = l1;
    while (quads[i].label)
        i = quads[i].label;
    quads[i].label = l2;
    return l1;
    }
}

vector<int> merge_tf_list(vector<int>l1 ,vector<int>l2){
    vector<int>merged;
    if(l1.size()==0){
        merged = l2;
    }else if(l2.size()==0){
        merged = l1;
    }else{
        merged = l1;
        for(auto it : l2){
            merged.push_back(it);
        }
        
    }
    
    return merged;
}

void backpatch_list(vector<int>list ,int quad){
   
     for(auto it : list){
         quads[it].label = quad;
         
     }
}


void new_tf_list(vector<int> &list ,int quad){
        list.push_back(quad);
}


expr_t* convert_to_bool(expr_t* expr,int tj,int fj,int line){
    assert(expr);
    expr_t* new_exp;
    if(expr->type!=boolexpr_e && expr->type!=constbool_e){
        expr=stringBoolToBool(expr);
        new_exp = newexpr(boolexpr_e);
        assert(new_exp);
        new_exp->sym = newtemp(line);
        new_tf_list(new_exp->truelist,tj);
        new_tf_list(new_exp->falselist,fj);
        emit(if_eq,expr,newexpr_constbool(1),NULL,0,line);
        emit(jump, NULL, NULL, NULL, 0, line);

        return new_exp;

    }
    return expr;
    

}
expr_t* evalOr(expr_t* e1 ,expr_t* e2,unsigned m_quad, int line){
    assert(e1&&e2);
    expr_t* e;
    e = newexpr(boolexpr_e);
    assert(e);
   
    e ->sym = newtemp(line);
    e->boolConst = -1;
    
    backpatch_list(e1->falselist,m_quad);
    e->truelist = merge_tf_list(e1->truelist,e2->truelist);
    
    e->falselist = e2->falselist;
    return e;
}

expr_t* evalAnd(expr_t* e1 ,expr_t* e2,unsigned m_quad, int line){
    assert(e1&&e2);
    expr_t* e;
   
    e = newexpr(boolexpr_e);
    assert(e);
    e->sym= newtemp(line);
    e->boolConst = -1;
    backpatch_list(e1->truelist,m_quad);

    e->truelist = e2->truelist;
    e->falselist = merge_tf_list(e1->falselist,e2->falselist);
    return e;

}

expr_t* evalNot(expr_t* e1, int line){ //convert to bool
    assert(e1);
    expr_t* e;
    e = newexpr(boolexpr_e);
    assert(e);
    e->boolConst = -1;
    e->sym = newtemp(line);
    e->truelist = e1->falselist;
    e->falselist = e1->truelist;
    return e;
}



expr_t* evalrelop(expr_t* e1 ,expr_t* e2,iopcode op,unsigned line){
     assert(e1 && e2);
     expr_t* ret = newexpr(boolexpr_e);
     assert(ret);
     ret->sym = newtemp(line);
     ret->boolConst = -1;
     
    
     new_tf_list(ret->truelist,nextquadlabel());
     new_tf_list(ret->falselist,nextquadlabel()+1);
   
     emit(op,e1,e2,NULL,0,line);
     emit(jump,NULL,NULL,NULL,0,line);
     return ret;

}
