#include "Joined.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include "exceptions.hpp"
#include "Interval_tree.hpp"
#include "defs.hpp"

template <class D>
class Annot_list{
	private:
		list<D> lst;
	public:
		int label;
		Annot_list(int lbl){label=lbl;}
		Annot_list(){label=-1;}
		Annot_list(int lbl, int val){
			label=lbl;
			lst.push_back(val);
		}
		void push_back(D k){lst.push_back(k);}
		void erase(){lst.erase();}
		list<D>& get_lst(){return lst;}
		void clear_list(){lst.clear();}
};


class Map{
	public:
		vector< list<Annot_list<int> > > vec;
		int n;
		int d;
		Map(int sz, int div){
			d=div;
			n=sz/d;
			vec.resize(n+10);
		}
		
		//adds val to the Annot_list whose label is id. It keeps the vector of
		//ids sorted.
		void add_element(int id, int val){ 
			int i;
			i=id/d;
			list<Annot_list<int> >::iterator it=vec[i].begin();
			for(;it!=vec[i].end();it++){
				if(id < it->label){
					/*vec[i].insert(it, Annot_list(id, val));
					return;*/
					break;
				}
				if(id == it->label){
					/*if(id==56137332)
						cout << "pushing back " << it->label << endl;*/

					it->push_back(val);
					return;
				}
			}

			vec[i].insert(it, Annot_list<int>(id, val));
			//if(id==56137332)
			//	cout << "i " << i << " id: "<< id<<" val "<< val << " label: " << it->label << endl;
		}

		void add_lst(int id, list<int>& lst){
			for (list<int>::iterator lit=lst.begin();lit!=lst.end();lit++)
				add_element(id, *lit);
			return;
		}

		
		list<int>& operator[](int id){
			list<int> ret;
			ret.clear();
			int i; 
			i=id/d;
			list< Annot_list<int> >::iterator it=vec[i].begin();
			for(; it!=vec[i].end(); it++)
				if(it->label==id)
					return it->get_lst();
		}

	private:
		struct __iterator{
		typedef struct __iterator Self;

		typedef std::input_iterator_tag category;
		typedef int value_type;
		typedef int* pointer;
		typedef int& reference;
		
	
		int i;
		int max_i;
		vector< list<Annot_list<int> > >* vec;
		list<Annot_list<int> >::iterator j;

		explicit __iterator(int n, vector< list<Annot_list<int> > >* vc){
			max_i=n;
			vec=vc;
			for(int k=0;k<n;k++){
				if((*vec)[k].size()>0){
					i=k;
					j=(*vec)[k].begin();
					return;
				}
			}
			i=n;
			j=(*vec)[i].end();
		}
		

		

		explicit __iterator(int n, int i, list<Annot_list<int> >::iterator j, vector<list<Annot_list<int> > >* vc){
			max_i=n;
			this->i=i;
			this->j=j;
			vec=vc;
		}



		~__iterator(){}

		
		Annot_list<int>& operator*(){ return *j;} //->get_lst();}

		Self& operator++(){ 
			if(i==max_i) return (*this);
			j++;
			if (j==(*vec)[i].end()){
				for(i=i+1;i<max_i;i++){//new row
					j=(*vec)[i].begin();
					if(!(*vec)[i].empty())
						break;
				}
			}
			if(i==max_i) j=(*vec)[i].begin();
			return (*this);
		}



		Self& operator ++(int k){
			if(i==max_i) return (*this);
			int cur_id=j->label;
			j++;
			if (j==(*vec)[i].end()){
				for(i=i+1;i<max_i;i++){//new row
					j=(*vec)[i].begin();
					if(!((*vec)[i].empty()))
						break;
				}
			}
			if(i==max_i) j=(*vec)[i].begin();
			return (*this);
		}

		
		bool operator==(const Self& x){
			return (i==x.i && j==x.j);
		}
		bool operator!=(const Self &x){
			if(i==max_i)
				cout << "careful" <<endl;
			return (i!=x.i || j!=x.j);
		}
	};

