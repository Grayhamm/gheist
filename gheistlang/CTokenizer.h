#pragma once
#undef EOF
struct _TOKEN_TYPES
{
	const int EOF = 0;
	const int OPEN_BRACKET = 1;
	const int CLOSE_BRACKET = 2;
	const int IDENTIFIER = 3;
	const int IF = 4;
	const int MATCH = 5;
	const int FN = 6;
	const int OPEN_PAREN = 7;
	const int CLOSE_PAREN = 8;
	const int RET = 9;
	const int USE = 10;
	const int QUOTE = 11;
	const int SEMI_COLON = 12;
	const int COLON = 13;
	const int INT_CONST = 14;
	const int FLOAT_CONST = 15;
	const int EQUALS = 16;
	const int COPY_EQUALS = 17; //:=
	const int PLUS = 18;
	const int MINUS = 19;
	const int MULT = 20;
	const int DIVIDE = 21;
	const int DOT = 22;
	const int VAR = 23;
	const int OPEN_ARRAY = 24;
	const int CLOSE_ARRAY = 25;
	const int ELSEIF = 26;
	const int ELSE = 27;
};

extern _TOKEN_TYPES TOKEN;

struct SToken
{
	int ID;	
	std::string token;

	bool operator == (int rhs) { return ID == rhs; };
	bool operator != (int rhs) { return ID != rhs; };
};

class CTokenizer
{
public:
	CTokenizer() {};
	
	std::vector<SToken> Tokenize(std::string_view v)
	{
		std::vector<SToken> tokens;

		while (v.length() > 0)
		{						
			tokens.push_back(GetNextToken(v));
			std::cout << "{" << tokens.back().ID << "}\t" << tokens.back().token << "\n"; 
		}

		return tokens;
	}

private:
	const std::vector<std::pair<std::string, int>> Keywords =
	{
		{"fn", TOKEN.FN},
		{"use", TOKEN.USE},
		{"ret", TOKEN.RET},
		{"var", TOKEN.VAR}
	};
		
	void EatWhiteSpace(std::string_view& v)
	{
		while (!v.empty())
		{
			if (!IsWhitespace(v[0]))
				return;
			v.remove_prefix(1);
		}
	}

	bool SimpleSymbol(char c, SToken& t)
	{
		const std::vector<std::pair<char, int>> sym =
		{
			{ '(', TOKEN.OPEN_PAREN },
			{ ')', TOKEN.CLOSE_PAREN },
			{ '{', TOKEN.OPEN_BRACKET },
			{ '}', TOKEN.CLOSE_BRACKET },
			{ ';', TOKEN.SEMI_COLON },
			{ ':', TOKEN.COLON },
			{ '=', TOKEN.EQUALS },
			{ '+', TOKEN.PLUS },
			{ '-', TOKEN.MINUS },
			{ '*', TOKEN.MULT },
			{ '/', TOKEN.DIVIDE },
			{ '[', TOKEN.OPEN_ARRAY },
			{ ']', TOKEN.CLOSE_ARRAY }
		};
		
		for (auto s : sym)		
		{
			if (s.first == c)
			{
				t.ID = s.second;		
				return true;
			}
		}
		return false;
	}

	SToken GetNextToken(std::string_view& v)
	{		
		SToken t;

		EatWhiteSpace(v);
		if (v.empty())
		{
			t.ID = TOKEN.EOF;
			return t;
		}

		char c = PeekNextChar(v);

		if (SimpleSymbol(c,t))
		{
			GetNextChar(v);
		}
		else if (c == '"')
		{
			HandleStringLiteral(t,v);			
		}
		else if (c == '.')
		{
			if (isdigit(v[1]))
			{
				HandleNumberConstant(t, v);
			}
			else
			{
				t.ID = TOKEN.DOT;
				//other
			}
		}
		else
		{
			if (isalpha(c))
			{
				//identifier
				auto ident = ReadIdentifier(v);

				//check keywords
				if (!CheckKeyword(ident, t))
				{
					//other identifier
					t.ID = TOKEN.IDENTIFIER;
					t.token = ident;
				}				
			}
			else if (isdigit(c))
			{
				HandleNumberConstant(t, v);
			}
			else
			{
				std::cout << "Error unknown toekn: " << c << "\n";
				v.remove_prefix(1);
			}
		}

		//str += c;
		return t;
	}

	void HandleNumberConstant(SToken& t, std::string_view& v)
	{
		while (isdigit(PeekNextChar(v)) || PeekNextChar(v) == '.')
		{
			t.token += GetNextChar(v);
		}

		int decimals = std::accumulate(t.token.begin(), t.token.end(), 0,
			[](int v, char c) {return v + (c == '.' ? 1 : 0); });

		if (decimals > 1)
		{
			throw "Too many .'s in numeric constant.";
		}		
		else if (decimals == 1)
		{
			t.ID = TOKEN.FLOAT_CONST;
		}
		else
		{
			t.ID = TOKEN.INT_CONST;
		}
	}

	void HandleStringLiteral(SToken& t, std::string_view& v)
	{
		t.ID = TOKEN.QUOTE;
		GetNextChar(v);

		while (PeekNextChar(v) != '"')
		{
			char c2 = GetNextChar(v);
			if (c2 == '\n')
			{
				throw "Unexpected newline in string literal.";
				//error: no closing "	
			}
			t.token += c2;
		}

		GetNextChar(v);
	}

	bool CheckKeyword(const std::string& ident, SToken& t)
	{
		for (auto k : Keywords)
		{
			if (k.first == ident)
			{
				t.ID = k.second; 
				return true;
			}
		}
		return false;
	}

	char PeekNextChar(const std::string_view& v)
	{
		return v[0];
	}

	char GetNextChar(std::string_view& v)
	{
		char c;
		char ret;
		c = v[0];
		ret = c;
		v.remove_prefix(1);
		
		if (IsWhitespace(c))
		{
			for (;;)
			{			
				c = v[0];
				if (IsWhitespace(c))
					v.remove_prefix(1);			
				else
					return 0;
			} 			
		}
		else
		{
			return ret;
		}	
	}

	bool IsWhitespace(char c)
	{
		return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
	}

	std::string ReadIdentifier(std::string_view& v)
	{
		std::string str;
		while (!v.empty())
		{
			char c = v[0];

			if (!isalnum(c))
			{
				return str;
			}

			str += c;

			v.remove_prefix(1);
		}
	}
};

class token_viewer
{
	std::vector<SToken>* TokenizedFile;
	int pos;
public:
	token_viewer(std::vector<SToken>* _TokenizedFile) :
		TokenizedFile{ _TokenizedFile },
		pos{ 0 }
	{

	}

	SToken& operator [] (int i)
	{
		return (*TokenizedFile)[i + pos];
	}

	SToken& next()
	{
		pos++;
		return (*this)[-1];
	}

	

	void consume(int n)
	{
		pos += n;
	}

	void consume()
	{
		consume(1);
	}

	int remain()
	{
		return TokenizedFile->size() - pos;
	}

	bool empty()
	{
		return remain() == 0;
	}
};