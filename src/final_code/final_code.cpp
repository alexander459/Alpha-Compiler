#include "final_code.hpp"
#include <stack>

incomplete_jump *ij_head = (incomplete_jump *)0;
unsigned ij_total = 0;
//unsigned currInstr = 0;

vector<string> stringArray;
vector<double> numArray;
vector<string> libfuncArray;
vector<incomplete_jump> ijArray;
vector<SymbolTableEntry*> funcArray;

stack<SymbolTableEntry*> funcstack;
instruction *instructions=NULL;
unsigned totalInstr=0;
unsigned int currInstr=0;
unsigned  current_proc_quad;
generator_func_t generators[] = {
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_ASSIGN,
    generate_NOP,
    generate_JUMP,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_LESS,
    generate_IF_LESSEQ,
    generate_IF_GREATER,
    generate_IF_GREATEREQ,
    generate_PARAM,
    generate_CALL,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_RETURN,
    generate_FUNCEND,
    generate_UMINUS
};


void generate_quads(void){
    current_proc_quad = 0;
    for(unsigned i = 0; i < currQuad; ++i){
        check_quad_funcs(&quads[i]);
        (*generators[quads[i].op])(&quads[i]);
        current_proc_quad++;
    }
    patch_incomplete_jumps();
    return;
}

void check_quad_funcs(quads_t* q){
    if(q->arg1!=NULL){
        if(q->arg1->sym!=NULL){
            if(q->arg1->sym->type==LIBFUNC)
                q->arg1->type=libraryfunc_e;
            else if(q->arg1->sym->type==USERFUNC)
                q->arg1->type=programfunc_e;
        }
    }

    if(q->arg2!=NULL){
        if(q->arg2->sym!=NULL){
            if(q->arg2->sym->type==LIBFUNC)
                q->arg2->type=libraryfunc_e;
            else if(q->arg2->sym->type==USERFUNC)
                q->arg2->type=programfunc_e;
        }
    }
    if(q->result!=NULL){
        if(q->result->sym!=NULL){
            if(q->result->sym->type==LIBFUNC)
                q->result->type=libraryfunc_e;
            else if(q->result->sym->type==USERFUNC)
                q->result->type=programfunc_e;
        }
    }
    return;
}

unsigned nextinstructionlabel()
{
    return currInstr;
}

void expandInstr(void){
    assert(totalInstr==currInstr);

    instruction *i=(instruction*)malloc(NEW_SIZE);

    if(instructions){
        memcpy(i, instructions, CURR_SIZE);
        free(instructions);
    }
    instructions=i;
    totalInstr+=EXPAND_SIZE;
    return;
}

void emit_instr(instruction t)
{
    if(currInstr==totalInstr)
        expandInstr();

    instructions[currInstr] = t;
    currInstr++;
    return;
}

void make_operand(expr *e, vmarg *arg){
    assert(e!=NULL);
    bool found=false;
    unsigned i=0;
    switch(e->type){
        case var_e:
        case assignexpr_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case newtable_e:{
            arg->val = e->sym->scope_offset;
            switch(e->sym->scope_space){
                case programm_variables:
                    arg->type = global_a;
                    break;
                case function_locals:
                    arg->type = local_a;
                    break;
                case formal_arguments:
                    arg->type = formal_a;
                    break;
                default:
                    cout << "Id is " << e->sym->id << endl;
                    cout << "Type is " << e->type << endl;
                    cout << "scope is: " << e->sym->scope_offset << endl;
                    assert(0);
            }   
            break;
        }
        case constbool_e:
            arg->val = e->boolConst;
            arg->type = bool_a;
            break;
        case conststring_e:
            arg->val = consts_newstring(e->strConst);
            arg->type = string_a;
            break; 
        case constint_e:
            arg->val = consts_newnumber(e->intConst);
            arg->type = number_a;
            break;
        case constdouble_e:
            arg->val = consts_newnumber(e->doubleConst);
            arg->type = number_a;
            break;
        case nil_e:
            arg->type = nil_a;
            break;
        case programfunc_e:
            arg->type = userfunc_a;
            while(i<funcArray.size() && found==false){
                if(funcArray[i]->taddress==e->sym->taddress){
                    arg->val=i;
                    found=true;
                }
                i++;
            }
            assert(found!=false);
            break;
        case libraryfunc_e:
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->value.funcVal->name);
            break;
        default:
            cout << "type: " << e->type << endl;
            assert(0);
    }
}

