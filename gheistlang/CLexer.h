#pragma once

#include "CTokenizer.h"
#include "syntax.h"

using namespace Syntax;

struct SType
{
	std::string type;
	int typeID;
	int size;		//#bytes
};

struct SVariable
{
	int typeID;
	SType* type; //not available on first pass
	std::string name;
};

struct _STATES
{
	const int TOP_LEVEL = 0;
	const int EXPRESSIOM = 1;
	const int FUNCTION_DEF = 2;
	const int FUNCTION_CALL = 3;
};

extern _STATES STATE;

struct SStateLayer
{
	int State;
};

struct SCodeBlock
{
	std::vector<std::unique_ptr<ISyntax>> Code;
	std::vector<SVariable> Locals;

	void Add(std::unique_ptr<ISyntax>&& x)
	{
		Code.push_back(std::move(x));
	}
};

class CLexer
{
private:
	token_viewer t;	
	std::stack<SStateLayer> State;
	std::vector<std::vector<SVariable>> Stack;
	std::vector<std::unique_ptr<ISyntax>> Syntax;

	SVariable& GetVariable(std::string name)
	{
		for (auto vsv : Stack)
		{
			for (SVariable& v : vsv)
			{
				if (v.name == name)
					return v;
			}
		}

		throw "Unknown variable";
	}
public:
	CLexer(std::vector<SToken>& tf) :
		t(&tf)
	{

	}
	using pSyn = std::unique_ptr<ISyntax>;
	using SynTree = std::vector<std::unique_ptr<ISyntax>>;
	SynTree ProcessTopLevel()
	{
		SynTree o;

		while (!t.empty())
		{
			if (t[0] == TOKEN.FN)
			{
				o.push_back(HandleFunctionDeclare());
			}
			else
			{
				t.consume();
			}
		}
		return o;
	}
	

private:

	std::unique_ptr<SCodeBlock> ReadCodeBlock(int blockType)
	{
		bool inlineBlock = false;
		auto s = std::make_unique<SCodeBlock>();

		if (t[0] != TOKEN.OPEN_BRACKET)
		{
			inlineBlock = true;
		}

		for (;;)
		{
			if (t.empty())
			{
				throw "Missing '}' in code block.";
			}
			
			if (t[0] == TOKEN.CLOSE_BRACKET)
			{
				if (!inlineBlock)
				{
					t.consume();
					break;
				}
				else
					throw "Unexpected } in inline block";
			}

			if (t[0] == ';' && inlineBlock)
			{
				t.consume();
				break;
			}

			if (t[0] == TOKEN.IF)
			{
				t.consume();
				s->Add(HandleIf());
			}
			else
			{
				t.consume();				
			}
		}

		return s;
	}

	pSyn HandleIf()
	{
		auto s = std::make_unique<Syntax::If>();
		
		if (t[0] != TOKEN.OPEN_PAREN)
			throw "Expected ( in 'if' statement.";

		t.consume();
		s->expression = ParseExpression();

		if (t[0] != TOKEN.CLOSE_PAREN)
			throw "Expected ')' in 'if' statement.";
		t.consume();

		s->block = ReadCodeBlock(TOKEN.IF);

		while (t[0] == TOKEN.ELSEIF)
		{
			if (t[1] != TOKEN.OPEN_PAREN)
				throw "Expected '(' in elseif statement.";

			t.consume(2);
			
			auto expr = ParseExpression();
			if (t[0] != TOKEN.CLOSE_PAREN)
				throw "Expected ')' in elseif statement.";
			t.consume();
			auto block = ReadCodeBlock(TOKEN.ELSEIF);

			s->elseif.push_back({ std::move(expr), std::move(block) });
		}

		if (t[0] == TOKEN.ELSE)
		{
			s->elseBlock = ReadCodeBlock(TOKEN.ELSE);
		}

		return s;
	}

	pSyn ParseExpression()
	{
		return nullptr;
	}

	pSyn HandleFunctionDeclare()
	{
		auto s = std::make_unique<FunctionDeclaration>();		
		t.consume();
		if (t[0] != TOKEN.IDENTIFIER)		
			throw "Expected Identifier";
		
		//read function name
		s->name = t[0].token;
		t.consume();

		//read parameters
		if (t[0] != TOKEN.OPEN_PAREN)		
			throw "Expected '(' in function declaration";

		t.consume();
		
		for (;;)
		{
			if (t[0] == TOKEN.CLOSE_PAREN)
			{
				t.consume();
				break;
			}
			else if (t[0] == TOKEN.IDENTIFIER)
			{
				std::string type, name;
				
				if (t[1] != TOKEN.COLON)				
					throw "Expected :type in function parameter declaration.";
				
				if (t[2] != TOKEN.IDENTIFIER)				
					throw "Expected type in function parameter declaration.";
				
				s->parameters.push_back({ t[2].token, t[0].token} );
				t.consume(3);
			}
		}

		if (t[0] != TOKEN.OPEN_BRACKET)
			throw "Expected code block after function declaration.";

		s->body = ReadCodeBlock(TOKEN.FN);

		return s;
	}
};