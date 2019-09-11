#pragma once
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "Tokenizer.hpp"

class formatter
{
public:
	formatter() = default;

	template <typename Type>
	formatter& operator << (const Type& value)
	{
		stream_ << value;
		return *this;
	}

	[[nodiscard]] std::string str() const { return stream_.str(); }
	operator std::string() const { return stream_.str(); }

private:
	std::stringstream stream_;
	formatter(const formatter&);
	formatter& operator = (formatter&);
};


inline std::runtime_error parsing_exception(token_iterator& iterator, const std::string& message, const std::int32_t offset = 0)
{
	const auto err_token = iterator.get_offset(offset);
	std::stringstream stream;
	stream << "Parsing error on line " << err_token.get_line() << ". " << message;
	return std::runtime_error(stream.str());
}

inline void parse_expression(token_iterator& iterator)
{
	// Assume we are parsing a variable. The end of a variable expression ends with a semicolon.

	iterator.next(); // Skip assignment operator.
	const auto original_token_index = iterator.get_index();
	const auto semi_token = iterator.find_next(token_type::semicolon);
	const auto semi_token_index = iterator.get_index();
	
	if (!semi_token.is_valid())
	{
		throw parsing_exception(iterator, formatter() << "Expected semicolon to terminate assignment expression.", -1);
	}

	const auto expr_begin_token = iterator.jump_to(original_token_index);

	std::cout << "Found " << semi_token_index - original_token_index << " tokens in expression!" << std::endl;

	// (5 * (1 + (5 + 5)))
	// (5 + 5)
	// + 1
	// * 5
	
	return;
}

inline void parse_handle_variable_value(token_iterator& iterator, const bool isVal)
{
	const auto value_token = iterator.next();

	if (!value_token.is_valid())
	{
		throw parsing_exception(iterator, formatter() << "Expected semicolon for var define. Found nothing.", -1);
	}

	if (value_token.get_type() == token_type::semicolon)
	{
		if(isVal)
		{
			throw parsing_exception(iterator, formatter() << "Val (const) must have defined value.");
		}
		return;
	}

	parse_expression(iterator);
}

inline void parse_handle_variable_type(token_iterator& iterator)
{
	const auto type_token = iterator.next();

	if (!type_token.is_valid())
	{
		throw parsing_exception(iterator, formatter() << "Expected type after var/val type symbol. Found nothing.");
	}

	if (type_token.get_type() != token_type::symbol)
	{
		throw parsing_exception(iterator, formatter() << "Expected type after var/val type symbol. Found " << type_token.get_text());
	}

	std::cout << "Parsed variable type as " << type_token.get_text() << std::endl;
}

inline void parse_handle_variable(token_iterator& iterator, const bool isVal)
{
	const auto name_token = iterator.next();
	auto definition_token = iterator.peek_before();

	if (!name_token.is_valid())
	{
		throw parsing_exception(iterator, formatter() << "Expected name after var/val token. Found nothing.");
	}

	if (name_token.get_type() != token_type::symbol)
	{
		throw parsing_exception(iterator, formatter() << "Expected symbol name after var/val token. Found " << name_token.get_text());
	}

	const auto assignment_token = iterator.next();
	/*

	Can either be : or =

	Examples:
	var someVar : int = 55;
	var someVar : int;

	val someVal : int = 55;
	val someVal = 55;
	 */

	if (!assignment_token.is_valid())
	{
		throw parsing_exception(iterator, formatter() << "Expecting either = or : assignment token after val/var name. Found nothing.");
	}
	
	if(assignment_token.get_type() != token_type::equal && assignment_token.get_type() != token_type::colon)
	{
		throw parsing_exception(iterator, formatter() << "Expecting either = or : after val/var name. Found " << assignment_token.get_text());
	}

	if(assignment_token.get_type() == token_type::colon)
	{
		parse_handle_variable_type(iterator);
	}

	parse_handle_variable_value(iterator, isVal);
}

bool parse_tokens(const token_array& tokens)
{
	token_iterator iterator(tokens);

	while (iterator.next().is_valid())
	{
		auto cur_token = iterator.here();

		const auto& cur_token_text = cur_token.get_text();
		const auto cur_token_type = cur_token.get_type();

		switch (cur_token_type)
		{
		case token_type::symbol:
		{
			if (cur_token_text == "val")
				parse_handle_variable(iterator, true);
			else if (cur_token_text == "var")
				parse_handle_variable(iterator, false);
		}
		default:;
		}

	}

	return true;
}
