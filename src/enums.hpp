#ifndef _ENUMS_HPP_
#define _ENUMS_HPP_

enum scopespace_t{
    programm_variables,
    function_locals,
    formal_arguments
};

enum expression_type{
    var_e, 
    tableitem_e, 
    
    programfunc_e, 
    libraryfunc_e, 

    arithexpr_e, 
    boolexpr_e,
    assignexpr_e, 
    newtable_e,  

    constint_e, 
    constdouble_e, 
    constbool_e, 
    conststring_e,

    nil_e
};

enum iopcode{
    add,    
    sub,
    mul,
    _div,
    mod,
    tablecreate,
    tablegetelem,
    tablesetelem,
    assign,
    nop,
    jump,
    if_eq,
    if_noteq,
    if_less,
    if_lesseq,
    if_greater,
    if_greatereq,
    param,
    call,
    getretval,
    funcstart,
    ret,
    funcend,
    uminus
};

enum SymbolType{
    VAR,
    LOCAL,
    GLOBAL,
    FORMAL,
    USERFUNC,
    LIBFUNC
};


//----------VM enums--------------------
enum vmopcode{
    assign_v,
    add_v,
    sub_v, 
    mul_v,
    div_v,
    mod_v,
    uminus_v,
    jump_v,
    jeq_v, 
    jne_v,
    jle_v,
    jge_v, 
    jlt_v,
    jgt_v,
    call_v,
    pusharg_v, 
    funcenter_v,
    funcexit_v, 
    newtable_v,
    tablegetelem_v,
    tablesetelem_v, 
    nop_v
};

enum vmarg_e{
    reset = -1,
    label_a = 0,
    global_a = 1,
    formal_a = 2,
    local_a = 3,
    number_a = 4,
    string_a = 5,
    bool_a = 6,
    nil_a = 7,
    userfunc_a = 8,
    libfunc_a = 9,
    retval_a = 10
};

enum avm_memcell_e{
    number_m = 0,
    string_m = 1,
    bool_m = 2, 
    table_m = 3,
    userfunc_m = 4,
    libfunc_m = 5,
    nil_m = 6,
    undef_m = 7,
    next_addr_m=8,
    old_top_m=9,
    old_topsp=10,
    num_of_actuals_m=11
};

#endif