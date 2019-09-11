#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <sstream>
#include <algorithm>
enum class tokentype
{
	invalid,
	space,
	lparen,
	rparen,
	lbrace,
	rbrace,
	lbracket,
	rbracket,
	colon,
	semicolon,
	equal,
	symbol,
	asterisk,
	carriagereturn,
	newline,
	tokentype_end
};

const char* to_string(tokentype e)
{
	switch (e)
	{
	case tokentype::invalid: return "invalid";
	case tokentype::space: return "space";
	case tokentype::lparen: return "lparen";
	case tokentype::rparen: return "rparen";
	case tokentype::lbrace: return "lbrace";
	case tokentype::rbrace: return "rbrace";
	case tokentype::lbracket: return "lbracket";
	case tokentype::rbracket: return "rbracket";
	case tokentype::colon: return "colon";
	case tokentype::semicolon: return "semicolon";
	case tokentype::equal: return "equal";
	case tokentype::symbol: return "symbol";
	case tokentype::asterisk: return "asterisk";
	case tokentype::tokentype_end: return "tokentype_end";
	case tokentype::carriagereturn: return "carriagereturn";
	case tokentype::newline: return "newline";
	default: return "unknown";
	}
}



class token
{
public:
	token(std::string delim, const tokentype type): delim_(std::move(delim)), type_(type)
	{
		
	}

	[[nodiscard]] tokentype get_type() const { return type_;  }

	[[nodiscard]] const std::string& get_delim() const { return delim_; }

	std::size_t match(const std::string_view& view)
	{
		if (view.length() < delim_.length())
			return 0;

		return (std::memcmp(view.data(), delim_.data(), delim_.length())) == 0 ? delim_.length() : 0;
 	}
	
private:
	std::string delim_;
	tokentype type_;
};

class parsedtoken
{
public:
	parsedtoken(const std::uint32_t pos, const tokentype type, std::string text): pos_(pos), type_(type), text_(
		                                                                              std::move(text))
	{

	}

	parsedtoken(): pos_(-1), type_(tokentype::invalid), text_("")
	{
	}

	[[nodiscard]] std::string to_string_simple() const
	{
		if (type_ == tokentype::symbol)
		{
			std::stringstream ret;
			ret << ::to_string(type_) << "(" << text_ << ")";
			return ret.str();
		}
		else
		{
			std::stringstream ret;
			ret << ::to_string(type_);
			return ret.str();
		}
	}
	
	[[nodiscard]] std::string to_string() const
	{
		if(type_ == tokentype::symbol)
		{
			std::stringstream ret;
			ret << "Token of type " << ::to_string(type_) << " " << text_ << " found at pos " << pos_;
			return ret.str();
		}
		else
		{
			std::stringstream ret;
			ret << "Token of type " << ::to_string(type_) << " found at pos " << pos_;
			return ret.str();
		}
	}

	[[nodiscard]] std::size_t length() const
	{
		return text_.length();
	}

	[[nodiscard]] tokentype get_type() const
	{
		return type_;
	}

	[[nodiscard]] const std::string& get_text() const
	{
		return text_;
	}
	
private:
	std::uint32_t pos_{};
	tokentype type_;
	std::string text_;
};

class tokenizer
{
public:

	tokenizer();

	std::vector<parsedtoken> tokenize(const std::string& input);
	
private:

	std::size_t next_token_pos(const std::string& original, const std::size_t offset, parsedtoken& parsed);
	
	std::vector<token> tokens_;
};

inline tokenizer::tokenizer()
{
	tokens_.emplace_back(" ", tokentype::space);
	tokens_.emplace_back("(", tokentype::lparen);
	tokens_.emplace_back(")", tokentype::rparen);
	tokens_.emplace_back("{", tokentype::lbrace);
	tokens_.emplace_back("}", tokentype::rbrace);
	tokens_.emplace_back("[", tokentype::lbracket);
	tokens_.emplace_back("]", tokentype::rbracket);
	tokens_.emplace_back(":", tokentype::colon);
	tokens_.emplace_back(";", tokentype::semicolon);
	tokens_.emplace_back("=", tokentype::equal);
	tokens_.emplace_back("*", tokentype::asterisk);
	tokens_.emplace_back("\r", tokentype::carriagereturn);
	tokens_.emplace_back("\n", tokentype::newline);
}


inline std::vector<parsedtoken> tokenizer::tokenize(const std::string& input)
{
	std::vector<parsedtoken> output;

	auto last_token_end_pos = 0;
	
	for(std::uint32_t i = 0; i < input.length();)
	{
		parsedtoken found_token;

		const auto found_token_pos = next_token_pos(input, last_token_end_pos, found_token);

		if (found_token_pos != -1)
		{
			if(found_token_pos > last_token_end_pos)
			{
				// symbol lives between tokens
				output.emplace_back(last_token_end_pos, tokentype::symbol, input.substr(last_token_end_pos, found_token_pos + found_token.length() - last_token_end_pos - 1));
			}

			output.emplace_back(found_token);			
			last_token_end_pos = found_token_pos + found_token.length();
			i = last_token_end_pos;
		}
	}
	
	return output;
}

inline std::size_t tokenizer::next_token_pos(const std::string& original, const std::size_t offset, parsedtoken& parsed)
{
	const auto input = original.substr(offset);
	
	for (std::uint32_t i = 0; i < input.length(); i++)
	{
		const std::string_view view(input.c_str() + i, input.length() - i);

		for (auto& token : tokens_)
		{
			const auto match_length = token.match(view);
			if (match_length > 0)
			{
				parsed = parsedtoken(i + offset, token.get_type(), token.get_delim());
				return i + offset;
			}
		}
	}

	return -1;
}