	public:
		typedef __iterator iterator;
	
		iterator begin(){ 
			return __iterator(n+1, &vec);
		}
		iterator end(){ ///////Weird but it gets the job done.
			return __iterator(n+1, n+1, vec[n+1].begin(), &vec);
		}
};

Joined::Joined(string dst, string src1, string src2, string type1, string type2, Intervals &in1, Intervals &in2):Table(dst){
	
	Intervals *small, *large;
	string type_small, type_large;
	string src_small, src_large;
	
	if (in1.size() < in2.size()){
		small=&in1;
		type_small=type1;
		src_small=src1;

		large=&in2;
		type_large=type2;
		src_large=src2;
	}
	else{
		small=&in2;
		type_small=type2;
		src_small=src2;

		large=&in1;
		type_large=type1;
		src_large=src1;
	}

	this->parent1=src_small;
	this->type1=type_small;
	this->parent2=src_large;
	this->type2=type_large;

	Interval_tree it(*small);
	for(int i=0;i<large->size();i++){
		Intervals ans;
		it.search_range((*large)[i].st, (*large)[i].nd, ans);
		for(int j=0;j<ans.size();j++){
			join_intervals(ans[j], (*large)[i]);
		}
	}

}


Joined::Joined(string dst, string src1, string src2, string tp1, string tp2):Table(dst){
	vec.clear();
	parent1=src1;
	parent2=src2;
	this->type1=tp1;
	this->type2=tp2;
}

Joined::Joined(string fname):Table(fname){
	load();
}

void Joined::join_intervals(Intervals::Intrvl &int1, Intervals::Intrvl &int2){
	for(int i=0;i<2;i++){
		for(int j=0;j<2;j++){
			Pair p;
			p.indx1=-1;
			p.indx2=-1;
			if(i==0)
				p.indx1=int1.rd1;
			if(i==1)
				p.indx1=int1.rd2;
			if(j==0)
				p.indx2=int2.rd1;
			if (j==1)
				p.indx2=int2.rd2;
			if(p.indx1>=0 && p.indx2>=0)
				vec.push_back(p);
		}
	}
}

/*it stores the contents of the object to file_loc*/
void Joined::store(){
	ofstream of(file_loc.c_str());
	of << type1 <<endl;
	of << parent1 << endl;
	of << type2 << endl;
	of << parent2 << endl;
	store_vector<Pair>(of, vec);
	of.close();
}

/*It overwrites the contents of the object from the contents of 
file_loc*/
void Joined::load(){
	int len=0;
	ifstream inf(file_loc.c_str());
	getline(inf, type1);
	getline(inf, parent1);
	getline(inf, type2);
	getline(inf, parent2);
	vec=get_vector<Pair>(inf, len);
	inf.close();
}

//similar to load, but it doesn't update any object variables*/
void Joined::load (string fname, vector<Pair>& vec, string &p1, string &t1, string &p2, string &t2)const{
	ifstream inf(fname.c_str());
	getline(inf, t1);
	getline(inf, p1);
	getline(inf, t2);
	getline(inf, p2);
	vec.clear();
	int len;
	vec.clear();
	vec=get_vector<Pair>(inf, len);
	inf.close();
}



template <class T>
//void initialize_map(map<int, list<int> > &cur_map, vector<T> &vec, int indx){
Map* initialize_map(vector<T>& vec, int indx, unsigned max_element, unsigned div){
	///cout << "***initialize_map*** " << vec.size() << endl;
	Map *cur_map=new Map(max_element+1, div);
	for (int i=0;i<vec.size();i++){
		if(indx==1){
			cur_map->add_element(vec[i].indx1, i);
			//cur_map[vec[i].indx1].push_back(i);
			///cout << "indx1 "<< vec[i].indx1 << endl;
		}
		else{
			//cur_map[vec[i].indx2].push_back(i);
			cur_map->add_element(vec[i].indx2, i);
			///cout << "indx2 " << vec[i].indx2 << endl;
		}
	}
	return cur_map;
}

