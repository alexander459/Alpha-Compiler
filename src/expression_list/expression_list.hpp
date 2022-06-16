#ifndef _EXPRLIST_HPP_
#define _EXPRLIST_HPP_
#include "../structs.hpp"


typedef struct expression_list* expr_list;
expr_list expr_list_new(void);
expr_list expr_list_push_front(expr_list e_list,struct expr* node);
expr_list expr_list_push_back(expr_list e_list,struct expr* node);
struct expr* expr_list_get_front(expr_list e_list);
struct expr* expr_list_pop_front(expr_list e_list);
void expr_list_print(expr_list e_list);
void expr_list_print_from_node(struct expr* node);
void expr_list_free(expr_t* node);
expr_t* reverse_list(expr_t* list);

#endif