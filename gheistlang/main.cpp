#include "stdafx.h"
#include "CTokenizer.h"
#include "CLexer.h"

const std::string strProgram = std::string{ "\n" } +
"fn main(arg0:string)\n" +
"{\n" +
"	var a := 5;\n" +
"	a := a * 6 +4;\n" +
"	print(\"a = {{a}}\");\n" +
"}\n" +
" ";

int main()
{
	CTokenizer t;
	try
	{		
		//t.Tokenize("fn main() { print(\"Herro Worrd!\"); var a = .34;}");
		std::cout << strProgram << "-----------------\n";
		auto ts = t.Tokenize(strProgram);
		CLexer lex(ts);
		auto l = lex.ProcessTopLevel();		
		for (auto &x : l)
		{
			x->textOut();
		}
	}
	catch (const char* str)
	{
		std::cout << "ERROR: " << str << "\n\n";
	}
    return 0;
}