/*it constructs a vector of sz elements and populates it with the proper values 
of var of the respective table. mmap is a map of the form mmap[i]:[k,l,m] which
means that the value of var of the i'th entry of the table is to be entered
to positions k,l,m of the returned vector*/

template <class D, class T>
//inline vector<T>* get_physical_data(string table, int sz, map<int, list<int> >& mmap, string var, string var_type){
inline vector<T>* get_physical_data(string table, int sz, Map *mmap, string var, string var_type){

	vector<T> *ret = new vector<T>(sz);
	Command cmd(var_type+"var "+var);
	vector<string> variables, types;
	variables.push_back(var);
	types.push_back(var_type);
	vector<int> sid;
	sid.push_back(0);

	D src(table);
	typename D::iterator dit=src.begin(); //It iterates the records of src
	list<int>::iterator lit; //It iterates the lists of the mmap
	//map<int, list<int> >::iterator mit=mmap.begin(); //it iterates the keys of the map
	Map::iterator mit=mmap->begin();
	int i=0, j=0;

	for(i=1;dit!=src.end();i++, dit++){
		if(i!=((*mit).label)){
			continue;
		}
		if(mit==mmap->end()){
			cout << "11111" << endl;
			throw new dereference_map_exception();
		}
		src.resolve_variables(dit, cmd, variables, types, sid);
		T value;
		cmd.get_const(value, 0); 

		//for(lit=mit->second.begin();lit!=mit->second.end();lit++){
		for(lit=(*mit).get_lst().begin();lit!=(*mit).get_lst().end();lit++){
			(*ret)[*lit]=value;
		}
		mit++;
		cmd.clear();
	}
	if(mit!=mmap->end()){
		cout << "2222" << endl;
		throw new dereference_map_exception();
	}
	return ret;
}

/*It returns 1 if fname==parent1 and 2 if fname==parent2. cur_type is the 
type of the respective table*/
int get_table_info(string fname, string parent1, string type1, string parent2, string type2, string &cur_type){
	string real_p1, real_p2;
	if(type1=="Reads") real_p1=parent1.substr(1); //ignore the mates_en
	else real_p1=parent1;

	if(type2=="Reads") real_p2=parent2.substr(1); //ignore the mates_en
	else real_p2=parent2;

	if(fname==real_p1){
		cur_type=type1;
		return 1;
	}
	if(fname==real_p2){
		cur_type=type2;
		return 2;
	}
	else throw new unknown_joined_input();
}

/*It takes a variable that looks like H1:H2:H3:...:Hn:arg where Hn is a basic
table (Reads, Intervals or a Text_table) and returns the list: [H1, H2,.., Hn]. 
low_level_var becomes arg in the end (i.e. the last element of the sequence*/

vector<string> parse_variable(string& var, string &low_level_var){
	stringstream ss(var);
	vector<string> ret;
	string buf;
	while(getline(ss, buf, ':')){
		ret.push_back(buf);
	}
	low_level_var=*(ret.end()-1);
	return vector<string>(ret.begin(), ret.end()-1);
}

//compares according to indx1
template <class D>
inline bool cmp_pair_indx1(D a, D b){ return a.indx1 < b.indx1;}

template <class D>
inline bool cmp_pair_indx2(D a, D b){return a.indx2 < b.indx2;}

