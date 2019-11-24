#include <iostream>
#include "bean_ast.hpp"
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "bean_vm.hpp"
#include "bean_ast_json.hpp"
void add(double a, std::int32_t b)
{
	//return a + b;
}

int main(int argc, char* argv[])
{
	int result = Catch::Session().run(argc, argv);
	
	auto token_gen = tokenizer();
	
	try {
		auto vm = bean::bean_vm();

		vm.bind_function("add", add);

		auto& state = vm.get_state();

		//vm.eval_file("C:\\Users\\madmin\\Documents\\Github\\bean\\lang.cs");

		auto jres = bean::ast_to_json("fun evaluate_complex_expression {var expr_value = (6.5 * 2 + 8.5) * (4 / 2) ^ 2;return expr_value;}return evaluate_complex_expression(); ");

		std::cout << jres.dump() << std::endl;

		std::cout << vm.get_state().variables.size() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception occured while parsing script." << std::endl;
		std::cout << e.what() << std::endl;
	}
}