void reset_instruction(instruction *t)
{
    t->arg1.type = reset;
    t->arg2.type = reset;
    t->result.type = reset;
}

void generate(vmopcode op, quads_t *q) 
{
    instruction t;
    reset_instruction(&t);
    t.opcode = op;
    t.srcLine = q->line;
    if(q->arg1!=NULL)
        make_operand(q->arg1, &t.arg1);
    if(q->arg2!=NULL)
        make_operand(q->arg2, &t.arg2);
    if(q->result!=NULL)
        make_operand(q->result, &t.result);

    q->taddress = nextinstructionlabel();
    emit_instr(t);
}

void generate_ADD (quads_t *q) 
{ 
    generate(add_v, q); 
}

void generate_SUB (quads_t  *q) 
{
    generate(sub_v, q); 
}

void generate_MUL (quads_t  *q) 
{ 
    generate(mul_v, q); 
}

void generate_DIV (quads_t  *q) 
{ 
    generate(div_v, q); 
}

void generate_MOD (quads_t  *q) 
{ 
    generate(mod_v, q); 
}

void generate_NEWTABLE (quads_t  *q) 
{ 
    generate(newtable_v, q); 
}

void generate_TABLEGETELEM (quads_t  *q)
{
    generate(tablegetelem_v, q); 
}

void generate_TABLESETELEM (quads_t  *q) 
{ 
    generate(tablesetelem_v, q); 
}

void generate_ASSIGN (quads_t  *q) 
{ 
    generate(assign_v, q); 
}

void generate_UMINUS(quads_t *q){
    q->taddress = nextinstructionlabel();
    instruction t;
    reset_instruction(&t);
    t.opcode = mul_v;
    t.srcLine = q->line;
    expr_t* newex=newexpr_constint(-1);
    make_operand(q->arg1, &t.arg1);
    make_operand(newex, &t.arg2);
    make_operand(q->result, &t.result);
    emit_instr(t);
    return;
}

void generate_JUMP (quads_t  *q) 
{ 
    generate_relational(jump_v, q); 
}

void generate_IF_EQ (quads_t  *q)
{ 
    generate_relational(jeq_v, q);
}

void generate_IF_NOTEQ(quads_t  *q) 
{ 
    generate_relational(jne_v, q);
}

void generate_IF_GREATER (quads_t  *q) 
{ 
    generate_relational(jgt_v, q); 
}

void generate_IF_GREATEREQ(quads_t  *q) 
{ 
    generate_relational(jge_v, q);
}

void generate_IF_LESS (quads_t  *q) 
{ 
    generate_relational(jlt_v, q); 
}

void generate_IF_LESSEQ (quads_t  *q) 
{ 
    generate_relational(jle_v, q); 
} 

void generate_NOP (quads_t  *q) 
{ 
    instruction t; 
    reset_instruction(&t);
    t.opcode= nop_v; 
    t.srcLine = q->line; 
    emit_instr(t); 
} 

void generate_relational (vmopcode op, quads_t *q) {
    instruction t;
    reset_instruction(&t);
    t.srcLine = q->line;
    t.opcode = op;
    if(q->arg1!=NULL)
        make_operand(q->arg1, &t.arg1);
    if(q->arg2!=NULL)
        make_operand(q->arg2, &t.arg2);
    t.result.type = label_a;
    if(q->label < current_proc_quad){

        t.result.val = quads[q->label].taddress; 
    }
    else{
        add_incomplete_jump(nextinstructionlabel(), q->label);}
    q->taddress = nextinstructionlabel();
    emit_instr(t);
}

void generate_PARAM(quads_t  *q) 
{
    q->taddress = nextinstructionlabel();
    instruction t;
    reset_instruction(&t);
    t.opcode = pusharg_v;
    t.srcLine = q->line;
    make_operand(q->arg1, &t.arg1);
    emit_instr(t);
}

