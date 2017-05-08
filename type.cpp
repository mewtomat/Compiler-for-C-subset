datatype createtype(type_specifier_class* typedata, declarator_class* decldata){
	datatype t;
	t.type=typedata->type;
	t.data=typedata->data;
	t.deref=decldata->deref;
	if(decldata->array.size()==0) {
		t.is_array=false;
	}
	else {
		t.is_array=true;
		t.array=decldata->array;
	}
	return t;
}

datatype createtype(type_specifier_class* typedata, int nstars){
	// std::cout<<"Entered"<<std::endl;
	datatype t;
	t.type=typedata->type;
	// std::cout<<t.type<<std::endl;
	t.data=typedata->data;
	t.deref=nstars;
	return t;
}

datatype createtype(int type)
{
	datatype t;
	t.type = type;
	t.is_array=false;
	t.deref=0;
	return t;
}

datatype createVoidStarType()
{
	datatype t;
	t.deref = 1;
	t.type = VOID_TYPE;
	t.is_array=false;
	t.data="";
	return t;
}

//////////////////////////////////////////////////////////////////////////////////

bool datatype::operator == (datatype t2) {
	return (type==t2.type) && (data==t2.data) && (deref==t2.deref) && (is_array==t2.is_array) && (array==t2.array);
}

bool datatype::operator < (const datatype &t2) const {
	return true;
}

void datatype::addpointer()
{
	reverse(array.begin(), array.end());
	array.push_back(-1);
	reverse(array.begin(), array.end());
	is_array=true;
}

void datatype::print()
{

	if(type==VOID_TYPE)cout<<"void ";
	if(type==INT_TYPE)cout<<"int ";
	if(type==FLOAT_TYPE)cout<<"float ";
	if(type==STRUCT_TYPE)cout<<"struct "<<data<<" ";
	for(int i=0;i<deref;++i)
	{
		cout<<"*";
	}
	if(is_array) cout<<" (*) ";
	for(int i=1;i<array.size();++i)
	{
		cout<<"["<<array[i]<<"]";
	}
}

void datatype::lprint()
{
	if(type==VOID_TYPE)cout<<"void ";
	if(type==INT_TYPE)cout<<"int ";
	if(type==FLOAT_TYPE)cout<<"float ";
	if(type==STRUCT_TYPE)cout<<"struct "<<data<<" ";
	for(int i=0;i<deref;++i)
	{
		cout<<"*";
	}
	// if(is_array && array[0] == -1) cout<<" (*) ";
	for(int i=0;i<array.size();++i)
	{
		if(i==0 && array[i] == -1)
		{
			cout<<"(*)";
		}
		else
		cout<<"["<<array[i]<<"]";
	}
}

datatype extract_type(datatype dt, int method)
{
	datatype t = dt;
	if(method == TYPE_ARRAY_REF)
	{
		t.array.erase(t.array.begin());
		if(t.array.size() == 0)
		{
			t.is_array=false;
		}
	}
	else if(method == TYPE_DEREF)
	{
		if(t.is_array)
		{
			t.array.erase(t.array.begin());
			if(t.array.size() == 0)
			{
				t.is_array=false;
			}
		}
		else
		{
			t.deref--;
		}
	}

	return t;
}

bool isVoidType(datatype t)
{
	if(t.type == VOID_TYPE && t.deref == 0 && t.is_array == false)
	{
		return true;
	}
	else return false;
}

bool isIntType(datatype t)
{
	if(t.type == INT_TYPE && t.deref == 0 && t.is_array == false)
	{
		return true;
	}
	else return false;
}

bool isFloatType(datatype t)
{
	if(t.type == FLOAT_TYPE && t.deref == 0 && t.is_array == false)
	{
		return true;
	}
	else return false;
}

bool isStructType(datatype t)
{
	if(t.type == STRUCT_TYPE && t.deref == 0 && t.is_array == false)
	{
		return true;
	}
	else return false;
}

bool isStructPointerType(datatype t)
{
	if(t.type == STRUCT_TYPE && t.deref == 1 && t.is_array == false)
	{
		return true;
	}
	else return false;
}

bool isDereferenceAllowed(datatype t)
{
	if(t.type == -1) return false;
	if(t.is_array || t.deref >1)
		return true;
	else if(t.deref == 1 && t.type!=VOID_TYPE )
		return true;
	else return false;
}

bool isIntOrFloatType(datatype t){
	return isIntType(t) || isFloatType(t);
}

