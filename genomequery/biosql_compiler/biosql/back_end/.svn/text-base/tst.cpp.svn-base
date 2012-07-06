#include<iostream>
#include<fstream>
#include "Command.hpp"
#include "Reads.hpp"
#include "Histogram.hpp"
#include "Text_table.hpp"
#include "Intervals.hpp"
#include "Interval_tree.hpp"
#include "Joined.hpp"

using namespace std;



int main(){
	//Command cmd;
	/*cmd.set_int(2, -1);
	cmd.set_operator("-unr", -1);
	cmd.set_int(0, -1);
	cmd.set_operator("<=", -1);
	cmd.set_const_str("mlk", -1);
	cmd.set_const_str("skt", -1);
	cmd.set_operator("==", -1);
	cmd.set_operator("!", -1);
	cmd.set_operator("&&", -1);
	cmd.set_int(2, -1);
	cmd.set_float(3.0, -1);
	cmd.set_operator("+", -1);
	cmd.set_float(2.0, -1);
	cmd.set_operator("+", -1);
	cmd.set_int(3, -1);
	cmd.set_operator("+", -1);
	cmd.set_float(10.4, -1);
	cmd.set_operator("==", -1);
	cmd.set_operator("&&", -1);
	int ev=cmd.evaluate();
	cout << "ev: " << ev << endl;*/

	string st1("NA18507/chr1.bam"), st2("NA18507/chr1.mates.indx");
	Reads rds(st2);
	//string cmd_str("int 0 intvar loc operator <=");
	vector<int>* filter1=rds.filter();
	for(int i=0;i<15;i++) cout << (*filter1)[i] << endl;

	/*string st3("H1.vld");
	string cmd_str2("int 0 intvar mate_loc operator <=");
	rds.filter(filter1, st3, cmd_str2);

	int ln;
	ifstream f3(st3.c_str());
	vector<int> filter2=get_vector<int> (f3, ln);
	
	
	Histogram hist(250000000);
	string dst("out.bam");
	rds.print(filter2, dst, hist);*/
	/*string cmd_str_st("intvar arg4");
	string cmd_str_nd("intvar arg5");
	//string ln("uc001aaa.2	chr1	+	1115	4121	1115	1115	3	1115,2475,3083,	2090,2584,4121,		uc001aaa.2");
	//string fname("genes.txt");
	string fname("chr1.genes.txt");
	Text_table tt(fname);
	/*vector <string>	variables;
	vector <string> types;
	vector <int> symbol_pos;

	variables=cmd.get_variables(types, symbol_pos);*/
	//tt.filter(cmd, ofname);
	/*string dst_in("dst_intervals");
	Intervals in_t(tt, cmd_str_st, cmd_str_nd, dst_in);
	//Interval_tree it(in);
	//Intervals res(" ");
	//it.search_range(0,1000000000, res);
	//cout << res.size() << endl;

	string st1("NA18507/chr1.mates.indx");
	Reads rds(st1);
	string cmd_str("int 0 intvar loc operator <= int 1000 intvar loc operator >= operator &&");
	rds.filter("read_flt", cmd_str);

	cmd_str_st="intvar loc";
	cmd_str_nd="intvar loc intvar read_len operator +";
	Intervals in_r(rds, "read_flt", cmd_str_st, cmd_str_nd, 0, "reads.intrvl");

	Joined jj("a.joined", "0"+st1, fname, "Reads", "Text_table", in_r, in_t);
	string cmd_str_j("intvar NA18507/chr1.mates.indx:loc int 0 operator >");
	jj.filter("b.joined", cmd_str_j);
	Joined jb("b.joined");
	jb.print("out");*/



	

	//in.print();
	//tt.filter(ofname, cmd_str);
	//cout << "done" << endl;
	
	//tt.resolve_variables(ln, cmd, variables, types, symbol_pos);

	//cout << "arg2 " << cmd.get_str(1) << endl;
}
