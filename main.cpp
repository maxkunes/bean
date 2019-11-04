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
		
		bean_function print_bind = [&](bean_objects params) -> bean_objects {
			for (auto& param : params) {
				std::cout << param->to_string() << std::endl;
			}
			return bean_objects();
		};

		state.functions["print"] = print_bind;

		vm.eval("var x = (6.5 + 8.5 * 4 / 2 ^ 2)");
		vm.eval("var y = x + 5.55");
		vm.eval("print(y + 5.55 + x)");

		std::cout << vm.get_state().variables.size() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception occured while parsing script." << std::endl;
		std::cout << e.what() << std::endl;
	}
}

