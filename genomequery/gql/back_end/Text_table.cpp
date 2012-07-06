#include "Text_table.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

		/*It updates the variables of cmd with their values from the input line.
The input vectors are the outcome of the call cmd.get_variables*/
void Text_table::resolve_variables(string line, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
	map<string, vector<int> > varmap;
	int argno=1;
	int j=0;
	stringstream ss(line);
	string buf;
	string arg;
	for(int i=0;i<variables.size();i++)
		varmap[variables[i]].push_back(i);
	
	map<string, vector<int> >::iterator it;
	while(ss >> buf){
		//tokenize the string and check whether current token appears in the 
		//variable list.
		stringstream str_i;
		str_i << argno;
		arg="arg"+str_i.str();
		it=varmap.find(arg);
		if(it==varmap.end()){
			argno++;
			continue;
		}
		//iterate over the elements of it->sec and update accordingly
		for(j=0;j<((*it).second).size(); j++){
			stringstream ssbuf(buf);
			int k=((*it)).second[j];
			if (types[k]=="int"){
				int intval;
				ssbuf >> intval;
				cmd.set_int(intval, symbol_pos[k]);
			}
			else if(types[k]=="float"){
				float floatval;
				ssbuf >> floatval;
				cmd.set_float(floatval, symbol_pos[k]);
			}
			else if(types[k]=="char"){
				char charval;
				ssbuf >> charval;
				cmd.set_float(charval, symbol_pos[k]);
			}
			else if(types[k]=="string"){
				string strval;
				ssbuf >> strval;
				cmd.set_const_str(strval, symbol_pos[k]);
			}
		}
		argno++;
	}
}

/*it creates a new file that contains only the lines that 
pass the filtering*/
void Text_table::filter(string dst_file){
	ifstream inf(file_loc.c_str());
	ofstream of(dst_file.c_str());
	//Command cmd(command_str);
	string line;
	while (getline(inf, line)){
		if(line.size()==0) continue; //ignore blank lines
		if(line[0]=='#' || line[0]=='>') continue; //ignore comments

		//this->resolve_variables(line, cmd, variables, types, symbol_id);
		//int ev=cmd.evaluate();
		int ev=eval_condition(line);
		if(ev==1)
			of << line << endl;
		else if(ev!=0) throw new non_boolean_expr();
		//cmd.clear();
	}
	of.close();
	inf.close();
}



/*It prints the contents of the table that are indicated by flt*/
void Text_table::print(string prefix, vector<int> &flt){
	ofstream of((prefix+".txt").c_str());
	int i=0, j=0;
	sort(flt.begin(), flt.end());
	iterator it;
	for(it=begin();it!=end();it++, i++){
		if (j>=flt.size()) 
			break;
		if(i==flt[j]){
			of << *it << endl;
			j++;
		}
	}
	of.close();
	print_metalinks(prefix+".links.txt", flt);
}

/*It prints the association between the rank of an entry in the original txt 
file and the rank of the same entry in the output txt file after applying 
filter flt*/
void Text_table::print_metalinks(string outfile, vector<int> &flt){
	vector<int> outvec=flt;
	sort(outvec.begin(), outvec.end());
	ofstream ofile;
	ofile.open(outfile.c_str(), ios::out);
	ofile << "current_rank original_rank" << endl;
	for(int i=0;i<flt.size();i++)
		ofile << i << " " << outvec[i] << endl;
	ofile.close();
}

/*It prints all contents of the table*/
void Text_table::print(string prefx){
	ofstream of((prefx+".txt").c_str());
	iterator it;
	for(it=begin();it!=end();it++){
		of << *it << endl;
	}
	of.close();

}