void generate_CALL(quads_t  *q) 
{
    q->taddress = nextinstructionlabel();
    instruction t;
    reset_instruction(&t);
    t.opcode = call_v;
    t.srcLine = q->line;
    make_operand(q->arg1, &t.arg1);
    emit_instr(t);
}

void generate_GETRETVAL(quads_t  *q) {
    q->taddress = nextinstructionlabel();
    instruction t;
    reset_instruction(&t);
    t.opcode = assign_v;
    t.srcLine = q->line;
    make_operand(q->result, &t.result);
    make_retvaloperand(&t.arg1);
    emit_instr(t);
}

void generate_FUNCSTART(quads_t  *q)
{
    assert(q->arg1->sym!=NULL);
    SymbolTableEntry *f=q->arg1->sym;
    f->value.funcVal->iaddress=nextinstructionlabel();
    q->taddress=nextinstructionlabel();
    funcArray.push_back(f);
    funcstack.push(f);
    instruction t;
    reset_instruction(&t);
    t.opcode=funcenter_v;
    t.srcLine=q->line;
    make_operand(q->arg1, &t.result);
    t.result.val=funcArray.size()-1;
    t.arg1.type = reset;
    t.arg1.val = 0;
    t.arg2.type = reset;
    t.arg2.val = 0;
    emit_instr(t);
    return;
}

void generate_RETURN(quads_t  *q){
    SymbolTableEntry* f;
    q->taddress=nextinstructionlabel();
    instruction t;
    reset_instruction(&t);
    t.opcode=assign_v;
    make_retvaloperand(&t.result);
    t.result.val=0;
    t.srcLine=q->line;
    if(q->arg1!=NULL)
        make_operand(q->arg1, &t.arg1);
    emit_instr(t);

    reset_instruction(&t);

    f=funcstack.top();
    f->value.funcVal->returnList.push_back(nextinstructionlabel());
    t.opcode=jump_v;
    t.srcLine=q->line;
    t.result.type=label_a;
    emit_instr(t);
    return;
}

void generate_FUNCEND(quads_t *q){
    SymbolTableEntry* f=funcstack.top();
    funcstack.pop();
    unsigned index;
    for(unsigned i=0; i<funcArray.size(); i++){
        if(funcArray[i]->value.funcVal->iaddress == f->value.funcVal->iaddress){
            index=i;
            break;
        }
    }
    backpatch_instructions(f->value.funcVal->returnList, nextinstructionlabel());
    q->taddress=nextinstructionlabel();
    instruction t;
    reset_instruction(&t);
    t.srcLine=q->line;
    t.opcode=funcexit_v;
    t.result.type = userfunc_a;
    t.result.val = index;
    emit_instr(t);
    return;
}

void backpatch_instructions(vector<unsigned>&list, unsigned addr){
    
    for(unsigned int i=0; i<list.size(); i++){
        instructions[list[i]].result.val = addr;
        instructions[list[i]].result.type = label_a;
        list[i]=addr; 
    }
    return;
}

void make_numberoperand(vmarg *arg, double val)
{
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}

void make_booloperand(vmarg *arg, unsigned val)
{
    arg->val = val;
    arg->type = bool_a;
}

void make_retvaloperand(vmarg *arg){
    arg->type = retval_a; 
}


unsigned consts_newstring(string s){
    int index;
   // auto it = find(stringArray.begin(), stringArray.end(), s);
   // if (it != stringArray.end()){
   //     index = it - stringArray.begin();
   // }
   //// else{
        index = stringArray.size();
        s=remove_quotes(s);
        stringArray.push_back(s);
    //}
    return index;
}

unsigned consts_newnumber(double num){
    int index;
   // auto it = find(numArray.begin(), numArray.end(), num);
    //if (it != numArray.end()){
      //  index = it - numArray.begin();
   // }
    //else{
        index = numArray.size();
        numArray.push_back(num);
    //}
    return index;
}

unsigned libfuncs_newused(string funcname){
    int index;
    auto it = find(libfuncArray.begin(), libfuncArray.end(), funcname);
    if (it != libfuncArray.end()){
        index = it - libfuncArray.begin();
    }
    else{
        index = libfuncArray.size();
        libfuncArray.push_back(funcname);
    }
    return index;
}

