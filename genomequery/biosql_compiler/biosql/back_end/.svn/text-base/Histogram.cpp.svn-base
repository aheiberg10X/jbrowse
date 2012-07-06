#include "Histogram.hpp"
#include <stdio.h>
#include <stdlib.h>

Histogram::Histogram(int chromo_len){
	chromo_length=chromo_len;
	mhist.ttl_histograms=13; //19;
	mhist.ttl_bins=(int*)malloc(mhist.ttl_histograms*sizeof(int));
	mhist.ttl_bins[0]=25;
	mhist.ttl_bins[1]=50;
	mhist.ttl_bins[2]=125;
	mhist.ttl_bins[3]=250;
	mhist.ttl_bins[4]=500;
	mhist.ttl_bins[5]=1250;
	mhist.ttl_bins[6]=2500;
	mhist.ttl_bins[7]=5000;
	mhist.ttl_bins[8]=12550;
	mhist.ttl_bins[9]=25000;
	mhist.ttl_bins[10]=50000;
	mhist.ttl_bins[11]=125000;
	mhist.ttl_bins[12]=250000;
	/*mhist.ttl_bins[13]=500000;
	mhist.ttl_bins[14]=1250000;
	mhist.ttl_bins[15]=2500000;
	mhist.ttl_bins[16]=5000000;
	mhist.ttl_bins[17]=12000050;
	mhist.ttl_bins[18]=25000000;*/

	mhist.hist=(hist_t**)malloc(sizeof(hist_t*)*(mhist.ttl_histograms+1));
	int i=0;
	int j=0;
	for(i=0;i<mhist.ttl_histograms;i++){
		mhist.hist[i]=(hist_t*)malloc(sizeof(hist_t)*(mhist.ttl_bins[i]+1));
		for(j=0;j<mhist.ttl_bins[i];j++)
			mhist.hist[i][j]=0;
	}
}


void Histogram::destroy_histogram(){
	int i=0;
	for (i=0;i<(mhist.ttl_histograms);i++)
		free(mhist.hist[i]);
	free(mhist.hist);
	mhist.hist=NULL;
	free(mhist.ttl_bins);
	mhist.ttl_bins=NULL;
}

void Histogram::print_histogram(const char *outfile){
	FILE *fp=fopen(outfile, "w");
	int i=0, j=0;
	fprintf(fp, "histograms = \n[\n");
	for(i=0;i<mhist.ttl_histograms;i++){
		fprintf(fp, "\t{\"basesPerBin\" : %d,\n",chromo_length/(mhist.ttl_bins[i]));
		fprintf(fp, "\t\"counts\" : [%d",mhist.hist[i][0]);
		for(j=1;j<mhist.ttl_bins[i];j++) //not j=0 because mhist[i][0] is above
			fprintf(fp, ",%d",mhist.hist[i][j]);
		fprintf(fp,"]\n");
		if (i==(mhist.ttl_histograms)-1) fprintf(fp, "}\n");
		else fprintf(fp, "},\n");
	}
	fprintf(fp, "]\n");
}

