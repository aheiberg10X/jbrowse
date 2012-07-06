/*it specifies a vector object for optimum performance*/

#ifndef __FVECTOR__HPP
#define __FVECTOR__HPP
#include <iostream>
#include "exceptions.hpp"

using namespace std;

template <class D>
class Fvector{
	private:
		int capacity; //the capacity of vec
		int ttl; //the total elements of vec
	public:
		D *vec;
		Fvector(int cap=2048){
			capacity=cap;
			vec=new D[cap+1];
			ttl=0;
		}
		~Fvector(){
			capacity=0;
			delete[] vec;
			ttl=0;
		}

		void push_back(D element){ //adds element to vec
			if(ttl==capacity) throw new no_vector_space();
			vec[ttl]=element;
			ttl++;
		}

		void pop(int ttl_elements=1){ //removes ttl_elements from vec
			ttl-=ttl_elements;
			if(ttl<0) ttl=0;
		}

		inline D& operator[] (int i){
			if (i<0 || i>=ttl) throw new out_of_bounds();
			return vec[i];
		}
		inline int size(){return ttl;}
		
		void assign( D ar[], int n){ //copy the contents of ar of size n
			ttl=0;
			for(int i=0;i<n;i++) push_back(ar[i]);
		}

		inline void clear(){ttl=0;}

};

#endif
