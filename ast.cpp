#include "helper.cpp"

//////////////////////////////

statement_astnode::statement_astnode()
{
}

/////////////////////////////

empty_astnode::empty_astnode():statement_astnode()
{
	id = "Empty";
	astnode_type=EmptyNode;
}
//third check
void empty_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"(Empty)";
}

std::string empty_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for empt"<<endl;
	return "";

}

void empty_astnode::register_req(bool left) {

}

void empty_astnode::set_funcname(string fname){

}

//////////////////////////

seq_astnode::seq_astnode():statement_astnode()
{
	id = "Seq";
	astnode_type=SeqNode;
}

void seq_astnode::pushback(statement_astnode* child)
{
	children_nodes.push_back(child);
}

void seq_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"(Block ["<<endl;
	for(int i=0;i<children_nodes.size();++i)
	{
		children_nodes[i]->print(ntabs+1);
		cout<<endl;
	}
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"]) ";
}

std::string seq_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for seq"<<endl;
	for(int i=0;i<children_nodes.size();++i)
	{
		children_nodes[i]->generate_code(st, true);
	}
	return "";
}

void seq_astnode::register_req(bool left) {
	for(int i=0;i<children_nodes.size();++i)
	{
		children_nodes[i]->register_req(true);
	}
}

void seq_astnode::set_funcname(string fname){
	f_name=fname;
	for(int i=0;i<children_nodes.size();++i)
	{
		children_nodes[i]->set_funcname(fname);
	}
}

///////////////////////////////////

ass_astnode::ass_astnode(ref_astnode* l , exp_astnode* r):statement_astnode()
{
	left = l;
	right = r;
	id = "Ass";
	astnode_type=AssNode;
}

void ass_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"(Ass ";	left->print(0);	cout<<" "; right->print(0); cout<<" )";
}

std::string ass_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for ass"<<endl;

	// never used in parse.y
	return "";
}

void ass_astnode::register_req(bool ileft) {
	left->register_req(true);
	right->register_req(false);
}

void ass_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	right->set_funcname(fname);
}

///////////////////////////////////

return_astnode::return_astnode(exp_astnode* c):statement_astnode()
{
	child = c;
	id = "Return";
	astnode_type=ReturnNode;
}
void return_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"(Return ";child->print(0);std::cout<<") ";
}

std::string return_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for ret"<<endl;
	assembly_comment("generate_code:return");
	if(isStructType(child->data_type)) {
		child->generate_code(st,true);

		string tempreg1 = registers.top();
		//tempreg1 holds the address of source
		string tempreg2 = "$4";
		cout<<"li "<<tempreg2<<", "<<4+gst.search(f_name)->symbtab->paramSize()<<endl;
		cout<<"add "<<tempreg2<<", "<<tempreg2<<", $fp"<<endl;
		//tempreg2 hold the address of designation

		int struct_size = gst.search(child->data_type.data)->size;

		cout<<"li $5,"<<struct_size<<endl;
		string L1 = newlabel();
		string L2 = newlabel();
		cout<<"j "<<L1<<endl;

		cout<<L2<<": "<<endl;
		cout<<"lw $7, 0("<<tempreg1<<")"<<endl;
		cout<<"sw $7, 0("<<tempreg2<<")"<<endl;
		cout<<"addi "<<tempreg1<<", "<<tempreg1<<",4"<<endl;
		cout<<"addi "<<tempreg2<<", "<<tempreg2<<", 4"<<endl;
		cout<<"addi $5,$5,-4"<<endl;

		cout<<L1<<" :"<<endl;
		cout<<"seq $6,$5,$0"<<endl;
		cout<<"beqz $6,"<<L2<<endl;

		cout<<"li $5, "<<struct_size<<endl;
		cout<<"sub "<<registers.top()<<", "<<tempreg2<<", $5"<<endl;

		//then deref and push word by word the value on stack
	}
	else {
		if(isFloatType(gst.search(f_name)->type)){
			//have to return float;
			child->generate_code(st,false);
			string tempreg1;
			string tempreg2 = "$4";
			cout<<"li "<<tempreg2<<", "<<4+gst.search(f_name)->symbtab->paramSize()<<endl;
			cout<<"add "<<tempreg2<<", "<<tempreg2<<", $fp"<<endl;
			//tempreg2 hold the address of designation
			if(isFloatType(child->data_type)){
				tempreg1=fregisters.top();
				cout<<"s.s "<<tempreg1<<", 0("<<tempreg2<<")"<<endl;
			} else {
				tempreg1=registers.top();
				cout<<"mtc1 "<<registers.top()<<", "<<fregisters.top()<<endl;
				cout<<"cvt.s.w "<<fregisters.top()<<", "<<fregisters.top()<<endl;
				cout<<"s.s "<<fregisters.top()<<", 0("<<tempreg2<<")"<<endl;
			}
		}else {
			child->generate_code(st,false);
			string tempreg1;
			string tempreg2 = "$4";
			cout<<"li "<<tempreg2<<", "<<4+gst.search(f_name)->symbtab->paramSize()<<endl;
			cout<<"add "<<tempreg2<<", "<<tempreg2<<", $fp"<<endl;
			//tempreg2 hold the address of designation

			if(isFloatType(child->data_type)){
				tempreg1=fregisters.top();
				cout<<"cvt.w.s "<<fregisters.top()<<", "<<fregisters.top()<<endl;
				cout<<"mfc1 "<<registers.top()<<", "<<fregisters.top()<<endl;
				cout<<"sw "<<registers.top()<<", 0("<<tempreg2<<")"<<endl;
			} else {
				tempreg1=registers.top();
				cout<<"sw "<<tempreg1<<", 0("<<tempreg2<<")"<<endl;
			}
		}

		//Push the 4 bytes on stack
	}
	// saved the value on the stack, and onto the registers/fregisters.top(). Now go to exit point of this function
	gencodeJ("j ","EXIT"+f_name);
	return "";
}

void return_astnode::register_req(bool left) {
	child->register_req(true);
}

void return_astnode::set_funcname(string fname){
	f_name=fname;
	child->set_funcname(fname);
}


////////////////////////////////////

if_astnode::if_astnode(exp_astnode* l , statement_astnode* m , statement_astnode* r):statement_astnode()
{
	left = l;
	middle = m;
	right = r;
	id="If";
	astnode_type=IfNode;
}

void if_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"(If ";left->print(0);std::cout<<endl;middle->print(ntabs+1);std::cout<<endl;right->print(ntabs+1);std::cout<<") ";
}

std::string if_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for if"<<endl;
	// l1=newlabel(),l2=newlabel(),l3=newlabel();
	// set left.fl=l2, left.tl=l1, left.fall=true;
	//if.code = left.code|| l1|| middle.code || goto l3 || l2 || right.code|| l3
	// FLOW CONTROL

	assembly_comment("generate_code:if_astnode");
	string thenLabel= newlabel();		///not used now, using fallthrough
	string elseLabel= newlabel();
	string nextLabel= newlabel();
	// left->fl=elseLabel;					//not used now
	// left->tl=thenLabel;					//not used now
	// left->fall = true;					//not used now

	left->generate_code(st, false);		//should pass some variable which tells to generate code with gotos and stuff.
	// do that code first, then only short circuiting wala code can be done.
	// For now, ditch lazy evaluation, use the value to do jumps and stuff.
	if(isFloatType(left->data_type)){
		cout<<"cvt.w.s "<<fregisters.top()<<", "<<fregisters.top()<<endl;
		cout<<"mfc1 "<<registers.top()<<", "<<fregisters.top()<<endl;
	}
	//regsiters.top() holds exp value				//IMPORTANT: note that float[||/&&/<=/==...]float should finally put value in registers.top()
	// gencodeR("beqz", registers.top(), "$0", elseLabel);
	cout<<"beqz "<<registers.top()<<", "<<elseLabel<<endl;
	middle->generate_code(st, false);								//IMPORTANT: the is_addr for statements should be true or false??
	gencodeJ("j",nextLabel);
	gencodeL(elseLabel);
	right->generate_code(st, false);								// Here too
	gencodeL(nextLabel);
	assembly_comment("/generate_code:if_astnode");
	return "";
}

void if_astnode::register_req(bool ileft) {
	left->register_req(true);
	middle->register_req(true);
	right->register_req(true);
}

void if_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	middle->set_funcname(fname);
	right->set_funcname(fname);
}


////////////////////////////////////

