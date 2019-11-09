#include <iostream>
#include "bean_ast.hpp"
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "bean_vm.hpp"



int main(int argc, char* argv[])
{
	int result = Catch::Session().run(argc, argv);
	
	auto token_gen = tokenizer();
	
	try {
		auto vm = bean_vm();
		auto& state = vm.get_state();

		vm.eval("fun get_pi { 3.141592 }");

		std::cout << vm.eval_result("get_pi()")->as_double() << std::endl;


		std::cout << vm.get_state().variables.size() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception occured while parsing script." << std::endl;
		std::cout << e.what() << std::endl;
	}
}

