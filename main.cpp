#include <iostream>
#include "Tokenizer.hpp"
#include <algorithm>

enum class expressiontype
{
	definition
};

class expression
{
private:
	
};

class token_iterator
{
	
};

void generate_expressions(const std::vector<parsedtoken>& tokens)
{
	for(auto& token : tokens)
	{
		switch(token.get_type())
		{
		case tokentype::symbol:

			// Handle language defined restricted keywords.

			if(token.get_text() == "val")
			{
				// Some sort of definition is occuring.
				
			}
			
		default: ;
		}
	}
}

int main()
{
	auto token_gen = tokenizer();

	auto tokens = token_gen.tokenize(
		"val somevar = 5;\r\n");
	
	generate_expressions(tokens);
	
	for(const auto& token : tokens)
	{
		if(token.get_type() != tokentype::space)
			std::cout << token.to_string_simple() << " ";
	}
	
	return 1;
}