while_astnode::while_astnode(exp_astnode* l, statement_astnode* r):statement_astnode()
{
	left = l;
	right = r;
	id="While";
	astnode_type=WhileNode;
}

void while_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"(While ";left->print(0);std::cout<<endl;right->print(ntabs+1);std::cout<<") ";
}

std::string while_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for while"<<endl;
	assembly_comment("generate_code:while_astnode");

	string condLabel = newlabel();
	string nextLabel = newlabel();

	gencodeL(condLabel);
	left->generate_code(st, false);
	if(isFloatType(left->data_type)){
		cout<<"cvt.w.s "<<fregisters.top()<<", "<<fregisters.top()<<endl;
		cout<<"mfc1 "<<registers.top()<<", "<<fregisters.top()<<endl;
	}
	//registers.top( ) holds the evaluated expression value
	// gencodeR("beqz",registers.top(), "$0", nextLabel);
	cout<<"beqz "<<registers.top()<<", "<<nextLabel<<endl;
	right->generate_code(st, false);
	gencodeJ("j",condLabel);
	gencodeL(nextLabel);
	assembly_comment("/generate_code:while_astnode");
	return "";

}

void while_astnode::register_req(bool ileft) {
	left->register_req(true);;
	right->register_req(true);
}

void while_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	right->set_funcname(fname);
}

/////////////////////////////////

for_astnode::for_astnode(exp_astnode* l , exp_astnode* m1 , exp_astnode* m2 , statement_astnode* r):statement_astnode()
{
	left = l;
	middle1 = m1;
	middle2 = m2;
	right = r;
	id="For";
	astnode_type=ForNode;
}

void for_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"(For ";left->print(0);std::cout<<" ";middle1->print(0);std::cout<<" ";middle2->print(0);std::cout<<endl;right->print(ntabs+1);std::cout<<") ";
}

std::string for_astnode::generate_code(const SymbTab& st, bool is_addr) {
	assembly_comment("generate_code:for_astnode");
	string condLabel = newlabel();
	string nextLabel = newlabel();

	left->generate_code(st, false);				//initialization
	// fall through
	gencodeL(condLabel);
	middle1->generate_code(st, false);				//eval condition
	//registers.top() holds evaluated exp value
	// gencodeR("beqz",registers.top(), "$0", nextLabel);
	cout<<"beqz "<<registers.top()<<", "<<nextLabel<<endl;
	right->generate_code(st, false);		//code for the statements
	middle2->generate_code(st, false);		//loop iteration update
	gencodeJ("j",condLabel);
	gencodeL(nextLabel);
	assembly_comment("/generate_code:for_astnode");
	return "";
}

void for_astnode::register_req(bool ileft) {
	left->register_req(true);
	middle1->register_req(true);
	middle2->register_req(true);
	right->register_req(true);
}

void for_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	middle1->set_funcname(fname);
	middle2->set_funcname(fname);
	right->set_funcname(fname);
}

//////////////////////////////////

exp_astnode::exp_astnode():statement_astnode()
{
	is_modified=false;
}

//////////////////////////////////

op_binary_astnode::op_binary_astnode(std::string val, exp_astnode* l, exp_astnode* r):exp_astnode()
{
	id=val;
	left = l;
	right = r;
	astnode_type=OpBinaryNode;
}

void op_binary_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"("<<id<<" ";left->print(0);std::cout<<" ";right->print(0);std::cout<<") ";
}

