#include "tokenizer.hpp"
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "bean_ast.hpp"
#include "bean_vm.hpp"

static std::shared_ptr<bean_object> eval_simple(const std::string script)
{
	auto vm = bean_vm();
	auto& state = vm.get_state();
	auto res =  vm.eval_result(script);
	return res;
}

bool are_same(double a, double b) {
	return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

TEST_CASE("VM")
{

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
		REQUIRE(are_same(eval_simple("3 / 10")->as_double(), double(3)/double(10)));
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

