all:    clean Scanner.ih Scanner.h Scannerbase.h lex.cc parse.cc main.cc Parser.h Parserbase.h Parser.ih
	g++ --std=c++0x lex.cc parse.cc main.cc -o parser
	./parser test.c > code.s


Scanner.ih: lex.l
	@if [ ! -e Scanner.ih ]; then \
		flexc++ lex.l;\
		sed -i '/include/a #include "Parserbase.h"' Scanner.ih; \
	fi

parse.cc Parser.h Parserbase.h Parser.ih: parse.y
	@bisonc++ --construction parse.y && sed -i '/#include/a #include "symbtab.h"\n#include "regstack.h"\nSymbTab gst,*st;\nSymbTabEntry* currFunc;\nstd::string currStruct;\nreg_stack registers(INT_TYPE_REGS);\nreg_stack fregisters(FLOAT_TYPE_REGS);\n int labelcount=0;\n int datalabelcount=0;\n int insideStruct = 0;\n' Parser.ih;
	@sed -i '/insert scannerobject/a \\tpublic:' Parser.h;
	@sed -i '/insert preincludes/a #include "ast.h"\n' Parserbase.h;
	@sed -i '\|STYPE__     d_val__;| d' Parserbase.h;
	@sed -i '\|typedef Meta__::SType STYPE__;| a \\tpublic: static STYPE__  d_val__; ' Parserbase.h;
	@sed -i '/include "Parser.ih"/a #include "ast.cpp"\n#include "symbtab.cpp"\n#include "type.cpp" \n extern std::string filename; \n extern std::vector<abstract_astnode*> ast;' parse.cc;
	@sed -i '\|void Parser::print__()| i Meta__::SType ParserBase::d_val__; ' parse.cc


.PHONY: clean
clean:
	@$(RM) Parser.ih Parser.h Parserbase.h parse.cc Scanner.ih Scanner.h Scannerbase.h lex.cc a.out graph.ps output parse.y.output parser *~