std::string op_binary_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for binary"<<endl;

	assembly_comment("generate_code:op_binary_astnode");
	int availreg = registers.size();
	int availfreg = fregisters.size();
	// int availreg = CHOOSE(isFloatType(data_type),AVAILABLE_REGS,AVAILABLE_FREGS);

	int leftreg = left->regs;
	int rightreg = right->regs;
	int leftf = left->fregs;
	int rightf = right->fregs;

	if(id == "OR_OP") {
		string L1=newlabel();
		string L2=newlabel();
		string L3=newlabel();

		left->generate_code(st,false);
		string tempreg1= registers.top();
		// string tempreg1 = CHOOSE(isFloatType(left->data_type),registers.top(),fregisters.top());
		if(isFloatType(left->data_type)){
			cout<<"c.ne.s $f0, "<<fregisters.top()<<endl;
			cout<<"bc1t "<<L1<<endl;
		}else{
			gencodeZ("bnez", tempreg1, L1);
		}
		right->generate_code(st,false);
		if(isFloatType(right->data_type)){
			cout<<"c.eq.s $f0, "<<fregisters.top()<<endl;
			cout<<"bc1t "<<L2<<endl;
		}else{
			string tempreg2 = registers.top();
			gencodeZ("beqz",tempreg2,L2);
		}

		gencodeL(L1);
		gencodeI("li",tempreg1,"ERROR in OR_OP",1);
		gencodeJ("j", L3);

		gencodeL(L2);
		gencodeI("li",tempreg1,"ERROR in OR_OP",0);

		gencodeL(L3);

		return "";

	}
	else if(id == "AND_OP") {

		//Left to right
		//Lazy evaluation
		string L1=newlabel();
		string L2=newlabel();
		string L3=newlabel();

		left->generate_code(st,false);
		string tempreg1 = registers.top();
		if(isFloatType(left->data_type)){
			cout<<"c.eq.s $f0, "<<fregisters.top()<<endl;
			cout<<"bc1t "<<L1<<endl;
		}else {
			gencodeZ("beqz", tempreg1, L1);
		}

		right->generate_code(st,false);
		string tempreg2 = registers.top();
		if(isFloatType(right->data_type)){
			cout<<"c.ne.s $f0, "<<fregisters.top()<<endl;
			cout<<"bc1t "<<L2<<endl;
		}else {
			gencodeZ("bnez",tempreg2,L2);
		}

		gencodeL(L1);
		gencodeI("li",tempreg1,"ERROR in OR_OP",0);
		gencodeJ("j", L3);

		gencodeL(L2);
		gencodeI("li",tempreg1,"ERROR in OR_OP",1);

		gencodeL(L3);

		return "";
	}


	if(isFloatType(left->data_type) && isFloatType(right->data_type)){
		if(is_modified) {
			left->generate_code(st,false);
			string leftcomputed=fregisters.top();
			if(rightf<availfreg) {
				fregisters.pop();
				right->generate_code(st,false);
				string rightcomputed=fregisters.top();
				genInstr(id, isBoolean(id)?registers.top():leftcomputed,leftcomputed,rightcomputed,0);
				fregisters.push(leftcomputed);
				assembly_comment("/generate_code:op_binary_astnode");
				return "";
			}//else we need to save in the stack
			else {
				gencodeI("addi","$sp","$sp",-4);
				gencodeI("s.s",leftcomputed,"$sp",0);

				right->generate_code(st,false);
				string rightcomputed=fregisters.top();

				leftcomputed="$29";

				gencodeI("l.s",leftcomputed,"$sp",0);	//right value
				gencodeI("addi","$sp", "$sp", 4);
				genInstr(id, isBoolean(id)?registers.top():rightcomputed,leftcomputed,rightcomputed,0);
				// cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
				assembly_comment("/generate_code:op_binary_astnode");
				return "";
			}
		} else{
			if(availfreg>leftf && leftf<rightf) {
				fregisters.swap();
				right->generate_code(st,false);
				string tempreg2 = fregisters.top();
				fregisters.pop();

				left->generate_code(st,false);
				string tempreg1 = fregisters.top();

				// binary operators || && == != < <= > >= + - * /
				genInstr(id, isBoolean(id)?registers.top():tempreg1, tempreg1,tempreg2,0);
				fregisters.push(tempreg2);
				fregisters.swap();

				assembly_comment("/generate_code:op_binary_astnode");
				return "";

			}
			else if (availfreg>rightf && leftf>=rightf) {
				left->generate_code(st,false);
				string tempreg1 = fregisters.top();
				fregisters.pop();

				right->generate_code(st,false);
				string tempreg2 = fregisters.top();
				// binary operators || && == != < <= > >= + - * /
				// cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
				genInstr(id,  isBoolean(id)?registers.top():tempreg1,tempreg1,tempreg2,0);
				fregisters.push(tempreg1);

				assembly_comment("/generate_code:op_binary_astnode");
				return "";

			}
			else {
				right->generate_code(st,false);
				string tempreg2 = "$f28";

				gencodeI("addi","$sp", "$sp", -4);
				gencodeI("s.s",fregisters.top(),"$sp",0);

				left->generate_code(st,false);
				string tempreg1 = fregisters.top();

				gencodeI("l.s",tempreg2,"$sp",0);	//right value
				gencodeI("addi","$sp", "$sp", 4);

				// binary operators || && == != < <= > >= + - * /
				// cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
				genInstr(id,isBoolean(id)?registers.top():tempreg1,tempreg1,tempreg2,0);
				assembly_comment("/generate_code:op_binary_astnode");
				return "";
			}
		}
	}else if(isFloatType(left->data_type) && isIntType(right->data_type)){				// first right and then left, as from regs required
		//if exactly one of the operands is float, do it in order
		left->generate_code(st,false);
		string leftcomputed=fregisters.top();
		if(rightf<availfreg) {
			fregisters.pop();
			right->generate_code(st,false);
			string rightcomputed=registers.top();
			cout<<"mtc1 "<<rightcomputed<<", $f28"<<endl;
			cout<<"cvt.s.w $f28,$f28"<<endl;
			genInstr(id, isBoolean(id)?rightcomputed:leftcomputed,leftcomputed,"$f28",0);
			fregisters.push(leftcomputed);
			assembly_comment("/generate_code:op_binary_astnode");
			return "";
		}
		//else we need to save in the stack
		else {
			gencodeI("addi","$sp","$sp",-4);
			gencodeI("s.s",leftcomputed,"$sp",0);

			right->generate_code(st,false);
			string rightcomputed=registers.top();

			leftcomputed="$f29";

			gencodeI("l.s",leftcomputed,"$sp",0);	//right value
			gencodeI("addi","$sp", "$sp", 4);
			cout<<"mtc1 "<<rightcomputed<<", $f28"<<endl;
			cout<<"cvt.s.w $f28,$f28"<<endl;
			genInstr(id,isBoolean(id)?rightcomputed: fregisters.top(),leftcomputed,"$f28",0);
			// cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
			assembly_comment("/generate_code:op_binary_astnode");
			return "";
		}
	}else if(isIntType(left->data_type) && isFloatType(right->data_type)){
		//if exactly one of the operands is float, do it in order
		left->generate_code(st,false);
		string leftcomputed=registers.top();
		if(rightreg<availreg) {	//notice, for above cases, fregs was used
			registers.pop();
			right->generate_code(st,false);
			string rightcomputed=fregisters.top();
			cout<<"mtc1 "<<leftcomputed<<", $f28"<<endl;
			cout<<"cvt.s.w $f28,$f28"<<endl;
			genInstr(id, isBoolean(id)?leftcomputed:rightcomputed,"$f28",rightcomputed,0);
			registers.push(leftcomputed);
			assembly_comment("/generate_code:op_binary_astnode");
			return "";
		}
		//else we need to save in the stack
		else {
			gencodeI("addi","$sp","$sp",-4);
			gencodeI("sw",leftcomputed,"$sp",0);

			right->generate_code(st,false);
			string rightcomputed=fregisters.top();

			leftcomputed="$4";

			gencodeI("lw",leftcomputed,"$sp",0);	//right value
			gencodeI("addi","$sp", "$sp", 4);
			cout<<"mtc1 "<<leftcomputed<<", $f28"<<endl;
			cout<<"cvt.s.w $f28,$f28"<<endl;
			genInstr(id,isBoolean(id)?registers.top(): rightcomputed,"$f28",rightcomputed,0);
			// cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
			assembly_comment("/generate_code:op_binary_astnode");
			return "";
		}
	}else {
		//both int operands
		if(is_modified){
			left->generate_code(st,false);
			string tempreg1 = registers.top();
			//if there are enough registersfor right
			if(rightreg<availreg) {
				registers.pop();
				right->generate_code(st,false);
				string tempreg2=registers.top();

				cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;

				registers.push(tempreg1);
				assembly_comment("/generate_code:op_binary_astnode");
				return "";
			}
			//else we need to save in the stack
			else {
				gencodeI("addi","$sp","$sp",-4);
				gencodeI("sw",tempreg1,"$sp",0);

				right->generate_code(st,false);
				string tempreg2=registers.top();

				tempreg1="$4";

				gencodeI("lw",tempreg1,"$sp",0);	//right value
				gencodeI("addi","$sp", "$sp", 4);

				cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl; 		//IMPORTANT: This is wrong, isn't it

				assembly_comment("/generate_code:op_binary_astnode");
				return "";
			}
		} else {
			if(availreg>leftreg && leftreg<rightreg) {
				registers.swap();
				right->generate_code(st,false);
				string tempreg2 = registers.top();
				registers.pop();

				left->generate_code(st,false);
				string tempreg1 = registers.top();

				// binary operators || && == != < <= > >= + - * /
				cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;

				registers.push(tempreg2);
				registers.swap();

				assembly_comment("/generate_code:op_binary_astnode");
				return "";

			}
			else if (availreg>rightreg && leftreg>=rightreg) {
				left->generate_code(st,false);
				string tempreg1 = registers.top();
				registers.pop();

				right->generate_code(st,false);
				string tempreg2 = registers.top();
				// binary operators || && == != < <= > >= + - * /
				cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;

				registers.push(tempreg1);

				assembly_comment("/generate_code:op_binary_astnode");
				return "";

			}
			else {
				right->generate_code(st,false);
				string tempreg2 = "$4";

				gencodeI("addi","$sp", "$sp", -4);
				gencodeI("sw",registers.top(),"$sp",0);

				left->generate_code(st,false);
				string tempreg1 = registers.top();

				gencodeI("lw",tempreg2,"$sp",0);	//right value
				gencodeI("addi","$sp", "$sp", 4);

				// binary operators || && == != < <= > >= + - * /
				cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;

				assembly_comment("/generate_code:op_binary_astnode");
				return "";
			}
		}
	}

	// if(availreg>leftreg && leftreg<rightreg) {
	// 	registers.swap();
	// 	right->generate_code(st,false);
	// 	string tempreg2 = registers.top();
	// 	registers.pop();
	//
	// 	left->generate_code(st,false);
	// 	string tempreg1 = registers.top();
	//
	// 	// binary operators || && == != < <= > >= + - * /
	// 	cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
	//
	// 	registers.push(tempreg2);
	// 	registers.swap();
	//
	// 	assembly_comment("/generate_code:op_binary_astnode");
	// 	return "";
	//
	// }
	// else if (availreg>rightreg && leftreg>=rightreg) {
	// 	left->generate_code(st,false);
	// 	string tempreg1 = registers.top();
	// 	registers.pop();
	//
	// 	right->generate_code(st,false);
	// 	string tempreg2 = registers.top();
	// 	// binary operators || && == != < <= > >= + - * /
	// 	cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
	//
	// 	registers.push(tempreg1);
	//
	// 	assembly_comment("/generate_code:op_binary_astnode");
	// 	return "";
	//
	// }
	// else {
	// 	right->generate_code(st,false);
	// 	string tempreg2 = "$4";
	//
	// 	gencodeI("addi","$sp", "$sp", -4);
	// 	gencodeI("sw",registers.top(),"$sp",0);
	//
	// 	left->generate_code(st,false);
	// 	string tempreg1 = registers.top();
	//
	// 	gencodeI("lw",tempreg2,"$sp",0);	//right value
	// 	gencodeI("addi","$sp", "$sp", 4);
	//
	// 	// binary operators || && == != < <= > >= + - * /
	// 	cout<<toInstr(typecast, id)<<" "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
	//
	// 	assembly_comment("/generate_code:op_binary_astnode");
	// 	return "";
	// }

}

