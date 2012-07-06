#include "Joined.hpp"

static void dereference_variables(vector<string>& variables, vector<string>& types, vector<long>& refs, Joined *obj){ 
	for(int i=0;i<variables.size();i++){
		if(types[i]=="int"){
			vector<int>* intvec;
			intvec=obj->dereference<int>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(intvec));
		}
		else if(types[i]=="float"){
			vector<float>* fltvec;
			fltvec=obj->dereference<float>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(fltvec));
		}
		else if(types[i]=="char"){
			vector<char>* charvec;
			charvec=obj->dereference<char>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(charvec));
		}
		else if(types[i]=="string"){
			vector<string>* strvec;
			strvec=obj->dereference<string>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(strvec));
		}
		else  throw new unsupported_type();
	}
}
static void delete_vectors(vector<long>& vec_addr, vector<string>& types);


//if called with -1 it releases the allocated memory
int Joined::eval_condition(int i){
	static vector<long> vec_addr;
	static vector<string> variables;
	static vector<string> types;
	if(i<0) delete_vectors(vec_addr, types);

	if(variables.size()==0){variables.push_back("loc"); types.push_back("int");}
	if(vec_addr.size()==0) dereference_variables(variables, types, vec_addr, this);
	
	vector<int>* arg1=reinterpret_cast<vector<int>* >(vec_addr[0]);
	vector<string>* arg2=reinterpret_cast<vector<string>* >(vec_addr[1]);
	vector<float>* arg3=reinterpret_cast<vector<float>* >(vec_addr[2]);
	vector<char>* arg4=reinterpret_cast<vector<char>* >(vec_addr[3]);
	vector<int>* arg5=reinterpret_cast<vector<int>* >(vec_addr[4]);

	return ((*arg1)[i]<(*arg5)[i]) && ((*arg2)[i]=="chr1") && ((*arg3)[i]>=0.99) && ((*arg4)[i]=='F');
}

//it deletes the memory that has been allocated by dereference
void delete_vectors(vector<long>& vec_addr, vector<string>& types){
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
