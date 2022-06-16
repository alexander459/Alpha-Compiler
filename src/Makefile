CC = g++
CFLAGS = -Wall -ansi -pedantic -g -std=c++11
SAN = -fsanitize=address
parser: parser.o scanner.o utils/util.o tokenlist/tokenlist.o symboltable/symboltable.o intermediate/intermediate.o quads/quads.o scopespace/scopespace.o expression_list/expression_list.o final_code/final_code.o
	@$(CC) $(CFLAGS)  $^ -o $@
%.o: %.cpp
	@$(CC) $(CFLAGS) -o $@ -c $<

scanner.cpp: scanner.l utils/util.hpp tokenlist/tokenlist.hpp
	@flex scanner.l
parser.cpp: utils/util.hpp symboltable/symboltable.o intermediate/intermediate.o quads/quads.o scopespace/scopespace.o expression_list/expression_list.o final_code/final_code.o
	@bison --yacc --defines -v --output=parser.cpp parser.y

clean:
	@rm -f parser
	@rm -f parser.output
	@rm -f parser.cpp
	@rm -f parser.hpp
	@rm *.o
	@rm -f scanner.cpp
	@rm -f utils/*.o tokenlist/*.o symboltable/*.o intermediate/*.o quads/*.o
	@rm -f scopesapace/*.o expression_list/expression_list.o scopespace/*.o final_code/*.o libfunctions/*.o