#include <string>
#include <iostream>

using namespace std;

typedef struct alpha_token_t {
  unsigned int     numline;
  unsigned int     numToken;
  string           content;
  string           type;
}token_t;


void update_token_num(unsigned int num);
void insert(string token,string type, int line);
//void print_list();