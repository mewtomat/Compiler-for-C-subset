%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP EQ_OP NE_OP GE_OP LE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN STRUCT IDENTIFIER INC_OP PTR_OP 
%polymorphic absnode: abstract_astnode*; seqnode: seq_astnode*; funcallnode: funcall_astnode*; unaryopnode: op_unary_astnode*; 
			string: std::string; stmtnode: statement_astnode*; expnode: exp_astnode*; refnode: ref_astnode*; 
			intconst: intconst_astnode*; floatconst: floatconst_astnode*; stringconst: stringconst_astnode*; 
			fundecl: fun_declarator_class*; typespec: type_specifier_class*; paramlist: parameter_list_class*;
			paramdecl: parameter_declaration_class*; declrtr: declarator_class*; declrtn: declaration_class*;
			declrtrlist: declarator_list_class*; decltnlist: declaration_list_class*;

%type <absnode> translation_unit
%type <absnode> compound_statement
//%type <absnode> constant_expression		no longer exists
%type <seqnode> statement_list
%type <stmtnode> statement
%type <stmtnode> assignment_statement
%type <expnode> expression
%type <expnode> logical_or_expression		//new entry
%type <expnode> logical_and_expression
%type <expnode> equality_expression
%type <expnode> relational_expression
%type <expnode> additive_expression
%type <expnode> multiplicative_expression
%type <expnode> unary_expression
%type <expnode> postfix_expression
%type <expnode> primary_expression
//%type <refnode> l_expression 			does no longer exists
%type <funcallnode> expression_list
%type <unaryopnode> unary_operator
%type <stmtnode> selection_statement
%type <stmtnode> iteration_statement
%type <absnode> function_definition
%type <string> STRING_LITERAL
%type <string> FLOAT_CONSTANT
%type <string> INT_CONSTANT
%type <string> IDENTIFIER
%type <fundecl> fun_declarator
%type <typespec> type_specifier
%type <paramlist> parameter_list
%type <paramdecl> parameter_declaration
%type <declrtr> declarator
%type <declrtn> declaration
%type <declrtrlist> declarator_list
%type <decltnlist> declaration_list
%%

translation_unit 
    :  struct_specifier
 	| function_definition{
 		ast.push_back($1);
 	} 
 	| translation_unit function_definition {
 		ast.push_back($2);
 	}
    | translation_unit struct_specifier
    ;

struct_specifier 
        : STRUCT IDENTIFIER {
        	st = new SymbTab();
        	currStruct=$2+"#";
        	if(gst.search($2+"#") !=NULL && gst.search($2+"#")->varfun=="struct" )
			{
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: '"<<$2<<"' has a previous definition"<<endl;
				exit(1);
			}
			insideStruct=1;
        }
        '{' declaration_list '}' ';' {
        	insideStruct=0;
        	currStruct="";
			gst.push($2+"#" , new SymbTabEntry("struct","",datatype(),-($5->offset),0,st));	// push the local symbol table in the global one
        }
        ;

function_definition
	: type_specifier 
	fun_declarator
	{
		st = new SymbTab();			//create new local symbol table
		datatype dt = createtype($1, $2->deref);
		// previous declaration error
		if(gst.search($2->identifier) !=NULL)
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: '"<<$2->identifier<<"' has a previous definition"<<endl;
			exit(1);
		}
		gst.push($2->identifier , new SymbTabEntry("fun","",dt,0,0,st));	// push the local symbol table in the global one
		currFunc = gst.search($2->identifier);
		parameter_declaration_class* param;
		datatype data_type;
		int total_params_size = 0;								// maybe replace the following loop by the total_size_function , if made
		parameter_list_class* pl = $2->param;
		if(pl!=NULL){
			for(int i=0;i<pl->param.size();++i)
			{
				total_params_size += ((pl->param)[i]->t).size();
			}
	
			int cumul = 0;
			for(int i=0;i<pl->param.size();++i)
			{
				param = (pl->param)[i];
				data_type = param->t;

				// struct is not defined 
				if(data_type.type == 3)
				{
					SymbTabEntry* temp = gst.search(data_type.data);
					if(temp ==NULL || temp->varfun != "struct")
					{
						cout<<filename<<": "<<d_scanner.lineNr()<<" Error: struct '"<<data_type.data<<"' is not defined."<<endl;
						exit(1);
					}	
				}
				// void data type declaration error
				if(data_type.type==0 && data_type.deref == 0)
				{
					cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Invalid use of type void"<<endl;
					exit(1);
				}
				// previous declaration error
				if(st->search(param->identifier) !=NULL)
				{
					cout<<filename<<": "<<d_scanner.lineNr()<<" Error: '"<<param->identifier<<"' has a previous declaration"<<endl;
					exit(1);
				}


				st->push(param->identifier,										// add the parameter entry into symbol table
					new SymbTabEntry("var","param",data_type,
										data_type.size(),
										total_params_size-cumul,				//offset
										NULL));	
				cumul += data_type.size();
			}
		}
	}
	compound_statement 
	{
		$$=$4;  //When you remove the action in between remember to change $4 to $3
		// st->print();
		$$->set_funcname($2->identifier);
	}
	;