void op_binary_astnode::register_req(bool is_left) {
	left->register_req(true);
	right->register_req(false);
	int leftreg=left->regs;
	int rightreg=right->regs;
	int leftf=left->fregs;
	int rightf=left->fregs;
	if(left->is_modified || right->is_modified)
		is_modified=true;

	//fregs
	if(isFloatType(left->data_type) && isFloatType(right->data_type)) {
		if(id=="OR_OP" || id=="AND_OP") {
			fregs=max(leftf,rightf);
		}
		else if(is_modified) {
			fregs=max(leftf,rightf+1);
		}
		else {
			if(leftreg==rightreg) {
				fregs=leftf+1;
			}
			else {
				fregs=max(leftf,rightf);
			}
		}
		regs= max(leftreg,rightreg);
	}
	else if(isFloatType(data_type)) {

		if(id=="OR_OP" || id=="AND_OP") {						// left and then right
			fregs=max(leftf,rightf);
			regs=max(leftreg,rightreg);
		}
		else {									//left and then right when exactly one of them is float
		// else if(is_modified) {
			if(isFloatType(right->data_type)){
				fregs = max(leftf, rightf);
				regs = max(leftreg,1+rightreg);
			} else {
				fregs = max(leftf, 1+rightf);
				regs = max(leftreg,rightreg);
			}
		}
		// else {
		// 	if(isFloatType(right->data_type)){
		// 		if(leftf>=rightf) {						//left and then right
		// 			fregs=leftf;
		// 			regs=max(leftreg,1+rightreg);
		// 		}else{
		// 			fregs=rightf;
		// 			regs=max(leftreg,rightreg);
		// 		}
		// 	}else{
		// 		if(rightf>=leftf) {
		// 			fregs=rightf;
		// 			regs=max(rightreg,1+leftreg);
		// 		}else{
		// 			fregs=leftf;						// left and then right
		// 			regs=max(leftreg,rightreg);
		// 		}
		// 	}
		// }

	}
	else {
		if(id=="OR_OP" || id=="AND_OP") {
			regs=max(leftreg,rightreg);
		}
		else if(is_modified) {
			regs=max(leftreg,rightreg+1);
		}
		else {
			if(leftreg==rightreg)
				regs=leftreg+1;
			else
				regs=max(leftreg, rightreg);
		}
		fregs = max(leftf, rightf);
	}
	regs=max(1,regs);
}

void op_binary_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	right->set_funcname(fname);
}

///////////////////////////////////

op_unary_astnode::op_unary_astnode(std::string val):exp_astnode()
{
	id = val;
	astnode_type=OpUnaryNode;
}

void op_unary_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"("<<id<<" ";child->print(0);std::cout<<") ";
}

op_unary_astnode::op_unary_astnode(std::string val, exp_astnode* l):exp_astnode()
{
	id = val;
	child=l;
	astnode_type=OpUnaryNode;
}

std::string op_unary_astnode::getoperator() {
	return id;
}

std::string op_unary_astnode::generate_code(const SymbTab& st, bool is_addr) {
	assembly_comment("generate_code:op_unary_astnode");
	if(id=="Uminus") {
		child->generate_code(st,false);
		if(isFloatType(child->data_type)){					//IMPORTANT
			string tempreg1 = fregisters.top();
			// cout<<"subu "<<tempreg1<<", $f0, "<<tempreg1<<endl;
			cout<<"neg.s "<<tempreg1<<", "<<tempreg1<<endl;
		}else {
			string tempreg1 = registers.top();
			cout<<"subu "<<tempreg1<<", $0, "<<tempreg1<<endl;
		}
		assembly_comment("/generate_code:op_unary_astnode");
		return "";
	}
	else if(id=="Not") {
		child->generate_code(st,false);
		if(isFloatType(child->data_type)){
			string tempreg1 = fregisters.top();
			cout<<"c.eq.s , $f0, "<<tempreg1<<endl;
			string l1=newlabel();
			string l2=newlabel();
			cout<<"bc1f "<<l1<<endl;
			cout<<"li "<<registers.top()<<", 1"<<endl;
			gencodeJ("j", l2);
			gencodeL(l1);
			cout<<"li "<<registers.top()<<" ,0"<<endl;
			gencodeL(l2);

		}else {
			string tempreg1 = registers.top();
			cout<<"sltu "<<tempreg1<<", "<<tempreg1<<", 1"<<endl;
		}
		assembly_comment("/generate_code:op_unary_astnode");
		return "";
	}
	else if(id=="Address") {
		child->generate_code(st,true);
		assembly_comment("/generate_code:op_unary_astnode");
		return "";
	}
	else if(id=="Dereference") {
		child->generate_code(st,false);
		string tempreg1=registers.top();
		if(!is_addr){
			if(isFloatType(data_type)){
				cout<<"l.s "<<fregisters.top()<<", 0("<<tempreg1<<")"<<endl;
			}else {
				cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
			}
		}
		assembly_comment("/generate_code:op_unary_astnode");
		return "";
	}
	else {
		child->generate_code(st,true);
		if(isFloatType(data_type)){
			string tempreg1=registers.top();
			cout<<"move $f4,"<<tempreg1<<endl;
			gencodeI("l.s",fregisters.top(),tempreg1,0);
			cout<<"li.s $f29, 1.0"<<endl;
			// cout<<"li $4, 1"<<endl;
			// cout<<"mtc1 $4, "<<"$f29"<<endl;
			// cout<<"cvt.s.w "<<"$f29"<<", "<<"$f29"<<endl;
			
			gencodeR("add.s","$f29",fregisters.top(),"$f29");
			gencodeI("s.s","$f29","$4",0);
		}else{
			string tempreg1=registers.top();
			cout<<"move $4,"<<tempreg1<<endl;
			gencodeI("lw",tempreg1,tempreg1,0);
			gencodeI("addi","$5",tempreg1,1);
			gencodeI("sw","$5","$4",0);
		}
		return "";
	}

}

void op_unary_astnode::register_req(bool left) {
	child->register_req(left);
	regs=child->regs;
	fregs = child->fregs;
	if(isFloatType(data_type)) {
		if(id=="Dereference")
			fregs=max(1,fregs);
	}
	if(id=="Address" || id=="Not") {
		regs=max(regs,1);
	}
	if(child->is_modified) is_modified=true;
	if(id=="PP")
	{
		is_modified=true;
	}
}

void op_unary_astnode::set_funcname(string fname){
	f_name=fname;
	child->set_funcname(fname);
}

///////////////////////////////////

assign_astnode::assign_astnode(exp_astnode* l, exp_astnode* r):exp_astnode()
{
	left = l;
	right = r;
	astnode_type=AssignNode;
}

void assign_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(Ass ";left->print(0);std::cout<<" ";right->print(0);std::cout<<") ";
}

