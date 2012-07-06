#include "Text_table.hpp"
#include <sstream>
#include <cstdlib>

void Text_table::eval_expressions(string line, int& expr1, int& expr2){
	vector<string> tokens;
	stringstream ss(line);
	string str;
	while(ss >> str) tokens.push_back(str);
	expr1= atoi(tokens[1].c_str()) ;
	expr2= atoi(tokens[2].c_str()) ;
}
