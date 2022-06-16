#include "expression_list.hpp"
#include <assert.h>
#include <iostream>

using namespace std;

expr_list expr_list_new(void){
    expr_list new_list=new struct expression_list;
    assert(new_list!=NULL);
    new_list->head=NULL;
    return new_list;
}

expr_list expr_list_push_front(expr_list e_list, expr_t* node){
    assert(e_list!=NULL);
    assert(node!=NULL);
    node->next=e_list->head;
    e_list->head=node;
    return e_list;
}

expr_list expr_list_push_back(expr_list e_list, expr_t* node){
    assert(e_list!=NULL);
    assert(node!=NULL);
    expr_t* temp=e_list->head;
    if(temp==NULL){
        e_list->head=node;
        node->next=NULL;
        return e_list;
    }
    while(temp->next!=NULL)
        temp=temp->next;
    temp->next=node;
    node->next=NULL;
    return e_list;
}

expr_t* reverse_list(expr_t* list){
    expr_list new_list=expr_list_new();
    if(list==NULL)
        return list;
    assert(new_list!=NULL);
    expr_t* temp, *next;
    temp=list;
    while(temp!=NULL){
        next=temp->next;
        new_list=expr_list_push_front(new_list, temp);
        temp=next;
    }
    return new_list->head;
}

expr_t* expr_list_get_front(expr_list e_list){
    assert(e_list!=NULL);
    return e_list->head;
}

expr_t* expr_list_pop_front(expr_list e_list){
    expr_t* temp;
    assert(e_list!=NULL);
    if(e_list->head==NULL)
        return NULL;
    temp=e_list->head;
    e_list->head=temp->next;
    temp->next=NULL;
    return temp;
}

void expr_list_free(expr_t* node){
    expr_t* next, *temp;
    if(node==NULL){
        return;
    }
    temp=node;
    while(temp!=NULL){

        next=temp->next;
        if(temp->index!=NULL)
            free(temp->index);
            
        free(temp);

        temp=next;
    }
    return;
}

void expr_list_print(expr_list e_list){
    expr_t* temp;
    assert(e_list!=NULL);

    temp=e_list->head;
    while(temp!=NULL){
        switch (temp->type){
        case constint_e:
            cout << "Value of int is " << temp->intConst << endl;
            break;
        case constdouble_e:
            cout << "Value of double is " << temp->doubleConst << endl;
            break;
        case conststring_e:
            cout << "Value of str is " << temp->strConst << endl;
            break;
        case constbool_e:
            printf("Value of bool is %d\n", temp->boolConst);
            break;
        case nil_e:
            printf("Value is nil\n");
            break;
        case var_e:
            cout << "Symbol is " << temp->sym->id << " in " << temp->sym->scope_space << endl;
            break;
        case programfunc_e:
            cout << "Symbol is " << temp->sym->id << endl;
            break;
        case arithexpr_e:
            cout << "Arithmetic expression temp var is " << temp->sym->id << endl;
            break;
        case assignexpr_e:
            cout << "Assign expression temp var is " << temp->sym->id << endl;
            break;
        case newtable_e:
            cout << "New table expression temp var is " << temp->sym->id << endl;
            break;
        default:
            cout << temp->type << endl;
            assert(0);
            break;
        }
        temp=temp->next;
    }
}

void expr_list_print_from_node(expr_t* node){
    expr_t* temp;
    assert(node!=NULL);

    temp=node;
    while(temp!=NULL){
        switch (temp->type){
        case constint_e:
            cout << "Value of int is " << temp->intConst << endl;
            break;
        case constdouble_e:
            cout << "Value of double is " << temp->doubleConst << endl;
            break;
        case conststring_e:
            cout << "Value of str is " << temp->strConst << endl;
            break;
        case constbool_e:
            printf("Value of bool is %d\n", temp->boolConst);
            break;
        case nil_e:
            printf("Value is nil\n");
            break;
        case var_e:
            cout << "Symbol is " << temp->sym->id << " in " << temp->sym->scope_space << endl;
            break;
        case programfunc_e:
            cout << "Symbol is " << temp->sym->id << endl;
            break;
        case arithexpr_e:
            cout << "Arithmetic expression temp var is " << temp->sym->id << endl;
            break;
        case assignexpr_e:
            cout << "Assign expression temp var is " << temp->sym->id << endl;
            break;
        case newtable_e:
            cout << "New table temp var is " << temp->sym->id << endl;
            break;
        default:
        cout << temp->type << endl;
            assert(0);
            break;
        }
        temp=temp->next;
    }
    return;
}