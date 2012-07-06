#include "Fvector.hpp"
#include <cstdlib>

using namespace std;
template <class D>
Fvector<D>::Fvector(int cap){
	capacity=cap;
	vec=new D[cap+1];
	ttl=0;
}

template <class D>
Fvector<D>::~Fvector(){
	capacity=0;
	delete[] vec;
	ttl=0;
}

template <class D>
void Fvector<D>::push_back(D element){
	if(ttl==capacity) throw new no_vector_space();
	vec[ttl]=element;
	ttl++;
}

template <class D>
void Fvector<D>::pop(int ttl_elements){
	ttl-=ttl_elements;
	if(ttl<0) ttl=0;
}

template <class D> 
void Fvector<D>::assign( D ar[], int n){
	ttl=0;
	for(int i=0;i<n;i++) push_back(ar[i]);
}

/*int main(){
	Fvector<string> fv(2);
	fv.push_back("hello");
	fv.push_back("world");
	fv.pop(2);
	fv.push_back("mom");
	cout << fv[0] << endl;
}*/
