#include "Intervals.hpp"
#include "Reads.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "defs.hpp"
#include "exceptions.hpp"
#include "Joined.hpp"

using namespace std;

class Joined;


void ioerror(string msg){
	cout << "FATAL ERROR!!! " << msg << endl;
	exit(1);
}


void Intervals::store(){
	ofstream of(file_loc.c_str());
	store_vector<Intrvl> (of, vec);
}

void Intervals::print(){
	for(int i=0;i<vec.size();i++)
		printf("%d\t%d\t%c\n",vec[i].st, vec[i].nd, 'F');
}

/*It prints the association between the rank of an entry in the original 
file and the rank of the same entry in the output file after applying 
filter flt*/
void Intervals::print_metalinks(string outfile, vector<int> &flt){
	ofstream ofile;
	ofile.open(outfile.c_str(), ios::out);
	ofile << "current_rank original_rank" << endl;
	for(int i=0;i<flt.size();i++)
		ofile << i << " " << flt[i] << endl;
	ofile.close();
}

void Intervals::print(string prefix, vector<int>& flt){
	ofstream of((prefix+"interval").c_str());
	int cur_i=0, i=0;
	for( cur_i=0;cur_i<flt.size();cur_i++){
		i=flt[cur_i];
		of << vec[i].st << "\t" << vec[i].nd << "\tF" << endl;
	}
	of.close();
	print_metalinks(prefix+"interval.links.txt", flt);
}

void Intervals::print(string prefix){
	ofstream of((prefix+".interval").c_str());
	for( int i=0;i<vec.size();i++){
		of << vec[i].st << "\t" << vec[i].nd << "\tF" << endl;
	}
	of.close();
}

/*It loads intervals from file_loc. Its use is recommended immediately after
the constructor call:Intervals(file_loc).*/
void Intervals::load(){
	int len;
	ifstream inf(file_loc.c_str());
	vec=get_vector<Intrvl>(inf, len);
	return;
}

/*It updates the variables of cmd with their values from the input line.
The input vectors are the outcome of the call to cmd.get_variables*/
void Intervals::resolve_variables(Intrvl in, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
	for(int k=0;k<variables.size();k++){
		if(variables[k]=="start"){
			if(types[k]!="int") throw new bad_type_field();
			cmd.set_int(in.st, symbol_pos[k]);
		}
		else if(variables[k]=="end"){
			if(types[k]!="int") throw new bad_type_field();
			cmd.set_int(in.nd, symbol_pos[k]);
		}
	}
}

/*it creates another set of intervals, subset of the current one and 
stores in a file. It either returns it or it stores it in a file*/
Intervals Intervals::filter(string dst_file){
	Intervals ret(dst_file);
	int ev;

	for(int i=0;i<vec.size(); i++){
		ev=eval_condition(i);
		if(ev==1) ret.append(vec[i].st, vec[i].nd, vec[i].rd1, vec[i].rd2);
		else if(ev!=0) throw new non_boolean_expr();
	}
	return ret;
	//ret.store();
}





/*It creates intervals from a read table which has been filtered and stored
in filtered_file. If mates_on is enabled, the resulted intervals will contain
back pointers to both mates of an involved clone.*/
Intervals::Intervals(Reads& tbl, string filtered_file, int mates_on, string dst_file):Table(dst_file){
	
	int ttl_filt=0;
	ifstream inf(filtered_file.c_str());
	vector<int> tbl_flt=get_vector<int>(inf, ttl_filt);
	inf.close();
	int ev_st, ev_nd;

	for(int i=0;i<ttl_filt; i++){
		int k=tbl_flt[i];
		int mate_k=tbl.get_mate(k);////
		Intrvl in;
		int ev_st, ev_nd;

		tbl.eval_expressions(k, ev_st, ev_nd);

		if(ev_st>ev_nd) continue;
		in.st=ev_st;
		in.nd=ev_nd;
		in.rd1=k;
		in.rd2=(mates_on==1 && mate_k>0)?mate_k:-1;
		vec.push_back(in);
	}
}


Intervals::Intervals(Text_table &tbl, string dst_file):Table(dst_file){
	Text_table::iterator it;
	int ev_st, ev_nd;
	int i=0;
	

	for(it=tbl.begin();it!=tbl.end(); it++){
		Intrvl in;
		tbl.eval_expressions(*it, ev_st, ev_nd);

		if(ev_st>ev_nd) continue;
		in.st=ev_st;
		in.nd=ev_nd;
		in.rd1=i;
		in.rd2=-1;
		vec.push_back(in);
		i++;
	}
}