type_specifier                   // This is the information 
    : VOID 	
    {
    	$$ = new type_specifier_class(VOID_TYPE);
    }                 // that gets associated with each identifier
    | INT {
    	$$ = new type_specifier_class(INT_TYPE);
    }  
	| FLOAT {
		$$ = new type_specifier_class(FLOAT_TYPE);
	}
    | STRUCT IDENTIFIER {
    	$$ = new type_specifier_class(STRUCT_TYPE, $2+"#");
    }
    ;

fun_declarator
	: IDENTIFIER '(' parameter_list ')' {
		$$ = new fun_declarator_class($1,$3);
	}
	| IDENTIFIER '(' ')' {
		$$ = new fun_declarator_class($1);
	}
	| '*' fun_declarator  //The * is associated with the 
	{
		$2->addstar();
		$$=$2;
	}
	;                      //function name



parameter_list
	: parameter_declaration 
	{
		$$ = new parameter_list_class();
		$$->push($1);
	}
	| parameter_list ',' parameter_declaration 
	{
		$1->push($3);
		$$ = $1;
	}
	;

parameter_declaration
	: type_specifier declarator 
	{
		$$ = new parameter_declaration_class($1, $2);
	}
        ;

declarator
	: IDENTIFIER 
	{
		$$= new declarator_class($1);
	}
	| declarator '[' primary_expression']' // check separately that it is a constant
	{
		// check that primary_exp is constant;
		if(!$3->is_calculable )
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Array indices should be integer"<<endl;
			exit(1);
		}
		else if($3->int_val <0)
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: size of array is negative"<<endl;
			exit(1);
    	}
		// if(((intconst_astnode*)$3)->value < 0 )			// decide if you wan't to allow negative indices later on.
		// {
		// 	cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Array indices can not be negative("<<((intconst_astnode*)$3)->value<<")"<<endl;
		// 	exit(1);
		// }
		$1->add_array_index(((intconst_astnode*)$3)->value);							//should it be (intconst)($3) ?
		$$=$1;
	}
    | '*' declarator 
    {
    	$2->addstar();
    	$$=$2;
    }
    ;

primary_expression              // The smallest expressions, need not have a l_value
    : IDENTIFIER {
    	SymbTabEntry* temp = st->search($1);
        if(temp == NULL)
        {
        	cout<<filename<<": "<<d_scanner.lineNr()<<" Error: variable '"<<$1<<"' not declared"<<endl;
			exit(1);
        }
    	$$=new identifier_astnode($1);
        $$->data_type = temp->type;
        $$->lvalue=true;
    }            // primary expression has IDENTIFIER now
    | INT_CONSTANT {
    	int arg1 = std::stoi($1);
    	$$=new intconst_astnode(std::stoi($1));				// the values of these expressions are already stored through the constructor function
    	$$->data_type = createtype(INT_TYPE);
    	$$->lvalue=false;
    	$$->is_calculable = true;
    	$$->int_val =arg1;
    }
    | FLOAT_CONSTANT{
    	$$=new floatconst_astnode(std::stof($1));
    	$$->data_type = createtype(FLOAT_TYPE);
    	$$->lvalue=false;
    }	
    | STRING_LITERAL{
		$$=new stringconst_astnode($1);				// did not set the data type for primary_expression here
		$$->lvalue=false;
		$$->data_type=createtype(-1);
	}
    | '(' expression ')'{
		$$=$2;
	}	 
    ;

compound_statement
	: '{' '}' {
		$$=new empty_astnode();
	}
	| '{' statement_list '}' {
		$$=$2;
	}
    | '{' declaration_list statement_list '}' {
		$$=$3;
	}
	;

