#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "catch.hpp"

static double eval_simple(const std::string& expr)
{
	auto token_gen = tokenizer();
	auto tree = token_node();
	auto tokens = token_gen.tokenize(expr);
	tree.parse(tokens);
	return tree.eval();
}

bool are_same(double a, double b) {
	return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

TEST_CASE("Evaluator", "[eval]") {
	
	SECTION("Simple", "[simple]") {
		REQUIRE(are_same(eval_simple("3"), 3));
		REQUIRE(are_same(eval_simple("3 + 2"), 5));
		REQUIRE(are_same(eval_simple("2 + 3"), 5));
		REQUIRE(are_same(eval_simple("3 - 2"), 1));
		REQUIRE(are_same(eval_simple("2 - 3"), -1));
		REQUIRE(are_same(eval_simple("3 * 2"), 6));
		REQUIRE(are_same(eval_simple("2 * 3"), 6));
		REQUIRE(are_same(eval_simple("6 / 2"), 3));
		REQUIRE(are_same(eval_simple("2 / 6"), double(1)/double(3)));
	}
	
	SECTION("Weird spacing", "[spacing]") {
		REQUIRE(are_same(eval_simple(" 3"), 3));
		REQUIRE(are_same(eval_simple(" 3 "), 3));
		REQUIRE(are_same(eval_simple("  3 "), 3));
		REQUIRE(are_same(eval_simple("  3  "), 3));
	}

	SECTION("Order of ops sans parentheses", "[emdas]") {
		REQUIRE(are_same(eval_simple("3 + 3 * 6"), 21));
		REQUIRE(are_same(eval_simple("3 + 2 * 4"), 11));
		REQUIRE(are_same(eval_simple("3+2-1"), 4));
		REQUIRE(are_same(eval_simple("7+4*9-1"), 42));
		REQUIRE(are_same(eval_simple("9 * 7 / 4"), 15.75));
	}

	SECTION("Parentheses", "[parentheses]") {
		REQUIRE(are_same(eval_simple("(3)"), 3));
		REQUIRE(are_same(eval_simple("((3))"), 3));
		REQUIRE(are_same(eval_simple("(((3)))"), 3));
		REQUIRE(are_same(eval_simple("(3 + 2) * 4"), 20));
		REQUIRE(are_same(eval_simple("((3 + 2) * 4)"), 20));
		REQUIRE(are_same(eval_simple("(4 + 5 * (2 + 3) * 5) * 5 * (4/3)"), 860));
	}

	SECTION("Exponents", "[exponents]")
	{
		REQUIRE(are_same(eval_simple("(3^2)"), 9));
		REQUIRE(are_same(eval_simple("(3 pow 2)"), 9));
	}
	
}