//it deletes the memory that has been allocated by dereferencing a Joined table
void delete_joined_vectors(vector<long> vec_addr, vector<string> types){
	vector<int>* vint; vector<string>* vstr; vector<float>* vflt; vector<char>* vchar;
	for (int k=0;k<vec_addr.size();k++){
		if(types[k]=="int"){
			vint=reinterpret_cast<vector<int>* >(vec_addr[k]);
			delete vint;
		}
		else if(types[k]=="float"){
			vflt=reinterpret_cast<vector<float>* >(vec_addr[k]);
			delete vflt;
		}
		else if(types[k]=="char"){
			vchar=reinterpret_cast<vector<char>* >(vec_addr[k]);
			delete vchar;
		}
		else if	(types[k]=="string"){
			vstr=reinterpret_cast<vector<string>* >(vec_addr[k]);
			delete vstr;
		}
	}
}

Intervals::Intervals(Joined& tbl, string dst_file):Table(dst_file){


	for(Joined::iterator it=tbl.begin();it!=tbl.end();it++){
		int ev_st, ev_nd;
		tbl.eval_expressions(*it, ev_st, ev_nd);
		if(ev_st <= ev_nd)
			this->append(ev_st, ev_nd, *it, -1);

	}
	
}

Intervals::Intervals(Intervals& tbl, string dst_file):Table(dst_file){
	int i=0;
	for(i=0;i<tbl.size();i++){
		int ev_st, ev_nd;
		tbl.eval_expressions(i, ev_st, ev_nd);
		if(ev_st <= ev_nd)
			this->append(ev_st, ev_nd, i, -1);

	}
}



typedef struct{
	int loc;
	int intrvl_indx;//Interval location at the intrvl_list
	char direction; //0 for opening, 1 for closing.
} Unrolled_intervals;

/*
Find the first interval between prev_intrvl_start and intrvl_end that overlaps with [region_st, region_nd]. The function returns the index of that interval in the
interval_lst.
*/
/*int get_starting_interval(Intervals& intervals, int prev_intrvl_start, int intrvl_end, int region_st, int region_nd){
	if(prev_intrvl_start<0) prev_intrvl_start=-1;
	int i=0;
	int st=0,nd=0;
	for(i=prev_intrvl_start;i<=intrvl_end;i++){ //verify whether i=prev or i=prev+1
		if(i<0) continue;
		st=intervals[i].st;
		nd=intervals[i].nd;
		////nd=intrvl_lst[i].nd;
		if( (region_st<=st && st<=region_nd) || (st<=region_st && nd >= region_st))
			return i;
	}
}*/

/*
Find the last interval before intrvl_end that overlaps with [region_st, region_nd]. The function returns the index of that interval in the
interval_lst.
*/
/*int get_ending_interval(Intervals& intervals, int intrvl_end, int region_st, int region_nd){
	int i=0;
	int st=0,nd=0;
	for(i=intrvl_end-1;i>=0;i--){ //verify whether i=prev or i=prev+1
		st=intervals[i].st;
		nd=intervals[i].nd;
		if( (region_st<=st && st<=region_nd) || (st<=region_st && nd >= region_st))
			return i;
	}
	ioerror("Cannot find an ending interval that overlaps with region");
}*/

/*Useful to compare intervals*/
inline int intrvl_cmp(Unrolled_intervals a, Unrolled_intervals b){ return (a.loc < b.loc);}

