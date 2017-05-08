#define CHOOSE(type_,CHOICE1,CHOICE2) (type_==1?CHOICE2:CHOICE1)

void copyContent(int type,datatype dtype, string source , string destination)
{
	//restores the source and destination registers to initial values
	if(type==STRUCT_TYPE){
		string tempreg1 = source;
		//tempreg1 holds the address of source
		string tempreg2 = destination;
		//tempreg2 hold the address of designation

		int struct_size = gst.search(dtype.data)->size;

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
		cout<<"sub "<<tempreg1<<", "<<tempreg1<<", $5"<<endl;
	}
}

std::string toInstr(string typecast,string id)
{
	//ditching typecast for now
	if(id=="OR_OP")
	{
		return "or";
	}
	if(id=="AND_OP")
	{
		return "and";
	}
	if(id=="EQ_OP-Int")//simlarly for float
						//similarly for NE, LT, ..
	{
		return "seq";
	}
	if(id=="NE_OP-Int")
	{
		return "sne";
	}
	if(id=="Plus-Int")
	{
		return "add";
	}
	if(id=="Minus-Int")
	{
		return "sub";
	}
	if(id=="Mult-Int")
	{
		return "mul";
	}
	if(id=="Div-Int")
	{
		return "div";
	}
	if (id == "LE_OP-Int") {
		return "sle";
	}
	if (id == "LT_OP-Int") {
		return "slt";
	}
	if (id == "GE_OP-Int") {
		return "sge";
	}
	if (id == "GT_OP-Int") {
		return "sgt";
	}
	if(id=="EQ_OP-Float")
	{
		return "c.eq.s";
	}
	if(id=="NE_OP-Float")
	{
		return "c.ne.s";
	}
	if(id=="Plus-Float")
	{
		return "add.s";
	}
	if(id=="Minus-Float")
	{
		return "sub.s";
	}
	if(id=="Mult-Float")
	{
		return "mul.s";
	}
	if(id=="Div-Float")
	{
		return "div.s";
	}
	if (id == "LE_OP-Float") {
		return "c.le.s";
	}
	if (id == "LT_OP-Float") {
		return "c.lt.s";
	}
	if (id == "GE_OP-Float") {
		return "c.ge.s";
	}
	if (id == "GT_OP-Float") {
		return "c.gt.s";
	}
	return "ERROR, BINARY OPERATOR INSTR NOT DEFINED";
}

std::string newlabel() {
	labelcount++;
	stringstream ss;
	ss<<labelcount;
	return "L" + ss.str();
}


std::string newdatalabel() {
	datalabelcount++;
	stringstream ss;
	ss<<datalabelcount;
	return "S" + ss.str();
}

void gencodeR(string instr, string regdest, string regsrc1, string regsrc2) {
	cout<<instr<<" "<<regdest<<", "<<regsrc1<<", "<<regsrc2<<endl;
}

void gencodeI(string instr, string regdest, string regsrc1, int imm) {
	if(instr=="lw" || instr=="sw") {
		cout<<instr<<" "<<regdest<<", "<<imm<<"("<<regsrc1<<") "<<endl;
	}
	else if (instr=="li") {
		cout<<instr<<" "<<regdest<<", "<<imm<<endl;
	}
	else {
		cout<<instr<<" "<<regdest<<", "<<regsrc1<<", "<<imm<<endl;
	}
}

void gencodeJ(string instr, string label) {
	cout<<instr<<" "<<label<<endl;
}

void gencodeL(string label)
{
	cout<<label<<":"<<endl;
}

void assembly_comment(string mess)
{
	cout<<"		#"<<mess<<endl;
}

void gencodeZ(string instr, string reg, string label) {
	cout<<instr<<" "<<reg<<", "<<label<<endl;
}


bool isBoolean(string id)
{
	if(id=="EQ_OP-Float" || id=="NE_OP-Float" || id=="LT_OP-Float" || id=="LE_OP-Float" || id=="GE_OP-Float" || id=="GT_OP-Float")
		return true;
	else
		return false;
}

