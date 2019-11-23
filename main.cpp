#include <iostream>
#include "bean_ast.hpp"
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "bean_vm.hpp"

void add(double a, std::int32_t b)
{
	//return a + b;
}

int main(int argc, char* argv[])
{
	int result = Catch::Session().run(argc, argv);
	
	auto token_gen = tokenizer();
	
	try {
		auto vm = bean_vm();

		vm.bind_function("add", add);

		auto& state = vm.get_state();

		auto result = vm.eval_result("add(2.1, 1);");

		std::cout << vm.get_state().variables.size() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception occured while parsing script." << std::endl;
		std::cout << e.what() << std::endl;
	}
}

