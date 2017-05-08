#include <iostream>
#include <string>
#include <vector>

#ifndef TYPE_H
#define TYPE_H

#define VOID_TYPE 0
#define INT_TYPE 1
#define FLOAT_TYPE 2
#define STRUCT_TYPE 3

#define TYPE_ARRAY_REF 5
#define TYPE_DEREF 6

class declarator_class;
class type_specifier_class;

using namespace std;

class datatype {
public:
	int type;
	std::string data;
	int deref;
	bool is_array;
	std::vector<int> array;

	datatype();
	int size();
	bool operator == (datatype t2);
	bool operator < (const datatype &t2) const;
	void print();
	void lprint();
	void addpointer();
	int elem();
};

datatype createtype(type_specifier_class* t, declarator_class* d);
datatype createtype(type_specifier_class* t, int nstars);
datatype createtype(int type);
datatype extract_type(datatype dt, int method);
bool is_compatible(int x, int y);
bool compatible(datatype t1, datatype t2);
bool isVoidType(datatype t);
bool isIntType(datatype t);
bool isFloatType(datatype t);
bool isIntOrFloatType(datatype t);
bool isStructType(datatype t);
bool isStructPointerType(datatype t);
bool isPointer(datatype t);

// class struct_specifier_class {
// public:
// 	struct_specifier_class();
// };

class type_specifier_class {
public:
	int type;
	string data;
	type_specifier_class(int type);
	type_specifier_class(int type, string data);
};

// class declaration_list_class {

// };

class declarator_list_class {
public:
	std::vector<declarator_class*> v;

	declarator_list_class();
	void push(declarator_class* data);
};

class declaration_class {
public:
	type_specifier_class* type;
	declarator_list_class* decl_list;

	declaration_class();
	declaration_class(type_specifier_class* t, declarator_list_class* d);
};

class declarator_class {
public:
	string identifier;
	int deref;
	std::vector<int> array;

	declarator_class();
	declarator_class(string name);
	void addstar();
	void add_array_index(int index);

};

class parameter_declaration_class {
public:
	datatype t;
	string identifier;

	parameter_declaration_class();
	parameter_declaration_class(type_specifier_class* typedata, declarator_class* decldata);
};

class parameter_list_class {
public:	
	std::vector<parameter_declaration_class* > param;
	int length;

	parameter_list_class();
	void push(parameter_declaration_class* data);	

};

class fun_declarator_class {
public:	
	int deref;
	string identifier;
	parameter_list_class* param;

	fun_declarator_class();
	fun_declarator_class(string name);
	fun_declarator_class(string name,parameter_list_class* param);

	void addstar();
};

class declaration_list_class {
public:
	int offset;

	declaration_list_class();
};

#endif