#ifndef __DEFS_HPP
#define __DEFS_HPP
#include<stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

/*void ioerror(string msg){
	cout << "FATAL ERROR!!! "<< msg << endl;
	exit(1);
}*/

template <class T>
vector<T> get_vector(ifstream& infile, int &len){
	/*infile >> len;
	infile.ignore(1); //ignore the newline*/
	int tmp_len=len;
	infile.read((char*)&tmp_len, sizeof(int));
	len=tmp_len;
	T *tmp=new T[len+3];
	vector<T> ret;
	infile.read((char*)tmp, len*sizeof(T));
	ret.assign(tmp, tmp+len);
	cout << ret.size() << endl;
	delete tmp;
	return ret;
}
	
template <class T>
void store_vector(ofstream& ofile, vector<T>& vec){
	//ofile << vec.size() << endl;
	int len=vec.size();
	ofile.write((char*)(&len), sizeof(int));
	T *tvec=&(vec[0]);
	ofile.write((char*)tvec, vec.size()*sizeof(T));
}
/*
//histogram related definitions
typedef int hist_t;
//multiple histograms of different number of bins
typedef struct{
	hist_t **hist;
	int *ttl_bins; //the number of bins per histogram
	int ttl_histograms; //the number of rows of hist
}Mult_hist;

//combine histogram bins with chromosome length.
typedef struct{
	Mult_hist mhist;
	//int ttl_bins;
	int chromo_length;
}Hist_info; 


//it initializes and returns a Mult_hist variable. The bin sizes are the ones
//that Andrew specified in his email on May 5th 2011.
Mult_hist create_histogram();
//For all histograms of hist it increases the bin value that pos falls to. 
void update_histogram_solo(Mult_hist *mhist, int pos, int chromo_len);

void print_histogram(FILE* fp, Mult_hist mhist, int chromo_len);

void destroy_histogram(Mult_hist* mhist);

void ioerror(char *msg);*/

#endif
