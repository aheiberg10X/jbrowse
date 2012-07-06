#include "defs.hpp"
#include <stdio.h>

void ioerror(char *msg){
	fprintf(stderr, "FATAL ERROR!!! %s\n",msg);
	exit(1);
}

Mult_hist create_histogram(){
	Mult_hist ret;
	ret.ttl_histograms=13; //19;
	ret.ttl_bins=(int*)malloc(ret.ttl_histograms*sizeof(int));
	ret.ttl_bins[0]=25;
	ret.ttl_bins[1]=50;
	ret.ttl_bins[2]=125;
	ret.ttl_bins[3]=250;
	ret.ttl_bins[4]=500;
	ret.ttl_bins[5]=1250;
	ret.ttl_bins[6]=2500;
	ret.ttl_bins[7]=5000;
	ret.ttl_bins[8]=12550;
	ret.ttl_bins[9]=25000;
	ret.ttl_bins[10]=50000;
	ret.ttl_bins[11]=125000;
	ret.ttl_bins[12]=250000;
	/*ret.ttl_bins[13]=500000;
	ret.ttl_bins[14]=1250000;
	ret.ttl_bins[15]=2500000;
	ret.ttl_bins[16]=5000000;
	ret.ttl_bins[17]=12000050;
	ret.ttl_bins[18]=25000000;*/

	ret.hist=(hist_t**)malloc(sizeof(hist_t*)*(ret.ttl_histograms+1));
	int i=0;
	int j=0;
	for(i=0;i<ret.ttl_histograms;i++){
		ret.hist[i]=(hist_t*)malloc(sizeof(hist_t)*(ret.ttl_bins[i]+1));
		for(j=0;j<ret.ttl_bins[i];j++)
			ret.hist[i][j]=0;
	}
	return ret;
}

inline void update_histogram_solo(Mult_hist *mhist, int pos, int chromo_len){
	//int bin_range=chromo_len/ttl_bins;
	//int i=pos/bin_range;
	int bin_range;
	int i,j;
	for (i=0;i<(mhist->ttl_histograms);i++){
		if(chromo_len%(mhist->ttl_bins[i])==0)
			bin_range=chromo_len/(mhist->ttl_bins[i]);
		else
			bin_range=chromo_len/(mhist->ttl_bins[i])+1;
		j=pos/bin_range;
		if(j>mhist->ttl_bins[i])
			printf("i: %d j: %d ttl_bins[i]: %d bin_range: %d chromo_len: %d pos: %d\n",i, j, mhist->ttl_bins[i], bin_range, chromo_len, pos);
		mhist->hist[i][j]+=1;
	}
}

void destroy_histogram(Mult_hist *mhist){
	int i=0;
	for (i=0;i<(mhist->ttl_histograms);i++)
		free(mhist->hist[i]);
	free(mhist->hist);
	mhist->hist=NULL;
	free(mhist->ttl_bins);
	mhist->ttl_bins=NULL;
}

void print_histogram(FILE *fp, Mult_hist mhist, int chromo_len){
	int i=0, j=0;
	fprintf(fp, "histograms = \n[\n");
	for(i=0;i<mhist.ttl_histograms;i++){
		fprintf(fp, "\t{\"basesPerBin\" : %d,\n",chromo_len/(mhist.ttl_bins[i]));
		fprintf(fp, "\t\"counts\" : [%d",mhist.hist[i][0]);
		for(j=1;j<mhist.ttl_bins[i];j++) //not j=0 because mhist[i][0] is above
			fprintf(fp, ",%d",mhist.hist[i][j]);
		fprintf(fp,"]\n");
		if (i==(mhist.ttl_histograms)-1) fprintf(fp, "}\n");
		else fprintf(fp, "},\n");
	}
	fprintf(fp, "]\n");
}

