#pragma once

struct SCodeBlock;

namespace Syntax
{
	class ISyntax
	{
	public:
		virtual void textOut() { std::cout << "No textOut();\n"; };
		virtual std::string GenCode() { throw "GenCode not defined."; };
	};

	class If : public ISyntax
	{
	public:
		std::unique_ptr<ISyntax> expression;
		std::unique_ptr<SCodeBlock> block;
		std::vector<std::pair<std::unique_ptr<ISyntax>, std::unique_ptr<SCodeBlock>>> elseif;
		std::unique_ptr<SCodeBlock> elseBlock;
	};

	class FunctionDeclaration : public ISyntax
	{
	public:
		FunctionDeclaration()			
		{

		}

		std::string name;
		std::vector<std::pair<std::string, std::string>> parameters; //type,name
		std::unique_ptr<SCodeBlock> body;

		void textOut()
		{
			std::cout << "Function " << name << "(";
			for (auto p : parameters)
			{
				std::cout << p.first << " " << p.second << ", ";
			}
			std::cout << ")\n";
		}
	};
}