/*It returns the value of the variable for each entry of the vec. The 
variable string should be of the format file1:file2:...:filen:arg*/
template <class T>
vector<T>* Joined::dereference(string var, string type){
	string low_level_var;
	vector<string> file_names=parse_variable(var, low_level_var);
	//map<int, list<int> > cur_map;
	//map<int, list<int> > new_map;
	Map *cur_map=NULL, *new_map=NULL;


	int pair_id;
	string cur_type;
	ifstream inf;
	string p1=parent1;
	string t1=type1;
	string p2=parent2;
	string t2=type2;

	vector<Pair> cur_vec;

	vector<string>::iterator fname;
	//map<int, list<int> >::iterator map_it;
	 	
	fname=file_names.begin(); //ignore the first file... already processed
	fname++;

	pair_id=get_table_info(*fname, parent1, type1, parent2, type2, cur_type);
	Pair max_elmnt;
	int max_key; 
	if(pair_id==1){
		max_elmnt=*max_element(vec.begin(), vec.end(), cmp_pair_indx1<Pair>);
		max_key=max_elmnt.indx1;
	}
	else{
		max_elmnt=*max_element(vec.begin(), vec.end(), cmp_pair_indx2<Pair>);
		max_key=max_elmnt.indx2;
	}
	int div_map=10;
	cur_map=initialize_map<Pair>(/*cur_map,*/ vec, pair_id, max_key, div_map);
	for(;fname!=file_names.end();fname++){
		pair_id=get_table_info(*fname, p1, t1, p2, t2, cur_type);
		if(cur_type!="joined") break;
		//read_file(*fname, cur_vec, p1, t1, p2, t2);
		load(*fname, cur_vec, p1, t1, p2, t2);
		if(pair_id==1){
			max_elmnt=*max_element(vec.begin(), vec.end(),cmp_pair_indx1<Pair>);
			max_key=max_elmnt.indx1;
		}
		else{
			max_elmnt=*max_element(vec.begin(), vec.end(), cmp_pair_indx2<Pair>);
			max_key=max_elmnt.indx2;
		}
		new_map=new Map(max_key, div_map);
		for(Map::iterator map_it=cur_map->begin();map_it!=cur_map->end();map_it++){
			//int choose_pair=(pair_id==1? cur_vec[map_it->first].indx1: cur_vec[map_it->first].indx2);
			int choose_pair=(pair_id==1? cur_vec[(*map_it).label].indx1: cur_vec[(*map_it).label].indx2);
			//new_map[choose_pair].insert(new_map[choose_pair].end(), map_it->second.begin(), map_it->second.end());
			new_map->add_lst(choose_pair, (*map_it).get_lst());
			//map_it->second.clear();
			(*map_it).clear_list();
		}
		delete cur_map;
		cur_map=new_map;
		new_map=NULL;


		//cur_map.swap(new_map);
		//new_map.clear();
	}
	///If everything is working by now,
	vector<T>* ret;
	if(cur_type=="Reads")
		ret=get_physical_data<Reads,T>(*fname, vec.size(), cur_map, low_level_var, type);
	else if(cur_type=="Text_table")
		ret=get_physical_data<Text_table,T>(*fname, vec.size(), cur_map, low_level_var, type);
	else if(type=="Intervals")
		ret=get_physical_data<Intervals, T>(*fname, vec.size(), cur_map, low_level_var, type);
	else
		throw new unsupported_type();

	return ret;
}

template vector<int>* Joined::dereference<int>(string var, string type);
template vector<float>* Joined::dereference<float>(string var, string type);
template vector<string>* Joined::dereference<string>(string var, string type);
template vector<char>* Joined::dereference<char>(string var, string type);

