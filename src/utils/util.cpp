#include "util.hpp"
#include <string.h>
#include <iostream>
#include "../symboltable/symboltable.hpp"


int errors=0;

extern string lib_funcs[12];
string upperCase(string str){
  int i;
  for(i=0; str[i]!=0; i++)
    str[i]=::toupper(str[i]);
  return str;
}


string str_append(string _str, char character){
  _str += character;
  return _str;
}


bool check_func(string name){
  string lib_funcs[12] = { "print",
				 "input",
				 "objectmemberkeys",
				 "objecttoalmembers",
				 "objectcopy",
				 "totalarguments",
				 "argument",
				 "typeof",
				 "strtonum",
				 "sqrt",
				 "cos",
				 "sin" };
  int i;
  for(i=0; i<12; i++){
    if(name==lib_funcs[i])
      return false;
  }
  return true;
}



bool is_valid_string(char* str){
  int length=strlen(str);
  int i;
  for(i=0; i<length; i++){
    if(str[i]=='\\'){
      i++;
      if(str[i]!='n' && str[i]!='t' && str[i]!='\\' && str[i]!='\"' && str[i]!='r')
        return false;
    }
  }
  return true;
}


string format_str(char* str){
  int length=strlen(str);
  int i;
  string new_str="";
  for(i=0; i<length; i++){
    if(str[i]=='\\'){
      i++;
      switch (str[i]){
      case 'n':
        new_str+=10;
        break;
      case 't':
        new_str+=9;
        break;
      case '\\':
        new_str+=92;
        break;
      case '\"':
        new_str+=34;
        break;
      case 'r':
        new_str+=13;
        break;
      }
    }else{
      new_str+=str[i];
    }
  }
  return new_str;
}

using namespace std;
string isolate_func_name(string name){
  assert(!name.empty());
  string func="";
  int i;
  for(i=0; name[i]!='(' && name[i]!=' ' && name[i]!='\0'; i++);
  if(name[i]=='\0')
    return name;
  func=name.substr(0, i);
  return func;
}

int get_errors(void){
  return errors;
}

void incr_error(void){
  errors++;
  return;
}

void comperror (string msg, int line){
  errors++;
  printf(RED "%s in line %d\n" WHITE, msg.c_str(), line);
}

bool check_arith (struct expr* e, int line) {
  if (e->type == constbool_e ||
      e->type == conststring_e || 
      e->type == nil_e ||
      e->type == newtable_e ||
      e->type == programfunc_e ||
      e->type == libraryfunc_e ||
      e->type == boolexpr_e) {
        comperror("Illegal expr used", line);
        return false;
      }
  return true;
}

string remove_quotes(string s){
  if(s[0]=='\"' && s[s.size()-1]=='\"')
    return s.substr(1, s.size()-2);
  return s;
}

bool is_temp_var(string name){
  return name[0]=='^';
}

bool is_temp_expr(expr_t* e){
  assert(e!=NULL);
  return e->sym && is_temp_var(e->sym->id);
}

void check_func_assign(SymbolTableEntry* sym, int line){
  SymbolTableEntry* ent;

  if(sym==NULL){
    //comperror("Error: Cannot do assignment with functions", line);
    return;
  }
  ent=look_up(sym->id);

  if(ent==NULL)
    return;
  
  if(ent->type!=USERFUNC && ent->type!=LIBFUNC)
    return;

  if(ent->isActive==false)
    return;

  if((ent->type==LIBFUNC) || (sym->value.varVal->scope <= ent->value.funcVal->scope)){
    comperror("Error: Cannot do assignment with functions", line);
  }
}


expr_t* stringBoolToBool(expr_t* exp){
  if(exp->type==conststring_e){
    if(exp->strConst=="true"){
      exp->type=constbool_e;
      exp->boolConst=1;
    }else if(exp->strConst=="false"){
      exp->type=constbool_e;
      exp->boolConst=0;
    }
  }
  return exp;
}