std::string assign_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for assign"<<endl;

	assembly_comment("generate_code:assign_astnode");
	// int availreg = registers.size();
	int availreg = AVAILABLE_REGS;

	int leftreg = left->regs;
	int rightreg = right->regs;

	if(left->is_modified || right->is_modified || isFloatType(left->data_type) ||isFloatType(right->data_type)) {
		//left to right evaluation
		if(isStructType(right->data_type)) {

			left->generate_code(st,true);
			string tempreg1 = registers.top();

			if(rightreg<availreg) {
				registers.pop();

				right->generate_code(st, true);
				int struct_size = gst.search(right->data_type.data)->size;
				//tempreg1 holds the address of destination
				//registers.top() hold the address of source

				cout<<"li $5,"<<struct_size<<endl;
				string L1 = newlabel();
				string L2 = newlabel();
				cout<<"j "<<L1<<endl;

				cout<<L2<<": "<<endl;
				cout<<"lw $7, 0("<<registers.top()<<")"<<endl;
				cout<<"sw $7, 0("<<tempreg1<<")"<<endl;
				cout<<"addi "<<registers.top()<<", "<<registers.top()<<",4"<<endl;
				cout<<"addi "<<tempreg1<<", "<<tempreg1<<", 4"<<endl;
				cout<<"addi $5,$5,-4"<<endl;

				cout<<L1<<" :"<<endl;
				cout<<"seq $6,$5,$0"<<endl;
				cout<<"beqz $6,"<<L2<<endl;

				cout<<"li $5, "<<struct_size<<endl;
				cout<<"sub "<<tempreg1<<", "<<tempreg1<<", $5"<<endl;
				registers.push(tempreg1);
				assembly_comment("/generate_code:assign_astnode");
				return "";
			}
			else {

				gencodeI("addi","$sp", "$sp", -4);
				gencodeI("sw",tempreg1,"$sp",0);
				tempreg1 = "$4";

				right->generate_code(st, true);

				gencodeI("lw",tempreg1,"$sp",0);	//left address
				gencodeI("addi","$sp", "$sp", 4);

				int struct_size = gst.search(right->data_type.data)->size;
				//tempreg1 holds the address of destination
				//registers.top() hold the address of source

				cout<<"li $5,"<<struct_size<<endl;
				string L1 = newlabel();
				string L2 = newlabel();
				cout<<"j "<<L1<<endl;

				cout<<L2<<": "<<endl;
				cout<<"lw $7, 0("<<registers.top()<<")"<<endl;
				cout<<"sw $7, 0("<<tempreg1<<")"<<endl;
				cout<<"addi "<<registers.top()<<", "<<registers.top()<<",4"<<endl;
				cout<<"addi "<<tempreg1<<", "<<tempreg1<<", 4"<<endl;
				cout<<"addi $5,$5,-4"<<endl;

				cout<<L1<<" :"<<endl;
				cout<<"seq $6,$5,$0"<<endl;
				cout<<"beqz $6,"<<L2<<endl;

				cout<<"li $5, "<<struct_size<<endl;
				cout<<"sub "<<tempreg1<<", "<<tempreg1<<", $5"<<endl;

				assembly_comment("/generate_code:assign_astnode");
				return "";

			}
		} else {
			left->generate_code(st,true);
			string tempreg1 = registers.top();
			if(isFloatType(right->data_type) && isFloatType(left->data_type)){			//IMPORTANT: verify the new part
				if(rightreg<availreg){
					registers.pop();
					right->generate_code(st,false);
					cout<<"s.s "<<fregisters.top()<<", 0("<<tempreg1<<") "<<endl;
					registers.push(tempreg1);
					assembly_comment("/generate_code:assign_astnode");
					return "";
				} else {
					gencodeI("addi","$sp", "$sp", -4);
					gencodeI("sw",tempreg1,"$sp",0);
					tempreg1 = "$4";
					right->generate_code(st,false);

					gencodeI("lw",tempreg1,"$sp",0);	//left address
					gencodeI("addi","$sp", "$sp", 4);

					cout<<"s.s "<<fregisters.top()<<", 0("<<tempreg1<<") "<<endl;
					assembly_comment("/generate_code:assign_astnode");
					return "";
				}

			}else if(isIntType(left->data_type) && isFloatType(right->data_type)) {
				if(rightreg<availreg){
					registers.pop();
					right->generate_code(st,false);
					cout<<"cvt.w.s "<<fregisters.top()<<", "<<fregisters.top()<<endl;//convert float to int
					cout<<"s.s "<<fregisters.top()<<", 0("<<tempreg1<<") "<<endl;	//do the store
					if(!is_addr)
						cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
					registers.push(tempreg1);
					assembly_comment("/generate_code:assign_astnode");
					return "";
				} else {
					gencodeI("addi","$sp", "$sp", -4);
					gencodeI("sw",tempreg1,"$sp",0);
					tempreg1 = "$4";

					right->generate_code(st,false);

					gencodeI("lw",tempreg1,"$sp",0);	//left address
					gencodeI("addi","$sp", "$sp", 4);

					cout<<"cvt.w.s "<<fregisters.top()<<", "<<fregisters.top()<<endl;
					cout<<"s.s "<<fregisters.top()<<", 0("<<tempreg1<<") "<<endl;
					if(!is_addr)
						cout<<"lw "<<registers.top()<<", 0("<<tempreg1<<")"<<endl;
					assembly_comment("/generate_code:assign_astnode");
					return "";
				}
			} else if(isFloatType(left->data_type) && isIntType(right->data_type)){
				if(rightreg<availreg){
					registers.pop();
					right->generate_code(st,false);
					cout<<"mtc1 "<<registers.top()<<", "<<fregisters.top()<<endl;
					cout<<"cvt.s.w "<<fregisters.top()<<", "<<fregisters.top()<<endl;	//fregisters.top() now holds the float typecasted value of left
					cout<<"s.s "<<fregisters.top()<<", 0("<<tempreg1<<") "<<endl;
					// if(!is_addr)							//not needed. value required is already in fregisters.top()
					// 	cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
					registers.push(tempreg1);
					assembly_comment("/generate_code:assign_astnode");
					return "";
				} else {
					gencodeI("addi","$sp", "$sp", -4);
					gencodeI("sw",tempreg1,"$sp",0);
					tempreg1 = "$4";

					right->generate_code(st,false);

					gencodeI("lw",tempreg1,"$sp",0);	//left address
					gencodeI("addi","$sp", "$sp", 4);

					cout<<"mtc1 "<<registers.top()<<", "<<fregisters.top()<<endl;
					cout<<"cvt.s.w "<<fregisters.top()<<", "<<fregisters.top();	//fregisters.top() now holds the float typecasted value of left
					cout<<"s.s "<<fregisters.top()<<", 0("<<tempreg1<<") "<<endl;
					assembly_comment("/generate_code:assign_astnode");
					return "";
				}
			}else {
				if(rightreg<availreg) {
					registers.pop();
					right->generate_code(st,false);
					cout<<"sw "<<registers.top()<<", 0("<<tempreg1<<") "<<endl;
					if(!is_addr)
						cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
					registers.push(tempreg1);
					assembly_comment("/generate_code:assign_astnode");
					return "";
				}else {
					gencodeI("addi","$sp", "$sp", -4);
					gencodeI("sw",tempreg1,"$sp",0);
					tempreg1 = "$4";

					right->generate_code(st,false);

					gencodeI("lw",tempreg1,"$sp",0);	//left address
					gencodeI("addi","$sp", "$sp", 4);

					cout<<"sw "<<registers.top()<<", 0("<<tempreg1<<") "<<endl;
					if(!is_addr)
						cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
					assembly_comment("/generate_code:assign_astnode");
					return "";
				}
			}
		}
	}

	if(availreg>leftreg && leftreg<rightreg) {

		registers.swap();
		if(isStructType(right->data_type)) {
			right->generate_code(st, true);
			string tempreg1 = registers.top();
			registers.pop();

			left->generate_code(st,true);
			string tempreg2 = registers.top();

			int struct_size = gst.search(right->data_type.data)->size;
			//tempreg1 holds the address of source
			//tempreg2 hold the address of designation

			cout<<"li $5,"<<struct_size<<endl;
			string L1 = newlabel();
			string L2 = newlabel();
			cout<<"j "<<L1<<endl;

			cout<<L2<<": "<<endl;
			cout<<"lw $7, 0("<<tempreg1<<")"<<endl;
			cout<<"sw $7, 0("<<tempreg2<<")"<<endl;
			cout<<"addi "<<tempreg1<<", "<<tempreg1<<",4"<<endl;
			cout<<"addi "<<tempreg2<<", "<<tempreg2<<", 4"<<endl;
			cout<<"addi $5,$5,-4"<<endl;

			cout<<L1<<" :"<<endl;
			cout<<"seq $6,$5,$0"<<endl;
			cout<<"beqz $6,"<<L2<<endl;

			cout<<"li $5, "<<struct_size<<endl;
			cout<<"sub "<<tempreg2<<", "<<tempreg2<<", $5"<<endl;
			registers.push(tempreg1);
			registers.swap();
		}
		else {

			right->generate_code(st,false);
			string tempreg2 = registers.top();
			registers.pop();
			left->generate_code(st,true);
			string tempreg1 = registers.top();

			cout<<"sw "<<tempreg2<<", 0("<<tempreg1<<") "<<endl;
			if(!is_addr)
				cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
			registers.push(tempreg2);
			registers.swap();
		}
		assembly_comment("/generate_code:assign_astnode");
		return "";

	}
	else if (availreg>rightreg && leftreg>=rightreg) {
		left->generate_code(st,true);
		string tempreg1 = registers.top();
		registers.pop();
		if(isStructType(right->data_type)) {
			right->generate_code(st, true);
			int struct_size = gst.search(right->data_type.data)->size;
			//tempreg1 holds the address of destination
			//registers.top() hold the address of source

			cout<<"li $5,"<<struct_size<<endl;
			string L1 = newlabel();
			string L2 = newlabel();
			cout<<"j "<<L1<<endl;

			cout<<L2<<": "<<endl;
			cout<<"lw $7, 0("<<registers.top()<<")"<<endl;
			cout<<"sw $7, 0("<<tempreg1<<")"<<endl;
			cout<<"addi "<<registers.top()<<", "<<registers.top()<<",4"<<endl;
			cout<<"addi "<<tempreg1<<", "<<tempreg1<<", 4"<<endl;
			cout<<"addi $5,$5,-4"<<endl;

			cout<<L1<<" :"<<endl;
			cout<<"seq $6,$5,$0"<<endl;
			cout<<"beqz $6,"<<L2<<endl;

			cout<<"li $5, "<<struct_size<<endl;
			cout<<"sub "<<tempreg1<<", "<<tempreg1<<", $5"<<endl;
		}
		else {
			right->generate_code(st,false);
			cout<<"sw "<<registers.top()<<", 0("<<tempreg1<<") "<<endl;
			if(!is_addr)
				cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
		}
		registers.push(tempreg1);
		assembly_comment("/generate_code:assign_astnode");
		return "";
	}
	else {

		if(isStructType(right->data_type)) {
			right->generate_code(st, true);
			string tempreg1 = "$4";

			gencodeI("addi","$sp", "$sp", -4);
			gencodeI("sw",registers.top(),"$sp",0);

			left->generate_code(st,true);
			string tempreg2 = registers.top();

			gencodeI("lw",tempreg1,"$sp",0);	//right address
			gencodeI("addi","$sp", "$sp", 4);

			int struct_size = gst.search(right->data_type.data)->size;
			//tempreg1 holds the address of destination
			//registers.top() hold the address of source

			cout<<"li $5,"<<struct_size<<endl;
			string L1 = newlabel();
			string L2 = newlabel();
			cout<<"j "<<L1<<endl;

			cout<<L2<<": "<<endl;
			cout<<"lw $7, 0("<<tempreg1<<")"<<endl;
			cout<<"sw $7, 0("<<tempreg2<<")"<<endl;
			cout<<"addi "<<tempreg1<<", "<<tempreg1<<",4"<<endl;
			cout<<"addi "<<tempreg2<<", "<<tempreg2<<", 4"<<endl;
			cout<<"addi $5,$5,-4"<<endl;

			cout<<L1<<" :"<<endl;
			cout<<"seq $6,$5,$0"<<endl;
			cout<<"beqz $6,"<<L2<<endl;

			cout<<"li $5, "<<struct_size<<endl;
			cout<<"sub "<<tempreg2<<", "<<tempreg2<<", $5"<<endl;
		}
		else {

			right->generate_code(st,false);
			string tempreg2 = "$4";
			gencodeI("addi","$sp", "$sp", -4);
			gencodeI("sw",registers.top(),"$sp",0);

			left->generate_code(st,true);
			string tempreg1 = registers.top();

			gencodeI("lw",tempreg2,"$sp",0);	//right address
			gencodeI("addi","$sp", "$sp", 4);

			cout<<"sw "<<tempreg2<<", 0("<<tempreg1<<") "<<endl;
			if(!is_addr)
				cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
		}
		assembly_comment("/generate_code:assign_astnode");
		return "";
	}
}

