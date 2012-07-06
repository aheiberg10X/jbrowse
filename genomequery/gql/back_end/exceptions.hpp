#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include<iostream>
#include<exception>

using namespace std;

class cmd_bad_symbol: public exception{
	virtual const char* what() const throw(){
		return "Unevaluated variables during evaluation";
	}
};

class cmd_wrong_stack: public exception{	
	virtual const char* what() const throw(){
		return "Bad state of stack at the end of evaluation";
	}
};

class cmd_bad_type: public exception{
	virtual const char* what() const throw(){
		return "Unknown type";
	}
};

class cmd_bad_content: public exception{
	virtual const char* what() const throw(){
		return "Bad command structure";
	}
};

class non_boolean_expr: public exception{
	virtual const char* what() const throw(){
		return "Boolean expression expected";
	}
};

class bad_type_field: public exception{
	virtual const char* what() const throw(){
		return "Incompatible variable type";
	}
};

class neg_bitvector_pos: public exception{
	virtual const char* what() const throw(){
		return "negative indexing of bitvector";
	}
};

class bad_bitvector_pos: public exception{
	virtual const char* what() const throw(){
		return "Out of bound indexing of bitvector";
	}
};

class unpaired_rd_exception: public exception{
	virtual const char* what() const throw(){
		return "An unpaired read found while printing mates";
	}
};

class negative_coverage: public exception{
	virtual const char* what() const throw(){
		return "Negative coverage while merging intervals";
	}
};

class interval_tree_search: public exception{
	virtual const char* what() const throw(){
		return "interval tree search_range missed a case";
	}
};

class illegal_read_prefx: public exception{
	virtual const char* what() const throw(){
		return "Read prefix found without the mate_flag";
	}
};

class dereference_map_exception: public exception{
	virtual const char* what() const throw(){
		return "Unexpected end of map entries";
	}
};
class unknown_joined_input: public exception{
	virtual const char* what() const throw(){
		return "Given table does not appear as a parent of a Joined table";
	}
};
class unsupported_type: public exception{
	virtual const char* what() const throw(){
		return "Type is not one of int, float, string, char";
	}
};

class out_of_bounds: public exception{
	virtual const char* what() const throw(){
		return "Out of bound indexing";
	}
};

class no_vector_space: public exception{
	virtual const char* what() const throw(){
		return "No space available";
	}
};

#endif
