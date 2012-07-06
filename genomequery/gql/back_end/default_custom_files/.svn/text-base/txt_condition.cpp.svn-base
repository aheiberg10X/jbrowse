#include "Text_table.hpp"
#include <sstream>
#include <cstring>

int Text_table::eval_condition(string line){
	vector<string> tokens;
	stringstream ss(line);
	string str;
	while(ss >> str) tokens.push_back(str);
	return tokens[1]=="chr1";
}

