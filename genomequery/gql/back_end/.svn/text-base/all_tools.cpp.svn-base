#include<iostream>
#include<fstream>
#include<string>
#include "Table.hpp"
#include "Reads.hpp"
#include "Text_table.hpp"
#include "Intervals.hpp"
#include "Joined.hpp"
#include "defs.hpp"

using namespace std;

void user_error(string msg){
	cout << "FATAL ERROR!!! " << msg << endl;
	exit(1);
}

void unknown_command(){
	user_error("usage: ./all_tools filter_reads | filter_intervals | filter_text | filter_joined | reads2intervals | text2intervals | joined2intervals | interval_merge | joined_creation | print ");
}



void to_string(int argc, char **argv, vector<string>& sargv){
	int i=0;
	for(i=0;i<argc;i++)
		sargv.push_back(argv[i]);
}

int main(int argc, char **argv){
	vector<string> sargv;
	to_string(argc, argv, sargv);
	if(argc==1)
		unknown_command();
	if(sargv[1]=="filter_reads"){
		if(argc!=5 && argc!=4)
			user_error("usage: ./all_tools filter_reads <indx_file> <dst_file> [<src_filter>]");
		Reads tbl(sargv[2]);
		if(argc==4) tbl.filter(sargv[3]);
		else tbl.filter(sargv[4], sargv[3]);
	}
	else if(sargv[1]=="filter_intervals"){
		if(argc!=4)
			user_error("usage: ./all_tools filter_intervals <indx_file> <dst_file>");
		Intervals tbl(sargv[2]); ///tbl.load()????
		tbl.filter(sargv[3]);
	}
	else if(sargv[1]=="filter_text"){
		if(argc!=4)
			user_error("usage: ./all_tools filter_text <indx_file> <dst_file>");
		Text_table tbl(sargv[2]);
		tbl.filter(sargv[3]);
	}
	else if(sargv[1]=="filter_joined"){
		if (argc!=4)
			user_error("usage: ./all_tools filter_joined <indx_file> <dst_file>");
		Joined tbl(sargv[2]);
		//tbl.load();
		tbl.filter(sargv[3]);
	}
	else if(sargv[1]=="reads2intervals"){
		if (argc!=6)
			user_error("usage: ./all_tools reads2intervals <rindx_file> <filter_file> <mates_on> <dst_file>");
		int mates_on=atoi(argv[4]);
		Reads srctbl(sargv[2]);
		Intervals dsttbl(srctbl, argv[3], mates_on, argv[5]);
		dsttbl.store();
	}
	else if(sargv[1]=="text2intervals"){
		if (argc!=4)
			user_error("usage: ./all_tools text2intervals <tbl_file> <dst_file>");
		Text_table stbl(argv[2]);
		Intervals itbl(stbl, argv[3]);
		itbl.store();
	}
	else if(sargv[1]=="joined2intervals"){
		if (argc!=4) 
			user_error("usage: ./all_tools joined2intervals <joined_file> <dst_file>");
		Joined stbl(argv[2]);
		Intervals itbl(stbl, argv[3]);
		itbl.store();
	}
	else if(sargv[1]=="interval_merge"){
		if (argc!=6)
			user_error("usage: ./all_tools interval_merge <src_interval_file> <dst_interval_file> lower|upper <coverage>");
		Intervals stbl(sargv[2]);
		stbl.load();
		unsigned coverage=atoi(argv[5]);
		if(sargv[4] == "lower") stbl.merge_low_bound(coverage, sargv[3]);
		else if(sargv[4] == "upper") stbl.merge_upper_bound(coverage, sargv[3]);
		else user_error("usage: ./all_tools interval_merge <src_interval_file> <dst_interval_file> lower|upper <coverage>");
	}
	else if(sargv[1]=="joined_creation"){
		if (argc!=9)
			user_error("usage: ./all_tools joined_creation <dst_file> <src1_file> <type1> <src2_file> <type2> <intrvl_file1> <intrvl_file2>");
		Intervals in1(sargv[7]);
		Intervals in2(sargv[8]);
		in1.load(); in2.load();
		Joined dst(sargv[2], sargv[3], sargv[5], sargv[4], sargv[6], in1, in2);
		dst.store();
	}
	else if(sargv[1]=="print"){
		if(argc!=5 && argc!=6)
			user_error("usage ./all_tools print <type> <table_file> [filter_file] <out_file>");
		if(sargv[2]=="Reads"){
			if(argc!=6) user_error("usage ./all_tools print Reads <table_file> <filter_file> <output_prefx>");
			Reads rd(sargv[3]);
			ifstream fp(sargv[4].c_str());
			int vec_len;
			vector<int> flt=get_vector<int>(fp, vec_len);
			fp.close();
			rd.print(sargv[5], flt);
		}
		else if(argc!=5) user_error("usage ./all_tools print Text_table|Intervals|Joined <table_file> <out_prefx>");
		else if(sargv[2]=="Text_table"){
			Text_table tt(sargv[3]);
			tt.print(sargv[4]);
		}
		else if(sargv[2]=="Intervals"){
			Intervals it(sargv[3]);
			it.load();
			it.print(sargv[4]);
		}
		else if(sargv[2]=="Joined"){
			Joined jt(sargv[3]);
			//jt.load();
			jt.print(sargv[4]);
		}
	}

			
	else
		unknown_command();
}