//it deletes the memory that has been allocated by dereference
void delete_vectors(vector<long> vec_addr, vector<string> types){
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


void Joined::filter(string dst){
	Joined flt(dst, parent1, parent2, type1, type2);
	//vector<Pair>::iterator it=vec.begin();
	
	/*Command cmd(command_str);
	vector<string> types;
	vector<int> symbol_pos;
	vector <string> variables=cmd.get_variables(types, symbol_pos);

	vector<long> refs;

	vector<int>* vint; vector<string>* vstr; vector<float>* vflt; vector<double>* vdbl;
	
	for(int i=0;i<variables.size();i++){
		if(types[i]=="int"){
			vector<int>* intvec=dereference<int>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(intvec));
		}
		else if(types[i]=="float"){
			vector<float>* fltvec=dereference<float>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(fltvec));
		}
		else if(types[i]=="char"){
			vector<char>* charvec=dereference<char>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(charvec));
		}
		else if(types[i]=="string"){
			vector<string>* strvec=dereference<string>(variables[i], types[i]);
			refs.push_back(reinterpret_cast<long>(strvec));
		}
		else  throw new unsupported_type();
	}*/

	for(int i=0;i<vec.size();i++){
		int ev=eval_condition(i);
		//resolve_variables(i, refs, cmd, variables, types, symbol_pos);
		//int ev=cmd.evaluate();
		if(ev==1){
			flt.vec.push_back(vec[i]);
			cout << "filtered_join["<<i<<"]: "<< vec[i].indx1 << " " << vec[i].indx2 << endl;
		}
		else if(ev!=0) throw new non_boolean_expr();
		//cmd.clear();

	}
	flt.store();
	eval_condition(-1);//delete_vectors(refs, types);
	
}



/*It evaluates cmd on the k-th record. vec_addr is the set of the
addresses of the vectors that dereference the various variables of
cmd. (they are returned by the dereference command*/
void Joined::resolve_variables(int i, vector<long>& vec_addr, Command &cmd, vector<string>& variables, vector<string>& types, vector<int>& symbol_pos){
	
	vector<int>* vint; vector<string>* vstr; vector<float>* vflt; vector<char>* vchar;
	for (int k=0;k<variables.size();k++){
		if(types[k]=="int"){
			vint=reinterpret_cast<vector<int>* >(vec_addr[k]);
			cmd.set_int((*vint)[i], symbol_pos[k]);
		}
		else if(types[k]=="float"){
			vflt=reinterpret_cast<vector<float>* >(vec_addr[k]);
			cmd.set_float((*vint)[i], symbol_pos[k]);
		}
		else if(types[k]=="char"){
			vchar=reinterpret_cast<vector<char>* >(vec_addr[k]);
			cmd.set_char((*vint)[i], symbol_pos[k]);
		}
		else if	(types[k]=="string"){
			vstr=reinterpret_cast<vector<string>* >(vec_addr[k]);
			cmd.set_const_str((*vstr)[i], symbol_pos[k]); //make sure it is not
			//set_variable here
		}
	}
	return;
}



/*True if str exists as a key in the history_log and the respective
value is "true"*/
inline bool already_printed(map<string, bool>& history_log, string& str){
	map<string, bool>::iterator it;
	it=history_log.find(str);
	if(it==history_log.end()) return false;
	if (it->second)
		return true;
	else
		return false;
}


/*It isolates the data from the individual tables. flt contains the entries
that need to be considered.*/
void Joined::print(string out_prefx, vector<int>& flt){
	vector <int> vec1;
	vector <int> vec2;
	vector<int>::iterator it;
	static int table_no=0;
	static map<string, bool> history_log;

	for(int i=0;i<flt.size();i++){
		vec1.push_back(vec[flt[i]].indx1);
		vec2.push_back(vec[flt[i]].indx2);
	}

	sort(vec1.begin(), vec1.end()); //unique removes only consecutive duplicates
	it=unique(vec1.begin(), vec1.end()); //consider indices only once
	vec1.resize(it-vec1.begin());
	
	sort(vec2.begin(), vec2.end());
	it=unique(vec2.begin(), vec2.end());
	vec2.resize(it-vec2.begin());

	///now create objects from src1 and src2 and call respective print functions.
	Table* tbl1;
	Table* tbl2;

	string out_prefx1=out_prefx; //to resolve peculiarity with the convention on 
								//Reads
	string out_prefx2=out_prefx;

	if(type1=="Reads"){
		string real_parent=parent1.substr(1);
		tbl1=new Reads(real_parent); //don't forget that parent1[0] is themate_en flag 
		if(parent1[0]=='0') out_prefx1="0"+out_prefx1;
		else out_prefx1="1"+out_prefx1;
	}
	if(type1=="Text_table"){
		tbl1=new Text_table(parent1);
	}
	if(type1=="Intervals"){
		tbl1=new Intervals(parent1);
		tbl1->load();
	}
	if(type1=="Joined"){
		tbl1=new Joined(parent1);
	}

	if(type2=="Reads"){
		string real_parent=parent2.substr(1);
		tbl2=new Reads(real_parent);
		if(parent2[0]=='0') out_prefx2="0"+out_prefx2;
		else out_prefx2="1"+out_prefx2;

	}
	if(type2=="Text_table"){
		tbl2=new Text_table(parent2);
	}
	if(type2=="Intervals"){
		tbl2=new Intervals(parent2);
		tbl2->load();
	}
	if(type2=="Joined"){
		tbl2=new Joined(parent2);
	}

	if (type1=="Joined") tbl1->print(out_prefx1, vec1);
	else{
		if(!already_printed(history_log, parent1)){
			history_log[parent1]=true;
			table_no++;
			stringstream ss;
			ss << table_no;
			out_prefx1=out_prefx1+"+"+ss.str();
			tbl1->print(out_prefx1, vec1);
		}
	}
	if (type2=="Joined") tbl2->print(out_prefx2, vec2);
	else{
		if(!already_printed(history_log, parent2)){
			history_log[parent2]=true;
			table_no++;
			stringstream ss;
			ss << table_no;
			out_prefx2=out_prefx2+"+"+ss.str();
			tbl2->print(out_prefx2, vec2);
		}
	}

	delete tbl1;
	delete tbl2;
}

void Joined::print(string out_prefx){
	vector<int> flt;
	for(int i=0;i<vec.size();i++) flt.push_back(i);
	print(out_prefx, flt);
	vector<string> all_tables;
	vector<vector<int> >lnks=get_links(flt, all_tables);
	string outfile=out_prefx+".links.txt";
	ofstream ofile;
	ofile.open(outfile.c_str(), ios::out);
	
	for(int i=0;i<all_tables.size();i++)
		ofile << all_tables[i] << " ";
	ofile << endl << endl;
	if(lnks.size()==0){ofile.close(); return;}
	for(int j=0;j<lnks[0].size();j++){
		for(int i=0;i<lnks.size();i++)
			ofile << lnks[i][j] <<" ";
		ofile << endl;
	}
	ofile.close();
}

//It returns a set of vectors that show the links between the basic tables
//that comprise a join
vector<vector<int> > Joined::get_links(vector<int>& flt, vector<string>& tables){
	vector<vector<int> > ret;
	vector <int> vec1;
	vector <int> vec2;

	for(int i=0;i<flt.size();i++){
		vec1.push_back(vec[flt[i]].indx1);
		vec2.push_back(vec[flt[i]].indx2);
	}

	if(type1!="Joined"){
		ret.push_back(vec1);
		if(type1!="Reads")
			tables.push_back(parent1);
		else tables.push_back(parent1.substr(1));
	}
	else{
		Joined tbl1(parent1);
		vector<vector<int> > tmp;
		tmp=tbl1.get_links(vec1, tables);
		for(int i=0;i<tmp.size();i++)
			ret.push_back(tmp[i]);
	}

	if(type2!="Joined"){
		ret.push_back(vec2);
		if(type2!="Reads")
			tables.push_back(parent2);
		else tables.push_back(parent2.substr(1));
	}
	else{
		Joined tbl2(parent2);
		vector<vector<int> > tmp;
		tmp=tbl2.get_links(vec2, tables);
		for(int i=0;i<tmp.size();i++)
			ret.push_back(tmp[i]);
	}
	return ret;
}








