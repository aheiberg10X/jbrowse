void ioerror(char *msg);

struct st_node{//an entry of the symbol table
	struct st_node *next;
	char *name;
	char *type;//integer-string-table-char etc
	char *kind;//attribute-table etc
	char *place;//used for the intermediate code
	struct st_node *owner;//a pointer to the owner entity. In case of an attribute, 
	//the owner is the table that contains it. NULL if no owner exists.
};
typedef struct st_node st_node;

/*lst is the head of a list of st_node. The function
returns the node whose name, type, kind and owner match the input. If any of
type, kind, owner is NULL it is considered as wildcard.
The function returns NULL on failure*/
st_node *lookup(st_node *lst, char *name, char *type, char *kind, st_node *owner);

//it creates a node whose properties are initialized from the input args
st_node *create_node(char *name, char *type, char *kind, st_node *owner);

void destroy_list(st_node *lst);

/*lst is the head of a list of st_node. The function creates a node
whose data are taken from the input and adds it at the beginning of the list*/
//void add_node(st_node *lst, char *name, char *type, char *kind, st_node *owner);
/*lst is the head of a list of st_node in which newnode is going to be added
at the beginning of the list*/
void add_node(st_node **lst, st_node *newnode);

//It adds newnode to the array list lst of length n. The basic operation is
//lst[n++]=newnode
void add_node_to_array(st_node **lst, int *n, st_node *newnode);

//If lst does not contain already an attribute with the same name and the same
//owner, it creates one. Otherwise it returns NULL
st_node *check_and_create(st_node *lst, char *name, char *type, char *kind, st_node *cur_owner);

//It returns that node of lst where the name matches and the owner is one
//of the tables of tbl_lst. Both lists contain pointers to the same objects.
st_node *lookup_mult_tables(st_node **tbl_lst, int len_tbl_lst, st_node *lst, char *name);

//it checks whether each of the nodes of arg_lst is a property of any of the 
//tables in the "from_lst" and adds them in lst with owner as specified
//by the input
void check_and_copy_from_mult(st_node **lst, st_node *arg_lst, st_node **from_lst, int len_from_lst, st_node *owner);


//It replicates all properties of lst whose owner is a table in the from_lst
//and cur_owner is the owner of the new attributes.
void replicate_args(st_node **from_lst, int len_from_lst, st_node *lst, st_node *cur_owner);

//it adds to em_lst an st_node whose name is the concatenation of str1...4 
//and also increases nextstat.
void emit(st_node **em_lst, char *str1, char *str2, char *str3, char *str4, char *str5);

//it traverses lst backwards and outputs the name of each node.
void output_emit_list(st_node *lst);

//It creates a node with name node_info and adds it to the list at the place that precedes
//where_str. For example if the list consists of A->B->C and where_str=B, the new node
//is going to be placed between B and C. Now if there are multiple B's such as A->B->B-C
//the new node is going to be inserted between B and C
void squeeze_node(st_node *lst, char *where_str, char *node_info);


//It returns that node of lst that has been entered prior to list_node. In this implementation
//the answer is lst_node->next, but it can change if the list implementation changes.
st_node *get_previous_node(st_node *lst_node);

//it generates a new temporary variable
char *get_newtemp();

//It opens the file that contains the parsed tables and it loads the table and the respective
//parameters to symbl_lst. If name!=READS, it also populates emit_lst with the proper commands
void load_table(st_node **symbl_lst, st_node **emit_lst, char *parsed_table_file, char *name);

