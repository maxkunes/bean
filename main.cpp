#include <iostream>
#include "Parser.hpp"
#include <algorithm>



int main()
{
	auto token_gen = tokenizer();

	//auto tokens = token_gen.tokenize(
		//"val somevar = 5;\r\n");

	std::ifstream inputFile(R"(C:\Users\madmin\source\repos\script\lang.cs)");

	try {
		parse_tokens(token_gen.tokenize(inputFile));
	}
	catch(const std::exception& e)
	{
		std::cout << "Exception occured while parsing script." << std::endl;
		std::cout << e.what() << std::endl;
	}
		
	return 1;
}