void add_incomplete_jump(unsigned instrNo, unsigned iaddress){
    incomplete_jump ij;
    ij.instrNo=instrNo;
    ij.iaddress=iaddress;
    ij.next=NULL;
    ijArray.push_back(ij);
    ij_total++;
    return;
}

void patch_incomplete_jumps(void){
    for(unsigned int i=0; i<ij_total; i++){
        instructions[ijArray[i].instrNo].result.type=label_a;
        if(ijArray[i].iaddress==nextquadlabel()){
            instructions[ijArray[i].instrNo].result.type = label_a;
            instructions[ijArray[i].instrNo].result.val=nextinstructionlabel();
        }else{
                        instructions[ijArray[i].instrNo].result.type = label_a;

            instructions[ijArray[i].instrNo].result.val=quads[ijArray[i].iaddress].taddress;
        }
    }
    return;
}


void print_instructions_bin(string fname){
    fname=fname+".abc";
    FILE *binfile=fopen(fname.c_str(), "wb");
    int i;
    string buff;
    int size;
    int magic_number=420420420;
    assert(binfile!=NULL);
    int tmp;

    fwrite(&magic_number, sizeof(magic_number), 1, binfile);
    
    tmp=get_num_of_globals();
    cout << "***************************** " << tmp << endl;
    fwrite(&tmp, sizeof(tmp), 1, binfile);

    size=stringArray.size();
    fwrite(&size, sizeof(size), 1, binfile);
    for (i=0; i<size; i++){
        int len;
        len=stringArray[i].size();
        fwrite(&len, sizeof(len), 1, binfile);
        fwrite(stringArray[i].c_str(), len, 1, binfile);
    }

    size=funcArray.size();
    fwrite(&size, sizeof(size), 1, binfile);
    for(unsigned int i=0; i<funcArray.size(); i++){
        int len;
        len=funcArray[i]->id.size();
        fwrite(&len, sizeof(len), 1, binfile);
        fwrite(funcArray[i]->id.c_str(), len, 1, binfile);
        fwrite(&funcArray[i]->value.funcVal->iaddress, sizeof(funcArray[i]->value.funcVal->iaddress), 1, binfile);
        fwrite(&funcArray[i]->value.funcVal->totalLocals, sizeof(funcArray[i]->value.funcVal->totalLocals), 1, binfile);
    }

    size=libfuncArray.size();
    fwrite(&size, sizeof(size), 1, binfile);
    for(i=0; i<size; i++){
        int len;
        len=libfuncArray[i].size();
        fwrite(&len, sizeof(len), 1, binfile);
        fwrite(libfuncArray[i].c_str(), len, 1, binfile);
    }

    size=numArray.size();
    fwrite(&size, sizeof(size), 1, binfile);
    for(i=0; i<size; i++){
        fwrite(&numArray[i], sizeof(numArray[i]), 1, binfile);
    }

    fwrite(&currInstr, sizeof(currInstr), 1, binfile);
    for(unsigned i=0; i<currInstr; i++){
        fwrite(&instructions[i].opcode, sizeof(instructions[i].opcode), 1, binfile);

        fwrite(&instructions[i].result.type, sizeof(instructions[i].result.type), 1, binfile);
        fwrite(&instructions[i].result.val, sizeof(instructions[i].result.val), 1, binfile);

        fwrite(&instructions[i].arg1.type, sizeof(instructions[i].arg1.type), 1, binfile);
        fwrite(&instructions[i].arg1.val, sizeof(instructions[i].arg1.val), 1, binfile);

        fwrite(&instructions[i].arg2.type, sizeof(instructions[i].arg2.type), 1, binfile);
        fwrite(&instructions[i].arg2.val, sizeof(instructions[i].arg2.val), 1, binfile);
        fwrite(&instructions[i].srcLine, sizeof(instructions[i].srcLine), 1, binfile);
    }
    fclose(binfile);
    return;
}

