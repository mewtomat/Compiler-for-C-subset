#include <iostream>
#include <string>
#include "Scanner.h"
#include "Parser.h"
#include <unistd.h>
#include <strings.h>
#include "symbtab.h"
#include "helper.h"

// #define SHOW_AST
// #define SHOW_SYMBTAB

using namespace std;

extern SymbTab gst;
string filename;
std::vector<abstract_astnode*> ast;

int main (int argc, char** argv)
{
	freopen (argv[1],"r",stdin);
	filename = argv[1];
	Parser parser;
	parser.parse();
	cout<<".data"<<endl;
	for(int i=0; i<ast.size(); i++) {
		ast[i]->register_req(true);
	}
	#ifdef SHOW_AST
	cout<<"=====================AST====================="<<endl;
	for(int i=0; i<ast.size(); i++) {
		ast[i]->print(0);
		cout<<endl;
	}
	cout<<endl;
	#endif
	#ifdef SHOW_SYMBTAB
	cout<<"===================SYMBOL TABLE=================="<<endl;
	cout<<endl;
	gst.printgst();
	cout<<endl;
	#endif
	cout<<".text"<<endl;
	cout<<".globl main"<<endl<<".ent main"<<endl;
	cout<<"mtc1 $0,$f0"<<endl;
	for(int i=0; i<ast.size(); i++) {
		cout<<endl;
		SymbTab* entry = (gst.search(ast[i]->f_name))->symbtab;
		cout<<ast[i]->f_name<<":"<<endl;
		assembly_comment("calle work = setting up space for fp, locals,other registers");
		cout<<"addi $sp, $sp, -4"<<endl;
		cout<<"sw $fp, 0($sp)"<<endl;		//$fp
		cout<<"move $fp, $sp"<<endl;
		cout<<"li $7, "<<entry->localSize()+4*19+26*4<<endl;
		cout<<"sub $sp, $sp, $7"<<endl;		//space for locals and callee saved regs
		cout<<"sw $ra, "<<18*4+ 26*4<<"($sp)"<<endl;
		for(int j=0;j<8;++j) {
			cout<<"sw $s"<<j<<", "<<18*4+26*4-4-j*4<<"($sp)"<<endl;
		}
		for(int j=0;j<10;++j){
			cout<<"sw $t"<<j<<", "<<18*4+26*4-4-28-4-4*j<<"($sp)"<<endl;
		}
		int mycount=0;
		for(int j=1; j<28; j++) {
			if(j==12)
				continue;
			cout<<"s.s $f"<<j<<", "<<26*4-4-4*mycount<<"($sp)"<<endl;
			mycount++;
		}
		assembly_comment("done setting up space for fp, locals,other registers");
		ast[i]->generate_code(*entry,true);
		cout<<"EXIT"<<ast[i]->f_name<<":"<<endl;
		assembly_comment("callee clearing up stack");

		for(int j=27; j>=1; j--) {
			if(j==12)
				continue;
			mycount--;
			cout<<"l.s $f"<<j<<", "<<26*4-4-4*mycount<<"($sp)"<<endl;
		}
		
		for(int j=9;j>=0;--j){
			cout<<"lw $t"<<j<<", "<<18*4+26*4-4-28-4-4*j<<"($sp)"<<endl;
		}
		for(int j=7;j>=0;--j) {
			cout<<"lw $s"<<j<<", "<<26*4+18*4-4-j*4<<"($sp)"<<endl;
		}
		cout<<"lw $ra, "<<18*4+26*4<<"($sp)"<<endl;
		cout<<"li $7, "<<entry->localSize()+4*19+26*4<<endl;
		cout<<"add $sp, $sp, $7"<<endl;
		cout<<"lw $fp, 0($sp)"<<endl;
		cout<<"addi $sp, $sp , 4"<<endl;
		cout<<"j $ra"<<endl;
		assembly_comment("callee's clearing done");
		cout<<".end "<<ast[i]->f_name<<endl;
		cout<<endl;
		cout<<endl;
	}
}


/*
	(BY CALLER)
	CALLER SAVED REGISTERS					--not to be done
	RETURN VALUE
	PARAMETERS
	----------------------------------
	(BY CALLEE)
	$FP
				<-------curr fp
	LOCALS
	CALLEE-SAVED
		$RA
		$S0-$S7

*/
