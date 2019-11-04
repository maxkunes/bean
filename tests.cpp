#include "tokenizer.hpp"
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "bean_ast.hpp"
#include "bean_vm.hpp"

static std::pair<bean_state, std::shared_ptr<bean_object>> eval_simple_all(const std::string script)
{
	auto vm = bean_vm();
	auto& state = vm.get_state();
	auto res = vm.eval_result(script);
	return std::make_pair(state, res);
}

static std::shared_ptr<bean_object> eval_simple(const std::string script)
{
	return eval_simple_all(script).second;
}


bool are_same(double a, double b) {
	return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

TEST_CASE("VM")
{
	SECTION("Mathmatical Expressions") {

		SECTION("Testing return types of mathmatical expressions.")
		{
			REQUIRE(eval_simple("3")->type() == BeanObjectType::INT);
			REQUIRE(eval_simple("3.0")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.5")->type() == BeanObjectType::DOUBLE);

			REQUIRE(eval_simple("3 + 10")->type() == BeanObjectType::INT);
			REQUIRE(eval_simple("3.0 + 10")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3 + 10.0")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.0 + 10.0")->type() == BeanObjectType::DOUBLE);


			REQUIRE(eval_simple("3 - 10")->type() == BeanObjectType::INT);
			REQUIRE(eval_simple("3.0 - 10")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3 - 10.0")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.0 - 10.0")->type() == BeanObjectType::DOUBLE);

			REQUIRE(eval_simple("3 * 10")->type() == BeanObjectType::INT);
			REQUIRE(eval_simple("3.0 * 10")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3 * 10.0")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.0 * 10.0")->type() == BeanObjectType::DOUBLE);

			REQUIRE(eval_simple("3 / 10")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.0 / 10")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3 / 10.0")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.0 / 10.0")->type() == BeanObjectType::DOUBLE);

			REQUIRE(eval_simple("3 ^ 10")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.0 ^ 10")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3 ^ 10.0")->type() == BeanObjectType::DOUBLE);
			REQUIRE(eval_simple("3.0 ^ 10.0")->type() == BeanObjectType::DOUBLE);
		}

		SECTION("Testing correctness of mathmatical operators.")
		{
			REQUIRE(eval_simple("3")->as_int() == 3);
			REQUIRE(are_same(eval_simple("3.0")->as_double(), 3.0));
			REQUIRE(are_same(eval_simple("3.5")->as_double(), 3.5));

			REQUIRE(eval_simple("3 + 10")->as_int() == 13);
			REQUIRE(are_same(eval_simple("3.0 + 10")->as_double(), 13.0));
			REQUIRE(are_same(eval_simple("3 + 10.0")->as_double(), 13.0));
			REQUIRE(are_same(eval_simple("3.0 + 10.0")->as_double(), 13.0));


			REQUIRE(eval_simple("3 - 10")->as_int() == -7);
			REQUIRE(are_same(eval_simple("3.0 - 10")->as_double(), -7.0));
			REQUIRE(are_same(eval_simple("3 - 10.0")->as_double(), -7.0));
			REQUIRE(are_same(eval_simple("3.0 - 10.0")->as_double(), -7.0));

			REQUIRE(eval_simple("3 * 10")->as_int() == 30);
			REQUIRE(are_same(eval_simple("3.0 * 10")->as_double(), 30.0));
			REQUIRE(are_same(eval_simple("3 * 10.0")->as_double(), 30.0));
			REQUIRE(are_same(eval_simple("3.0 * 10.0")->as_double(), 30.0));

			// all division results in a floating point value.
			REQUIRE(are_same(eval_simple("10 / 2")->as_double(), 5.0));
			REQUIRE(are_same(eval_simple("3 / 10")->as_double(), double(3) / double(10)));
			REQUIRE(are_same(eval_simple("3.0 / 10")->as_double(), double(3) / double(10)));
			REQUIRE(are_same(eval_simple("3 / 10.0")->as_double(), double(3) / double(10)));
			REQUIRE(are_same(eval_simple("3.0 / 10.0")->as_double(), double(3) / double(10)));

			// all pow results in a floating point value.
			REQUIRE(are_same(eval_simple("2 ^ 2")->as_double(), 4.0));
			REQUIRE(are_same(eval_simple("3 ^ 10")->as_double(), 59049.0));
			REQUIRE(are_same(eval_simple("3.0 ^ 10")->as_double(), 59049.0));
			REQUIRE(are_same(eval_simple("3 ^ 10.0")->as_double(), 59049.0));
			REQUIRE(are_same(eval_simple("3.0 ^ 10.0")->as_double(), 59049.0));
		}

		SECTION("Ensuring any ammount of spaces seperating operators is allowed and does not affect the output.") {
			REQUIRE(eval_simple("   3")->as_int() == 3);
			REQUIRE(eval_simple("3    ")->as_int() == 3);
			REQUIRE(eval_simple("   3   ")->as_int() == 3);
			REQUIRE(eval_simple(" 3  ")->as_int() == 3);
			REQUIRE(eval_simple("  3    ")->as_int() == 3);

			REQUIRE(eval_simple(" 1  +3")->as_int() == 4);
			REQUIRE(eval_simple("3  +1  ")->as_int() == 4);
		}

		SECTION("Order of ops sans parentheses") {
			REQUIRE(are_same(eval_simple("3 + 3 * 6")->as_int(), 21));
			REQUIRE(are_same(eval_simple("3 + 2 * 4")->as_int(), 11));
			REQUIRE(are_same(eval_simple("3+2-1")->as_int(), 4));
			REQUIRE(are_same(eval_simple("7+4*9-1")->as_int(), 42));
			REQUIRE(are_same(eval_simple("9 * 7 / 4")->as_double(), 15.75));
		}

		SECTION("Parentheses", "[parentheses]") {
			REQUIRE(are_same(eval_simple("(3)")->as_int(), 3));
			REQUIRE(are_same(eval_simple("((3))")->as_int(), 3));
			REQUIRE(are_same(eval_simple("(((3)))")->as_int(), 3));
			REQUIRE(are_same(eval_simple("(3 + 2) * 4")->as_int(), 20));
			REQUIRE(are_same(eval_simple("((3 + 2) * 4)")->as_int(), 20));
			REQUIRE(are_same(eval_simple("(4 + 5 * (2 + 3) * 5) * 5 * (4/3)")->as_double(), 860));
		}

	}

	SECTION("Variables")
	{
		{
			auto [state, result] = eval_simple_all("var x = (1 + 2)");
			REQUIRE(state.variables.size() == 1);
			REQUIRE(state.variables.count("x") == 1);
			REQUIRE(state.variables["x"]->as_int() == 3);
		}

		{
			auto [state, result] = eval_simple_all("var x = (1 + 2); var y = x + 1;");
			
			REQUIRE(state.variables.size() == 2);
			
			REQUIRE(state.variables.count("x") == 1);
			REQUIRE(state.variables["x"]->as_int() == 3);

			REQUIRE(state.variables.count("y") == 1);
			REQUIRE(state.variables["y"]->as_int() == 4);
		}
	}
	
	SECTION("Binding Functions") {

		{
			int value = 0;

			bean_function_decl inc_value_cpp = [&](bean_objects& params) -> bean_objects {
				value++;
				return bean_objects();
			};

			auto vm = bean_vm();
			auto& state = vm.get_state();
			state.functions.emplace_back(std::make_shared<bean_function>(inc_value_cpp, "inc_value"));

			auto res = vm.eval_result("inc_value()");

			REQUIRE(value == 1);

		}
	}

	{

		bean_function_decl get_pi_cpp = [&](bean_objects& params) -> bean_objects {
			bean_objects returnValues;
			returnValues.push_back(std::make_shared<bean_object_double>(3.14159265));
			return returnValues;
		};

		auto vm = bean_vm();
		auto& state = vm.get_state();
		state.functions.emplace_back(std::make_shared<bean_function>(get_pi_cpp, "pi"));

		vm.eval_result("var x = pi()");

		REQUIRE(state.variables.count("x") == 1);
		REQUIRE(are_same(state.variables["x"]->as_double(), 3.14159265));

	}

}