void assign_astnode::register_req(bool ileft) {
	left->register_req(true);
	right->register_req(true);
	int leftreg=left->regs;
	int rightreg=right->regs;
	int leftf = left->fregs;
	int rightf = right->fregs;
	is_modified=true;

	if(isFloatType(left->data_type)||isFloatType(right->data_type)) {
		fregs=max(leftf,rightf);
		regs=max(leftreg,1+rightreg);
	}
	else {
		if(is_modified) {
			regs=max(leftreg,rightreg+1);
		}
		else {
			if(leftreg==rightreg)
				regs=leftreg+1;
			else
				regs=max(leftreg, rightreg);
		}
}
}

void assign_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	right->set_funcname(fname);
}

///////////////////////////////////

funcall_astnode::funcall_astnode():exp_astnode()
{
	id="Funcall";
	astnode_type=FunCallNode;
}

funcall_astnode::funcall_astnode(identifier_astnode* child)
{
	funcname = child;
	astnode_type=FunCallNode;
}

void funcall_astnode::setname(std::string name)
{
	funcname = new identifier_astnode(name);
}

void funcall_astnode::pushback(exp_astnode* subtree)
{
	children.push_back(subtree);
}

void funcall_astnode::print(int ntabs)
{

	for(int i=0; i<ntabs; i++) cout<<"\t";
	cout<<typecast<<"(Func ";
	funcname->print(0); cout<<" ";
	// std::cout<<"(Block [";
	for(int i=0;i<children.size();++i)
	{
		children[i]->print(0);
	}
	std::cout<<") ";
}

std::string funcall_astnode::generate_code(const SymbTab& st, bool is_addr) {

	if(funcname->id == "printf") {
		assembly_comment("generate_code:printf");

		for(int i=0; i<children.size(); i++) {
			children[i]->generate_code(st,false);

			if(isFloatType(children[i]->data_type)) {
				cout<<"li $v0, 2"<<endl;
				cout<<"mov.s $f12, "<<fregisters.top()<<endl;
				cout<<"syscall"<<endl;
			}
			else if(isIntType(children[i]->data_type)) {
				cout<<"li $v0, 1"<<endl;
	    			cout<<"move $a0, "<<registers.top()<<endl;
			    	cout<<"syscall"<<endl;
			}
			else {
			    	cout<<"li $v0, 4"<<endl;
	    			cout<<"move $a0, "<<registers.top()<<endl;
			    	cout<<"syscall"<<endl;
			}

		}
		assembly_comment("/generate_code:printf");

		return "";

	}
	//cerr<<"gen code for func"<<endl;
	// gencodeI("li","$4","ERROR-FUNCALL",gst.search(f_name)->paramSize());
	// gencodeR("sub","$sp","$sp","$4");
	//Go left to right
	assembly_comment("generate_code:funcall_astnode");
	gencodeI("li","$4","ERROR",gst.search(funcname->id)->type.size());
	gencodeR("sub","$sp","$sp","$4");		// make space on stack
	for(int i=0; i<children.size(); i++) {

		if(isStructType(children[i]->data_type)) {
			children[i]->generate_code(st,true);
			//registers.top() holds the pointer to struct source
			gencodeI("li","$4","ERROR",gst.search(children[i]->data_type.data)->size);
			gencodeR("sub","$sp","$sp","$4");		// make space on stack
			copyContent(STRUCT_TYPE,children[i]->data_type, registers.top(),"$sp");
			//push on stack byte by byte

		}
		else {
			if(children[i]->data_type.is_array)
				children[i]->generate_code(st,true);
			else				
				children[i]->generate_code(st,false);
			gencodeI("addi","$sp","$sp",-4);
			if(isFloatType(children[i]->data_type)){					//IMPORTANT
				if(!isFloatType(gst.search(funcname->id)->type)){
					cout<<"cvt.w.s "<<fregisters.top()<<", "<<fregisters.top()<<endl;
				}
				cout<<"s.s "<<fregisters.top()<<", 0($sp)"<<endl;
			} else {
				//child is int
				if(isFloatType(gst.search(funcname->id)->type)){
					//typecast int to float
					cout<<"mtc1 "<<registers.top()<<", $f28"<<endl;
					cout<<"cvt.s.w $f28, $f28"<<endl;
					cout<<"s.s $f28, 0($sp)"<<endl;
				}else {
					cout<<"sw "<<registers.top()<<", 0($sp)"<<endl;
				}
				// cout<<"sw "<<registers.top()<<", 0($sp)"<<endl;
			}
		}
	}
	cout<<"jal "<<funcname->id<<endl;
	//pop space for return calue and arguments
	gencodeI("li","$4","ERROR-FUNCALL",gst.search(funcname->id)->symbtab->paramSize());
	gencodeR("add",registers.top(),"$sp","$4");
	if(!is_addr  && !isStructType(gst.search(funcname->id)->type)){
		if(isFloatType(gst.search(funcname->id)->type)){					//IMPORTANT
			cout<<"l.s "<<fregisters.top()<<", 0("<<registers.top()<<")"<<endl;
		}else {
			cout<<"lw "<<registers.top()<<", 0("<<registers.top()<<")"<<endl;
		}
	}
	gencodeI("li","$4","ERROR-FUNCALL",gst.search(funcname->id)->symbtab->paramSize()+gst.search(funcname->id)->type.size());
	gencodeR("add","$sp","$sp","$4");
	//setup $sp

	assembly_comment("/generate_code:funcall_astnode");
	return "";

}

void funcall_astnode::register_req(bool left) {
	//Will be filled later
	regs=1;
	fregs=1;
	is_modified=true;
	for(int i=0; i<children.size(); i++) {
		children[i]->register_req(true);
		regs=max(regs,children[i]->regs);
		fregs=max(fregs,children[i]->fregs);
	}

}

void funcall_astnode::set_funcname(string fname){
	f_name=fname;
	for(int i=0;i<children.size();++i)
	{
		children[i]->set_funcname(fname);
	}
	is_modified=true;
}

/////////////////////////////////////

