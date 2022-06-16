#include "tokenlist.hpp"
#include "../utils/util.hpp"
#include <bits/stdc++.h>


unsigned int num_of_tokens = 0;
ofstream out_file;
list<token_t*> token_list;

void insert(string token, string type, int line){
    token_t *node = new token_t;
    string tmp=token;
    if(type.compare("STRING")!=0)
      tmp=upperCase(token);
    type=type + " " + tmp;
    node->content = token;
    node->type = type;
    num_of_tokens = token_list.size() + 1; 
    node->numToken = num_of_tokens;
    node->numline = line;
    token_list.push_back(node);
}

void update_token_num(unsigned int num){
    num_of_tokens = num;
}

/*void print_list()
{
    list<token_t*>::iterator it;
    for(it=token_list.begin(); it!=token_list.end();it++)
    {
    if(!out_flag){
      if((*it)->content != "")
       cout << (*it)->numline << ": #" << (*it)->numToken << " " << (*it)->content << " " << (*it)->type  << " " << endl;
      else
       cout << (*it)->numline << ": #" << (*it)->numToken << " " << (*it)->type << endl;
    }
    else{
       out_file.open(file_name,ios_base::app);
       if(!out_file) { 
          cerr << "Error: Could not open file to be written." << endl;
          exit(1);
       }

       if((*it)->content != "") 
         out_file << (*it)->numline << ": #" << (*it)->numToken << " " << (*it)->content << " " << (*it)->type  << " " <<  endl;
       else
         out_file << (*it)->numline << ": #" << (*it)->numToken << " " << (*it)->type << endl;  
     }
    } 

    if(out_flag)
      out_file.close();
  }*/