statement_list
	: statement{
		$$=new seq_astnode();
		$$->pushback($1);
	}
    | statement_list statement{
		$$=$1;
		$$->pushback($2);
	}		
	;

statement
    :'{' statement_list '}' {
    	$$=$2;
    } 
    | selection_statement {
    	$$=$1;
    }		
    | iteration_statement {
    	$$=$1;
    }		
	| assignment_statement{
    	$$=$1;
    }		
    | RETURN expression ';'	{
    	// check if the return type is correct
    	if(!compatible(currFunc->type,$2->data_type))
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: incompatible types when returning type ‘";
    		$2->data_type.lprint();
    		cout<<"’ but ‘";
    		currFunc->type.print();
    		cout<<"’ was expected"<<endl;
			exit(1);
    	}
    	if($2->data_type.is_array) {
    		if($2->data_type.array[0]==-1) {
    			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: function returns address of local variable "<<endl;
				exit(1);
    		}
    	}
    	// if(currFunc->deref>0 && )

    	if(isIntType($2->data_type) && isFloatType(currFunc->type)) {
    		$2->typecast="to-float";
    	}
    	if(isIntType(currFunc->type) && isFloatType($2->data_type)) {
    		$2->typecast="to-int";
    	}

    	$$= new return_astnode($2);
    }
    ;

assignment_statement
	: ';'{
		$$= new empty_astnode();
	}						
	|  expression ';'{					 
		$$ = $1;
	} 
	;

expression                                   //assignment expressions are right associative
    :  logical_or_expression{
		$$=$1;
	}
//      |  l_expression '=' expression       // This is the most significant change --
    |  unary_expression '=' expression{	// l_expression has been replaced by unary_expression.
    	
    	exp_astnode* arg1=$1;
    	// $1->data_type.print();
    	// cout<<endl;
    	// $3->data_type.print();


    	if(!$1->lvalue) {
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: lvalue required as left operand of assignment"<<endl;
			exit(1);
    	}
    	if(isPointer($1->data_type) && $3->is_calculable && $3->int_val==0) {
    		$3->data_type = createVoidStarType();
    	}

    	if($1->data_type.is_array) {
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: incompatible types when assigning to type ‘";
    		$1->data_type.lprint();
    		cout<<"’ from type ‘";
    		$3->data_type.print();
    		cout<<"’"<<endl;
			exit(1);
    	}
    	if(!compatible($1->data_type,$3->data_type)) {
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: incompatible types when assigning to type ‘";
    		$1->data_type.print();
    		cout<<"’ from type ‘";
    		$3->data_type.print();
    		cout<<"’"<<endl;
			exit(1);
    	}
//typecast
    	if(isIntType($1->data_type) && isFloatType($3->data_type)) {
    		$3->typecast="to-int";
    	}
    	if(isIntType($3->data_type) && isFloatType($1->data_type)) {
    		$3->typecast="to-float";
    	}

    	$$=new assign_astnode($1,$3);
    	$$->data_type=arg1->data_type;
    	$$->lvalue = false;
    }
    ;                                  		  // This may generate programs that are syntactically incorrect.
                                             // Eliminate them during semantic analysis.

logical_or_expression            // The usual hierarchy that starts here...
	: logical_and_expression{
    	$$=$1;
    }
    | logical_or_expression OR_OP logical_and_expression{
		exp_astnode* arg1=$1, *arg3=$3;

    	if(isStructType($1->data_type) || isStructType($3->data_type))
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: used struct type value where scalar is required "<<endl;
			exit(1);
    	}
    	$$=new op_binary_astnode("OR_OP", $1, $3);
    	if(arg3->is_calculable || arg1->is_calculable)
		{
			$$->is_calculable = true;
			$$->int_val = (arg1->int_val) && (arg3->int_val);
		}
    	$$->data_type = createtype(INT_TYPE);
    	$$->lvalue = false;
    }
	;
logical_and_expression
    : equality_expression{
    	$$=$1;
    }
    | logical_and_expression AND_OP equality_expression{
		exp_astnode* arg1=$1, *arg3=$3;
    	if(isStructType($1->data_type) || isStructType($3->data_type))
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: used struct type value where scalar is required "<<endl;
			exit(1);
    	}
    	$$=new op_binary_astnode("AND_OP", $1, $3);
    	if(arg3->is_calculable && arg1->is_calculable)
		{
			$$->is_calculable = true;
			$$->int_val = (arg1->int_val) && (arg3->int_val);
		}
    	$$->data_type = createtype(INT_TYPE);
    	$$->lvalue = false;
    } 
	;