intconst_astnode::intconst_astnode(int val):exp_astnode()
{
	value = val;
	astnode_type=IntConstNode;
}

void intconst_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(IntConst "<<value<<") ";

}

std::string intconst_astnode::generate_code(const SymbTab& st, bool is_addr) {
	assembly_comment("generate_code:intconst_astnode");
	cout<<"li "<<registers.top()<<", "<<value<<endl;
	assembly_comment("/generate_code:intconst_astnode");
	return "";
}

void intconst_astnode::register_req(bool left) {
	is_modified=false;
	regs=1;
	fregs=0;
}

void intconst_astnode::set_funcname(string fname){
	f_name=fname;
}
/////////////////////////////////////

floatconst_astnode::floatconst_astnode(float val):exp_astnode()
{
	value = val;
	astnode_type=FloatConstNode;
}

void floatconst_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(FloatConst "<<value<<") ";
}

std::string floatconst_astnode::generate_code(const SymbTab& st, bool is_addr) {
	assembly_comment("generate_code:floatconst_astnode");
	cout<<"li.s "<<fregisters.top()<<", ";printf("%f\n",value);
	// genInstr("li.s",fregisters.top(),"Error, li.s genInstr","Error: li.s genInstr",value);
	// cout<<"li $4, "<<value<<endl;
	// cout<<"mtc1 $4, "<<fregisters.top()<<endl;
	// cout<<"cvt.s.w "<<fregisters.top()<<", "<<fregisters.top()<<endl;
	assembly_comment("/generate_code:floatconst_astnode");
	return "";
}

void floatconst_astnode::register_req(bool left) {
	//Will come back later :P
	regs=0;
	fregs=1;
}

void floatconst_astnode::set_funcname(string fname){
	f_name=fname;
}

///////////////////////////////////
stringconst_astnode::stringconst_astnode(string val):exp_astnode()
{
	value = val;
	astnode_type=StringConstNode;
}

void stringconst_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(StringConst "<<value<<") ";

}

std::string stringconst_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for string"<<endl;
	cout<<"la "<<registers.top()<<", "<<datalabel<<endl;
	return "";

}

void stringconst_astnode::register_req(bool left) {
	datalabel = newdatalabel();
	cout<<datalabel<<": .asciiz "<<value<<endl;
	regs=1;
	fregs=0;
}

void stringconst_astnode::set_funcname(string fname){
	f_name=fname;
}

// /////////////////////////////////

ref_astnode::ref_astnode():exp_astnode()
{
}

/////////////////////////////////

identifier_astnode::identifier_astnode(std::string val):ref_astnode()
{
	id = val;
	astnode_type=IdentifierNode;
}

void identifier_astnode::print(int ntabs)
{
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(Id \""<<id<<"\") ";
}

std::string identifier_astnode::generate_code(const SymbTab& st, bool is_addr) {
	assembly_comment("generate_code:identifier_astnode");
	SymbTab localcpy= st;
	SymbTabEntry* entry = localcpy.search(id);
	int off = entry->offset;
	if(entry->type.is_array) {
		if(entry->scope=="local") {
			gencodeI("li",registers.top(),"error",off);
			gencodeR("add",registers.top(),registers.top(),"$fp");
			if(!is_addr)
				cout<<"lw "<<registers.top()<<", "<<"0("<<registers.top()<<")"<<endl;
		}
		else {
			gencodeI("li",registers.top(),"error",off);
			gencodeR("add",registers.top(),registers.top(),"$fp");
			cout<<"lw "<<registers.top()<<", "<<"0("<<registers.top()<<")"<<endl;			
			if(!is_addr)
				cout<<"lw "<<registers.top()<<", "<<"0("<<registers.top()<<")"<<endl;
		}
		return "";
	}
	if(isFloatType(entry->type)){
		gencodeI("li","$4","error",off);
		gencodeR("add","$4","$4","$fp");
		cout<<"move "<<registers.top()<<", $4"<<endl;
		if(!is_addr)
			cout<<"l.s "<<fregisters.top()<<", "<<"0("<<registers.top()<<")"<<endl;			//IMPORTANT
	} else {
		gencodeI("li",registers.top(),"error",off);
		gencodeR("add",registers.top(),registers.top(),"$fp");
		if(!is_addr)
			cout<<"lw "<<registers.top()<<", "<<"0("<<registers.top()<<")"<<endl;
	}

	assembly_comment("/generate_code:identifier_astnode");
	return "";
}

void identifier_astnode::register_req(bool left) {
	if(isFloatType(data_type)) {
		fregs=1;
		regs=0;
	}
	else {
		fregs=0;
		regs=1;
	}
}

void identifier_astnode::set_funcname(string fname){
	f_name=fname;
}

////////////////////////////////

arrayref_astnode::arrayref_astnode(exp_astnode* l, exp_astnode* r):ref_astnode()			// again, changed from ref to exp
{
	left = l;
	right = r;
	id= "ArrayRef";
	astnode_type=ArrayRefNode;
}

void arrayref_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(ArrayRef ";left->print(0);std::cout<<" ";
	right->print(0);cout<<") ";
}

std::string arrayref_astnode::generate_code(const SymbTab& st, bool is_addr) {
	assembly_comment("generate_code:arrayref_astnode");
	// int availreg = registers.size();
	int availreg = AVAILABLE_REGS;
	int leftreg = left->regs;
	int rightreg = right->regs;

	if(is_modified) {
		//left ten right
		left->generate_code(st, true);
		string tempreg1= registers.top();
		int sz = left->data_type.elem();

		if(rightreg<availreg) {
			registers.pop();
			string tempreg2 = "$7";
			right->generate_code(st,false);
			cout<<"li "<<tempreg2<<", "<<sz<<endl;
			cout<<"mul "<<registers.top()<<", "<<registers.top()<<", "<<tempreg2<<endl;
			cout<<"add "<<tempreg1<<", "<<tempreg1<<", "<<registers.top()<<endl;
			registers.push(tempreg1);
			if(!is_addr)
			{
				if(isFloatType(data_type)){
					cout<<"l.s "<<fregisters.top()<<", 0"<<registers.top()<<")"<<endl;		//IMPORTANT
				} else {
					cout<<"lw "<<registers.top()<<", 0("<<registers.top()<<")"<<endl;
				}
			}
			assembly_comment("/generate_code:arrayref_astnode");
			return "";
		}
		else {

			gencodeI("addi","$sp", "$sp", -4);
			gencodeI("sw",tempreg1,"$sp",0);
			tempreg1="$4";

			string tempreg2 = "$7";
			right->generate_code(st,false);
			cout<<"li "<<tempreg2<<", "<<sz<<endl;

			gencodeI("lw",tempreg1,"$sp",0);	//right address
			gencodeI("addi","$sp", "$sp", 4);

			cout<<"mul "<<registers.top()<<", "<<registers.top()<<", "<<tempreg2<<endl;
			cout<<"add "<<tempreg1<<", "<<tempreg1<<", "<<registers.top()<<endl;
			registers.push(tempreg1);
			if(!is_addr)
			{
				if(isFloatType(data_type)){								//IMPORTANT
					cout<<"l.s "<<fregisters.top()<<", 0"<<registers.top()<<")"<<endl;
				} else {
					cout<<"lw "<<registers.top()<<", 0("<<registers.top()<<")"<<endl;
				}
			}
			assembly_comment("/generate_code:arrayref_astnode");
			return "";
		}

	}

	if(availreg>leftreg && leftreg<rightreg) {
		registers.swap();
		string tempreg1= registers.top();
		right->generate_code(st,false);
		registers.pop();
		left->generate_code(st, true);
		int sz = left->data_type.elem();
		string tempreg2 = "$7";
		cout<<"li "<<tempreg2<<", "<<sz<<endl;
		cout<<"mul "<<tempreg1<<", "<<tempreg1<<", "<<tempreg2<<endl;
		cout<<"add "<<registers.top()<<", "<<registers.top()<<", "<<tempreg1<<endl;
		registers.push(tempreg1);
		registers.swap();
		if(!is_addr) {
			if(isFloatType(data_type)){
				cout<<"l.s "<<fregisters.top()<<", 0"<<registers.top()<<")"<<endl;
			} else {
				cout<<"lw "<<registers.top()<<", 0("<<registers.top()<<")"<<endl;
			}
		}
		assembly_comment("/generate_code:arrayref_astnode");
		return "";
	}
	else if (availreg>rightreg && leftreg>=rightreg) {

		//get the address of l in tempreg1
		left->generate_code(st, true);
		string tempreg1= registers.top();
		int sz = left->data_type.elem();
		registers.pop();
		string tempreg2 = "$7";
		right->generate_code(st,false);
		cout<<"li "<<tempreg2<<", "<<sz<<endl;
		cout<<"mul "<<registers.top()<<", "<<registers.top()<<", "<<tempreg2<<endl;
		cout<<"add "<<tempreg1<<", "<<tempreg1<<", "<<registers.top()<<endl;
		registers.push(tempreg1);
		if(!is_addr)
		{
			if(isFloatType(data_type)){
				cout<<"l.s "<<fregisters.top()<<", 0"<<registers.top()<<")"<<endl;
			} else {
				cout<<"lw "<<registers.top()<<", 0("<<registers.top()<<")"<<endl;
			}
		}
		assembly_comment("/generate_code:arrayref_astnode");
		return "";
	}
	else {

		right->generate_code(st,false);
		gencodeI("addi","$sp", "$sp", -4);
		gencodeI("sw",registers.top(),"$sp",0);
		left->generate_code(st, true);
		int sz = left->data_type.elem();
		string tempreg1 = registers.top();	//left adddress
		string tempreg2 = "$7";
		string tempreg3 = "$4";
		gencodeI("lw",tempreg3,"$sp",0);	//right address
		gencodeI("addi","$sp", "$sp", 4);
		cout<<"li "<<tempreg2<<", "<<sz<<endl;
		cout<<"mul "<<tempreg3<<", "<<tempreg3<<", "<<tempreg2<<endl;
		cout<<"add "<<tempreg1<<", "<<tempreg1<<", "<<tempreg3<<endl;
		if(!is_addr) {
			if(isFloatType(data_type)){
				cout<<"l.s "<<fregisters.top()<<", 0"<<registers.top()<<")"<<endl;
			} else {
				cout<<"lw "<<registers.top()<<", 0("<<registers.top()<<")"<<endl;
			}
		}
		assembly_comment("/generate_code:arrayref_astnode");
		return "";
	}
}

void arrayref_astnode::register_req(bool ileft) {

	left->register_req(true);
	right->register_req(true);
	int leftreg=left->regs;
	int rightreg=right->regs;
	int leftf = left->fregs;
	int rightf = right->fregs;
	is_modified=left->is_modified||right->is_modified;
	if(is_modified) {
		regs=max(leftreg,rightreg+1);
		fregs=max(leftf,rightf);
	}
	else {
		if(leftreg==rightreg) {
			regs=leftreg+1;
		}
		else {
			regs=max(leftreg, rightreg);
		}
		fregs=max(leftf,rightf);
	}
	if(isFloatType(data_type)) {
		fregs=max(1,fregs);
	}
	else
		regs=max(1,regs);

}

void arrayref_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	right->set_funcname(fname);
}

