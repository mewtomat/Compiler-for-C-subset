#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include "type.h"
#include "symbtab.h"
#include <sstream>

#ifndef AST_H
#define AST_H


using namespace std;

class exp_astnode;
class identifier_astnode;

enum typeExp {EmptyNode, SeqNode, AssNode, ReturnNode, IfNode, WhileNode, ForNode, 	// of statement kind
			  OpBinaryNode, OpUnaryNode, AssignNode, FunCallNode, IntConstNode,FloatConstNode, StringConstNode, PointerNode,
			  IdentifierNode, ArrayRefNode, DerefNode, MemberNode, ArrowNode};

std::string toInstr(string typecast,string id);
std::string newlabel();

class abstract_astnode
{
public:
	virtual void print (int ntabs) = 0;
	enum typeExp astnode_type;				//maybe this should be moved to protected portion{using getter functions , as per the specifications}
	virtual std::string generate_code(const SymbTab&, bool) = 0;
	virtual void register_req(bool left)=0;
	virtual void set_funcname(string fname)=0;
	// virtual enum typeExp getType() = 0;
	// virtual bool checkTypeofAST() = 0;
	std::string f_name;
	std::string id;
	protected:
	// virtual void setType(typeExp basic_types) = 0;				//originally setType(basic_types)=0;  ... verify if the modifi
	//private:													// originally was private

};

class statement_astnode : public abstract_astnode
{
public:
	statement_astnode();
};

class exp_astnode: public statement_astnode
{
public:
	bool lvalue;
	string typecast;
	datatype data_type;
	bool is_calculable;
	int int_val;
	exp_astnode();
	int regs;
	int fregs;
	bool isLeafNode;
	bool is_modified;
};

class ref_astnode: public exp_astnode
{
public:
	ref_astnode();
};

/////////////////////////////////////////

class empty_astnode: public statement_astnode
{
public:
	empty_astnode();
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
};

class seq_astnode: public statement_astnode
{
public: 
	seq_astnode();
	void pushback(statement_astnode* newchild);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	std::vector<statement_astnode*> children_nodes;
};

class ass_astnode: public statement_astnode
{
public:
	ass_astnode(ref_astnode* left, exp_astnode* right);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	ref_astnode* left;
	exp_astnode* right;
};

class return_astnode: public statement_astnode
{
public:
	return_astnode(exp_astnode* child);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* child;
};

class if_astnode: public statement_astnode
{
public:
	if_astnode(exp_astnode* left, statement_astnode* middle, statement_astnode* right);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	statement_astnode* middle;
	statement_astnode* right;
};

class while_astnode: public statement_astnode
{
public:
	while_astnode(exp_astnode* left, statement_astnode* right);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	statement_astnode* right;
};

class for_astnode: public statement_astnode
{
public:
	for_astnode(exp_astnode* left, exp_astnode* middle1,exp_astnode*middle2,  statement_astnode* right);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	exp_astnode* middle1;
	exp_astnode* middle2;
	statement_astnode* right;
};

///////////////////////////////////////////////////////////////

class op_binary_astnode: public exp_astnode
{
public:
	op_binary_astnode(std::string val, exp_astnode* left, exp_astnode* right);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	exp_astnode* right;
};

class op_unary_astnode: public exp_astnode
{
public:
	op_unary_astnode(std::string val);
	op_unary_astnode(std::string val, exp_astnode* child);
	// op_unary_astnode(abstract_astnode* child1, abstract_astnode* child2);
	std::string getoperator();
	// void setchild(abstract_astnode* child);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* child;
};

class assign_astnode: public exp_astnode
{
public:
	assign_astnode(exp_astnode* left, exp_astnode* right);				// earlier was ref_astnode*, now no concept of lvalue in grammar
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	exp_astnode* right;
};

class funcall_astnode: public exp_astnode
{
public:
	funcall_astnode();
	funcall_astnode(identifier_astnode* child);
	// funcall_astnode(abstract_astnode* ident, abstract_astnode* otherfunc);
	void setname(std::string funcname);
	void pushback(exp_astnode* child);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	std::vector<exp_astnode*> children;
	void set_funcname(string fname);
protected:
	identifier_astnode* funcname;
};

class intconst_astnode: public exp_astnode
{
public:
	intconst_astnode(int value);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
// protected:
	int value;
	void set_funcname(string fname);
};

class floatconst_astnode: public exp_astnode
{
public:
	floatconst_astnode(float value);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
// protected:
	void set_funcname(string fname);
	float value;
};

class stringconst_astnode: public exp_astnode
{
public:
	std::string datalabel;
	stringconst_astnode(std::string value);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
// protected:
	void set_funcname(string fname);
	std::string value;
};


class pointer_astnode: public exp_astnode
{
public:
	pointer_astnode(ref_astnode* child);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	ref_astnode* child;
};

////////////////////////////////////////////////

class identifier_astnode: public ref_astnode
{
public:
	identifier_astnode(std::string val);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
};

class arrayref_astnode: public ref_astnode
{
public:
	arrayref_astnode(exp_astnode* left, exp_astnode* right);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	exp_astnode* right;
};

class deref_astnode: public ref_astnode
{
public:
	deref_astnode(ref_astnode* child);
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	ref_astnode* child;
};

class member_astnode: public ref_astnode
{
public:
	member_astnode(exp_astnode* left, identifier_astnode* right);			// ref to exp
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	identifier_astnode* right;
};

class arrow_astnode: public ref_astnode
{
public:
	arrow_astnode(exp_astnode* left, identifier_astnode* right);			// ref to exp
	void print(int ntabs);
	std::string generate_code(const SymbTab& st, bool b);
	void register_req(bool left);
	void set_funcname(string fname);
protected:
	exp_astnode* left;
	identifier_astnode* right;
};

#endif