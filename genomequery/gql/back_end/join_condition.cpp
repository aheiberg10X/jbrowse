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
//it deletes the memory that has been allocated by dereference
static void delete_vectors(vector<long>& vec_addr, vector<string>& types){
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
int Joined::eval_condition(int i){
	static vector<long> vec_addr;
	static vector<string> variables;
	static vector<string> types;
	if(i<0){
		delete_vectors(vec_addr, types);
		return 0;
	}
	if (variables.size()==0){
		variables.push_back("dst/NA18507/chr2/H3.joined:/media/data/work/NA18507/chr2.mates.indx:location"); types.push_back("int");
		variables.push_back("dst/NA18507/chr2/H3.joined:dst/NA18507/chr2/genes.txt:arg4"); types.push_back("int");
	}
	if(vec_addr.size()==0) dereference_variables(variables, types, vec_addr, this);
	vector<int>* arg1=reinterpret_cast<vector<int>* >(vec_addr[0]);
	vector<int>* arg2=reinterpret_cast<vector<int>* >(vec_addr[1]);
	int ev = ( (*arg1)[i] == 15000000 ) && ( (*arg2)[i] > 23000 ) ;
	return ev;
}
