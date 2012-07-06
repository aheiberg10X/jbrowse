#include "Reads.hpp"
#include "Bit_vector.hpp"
#include "Command.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "bam.h"
#include "sam.h"
#include "Histogram.hpp"

using namespace std;



Reads::Reads(string& indx):Table(indx){
	indx_file=indx;
	ifstream inf(indx.c_str(), ifstream::in);
	inf.exceptions( ifstream::failbit);
	cout << "length_init: "<< inf.tellg() << endl;
	inf >> bam_file;
	inf.ignore(1);
	inf >> chromosome;
	inf.ignore(1);
	inf >> chr_len;
	inf.ignore(1);
	cout << "length_before mates: "<< inf.tellg() << endl;
	rindx=get_vector<Mates>(inf, ttl_mates);
	cout << "length_before strand: "<< inf.tellg() << endl;
	strand_vec.retrieve_bit_vector(inf); //////////
	cout << "length_eof: "<< inf.tellg() << endl;


	inf.close();
}

vector<int>* Reads:: filter(){
	int i=0;
	int ev=0;
	vector<int>* ret=new vector<int>();
	//retreserve(ttl_mates);
	//ret->reserve(ttl_mates);//////////////are you sure???
	for(i=1;i<=ttl_mates;i++){
		//if(i%2000000==0)
		//	return ret;
		/*cmd.clear();
		this->resolve_variables(i, cmd, variables, types, symbol_id);
		ev=cmd.evaluate();*/
		ev=eval_rd_condition(i);
		if(ev==1)
			ret->push_back(i);
		else if(ev!=0) throw new non_boolean_expr();
	}
	return ret;
}

vector<int>* Reads:: filter(vector<int>& prev_filt){
	int i=0, tmp_i=0;
	int ev=0;
	vector<int>* ret=new vector<int>();
	for(tmp_i=0;tmp_i<prev_filt.size();tmp_i++){
		i=prev_filt[tmp_i];
		ev=eval_rd_condition(i);
		if(ev==1)
			ret->push_back(i);
		else if(ev!=0) throw new non_boolean_expr();
	}
	return ret;
}
		
void Reads::filter(string dst_file){
	vector<int>* filt=filter();
	ofstream outfile;
	outfile.open(dst_file.c_str(), ofstream::out);
	store_vector<int>(outfile, *filt);
	outfile.close();
	delete filt;
}

void Reads::filter(vector<int>& prev_filt, string dst_file){
	vector<int>* filt=filter(prev_filt);
	ofstream outfile(dst_file.c_str());
	store_vector<int>(outfile, *filt);
	outfile.close();
	delete filt;
}

/*It updates the variables of cmd with their values from the i-th read.
The input vectors are the outcome of the call cmd.get_variables*/
void Reads::resolve_variables(int i, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
	int k;
	for(k=0;k<variables.size();k++){
		if (variables[k]=="location"){
			if(types[k]!="int") throw new bad_type_field();
			cmd.set_int(rindx[i].loc, symbol_pos[k]);
		}
		else if(variables[k]=="mate_loc"){
			if(types[k]!="int") throw new bad_type_field();
			cmd.set_int(rindx[rindx[i].mate_indx].loc, symbol_pos[k]);
		}
		else if(variables[k]=="strand"){
			if(types[k]!="char") throw new bad_type_field();
			if(strand_vec.get_bit(i)==0)
				cmd.set_char('F', symbol_pos[k]);
			else
				cmd.set_char('R', symbol_pos[k]);

		}

		else if(variables[k]=="mate_strand"){
			if(types[k]!="char") throw new bad_type_field();
			if(strand_vec.get_bit(rindx[i].mate_indx)==0)
				cmd.set_int('+', symbol_pos[k]);
			else
				cmd.set_int('-', symbol_pos[k]);

		}
		else if(variables[k]=="length"){
			if(types[k]!="int") throw new bad_type_field();
			cmd.set_int(rindx[i].read_len, symbol_pos[k]);

		}
		else if(variables[k]=="mate_length"){
			if(types[k]!="int") throw new bad_type_field();
			cmd.set_int(rindx[rindx[i].mate_indx].read_len, symbol_pos[k]);

		}


	}
}

//fp is a file pointer to raw data
//offst is the number of bytes to be skipped from the beginning of the file.
//ttl_reads is the total number of alignments to be fetched
//b is the buffer of an alignment which is initialized outsize
//of the function to avoid mallocing multimple times
//outfp is a file pointer to the output (default stdout)
void get_raw_data(samfile_t *fp, long long offst, int ttl_reads, bam1_t *b, samfile_t *outfp){
	int read_len;
	int back_offst;
	int i=0;
	bam_seek(fp->x.bam, offst, SEEK_SET);
	while(samread(fp,b)>0){
		samwrite(outfp, b);
		i++;
		if(i==ttl_reads) break;
	}
	//samclose(outfp);
}