equality_expression
	: relational_expression {
		$$=$1;
	}
    | equality_expression EQ_OP relational_expression {
		exp_astnode* arg1=$1, *arg3=$3;
		if(isPointer($1->data_type) && $3->is_calculable && $3->int_val==0) {
    		$3->data_type = createVoidStarType();
    	}

    	if(isPointer($3->data_type) && $1->is_calculable && $1->int_val==0) {
    		$1->data_type = createVoidStarType();
    	}
    	if(isStructType($1->data_type) || isStructType($3->data_type) || !compatible($1->data_type,$3->data_type))
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary == (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
    	}

    	if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("EQ_OP-Float", $1, $3);
		}
		else {
			$$=new op_binary_astnode("EQ_OP-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)==(arg3->int_val);
			}
		}
    	$$->data_type = createtype(INT_TYPE);
    	$$->lvalue = false;
    }	
	| equality_expression NE_OP relational_expression {
		exp_astnode* arg1=$1, *arg3=$3;

		if(isPointer($1->data_type) && $3->is_calculable && $3->int_val==0) {
    		$3->data_type = createVoidStarType();
    	}

    	if(isPointer($3->data_type) && $1->is_calculable && $1->int_val==0) {
    		$1->data_type = createVoidStarType();
    	}

		if(isStructType($1->data_type) || isStructType($3->data_type) || !compatible($1->data_type,$3->data_type))
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary != (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
    	}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("NE_OP-Float", $1, $3);
		}
		else {
			$$=new op_binary_astnode("NE_OP-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)!=(arg3->int_val);
			}
		}
    	$$->data_type = createtype(INT_TYPE);
		$$->lvalue = false;
	}
	;
relational_expression
	: additive_expression{
		$$=$1;
	}
    | relational_expression '<' additive_expression {
		exp_astnode* arg1=$1, *arg3=$3;

		if(isPointer($1->data_type) && $3->is_calculable && $3->int_val==0) {
    		$3->data_type = createVoidStarType();
    	}

    	if(isPointer($3->data_type) && $1->is_calculable && $1->int_val==0) {
    		$1->data_type = createVoidStarType();
    	}

    	if(isStructType($1->data_type) || isStructType($3->data_type) || !compatible($1->data_type,$3->data_type))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary < (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("LT_OP-Float", $1, $3);
		}
		else
		{
			$$=new op_binary_astnode("LT_OP-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)<(arg3->int_val);
			}
		}
		$$->data_type = createtype(INT_TYPE);
   		$$->lvalue = false;
   	}
	| relational_expression '>' additive_expression {
		exp_astnode* arg1=$1, *arg3=$3;

		if(isPointer($1->data_type) && $3->is_calculable && $3->int_val==0) {
    		$3->data_type = createVoidStarType();
    	}

    	if(isPointer($3->data_type) && $1->is_calculable && $1->int_val==0) {
    		$1->data_type = createVoidStarType();
    	}

		if(isStructType($1->data_type) || isStructType($3->data_type) || !compatible($1->data_type,$3->data_type))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary > (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("GT_OP-Float", $1, $3);
		}
		else
		{
			$$=new op_binary_astnode("GT_OP-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)>(arg3->int_val);
			}
		}
		$$->data_type = createtype(INT_TYPE);
		$$->lvalue = false;
	}
	| relational_expression LE_OP additive_expression {
		exp_astnode* arg1=$1, *arg3=$3;

		if(isPointer($1->data_type) && $3->is_calculable && $3->int_val==0) {
    		$3->data_type = createVoidStarType();
    	}

    	if(isPointer($3->data_type) && $1->is_calculable && $1->int_val==0) {
    		$1->data_type = createVoidStarType();
    	}

		if(isStructType($1->data_type) || isStructType($3->data_type) || !compatible($1->data_type,$3->data_type))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary <= (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("LE_OP-Float", $1, $3);
		}
		else
		{
			$$=new op_binary_astnode("LE_OP-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)<=(arg3->int_val);
			}
		}
		$$->data_type = createtype(INT_TYPE);
		$$->lvalue = false;
	}
    | relational_expression GE_OP additive_expression{
		exp_astnode* arg1=$1, *arg3=$3;

		if(isPointer($1->data_type) && $3->is_calculable && $3->int_val==0) {
    		$3->data_type = createVoidStarType();
    	}

    	if(isPointer($3->data_type) && $1->is_calculable && $1->int_val==0) {
    		$1->data_type = createVoidStarType();
    	}

    	if(isStructType($1->data_type) || isStructType($3->data_type) || !compatible($1->data_type,$3->data_type))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary >= (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("GE_OP-Float", $1, $3);
		}
		else
		{
			$$=new op_binary_astnode("GE_OP-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)>=(arg3->int_val);
			}
		}
		$$->data_type = createtype(INT_TYPE);
    	$$->lvalue = false;
    } 
	;