bool isPointer(datatype t) {
	if(t.is_array && t.array[0]==-1)
		return true;
	if(t.deref>0 && !t.is_array)
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////
datatype::datatype() {

}

int datatype::size() {
	int size=4;
	if(is_array) {
		if(data=="")
			size=4;
		else {
			if(gst.Entries.find(data) != gst.Entries.end())
			{
				size=gst.Entries[data].size;
			}
		}
		for(int i=0; i<array.size(); i++) {
			size*=array[i];
		}
		if(size<0)
			size=4;
	}
	else {
		if(deref>0) {
			return 4;
		} 
		else {
			if(type==0)
				return 0;
			if(data=="")
				size=4;
			else {
				if(gst.Entries.find(data) != gst.Entries.end())
				{
					size=gst.Entries[data].size;
				}
			}

			return size;
		}
	}
	return size;
}

int datatype::elem() {
	if(is_array) {
		datatype temp = extract_type(*this,TYPE_ARRAY_REF);
		return temp.size();
	}	
	else {
		datatype temp = *this;
		temp.deref--;
		return temp.size();
	}
}

//////////////////////////////////////////////////////////////////////////////////

type_specifier_class::type_specifier_class(int x) {
	type=x;
	data="";
}

type_specifier_class::type_specifier_class(int x, string y){
	type=x;
	data=y;
}

//////////////////////////////////////////////////////////////////////////////////

fun_declarator_class::fun_declarator_class(){
	deref=0;
	identifier="";
	param=NULL;
}

fun_declarator_class::fun_declarator_class(string s){
	deref=0;
	identifier=s;
	param=NULL;
}

fun_declarator_class::fun_declarator_class(string s,parameter_list_class* p) {
	deref=0;
	identifier=s;
	param=p;
}

void fun_declarator_class::addstar() {
	deref++;
}

//////////////////////////////////////////////////////////////////////////////////

parameter_list_class::parameter_list_class(){
	length=0;
}

void parameter_list_class::push(parameter_declaration_class* data){
	param.push_back(data);
	length++;
}

//////////////////////////////////////////////////////////////////////////////////

parameter_declaration_class::parameter_declaration_class() {

}

parameter_declaration_class::parameter_declaration_class(type_specifier_class* typedata, declarator_class* decldata) {
	t=*new datatype();
	t.type=typedata->type;
	t.data=typedata->data;
	t.deref=decldata->deref;
	if(decldata->array.size()==0) {
		t.is_array=false;
	}
	else {
		t.is_array=true;
		t.array=decldata->array;
	}
	identifier=decldata->identifier;
}

//////////////////////////////////////////////////////////////////////////////////

declarator_class::declarator_class(){
	deref=0;
}

declarator_class::declarator_class(string name) {
	identifier=name;
	deref=0;
}

void declarator_class::addstar() {
	deref++;
}

void declarator_class::add_array_index(int index) {
	array.push_back(index);
}

//////////////////////////////////////////////////////////////////////////////////

declaration_class::declaration_class() {

}

declaration_class::declaration_class(type_specifier_class* t1, declarator_list_class* d1) {
	type=t1;
	decl_list=d1;
}

//////////////////////////////////////////////////////////////////////////////////

declarator_list_class::declarator_list_class(){

}

void declarator_list_class::push(declarator_class* data) {
	v.push_back(data);
}

//////////////////////////////////////////////////////////////////////////////////

declaration_list_class::declaration_list_class() {

}

//////////////////////////////////////////////////////////////////////////////////

bool is_compatible(int x, int y, string s, string t){
	if(x==y && s==t)
		return true;
	if((x==INT_TYPE && y==FLOAT_TYPE) || (y==INT_TYPE && x==FLOAT_TYPE)) 
		return true;
	return false;
}


bool compatible(datatype t1, datatype t2) {
//exact match
	// bool b=(t1.type==t2.type) && (t1.data==t2.data) && (t1.deref==t2.deref) && (t1.is_array==t2.is_array) && (t1.array==t2.array);
	if(t1==t2)
		return true;

//need to check if t1 is an array in assignment expr

// void* to any * thingy
	if(t1.type==VOID_TYPE && t1.deref==1) {
		if(t2.is_array || t2.deref>0)
			return true;
	}
	if(t2.type==VOID_TYPE && t2.deref==1) {
		if(t1.is_array || t1.deref>0)
			return true;
	}
//normal int - float compatibility
	if(t1.deref==0 && !t1.is_array && t2.deref==0 && !t2.is_array) {
		return is_compatible(t1.type, t2.type, t1.data, t2.data);
	}
//arrays
	if(t1.is_array && t2.is_array) {
		if(t1.array.size() != t2.array.size())
			return false;
		for(int i=1; i<t1.array.size(); i++) {
			if(t1.array[i]!=t2.array[i])
				return false;
		}
		return (t1.type==t2.type) && (t1.deref==t2.deref) && (t1.data==t2.data);
	}

//int * = int []
//int ** = int* []
//int ** != int (*) []

	if(t1.deref==1 && t2.is_array && t2.array.size()==1) {
		if(t1.type==t2.type && t1.data==t2.data && t2.deref==0)
			return true;
	}
	if(t2.deref==1 && t1.is_array && t1.array.size()==1) {
		if(t2.type==t2.type && t2.data==t2.data && t1.deref==0)
			return true;
	}

	if(t1.deref>0 && t2.deref>0 && (t1.is_array||t2.is_array)) {
		t1.deref--;
		t2.deref--;
		return compatible(t1,t2);
	}

	return false;	
}