/*It prints to the destination bam_file those reads that are present in 
the input vector*/
void Reads::print_bam (vector<int> &flt, string dst_file, Histogram& hist){
	vector<int> outvec=flt;
	sort(outvec.begin(), outvec.end());
	samfile_t *fp=samopen(bam_file.c_str(), "rb",NULL);
	bam_header_t *bhr=fp->header;
	samfile_t *outfp;
	outfp=samopen(dst_file.c_str(), "wb", bhr);
	if (flt.size()==0) {
		samclose(outfp); 
		samclose(fp);
		return;
	}
	bam1_t *b=bam_init1();
	int i,j;
	long long offst;
	int prev_i=-1;
	int ttl_reads=0;
	for(j=0;j<flt.size();j++){
		i=flt[j];
		hist.update_histogram_solo(rindx[i].loc);

		if (j==0) offst=rindx[i].byte_offst; //initial offset
		//fprintf(stderr,"i: %d prev_i %d offst: %lld\n",i, prev_i, indx[i].byte_offst);
		if(prev_i<0 || i==prev_i+1)
			ttl_reads+=1;//(indx[i+1].byte_offst-indx[i].byte_offst);
		else{
			get_raw_data(fp, offst, ttl_reads, b, outfp); //print previous group of reads
			offst=rindx[i].byte_offst; //start a new group of reads
			ttl_reads=1;
		}
		prev_i=i;

	}
	get_raw_data(fp, offst, ttl_reads, b, outfp);
	bam_destroy1(b);

	samclose(fp);
	samclose(outfp);
}

//it prints the meta data of the reads whose locations on indx are stored in rd_lst. 
//It sorts the rd_lst and prints the output sorted according to mapping location
//If mate_flag is 1, it prints tuples: loc, loc+len-1, mate_loc, mate_loc+mlen-1
//Otherwise it prints tuples: loc, loc+len-1
void Reads::print_reduced(vector<int>& flt, string outfile, int mate_flag){
	vector<int> outvec=flt;
	sort(outvec.begin(), outvec.end());
	ofstream ofile;
	ofile.open(outfile.c_str(), ios::out);
	int i=0, j=0;
	int mate_indx=0;
	int loc, mloc, len, mlen=0;
	char strand, mstrand;
	for(j=0;j<flt.size();j++){
		i=flt[j];
		loc=rindx[i].loc;
		len=rindx[i].read_len;
		int strand_info=strand_vec.get_bit(i);
		strand=(strand_info==0)?'F':'R';
		mate_indx=rindx[i].mate_indx;
		if(mate_indx<i && mate_flag) continue; //don't print pairs twice.
		ofile << loc << "\t" << loc+len-1 << "\t" << strand << "\t";
		if(mate_flag){
			if(mate_indx>0){
				mloc=rindx[mate_indx].loc;
				mlen=rindx[mate_indx].read_len;
				mstrand=(strand_vec.get_bit(mate_indx)==0)?'F':'R';
				ofile << mloc << "\t" << mloc+mlen-1 << "\t" << mstrand << "\t";
			}
			else throw new unpaired_rd_exception();
		}
		ofile << endl;
	}
	ofile.close();
}

/*It prints the association between the rank of a read in the original bam file
and the rank of the same read in the output bam file after applying filter flt*/
void Reads::print_metalinks(vector<int>& flt, string outfile){
	vector<int> outvec=flt;
	sort(outvec.begin(), outvec.end());
	ofstream ofile;
	ofile.open(outfile.c_str(), ios::out);
	ofile << "current_rank original_rank" << endl;
	for(int i=0;i<flt.size();i++)
		ofile << i << " " << outvec[i] << endl;
	ofile.close();
}
	

/*It prints the reads that are indicated by flt. Note that the first
character of prefx is always 0 or 1 and carries the mate_flag
information. So the real prefix of the output products is 
prefx[1:len(prefx)-1]. Output products: .bam, .short, .hist*/
void Reads::print(string prefx, vector<int> &flt){
	string real_prefx;
	real_prefx.assign(prefx.begin()+1, prefx.end());
	int mate_flag;
	if (prefx[0]=='0') mate_flag=0;
	else if(prefx[0]=='1') mate_flag=1;
	else throw new illegal_read_prefx();

	Histogram hist(chr_len);
	string dst_bam=real_prefx+".bam";
	string dst_hist=real_prefx+".hist";
	string dst_short=real_prefx+".bam.short";
	string dst_metalinks=real_prefx+".bam.links.txt";

	print_bam (flt, dst_bam, hist);
	hist.print_histogram(dst_hist.c_str());
	print_reduced(flt, dst_short, mate_flag);
	print_metalinks(flt, dst_metalinks);


}