void print_instructions(void){
    unsigned int i;
    if(stringArray.size()!=0)
        cout << "------------STRING ARRAY--------------\n";
    for(i=0; i<stringArray.size(); i++){
        cout << "ELEMENT: " << i << ", " << stringArray[i] << endl;
    }
    if(stringArray.size()!=0)
        cout << endl;

    if(funcArray.size()!=0)
        cout << "------------USER FUNCTION ARRAY--------------\n";
    for(i=0; i<funcArray.size(); i++){
        cout << "ELEMENT: " << i << ", FUNC ID: " << funcArray[i]->id << ", FUNC ADDR: "
        << funcArray[i]->value.funcVal->iaddress << ", FUNC LOCALS: " << funcArray[i]->value.funcVal->totalLocals << endl;
    }
    if(funcArray.size()!=0)
        cout << endl;

    if(libfuncArray.size()!=0)
        cout << "------------LIB FUNC ARRAY--------------\n";
    for(i=0; i<libfuncArray.size(); i++){
        cout << "ELEMENT: " << i << ", " << libfuncArray[i] << endl;
    }
    if(libfuncArray.size()!=0)
        cout << endl;

    if(numArray.size()!=0)
        cout << "------------NUMBER ARRAY--------------\n";
    for(i=0; i<numArray.size(); i++){
        cout << "ELEMENT: " << i << ", " << numArray[i] << endl;
    }

    cout << "\n--------------------------------\n";

    for(i=0; i<currInstr; i++){
        cout << "Instr: " << i << ", ";
        cout << "OpCode: " << getOpcode(instructions[i].opcode);
        if(instructions[i].result.type!=reset){
            cout << ", RESULT: " + print_vmargs(instructions[i].result, false);
        }
        if(instructions[i].arg1.type!=reset){
            cout << ", ARG1: " + print_vmargs(instructions[i].arg1, false);
        }
        if(instructions[i].arg2.type!=reset){
            cout << ", ARG2: " + print_vmargs(instructions[i].arg2, false);
        }
        if(instructions[i].result.type!=retval_a)
            cout << " (line:" << instructions[i].srcLine << ")\n";
        else
            cout << endl;
    }
    return;
}



string print_vmargs(vmarg arg, bool bin){
    string s;
    if(bin)
        s="-";
    else
        s=" --> ";

    switch(arg.type){
        case reset:
            return "RESET";
        case label_a:
            return "label_a" + s + to_string(arg.val);
        case global_a:
            return "global_a" + s + to_string(arg.val);
        case formal_a:
            return "formal_a" + s + to_string(arg.val);
        case local_a:
            return "local_a" + s + to_string(arg.val);
        case number_a:
            return "number_a" + s + to_string(arg.val);
        case string_a:
            return "string_a" + s + to_string(arg.val);
        case bool_a:
            return "bool_a" + s + to_string(arg.val);
        case nil_a:
            return "nil_a";
        case userfunc_a:
            return "userfunc_a" + s + to_string(arg.val);
        case libfunc_a:
            return "libfunc_a" + s + to_string(arg.val);
        case retval_a:
            return "retval_a" + s + to_string(arg.val);
        default:
            cout << "\nType is : " << arg.type << endl;
            assert(0);
    }
}

string getOpcode(vmopcode op){
    switch(op){
        case assign_v:
            return "assign_v";
        case add_v:
            return "add_v";
        case sub_v:
            return "sub_v";
        case mul_v:
            return "mul_v";
        case div_v:
            return "div_v";
        case mod_v:
            return "mod_v";
        case uminus_v:
            return "uminus_v";
        case jeq_v:
            return "jeq_v";
        case jne_v:
            return "jne_vf";
        case jle_v:
            return "jle_v";
        case jge_v:
            return "jge_v";
        case jlt_v:
            return "jlt_v";
        case jgt_v:
            return "jgt_v";
        case call_v:
            return "call_v";
        case pusharg_v:
            return "pusharg_v";
        case funcenter_v:
            return "funcenter_v";
        case funcexit_v:
            return "funcexit_v";
        case newtable_v:
            return "newtable_v";
        case tablegetelem_v:
            return "tablegetelem_v";
        case tablesetelem_v:
            return "tablesetelem_v";
        case jump_v:
            return "jump_v";
        case nop_v:
            return "nop_v";
            break;
        default:
            assert(0);
    }
    assert(0);
}