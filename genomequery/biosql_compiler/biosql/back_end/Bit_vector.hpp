#ifndef BIT_VECTOR_HPP
#define BIT_VECTOR_HPP
#include<iostream>
#include<fstream>
#include<string>
#include "exceptions.hpp"

using namespace std;

class Bit_vector{
	private:
		unsigned long *vec;
		int size; //the length of the bit vector (and not of the array)
	public:

		Bit_vector(){
			size=0;
			vec=NULL;
		}
	
		//it initializes a vector of "size" bits to 0;
		Bit_vector(int size){
			this->size=size;
			int len=1+size/(sizeof(long)*8);
			vec=new unsigned long[len];
			for(int i=0;i<len;i++) vec[i]=0;
		}

		//It retrieves the information from a file.
		void retrieve_bit_vector(ifstream& infile){
			int len;
			infile.read((char*) &len, sizeof(int));
			this->size=len*(sizeof(long)*8); //=1+size/(sizeof(long)*8);
			vec=new unsigned long[len+5];
			infile.read((char*) vec, len*sizeof(long));
			return;
		}


		inline void set_bit(int pos){
			if(pos < 0) throw new neg_bitvector_pos();
			if(pos >= size) throw new bad_bitvector_pos();
			int x_pos=pos/(sizeof(long)*8); //convert bytes to bits
			int y_pos=pos%(sizeof(long)*8);
			unsigned long w=1;
			w=w<<y_pos;
			vec[x_pos]=vec[x_pos]|w;
			return;
		}

		inline int get_bit(int pos){
			if(pos < 0) throw new neg_bitvector_pos();
			if(pos >= size) throw new bad_bitvector_pos();
			int x_pos=pos/(sizeof(long)*8); //convert bytes to bits
			int y_pos=pos%(sizeof(long)*8);
			unsigned long w=vec[x_pos];
			w=w>>y_pos;
			return w&1;
		}

		int get_size(){return size;}
		
};

#endif
