#include <iostream>
#include "Parser.hpp"
#include <algorithm>



int main()
{
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