//It takes an object of type Intervals and returns the corresponding vector
//of unrolled_intervals. The returned array
//is sorted by loc
///typedef Unrolled_intervals Ui;
vector<Unrolled_intervals> *unroll_intervals(Intervals& intervals){
	
	vector<Unrolled_intervals> *ret=new vector<Unrolled_intervals>();
	int n=intervals.size()*2;
	Unrolled_intervals tmp;
	for(int i=0;i<intervals.size();i++){
		tmp.loc=intervals[i].st;
		tmp.intrvl_indx=i;
		tmp.direction=0;
		ret->push_back(tmp);
		
		tmp.loc=intervals[i].nd;
		tmp.intrvl_indx=i;
		tmp.direction=1;
		ret->push_back(tmp);
	}
	sort(ret->begin(), ret->end(), intrvl_cmp);
	return ret;
}
/*It creates a new set of intervals that are covered by at least coverage
intervals of the current set*/
Intervals Intervals::merge_low_bound(unsigned min_cov, string dst){
	Intervals ret(dst);
	vector<Unrolled_intervals> *unr_intr=unroll_intervals(*this);
	Unrolled_intervals cur;
	int coverage=0;
	int intrid=0;
	//int first_intrvl=-1; //the first interval of intrvl_lst that covers a region
	//int last_intrvl=0; //the last intervlal of intrvl_lst that covers a region.
	int region_st=0, region_nd=0;

	for(int i=0;i<unr_intr->size();i++){
		cur=(*unr_intr)[i];
		if(cur.direction==0){ //the opening of an interval
			if (vec[cur.intrvl_indx].st != cur.loc) ioerror("Discrepancy on an opening unrolled");
			//last_intrvl = cur.intrvl_indx;
			if(coverage == min_cov -1){
				region_st=cur.loc;
			}
			coverage += 1;
		}
		else if (cur.direction=1){ //the closing end of an interval
			if(vec[cur.intrvl_indx].nd!=cur.loc) ioerror("Discrepancy on a closing unrolled"); //the closing of an unrolled interval should match with the nd of the corresponding interval.
			//printf("-\n");
			coverage-=1;
			if(coverage==min_cov-1){
				region_nd=cur.loc;
				//first_intrvl=get_starting_interval(*this, first_intrvl, last_intrvl, region_st, region_nd);
				ret.append(region_st, region_nd, intrid++, -1);
			}
			if(coverage<0) throw new negative_coverage();
		}
		else ioerror("Unknown direction number");
	}
	if(coverage>=min_cov){
		region_nd=cur.loc;
		//first_intrvl=get_starting_interval(*this, first_intrvl, last_intrvl, region_st, region_nd);
		ret.append(region_st, region_nd, intrid++, -1);
	}
	delete unr_intr;
	ret.store();
	return ret;
}



/*It creates a new set of intervals that are covered by at at most max_cov
intervals of the current set*/
Intervals Intervals::merge_upper_bound(unsigned max_cov, string dst){
	Intervals ret(dst);
	vector<Unrolled_intervals> *unr_intr=unroll_intervals(*this);
	Unrolled_intervals cur;
	int coverage=0;
	int intrid=0;
	//int first_intrvl=-1; //the first interval of intrvl_lst that covers a region
	//int last_intrvl=0; //the last intervlal of intrvl_lst that covers a region.
	int region_st=0, region_nd=0;

	for(int i=0;i<unr_intr->size();i++){
		cur=(*unr_intr)[i];
		if(cur.direction==0){ //the opening of an interval
			if (vec[cur.intrvl_indx].st != cur.loc) ioerror("Discrepancy on an opening unrolled"); //The opening of an unrolled interval should match with the st of the corresponding interval.
			//last_intrvl=cur.intrvl_indx;
			if(coverage==max_cov){
				region_nd=cur.loc;
				//first_intrvl=get_starting_interval(*this, first_intrvl, last_intrvl, region_st, region_nd);
				//last_intrvl=get_ending_interval(*this, cur.intrvl_indx, region_st, region_nd);
				ret.append(region_st, region_nd, intrid++, -1);
			}
			coverage +=1;
		}
		else if(cur.direction==1){ //the closing end of an interval
			if(vec[cur.intrvl_indx].nd!=cur.loc) ioerror("Discrepancy on a closing unrolled"); //the closing of an unrolled interval should match with the nd of the corresponding interval.
			//printf("-\n");
			coverage-=1;
			if(coverage==max_cov){
				region_st=cur.loc;
				
			}
			if(coverage<0) throw new negative_coverage();
		}
		else ioerror("Unknown direction number");
	}
	if(coverage<=max_cov){
		region_nd=cur.loc;
		//first_intrvl=get_starting_interval(*this, first_intrvl, last_intrvl, region_st, region_nd);
		//last_intrvl=get_ending_interval(*this, cur.intrvl_indx, region_st, region_nd); //here cur=unr_intr[ttl_unr-1];
		ret.append(region_st, region_nd, intrid++, -1);
	}
	return ret;
}





