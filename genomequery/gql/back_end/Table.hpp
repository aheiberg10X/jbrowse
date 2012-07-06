#ifndef __TABLE__HPP
#define __TABLE__HPP
#include<string>
#include <vector>

using namespace std;

class Table{
	protected:
		string file_loc;
	public:
		Table(string str){file_loc=str;}
		string get_loc(){ return file_loc;}
		void set_loc(string str){file_loc=str;}
		
		/*it prints the contents of the table to the file whose prefix is prefx.
		Only the records from filt are considered. */
		virtual void print(string prefx, vector<int>& filt){;}

		virtual void load(){;}
};
#endif
