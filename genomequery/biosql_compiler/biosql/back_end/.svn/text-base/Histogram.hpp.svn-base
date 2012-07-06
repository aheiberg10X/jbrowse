#include<iostream>
#include<stdio.h>

#ifndef __HISTOGRAM__HPP
#define __HISTOGRAM__HPP
using namespace std;

class Histogram{
	protected:
		typedef int hist_t;
		//multiple histograms of different number of bins
		typedef struct{
			hist_t **hist;
			int *ttl_bins; //the number of bins per histogram
			int ttl_histograms; //the number of rows of hist
		}Mult_hist;

		//combine histogram bins with chromosome length.
		/*typedef struct{
			Mult_hist mhist;
			//int ttl_bins;
			int chromo_length;
		}Hist_info; */
		
		Mult_hist mhist;
		int chromo_length;
	public:
		Histogram(int chromo_length);
		inline void update_histogram_solo(int pos){
		int bin_range;
		int i,j;
		for (i=0;i<(mhist.ttl_histograms);i++){
			if(chromo_length%(mhist.ttl_bins[i])==0)
				bin_range=chromo_length/(mhist.ttl_bins[i]);
			else
				bin_range=chromo_length/(mhist.ttl_bins[i])+1;
			j=pos/bin_range;
			if(j>mhist.ttl_bins[i])
				printf("i: %d j: %d ttl_bins[i]: %d bin_range: %d chromo_length: %d pos: %d\n",i, j, mhist.ttl_bins[i], bin_range, chromo_length, pos);
			mhist.hist[i][j]+=1;
		}
	}

		void print_histogram(const char* out_file);

		void destroy_histogram();
};

#endif