additive_expression 
	: multiplicative_expression{
		$$=$1;
	}
	| additive_expression '+' multiplicative_expression{
		exp_astnode* arg1=$1, *arg3=$3;
		if(!(isIntOrFloatType($1->data_type) && isIntOrFloatType($3->data_type)))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary + (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("Plus-Float", $1, $3);
			$$->data_type = createtype(FLOAT_TYPE);
		}
		else
		{

			$$=new op_binary_astnode("Plus-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)+(arg3->int_val);
			}
			$$->data_type = createtype(INT_TYPE);
		}
		$$->lvalue = false;
	} 
	| additive_expression '-' multiplicative_expression {
		exp_astnode* arg1=$1, *arg3=$3;
		if(!(isIntOrFloatType($1->data_type) && isIntOrFloatType($3->data_type)))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary - (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("Minus-Float", $1, $3);
			$$->data_type = createtype(FLOAT_TYPE);
		}
		else
		{
			$$=new op_binary_astnode("Minus-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)-(arg3->int_val);
			}
			$$->data_type = createtype(INT_TYPE);
		}
		$$->lvalue = false;
	}
	;

multiplicative_expression
	: unary_expression{
		$$=$1;
	}
	| multiplicative_expression '*' unary_expression{
		exp_astnode* arg1=$1, *arg3=$3;
		if(!(isIntOrFloatType($1->data_type) && isIntOrFloatType($3->data_type)))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary * (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("Mult-Float", $1, $3);
			$$->data_type = createtype(FLOAT_TYPE);
		}
		else
		{
			$$=new op_binary_astnode("Mult-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				$$->int_val = (arg1->int_val)*(arg3->int_val);
			}
			$$->data_type = createtype(INT_TYPE);
		}
		$$->lvalue = false;
	} 
	| multiplicative_expression '/' unary_expression {
		exp_astnode* arg1=$1, *arg3=$3;
		if(!(isIntOrFloatType($1->data_type) && isIntOrFloatType($3->data_type)))
		{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid operands to binary / (have ‘";
			$1->data_type.print();
			cout<<"’ and ‘";
			$3->data_type.print();
			cout<<"’)"<<endl;
			exit(1);
		}
		if(isFloatType($1->data_type) || isFloatType($3->data_type))
		{
			if(isIntType($1->data_type))
				$1->typecast="to-float";
			if(isIntType($3->data_type))
				$3->typecast="to-float";
			$$=new op_binary_astnode("Div-Float", $1, $3);
			$$->data_type = createtype(FLOAT_TYPE);
		}
		else
		{

			$$=new op_binary_astnode("Div-Int", $1, $3);
			if(arg3->is_calculable && arg1->is_calculable)
			{
				$$->is_calculable = true;
				if(arg3->int_val == 0)
				{
					cout<<filename<<": "<<d_scanner.lineNr()<<" Error: division by 0"<<endl;
					exit(1);
				}
				$$->int_val = (arg1->int_val)/(arg3->int_val);
			}
			$$->data_type = createtype(INT_TYPE);
		}
		$$->lvalue = false;
	}
	;
unary_expression
	: postfix_expression  {
		$$=$1;
	}  								
	| unary_operator unary_expression {
		exp_astnode* arg2=$2;

		$$=new op_unary_astnode($1->getoperator(),$2);
		if($1->getoperator()=="Dereference") {
			if(!isDereferenceAllowed($2->data_type)) {
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: invalid type argument of unary ‘*’ (have ‘";
				arg2->data_type.print();
				cout<<"’)"<<endl;
				exit(1);
			}
			$$->lvalue=true;
			$$->data_type=extract_type(arg2->data_type, TYPE_DEREF);
		}
		else if($1->getoperator()=="Address") {
			if(!arg2->lvalue) {
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: lvalue required as unary ‘&’ operand"<<endl;
				exit(1);
			}
			$$->lvalue = false;
			$$->data_type = arg2->data_type;
			$$->data_type.addpointer();
			// $$->data_type.deref++;

		}
		else if($1->getoperator()=="Uminus" || $1->getoperator() == "Not")
		{
			if(!isIntOrFloatType(arg2->data_type)){
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: wrong type argument to unary minus"<<endl;
				exit(1);
			}
			if(arg2->is_calculable)
			{
				$$->is_calculable = true;
				if($1->getoperator()=="Uminus")
				{
					$$->int_val = -1*arg2->int_val ;
				}
				else
				{
					if(arg2->int_val !=0)
					{
						$$->int_val = 0;
					}
					else
					{
						$$->int_val = 1;
					}
				}
			}
			$$->lvalue = false;
			$$->data_type = arg2->data_type;
		}
	}   												// unary_operator can only be '*' on the LHS of '='
	;                                     				// you have to enforce this during semantic analysis

postfix_expression
	: primary_expression  	{
		$$=$1;
	}  							
    | IDENTIFIER '(' ')' {

    	SymbTabEntry* temp1 = st->search($1);
    	if(temp1!=NULL) {
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: called object ‘"<<$1<<"’ is not a function or function pointer"<<endl;
			exit(1);
    	}

    	SymbTabEntry* temp = gst.search($1);
    	if(temp ==NULL)
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Object '"<<$1<<"' is not declared"<<endl;
			exit(1);
    	}
    	if(temp->varfun != "fun")
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Called object '"<<$1<<"' is not a function"<<endl;
			exit(1);
    	}
    	std::vector< pair<int, datatype> > v=temp->symbtab->getparameters();
    	if(v.size()>0) {
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: too few arguments to function '"<<$1<<"'"<<endl;
			exit(1);
    	}
    	// function call has no datatype
    	$$=new funcall_astnode(new identifier_astnode($1));
		$$->data_type=gst.search($1)->type;
    	$$->lvalue = false;
    }													    // Cannot appear on the LHS of '='. Enforce this.
    | IDENTIFIER '(' expression_list ')'{


    	if($1=="printf") {
    		$3->setname($1);
			$$=$3;
			$$->data_type=createtype(INT_TYPE);
    		$$->lvalue = false;
    	}

    	else {
	    	SymbTabEntry* temp1 = st->search($1);
	    	if(temp1!=NULL) {
	    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: called object ‘"<<$1<<"’ is not a function or function pointer"<<endl;
				exit(1);
	    	}

	    	SymbTabEntry* temp = gst.search($1);
	    	if(temp ==NULL)
	    	{
	    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Object '"<<$1<<"' is not declared"<<endl;
				exit(1);
	    	}
	    	if(temp->varfun != "fun")
	    	{
	    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Called object '"<<$1<<"' is not a function"<<endl;
				exit(1);
	    	}
	    	std::vector< pair<int, datatype> > v=temp->symbtab->getparameters();
	    	if(v.size()<$3->children.size()) {
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: too many arguments to function '"<<$1<<"'"<<endl;
				exit(1);
	    	}
	    	if(v.size()>$3->children.size()) {
	    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: too few arguments to function '"<<$1<<"'"<<endl;
				exit(1);
	    	}
	    	for(int i=0; i<v.size(); ++i) {
	    		if(!compatible(v[i].second,$3->children[i]->data_type)) {
	    			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Expected '";
	    			v[i].second.print();
	    			cout<<"' but argument is of type ‘";
	    			$3->children[i]->data_type.print();
	    			cout<<"’"<<endl;
					exit(1);
				}
	    	}

	    	$3->setname($1);
			$$=$3;
			$$->data_type=gst.search($1)->type;
			$$->lvalue = false;
		}
	}													    // Cannot appear on the LHS of '='  Enforce this.
    | postfix_expression '[' expression ']'
    {
    	exp_astnode* arg1 = $1;
    	datatype expr_dt = $3->data_type;
    	if(!isIntType(expr_dt))
    	// if(!$3->is_calculable)
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: array subscript is not an integer"<<endl;
			exit(1);
    	}
   //  	else if($3->int_val <0)
   //  	{
   //  		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: size of array is negative"<<endl;
			// exit(1);
   //  	}
    	if(!$1->data_type.is_array && $1->data_type.deref ==0)
    	{
			cout<<filename<<": "<<d_scanner.lineNr()<<" Error: subscripted value is neither array nor pointer"<<endl;
			exit(1);
    	}
    	$$=new arrayref_astnode($1,$3);
    	if(arg1->data_type.is_array) 
		{
			$$->data_type = extract_type(arg1->data_type,TYPE_ARRAY_REF);
		}
    	else
    	{
    			$$->data_type = arg1->data_type;
    			$$->data_type.deref--; 
    	}
    	$$->lvalue = true;
    }
    | postfix_expression '.' IDENTIFIER
    {
    	//check if data type of postfix exp is struct
    	datatype tempdt = $1->data_type;
        if(!isStructType(tempdt))
        {
        	cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Request for member '"<<$3<<"' in something not a structure"<<endl;
			exit(1);
        }
        // if postfix's type is struct, check if it has member named identifier
        SymbTabEntry* temp = gst.search(tempdt.data);
        SymbTab* struct_table = (temp->symbtab);
        SymbTabEntry* identifier_entry =struct_table->search($3); 
        if(identifier_entry == NULL)
        {
        	cout<<filename<<": "<<d_scanner.lineNr()<<" Error: struct '"<<tempdt.data<<"' has no member named '"<<$3<<"'"<<endl;
			exit(1);
        }
    	$$=new member_astnode($1, new identifier_astnode($3));			//Add in lex
    	$$->data_type = identifier_entry->type;
    	$$->lvalue = true;

    }
    | postfix_expression PTR_OP IDENTIFIER 
    {
    	datatype left_dt=$1->data_type;
    	if(!isStructPointerType(left_dt))
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Request for member '"<<$3<<"' in something not a pointer to structure"<<endl;
			exit(1);
    	}
    	SymbTabEntry* temp = gst.search(left_dt.data);
        SymbTab* struct_table = (temp->symbtab);
        SymbTabEntry* identifier_entry =struct_table->search($3); 
        if(identifier_entry == NULL)
        {
        	cout<<filename<<": "<<d_scanner.lineNr()<<" Error: struct '"<<left_dt.data<<"' has no member named '"<<$3<<"'"<<endl;
			exit(1);
        }
		$$=new arrow_astnode($1, new identifier_astnode($3));		//Add in lex
		$$->data_type = identifier_entry->type;
		$$->lvalue=true;
    }	
    | postfix_expression INC_OP {
    	exp_astnode* arg1=$1;
    	if(!(isIntType($1->data_type)||isFloatType($1->data_type) ))
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: wrong type argument to increment"<<endl;
			exit(1);
    	}
    	if(!$1->lvalue)
    	{
    		cout<<filename<<": "<<d_scanner.lineNr()<<" Error: lvalue required as increment operand"<<endl;
			exit(1);
    	}							

		$$=new op_unary_astnode("PP",$1);				// type checking for incop
		$$->data_type = arg1->data_type;
		$$->lvalue = false;
	} 									       // Cannot appear on the LHS of '='   Enforce this
	;