///////////////////////////////

pointer_astnode::pointer_astnode(ref_astnode* c):exp_astnode()
{
	child = c;
	id = "Pointer";
	astnode_type=PointerNode;
}

void pointer_astnode::print(int ntabs)
{
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(Pointer ";child->print(0);std::cout<<") ";
}

std::string pointer_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//cerr<<"gen code for poiner"<<endl;
	return "";

}

void pointer_astnode::register_req(bool left) {
	// Not used anymore in the grammar
}

void pointer_astnode::set_funcname(string fname){
	f_name=fname;
	child->set_funcname(fname);
}

////////////////////////////////

deref_astnode::deref_astnode(ref_astnode* c):ref_astnode()
{
	child =c;
	id = "Deref";
	astnode_type=DerefNode;
}

void deref_astnode::print(int ntabs)
{
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(Deref ";child->print(0);std::cout<<") ";
}

std::string deref_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//Not Used anymore
	return "";
}

void deref_astnode::register_req(bool left) {

}

void deref_astnode::set_funcname(string fname){
	f_name=fname;
	child->set_funcname(fname);
}

/////////////////////////////////

member_astnode::member_astnode(exp_astnode* l, identifier_astnode* r)		// change from ref to exp(1st arg)
{
	left = l;
	right = r;
	astnode_type=MemberNode;
}

void member_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(Member ";left->print(0);std::cout<<" ";right->print(0);std::cout<<") ";
}

std::string member_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//l.r
	//gnerate code for l;
	assembly_comment("generate_code:member_astnode");
	left->generate_code(st,true);
	//Get address of l in register
	string tempreg1 = registers.top();
	// registers.pop();
	string tempreg2="$4";
	SymbTab* newst=gst.search(left->data_type.data)->symbtab;
	// string tempreg2 = registers.top();
	cout<<"li "<<tempreg2<<", "<<newst->search(right->id)->offset<<endl;
	cout<<"add "<<tempreg1<<", "<<tempreg2<<", "<<tempreg1<<endl;
	if(!is_addr) {
		if(isFloatType(newst->search(right->id)->type)){							//IMPORTANT
			cout<<"l.s "<<fregisters.top()<<" ,0("<<tempreg1<<")"<<endl;
		} else {
			cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
		}
	}
	// registers.push(tempreg1);
	assembly_comment("/generate_code:member_astnode");
	return "";

}

void member_astnode::register_req(bool ileft) {
	left->register_req(true);
	right->register_req(true);
	int leftreg=left->regs;
	int rightreg=right->regs;
	int leftf=left->fregs;
	int rightf=right->fregs;
	is_modified=left->is_modified||right->is_modified;
	if(is_modified) {
		regs=max(leftreg,rightreg+1);
	}
	else {
		if(leftreg==rightreg)
			regs=leftreg+1;
		else
			regs=max(leftreg, rightreg);
	}
	fregs = max(leftf,rightf);
	if(isFloatType(data_type)) {
		fregs=max(1,fregs);
	}
	else
		regs=max(1,regs);


}

void member_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	right->set_funcname(fname);
}

/////////////////////////////////

arrow_astnode::arrow_astnode(exp_astnode* l, identifier_astnode* r)
{
	left = l;
	right = r
;	astnode_type=ArrowNode;
}

void arrow_astnode::print(int ntabs)
{
	for(int i=0; i<ntabs; i++) cout<<"\t";
	std::cout<<"Regs : "<<regs<<" ";
	std::cout<<typecast<<"(Arrow ";left->print(0);std::cout<<" ";right->print(0);std::cout<<") ";
}

std::string arrow_astnode::generate_code(const SymbTab& st, bool is_addr) {
	//l->r
	//generate code for l;
	assembly_comment("generate_code:arrow_astnode");
	left->generate_code(st,false);
	//Get address pointed to by l (value) in register
	string tempreg1 = registers.top();
	// registers.pop();
	SymbTab* newst=gst.search(left->data_type.data)->symbtab;
	// string tempreg2 = registers.top();
	string tempreg2 = "$4";
	cout<<"li "<<tempreg2<<", "<<newst->search(right->id)->offset<<endl;
	cout<<"add "<<tempreg1<<", "<<tempreg2<<", "<<tempreg1<<endl;

	if(!is_addr) {
		if(isFloatType(newst->search(right->id)->type)){			//IMPORTANT
			cout<<"l.s "<<fregisters.top()<<" ,0("<<tempreg1<<")"<<endl;
		} else {
			cout<<"lw "<<tempreg1<<", 0("<<tempreg1<<")"<<endl;
		}
	}
	// registers.push(tempreg1);
	assembly_comment("/generate_code:arrow_astnode");
	return "";

}

void arrow_astnode::register_req(bool ileft) {
	left->register_req(true);
	right->register_req(true);
	int leftreg=left->regs;
	int rightreg=right->regs;
	int leftf=left->fregs;
	int rightf=right->fregs;
	is_modified=left->is_modified||right->is_modified;
	if(is_modified) {
		cout<<"ERROR in Arrow"<<endl;
		regs=max(leftreg,rightreg+1);
	}
	else {
		if(leftreg==rightreg)
			regs=leftreg+1;
		else
			regs=max(leftreg, rightreg);
	}
	fregs=max(leftf,rightf);
	if(isFloatType(data_type)) {
		fregs=max(1,fregs);
	}
	else
		regs=max(1,regs);

}

void arrow_astnode::set_funcname(string fname){
	f_name=fname;
	left->set_funcname(fname);
	right->set_funcname(fname);
}

/////////////////////////////////
