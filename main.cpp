#include <iostream>
#include "ast.h"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

int main(int argc, char* argv[])
{
	auto token_gen = tokenizer();

	try {
		/*
		std::vector<std::string> statements = split("var x = 6.5 + 8.5; print(x + 5);", ";");

		for(auto& statement : statements)
		{
			const auto tokens = token_gen.tokenize(statement);

			std::shared_ptr<ast> tree = ast_builder::parse(tokens);

			tree->eval();
			print_vars();
		}
		*/

		auto tokens = token_gen.tokenize("var x = 6.5 + 8.5");
		std::shared_ptr<ast> tree = ast_builder::parse(tokens);
		tree->eval();
		
		tokens = token_gen.tokenize("print(x + 5.55)");
		tree = ast_builder::parse(tokens);
		tree->eval();

		print_vars();
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception occured while parsing script." << std::endl;
		std::cout << e.what() << std::endl;
	}
}

/*
int main(int argc, char* argv[])
{
	int result = Catch::Session().run(argc, argv);
	
	auto token_gen = tokenizer();

	try {
		auto tree = token_node();
		//auto tokens = token_gen.tokenize("(4 + 5 * (2 + 3) * 5) * 5 * (4/3)");
		auto tokens = token_gen.tokenize("(3) + (6)");

		
		auto iter = token_iterator(tokens);

		auto rightmost = iter.find_rightmost_of_pemdas();


		tree.parse(tokens);

		std::cout << tree.eval() << std::endl;
		
		tree.print();
	}
	catch(const std::exception& e)
	{
		std::cout << "Exception occured while parsing script." << std::endl;
		std::cout << e.what() << std::endl;
	}
		
	return 1;
}
*/