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
};


inline std::runtime_error parsing_exception(token_iterator& iterator, const std::string& message, const std::int32_t offset = 0)
{
	const auto err_token = iterator.get_offset(offset);
	std::stringstream stream;
	stream << "Parsing error on line " << err_token.get_line() << ". " << message;
	return std::runtime_error(stream.str());
}

class token_node
{
public:
	token_node()
	{
		lh = nullptr;
		rh = nullptr;
	}

	void print()
	{
		std::cout << "(";
		if(lh)
			lh->print();
		std::cout << op.get_text();
		if(rh)
			rh->print();
		std::cout << ")";
	}

	void parse(const token_array& tokens)
	{
		token_iterator iterator(tokens);
		iterator.next();

		auto token_index = iterator.find_rightmost_of_pemdas();
		auto token = iterator.get_or_invalid(iterator.find_rightmost_of_pemdas());
		
		if (token.is_valid()) {
			op = token;
			
			const token_array lh_tokens = iterator.splice(0, token_index).get_tokens();
			const token_array rh_tokens = iterator.splice(token_index + 1, iterator.get_size()).get_tokens();

			lh = std::make_shared<token_node>();
			rh = std::make_shared<token_node>();

			lh->parse(lh_tokens);
			rh->parse(rh_tokens);
		}
		else if(iterator.get_size() == 1)
		{
			op = iterator.here();
		}
			
		return;
	}

	double eval()
	{
		if(op.get_type() == token_type::plus)
		{
			auto lhVal = lh->eval();
			auto rhVal = rh->eval();

			std::cout << "Adding " << lhVal << " and " << rhVal << std::endl;
			
			return lhVal + rhVal;
		}
		else if (op.get_type() == token_type::minus)
		{
			auto lhVal = lh->eval();
			auto rhVal = rh->eval();

			std::cout << "Subtracting " << lhVal << " and " << rhVal << std::endl;

			return lhVal - rhVal;
		}
		else if (op.get_type() == token_type::asterisk)
		{
			auto lhVal = lh->eval();
			auto rhVal = rh->eval();

			std::cout << "Multiplying " << lhVal << " and " << rhVal << std::endl;
			
			return lhVal * rhVal;
		}
		else if (op.get_type() == token_type::forward_slash)
		{
			auto lhVal = lh->eval();
			auto rhVal = rh->eval();

			std::cout << "Dividing " << lhVal << " and " << rhVal << std::endl;

			return lhVal / rhVal;
		}
		else if(op.get_type() == token_type::symbol)
		{
			std::stringstream stream;
			stream << op.get_text();

			double value = 0.0f;

			stream >> value;

			return value;
		}

		throw "Can't deal with eval!";
	}
	
	token op;
	std::shared_ptr<token_node> lh;
	std::shared_ptr<token_node> rh;
};