// There used to be a set of productions for l_expression at this point.

expression_list
    : expression{
    	$$=new funcall_astnode();
    	$$->pushback($1);	
    }
    | expression_list ',' expression{
    	$$=$1;
    	$$->pushback($3);
    }
	;

unary_operator
    : '-'	
    {
		$$=new op_unary_astnode("Uminus");
    }
	| '!'{
		$$=new op_unary_astnode("Not");
	}
    | '&' {
		$$=new op_unary_astnode("Address");					//These two are new
	}
    | '*' {
		$$=new op_unary_astnode("Dereference");
	}
	;

selection_statement
    : IF '(' expression ')' statement ELSE statement {
    	$$=new if_astnode($3,$5,$7);
    }
	;

iteration_statement

	: WHILE '(' expression ')' statement {
		$$=new while_astnode($3,$5);
	}		
	| FOR '(' expression ';' expression ';' expression ')' statement   {	//modified this production - (Not modified, right?)
		$$=new for_astnode($3,$5,$7,$9);
	}																		
        ;

declaration_list
    : declaration 									//push the declaration in the symbol table here itself, which is the local symbol table.{global declarations are not allowed.}
    {
    	declarator_class* decl;
    	type_specifier_class* type = $1->type;
    	int curr_offset = 0;
    	int cumul = 0;
    	declarator_list_class* dl = $1->decl_list;
    	datatype dt;
		for(int i=0;i<dl->v.size();++i)
		{
			decl = dl->v[i];
			dt = createtype(type, decl);
			curr_offset -= dt.size();
			// struct is not defined 
			if(dt.type == STRUCT_TYPE)
			{
				SymbTabEntry* temp = gst.search(dt.data);
				if(dt.data==currStruct && dt.deref ==0) {
					cout<<filename<<": "<<d_scanner.lineNr()<<" Error: struct '"<<dt.data<<"' is not defined."<<endl;
					exit(1);
				}
				if(dt.data==currStruct && dt.deref !=0) {

				}
				else if(temp==NULL || (temp->varfun != "struct")) {
					cout<<filename<<": "<<d_scanner.lineNr()<<" Error: struct '"<<dt.data<<"' is not defined."<<endl;
					exit(1);
				}	
			}
			// void data type declaration error
			if(dt.type==VOID_TYPE && dt.deref == 0)
			{
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Cannot declare variable of type void"<<endl;
				exit(1);
			}
			// previous declaration error
			if(st->search(decl->identifier) !=NULL)
			{
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: '"<<decl->identifier<<"' has a previous declaration"<<endl;
				exit(1);
			}
			if(insideStruct) {
				st->push(decl->identifier,					// pushing in the local symbol table
					new SymbTabEntry("var","local",
						dt,
						dt.size(),
						cumul,
						NULL));
			}
			else 
				st->push(decl->identifier,					// pushing in the local symbol table
					new SymbTabEntry("var","local",
						dt,
						dt.size(),
						curr_offset,
						NULL));
			cumul += dt.size();
		}			
		$$ = new declaration_list_class();
		if(insideStruct)
			$$->offset=cumul;	
		else
			$$->offset=curr_offset;	
    } 					
    | declaration_list declaration
    {
    	declarator_class* decl;
    	type_specifier_class* type = $2->type;
    	int curr_offset = $1->offset;
		int cumul=$1->offset;
    	declarator_list_class* dl = $2->decl_list;
    	datatype dt;

		for(int i=0;i<dl->v.size();++i)
		{
			decl = dl->v[i];
			dt = createtype(type, decl);
			curr_offset -= dt.size();
			// struct is not defined 
			if(dt.type == 3)
			{
				SymbTabEntry* temp = gst.search(dt.data);
				if(dt.data==currStruct && dt.deref ==0) {
					cout<<filename<<": "<<d_scanner.lineNr()<<" Error: struct '"<<dt.data<<"' is not defined."<<endl;
					exit(1);
				}
				if(dt.data==currStruct && dt.deref !=0) {

				}
				else if(temp==NULL || (temp->varfun != "struct")) {
					cout<<filename<<": "<<d_scanner.lineNr()<<" Error: struct '"<<dt.data<<"' is not defined."<<endl;
					exit(1);
				}	
			}
			// void data type declaration error
			if(dt.type==VOID_TYPE && dt.deref == 0)
			{
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: Cannot declare variable of type void"<<endl;
				exit(1);
			}
			// previous declaration error
			if(st->search(decl->identifier) !=NULL)
			{
				cout<<filename<<": "<<d_scanner.lineNr()<<" Error: '"<<decl->identifier<<"' has a previous declaration"<<endl;
				exit(1);
			}
			if(insideStruct) {
				st->push(decl->identifier,					// pushing in the local symbol table
					new SymbTabEntry("var","local",
						dt,
						dt.size(),
						cumul,
						NULL));
			}
			else {
				st->push(decl->identifier,					// pushing in the local symbol table
					new SymbTabEntry("var","local",
						dt,
						dt.size(),
						curr_offset,
						NULL));
			}
			cumul += dt.size();
		}
		$$ = new declaration_list_class();
		if(insideStruct)
			$$->offset=cumul;	
		else
			$$->offset=curr_offset;
    }
	;

declaration
	: type_specifier declarator_list';' 
	{
		$$ = new declaration_class($1, $2);
	}
	;

declarator_list
	: declarator
	{
		$$ = new declarator_list_class();
		$$->push($1);
	}
	| declarator_list ',' declarator
	{
		$1->push($3);
		$$ = $1;
	}
 	;