void genInstr(string instruction, string regdest, string reg1, string reg2, int imm)
{
	if(instruction=="EQ_OP-Float"){
		cout<<"c.eq.s "<<reg1<<", "<<reg2<<endl;
		//regdest here is int register
		string l1=newlabel();
		string l2=newlabel();
		cout<<"bc1f "<<l1<<endl;
		cout<<"li "<<regdest<<", 1"<<endl;
		gencodeJ("j", l2);
		gencodeL(l1);
		cout<<"li "<<regdest<<" ,0"<<endl;
		gencodeL(l2);
		return;
	}else if (instruction == "NE_OP-Float"){
		cout<<"c.ne.s "<<reg1<<", "<<reg2<<endl;
		//regdest here is int register
		string l1=newlabel();
		string l2=newlabel();
		cout<<"bc1f "<<l1<<endl;
		cout<<"li "<<regdest<<", 1"<<endl;
		gencodeJ("j", l2);
		gencodeL(l1);
		cout<<"li "<<regdest<<" ,0"<<endl;
		gencodeL(l2);
		return;
	}else if (instruction=="Plus-Float"){
		// assume both the values are in float registers
		cout<<"add.s "<<regdest<<", "<<reg1<<", "<<reg2<<endl;
	}else if (instruction=="Minus-Float"){
		cout<<"sub.s "<<regdest<<", "<<reg1<<", "<<reg2<<endl;
	}else if (instruction=="Mult-Float"){
		cout<<"mul.s "<<regdest<<", "<<reg1<<", "<<reg2<<endl;
	}else if (instruction=="Div-Float"){
		cout<<"div.s "<<regdest<<", "<<reg1<<", "<<reg2<<endl;
	}else if (instruction=="LE_OP-Float"){
		cout<<"c.le.s "<<reg1<<", "<<reg2<<endl;
		//regdest here is int register
		string l1=newlabel();
		string l2=newlabel();
		cout<<"bc1f "<<l1<<endl;
		cout<<"li "<<regdest<<", 1"<<endl;
		gencodeJ("j", l2);
		gencodeL(l1);
		cout<<"li "<<regdest<<" ,0"<<endl;
		gencodeL(l2);
		return;
	}else if (instruction=="LT_OP-Float"){
		cout<<"c.lt.s "<<reg1<<", "<<reg2<<endl;
		//regdest here is int register
		string l1=newlabel();
		string l2=newlabel();
		cout<<"bc1f "<<l1<<endl;
		cout<<"li "<<regdest<<", 1"<<endl;
		gencodeJ("j", l2);
		gencodeL(l1);
		cout<<"li "<<regdest<<" ,0"<<endl;
		gencodeL(l2);
		return;
	}else if (instruction=="GE_OP-Float"){
		cout<<"c.le.s "<<reg2<<", "<<reg1<<endl;
		//regdest here is int register
		string l1=newlabel();
		string l2=newlabel();
		cout<<"bc1f "<<l1<<endl;
		cout<<"li "<<regdest<<", 1"<<endl;
		gencodeJ("j", l2);
		gencodeL(l1);
		cout<<"li "<<regdest<<" ,0"<<endl;
		gencodeL(l2);
		return;
	}else if (instruction=="GT_OP-Float"){
		cout<<"c.lt.s "<<reg2<<", "<<reg1<<endl;
		//regdest here is int register
		string l1=newlabel();
		string l2=newlabel();
		cout<<"bc1f "<<l1<<endl;
		cout<<"li "<<regdest<<", 1"<<endl;
		gencodeJ("j", l2);
		gencodeL(l1);
		cout<<"li "<<regdest<<" ,0"<<endl;
		gencodeL(l2);
		return;
	}
	else {
		cout<<"Error in genInstr"<<endl;
	}
}
