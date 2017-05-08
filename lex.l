
%%
[ \t \n]                                    //skip, also , removed OTHERS from below, it is no longer needed
struct 										return Parser::STRUCT;
void										return Parser::VOID;
int 										return Parser::INT;
float										return Parser::FLOAT;
return 										return Parser::RETURN;
while										return Parser::WHILE;
for											return Parser::FOR;
if 											return Parser::IF;			
else										return Parser::ELSE;
"->"										return Parser::PTR_OP;
\*											return '*';
"&"											return '&';
\.											return '.';
"="											return '=';
"+"                                         return '+';
"-"                                         return '-';
"/"                                         return '/';
";"											return ';';
","                                         return ',';
"("                                         return '(';
")"                                         return ')';
"["                                         return '[';
"]"                                         return ']';
"{"                                         return '{';
"}"                                         return '}';
"!"											return '!';
"||"										return Parser::OR_OP;
"&&"										return Parser::AND_OP;
"=="										return Parser::EQ_OP;
"!="										return Parser::NE_OP;
"<"											return '<';
"<="										return Parser::LE_OP;
">"											return '>';
">="										return Parser::GE_OP;
"++"										return Parser::INC_OP;
[[:alpha:]_$][[:alnum:]_$]*					{
												Parser::d_val__ = std::string (matched());	
												return ParserBase::IDENTIFIER;
											}
((0[xX][0-9a-fA-F]+)|(0[0-7]*)|(0[bB][01]+)|([1-9][0-9]*)) {
												Parser::d_val__ = std::string (matched());
												return Parser::INT_CONSTANT;
											}		
([0-9]*\.[0-9]+)|([0-9]+\.[0-9]*)|((([0-9]*\.[0-9]+)|([0-9]+\.[0-9]*))([eE][+-]?[0-9]+))|(([0-9]+)([eE][+-]?[0-9]+))	{
												Parser::d_val__ = std::string (matched());
												return Parser::FLOAT_CONSTANT;	
											}
(\"([^\"\\]|(\\\")|(\\t)|(\\n)|(\\\\))*\")	{
												Parser::d_val__ = std::string (matched());
												return Parser::STRING_LITERAL;
											}
