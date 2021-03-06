#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <sstream>
#include <algorithm>
#include <string>
#include <fstream>
#include <streambuf>

constexpr std::uint32_t invalid_token_index = -1;

inline std::vector<std::string> split_string(const std::string& str,
	const std::string& delimiter)
{
	std::vector<std::string> strings;

	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = str.find(delimiter, prev)) != std::string::npos)
	{
		strings.push_back(str.substr(prev, pos - prev));
		prev = pos + delimiter.size();
	}

	// To get the last substring (or only, if delimiter is not found)
	strings.push_back(str.substr(prev));

	return strings;
}

enum class token_type
{
	invalid,
	space,
	comma,
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
	plus,
	minus,
	forward_slash,
	tokentype_end,
	carrot,
	pow
};

inline const char* to_string(token_type e)
{
	switch (e)
	{
	case token_type::invalid: return "invalid";
	case token_type::space: return "space";
	case token_type::comma: return "comma";
	case token_type::lparen: return "lparen";
	case token_type::rparen: return "rparen";
	case token_type::lbrace: return "lbrace";
	case token_type::rbrace: return "rbrace";
	case token_type::lbracket: return "lbracket";
	case token_type::rbracket: return "rbracket";
	case token_type::colon: return "colon";
	case token_type::semicolon: return "semicolon";
	case token_type::equal: return "equal";
	case token_type::symbol: return "symbol";
	case token_type::asterisk: return "asterisk";
	case token_type::tokentype_end: return "tokentype_end";
	case token_type::carriagereturn: return "carriagereturn";
	case token_type::newline: return "newline";
	case token_type::plus: return "plus";
	case token_type::minus: return "minus";
	case token_type::forward_slash: return "forward slash";
	case token_type::carrot: return "carrot";
	case token_type::pow: return "pow";
	default: return "unknown";
	}
}



class token_delim
{
public:
	token_delim(std::string delim, const token_type type): delim_(std::move(delim)), type_(type)
	{
		
	}

	[[nodiscard]] token_type get_type() const { return type_;  }

	[[nodiscard]] const std::string& get_delim() const { return delim_; }

	std::size_t match(const std::string_view& view) const
	{
		if (view.length() < delim_.length())
			return 0;

		return (std::memcmp(view.data(), delim_.data(), delim_.length())) == 0 ? delim_.length() : 0;
 	}
	
private:
	std::string delim_;
	token_type type_;
};

class token : public std::enable_shared_from_this<token>
{
public:
	token(const std::uint32_t pos, const std::uint32_t line, const token_type type, std::string text): pos_(pos), line_(line), type_(type), text_(
		                                                                              std::move(text)), valid_(true)
	{

	}

	token(): pos_(invalid_token_index), line_(-1), type_(token_type::invalid), text_(""), valid_(false)
	{
		
	}

	[[nodiscard]] std::string to_string_simple() const
	{
		if (type_ == token_type::symbol)
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
		if(type_ == token_type::symbol)
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

	[[nodiscard]] token_type get_type() const
	{
		return type_;
	}

	[[nodiscard]] const std::string& get_text() const
	{
		return text_;
	}

	[[nodiscard]] std::uint32_t get_line() const
	{
		return line_;
	}

	[[nodiscard]] std::uint32_t get_pos() const
	{
		return pos_;
	}

	[[nodiscard]] bool is_valid() const
	{
		return valid_;
	}
	
private:
	std::uint32_t pos_;
	std::uint32_t line_;
	token_type type_;
	std::string text_;
	bool valid_;
};

using token_array = std::vector<token>;
using tokendelim_array = std::vector<token_delim>;

const static tokendelim_array tokenDelims_ = {
		{" ", token_type::space},
		{",", token_type::comma},
		{"(", token_type::lparen},
		{")", token_type::rparen},
		{"{", token_type::lbrace},
		{"}", token_type::rbrace},
		{"[", token_type::lbracket},
		{"]", token_type::rbracket},
		{":", token_type::colon},
		{";", token_type::semicolon},
		{"=", token_type::equal},
		{"*", token_type::asterisk},
		{"\r", token_type::carriagereturn},
		{"+", token_type::plus},
		{"-", token_type::minus},
		{"/", token_type::forward_slash},
		{"^", token_type::carrot},
		{"pow", token_type::pow},
		{"\n", token_type::newline},

};

class tokenizer
{
public:
	static token_array tokenize(const std::string& raw_input);
	static token_array tokenize(std::ifstream& file);
private:
	static std::size_t next_token_pos(const std::string& original, const std::size_t offset, token& parsed, std::int32_t lineNumber)
	{
		const auto input = original.substr(offset);

		for (std::uint32_t i = 0; i < input.length(); i++)
		{
			const std::string_view view(input.c_str() + i, input.length() - i);

			for (const auto& token_delim : tokenDelims_)
			{
				const auto match_length = token_delim.match(view);
				if (match_length > 0)
				{
					parsed = token(i + offset, lineNumber, token_delim.get_type(), token_delim.get_delim());
					return i + offset;
				}
			}
		}

		return invalid_token_index;
	}
};

inline token_array tokenizer::tokenize(const std::string& raw_input)
{
	token_array output;
	std::int32_t lineNumber = 1;
	
	auto lines = split_string(raw_input, "\n");

	for (auto& line : lines) {

		auto last_token_end_pos = 0;

		for (std::uint32_t i = 0; i < line.length();)
		{
			token found_token;

			const auto found_token_pos = next_token_pos(line, last_token_end_pos, found_token, lineNumber);

			if (found_token_pos != invalid_token_index)
			{
				if (found_token_pos > last_token_end_pos)
				{
					// symbol lives between tokens
					output.emplace_back(last_token_end_pos, lineNumber, token_type::symbol, line.substr(last_token_end_pos, found_token_pos + found_token.length() - last_token_end_pos - 1));
				}

				output.emplace_back(found_token);
				last_token_end_pos = found_token_pos + found_token.length();
				i = last_token_end_pos;
			}
			else
			{
				// No end token exist. Symbol is the last bit of the line.
				output.emplace_back(last_token_end_pos, lineNumber, token_type::symbol, line.substr(last_token_end_pos, line.size() - last_token_end_pos));
				i += line.size() - last_token_end_pos;
			}
		}

		lineNumber++;
	}
		
	return output;
}

inline token_array tokenizer::tokenize(std::ifstream& file)
{
	return tokenize(std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()));
}


class token_iterator
{
public:

	token_iterator(token_array tokens, std::uint32_t token_index = invalid_token_index) : tokens_(std::move(tokens)), token_index_(token_index)
	{
		tokens_.erase(std::remove_if(tokens_.begin(), tokens_.end(), [&](token tok)
		{
			return tok.get_type() == token_type::space;
		}), tokens_.end());
	}

	auto here_details()
	{
		return std::make_tuple(here(), here().get_type(), here().get_text());
	}

	auto next_details()
	{
		next();
		return here_details();
	}
	
	token& here()
	{
		return get_or_invalid(token_index_);
	}
	
	token& peek_next()
	{
		return get_or_invalid(token_index_ + 1);
	}

	token& next()
	{
		token_index_++;
		return get_or_invalid(token_index_);
	}

	token& peek_before()
	{
		return get_or_invalid(token_index_ - 1);
	}
	
	token& before()
	{
		token_index_--;
		return get_or_invalid(token_index_);
	}

	token& get_offset(const std::int32_t offset)
	{
		return get_or_invalid(token_index_ + offset);
	}

	token& find_next(const token_type type)
	{
		while(next().is_valid())
		{
			if (here().get_type() == type)
				return here();
		}
		return invalid_token_;
	}

	token& jump_to(const std::uint32_t token_index)
	{
		if (token_index < 0 || token_index >= tokens_.size())
			return invalid_token_;

		token_index_ = token_index;

		return get_or_invalid(token_index_);
	}
	
	token& get_or_invalid(const std::uint32_t index)
	{
		if(index < 0 || index >= tokens_.size())
			return invalid_token_;

		return tokens_[index];
	}

	[[nodiscard]] std::uint32_t get_index() const
	{
		return token_index_;
	}

	token_iterator get_rest()
	{
		return token_iterator(token_array(tokens_.begin() + get_index() + 1, tokens_.begin() + size()));
	}

	std::vector<token_iterator> split(const token_type delimiter)
	{
		std::vector<token_iterator> result;

		int last_split = 0;
		
		for(int i = 0; i < size();i++)
		{
			if(get_or_invalid(i).get_type() == delimiter)
			{
				result.push_back(splice(last_split + 0, i));
				last_split = i;
			}
		}

		result.push_back(splice(last_split + 1, size()));
	

		return result;
	}
	
	token_iterator splice(const std::uint32_t start, const std::uint32_t end)
	{
		return token_iterator(token_array(tokens_.begin() + start, tokens_.begin() + end));
	}

	std::uint32_t find_first_of(const token_type type)
	{
		const std::uint32_t o_pos = get_index();

		while (next().is_valid())
		{
			if (here().get_type() == type) {
				const std::uint32_t found_pos = get_index();
				jump_to(o_pos);
				return found_pos;
			}
		}

		jump_to(o_pos);

		return invalid_token_index;
	}
	
	std::uint32_t find_last_pos_of_open_close(const token_type open, const token_type close) {

		std::uint32_t stack = 0;

		const std::uint32_t o_pos = get_index();

		while (next().is_valid())
		{
			if (here().get_type() == open)
				stack++;
			if (here().get_type() == close) {
				stack--;
				if (stack == 0) {
					const std::uint32_t found_pos = get_index();
					jump_to(o_pos);
					return found_pos;
				}
			}
		}

		jump_to(o_pos);

		return invalid_token_index;
	}

	token_array get_tokens() const
	{
		return tokens_;
	}

	std::size_t size() const
	{
		return tokens_.size();
	}

	bool empty() const
	{
		return size() == 0;
	}

	std::uint32_t find_rightmost_of(const token_type type, const bool avoidParen = true)
	{
		for(int i = size() - 1; i >= int(token_index_); i--)
		{
			auto tokenType = get_or_invalid(i).get_type();

			if (avoidParen) {
				if (tokenType == token_type::rparen)
				{
					// account for first one
					int parenCount = 1;
					
					for (int j = i - 1; j >= 0; j--)
					{
						auto parenToken = get_or_invalid(j);
						
						if (get_or_invalid(j).get_type() == token_type::rparen)
							parenCount++;

						if (get_or_invalid(j).get_type() == token_type::lparen)
						{
							parenCount--;
							if (parenCount == 0) {
								i = j - 1;
								goto outParenCheck;
							}
						}
						
					}

				}
			}
			outParenCheck:

			// update token incase paren check change 'i'
			tokenType = get_or_invalid(i).get_type();
			
			if (tokenType == type)
				return i;
		}
		return invalid_token_index;
	}

	std::uint32_t find_rightmost_of(const std::vector<token_type> types)
	{
		std::uint32_t maxOffset = invalid_token_index;
		
		for (auto type : types)
		{
			const auto off = find_rightmost_of(type);

			if(off != invalid_token_index)
			{
				if (maxOffset == invalid_token_index)
				{
					maxOffset = off;
				}

				if (off > maxOffset)
					maxOffset = off;
			}
		}
		return maxOffset;
	}
	
	std::uint32_t find_rightmost_of_pemdas()
	{
		const std::vector<std::vector<token_type>> typeOrder = {
			{
				token_type::equal
			},
			{
				token_type::plus,
				token_type::minus
			},
			{
				token_type::asterisk,
				token_type::forward_slash
			},
			{
				token_type::carrot,
				token_type::pow
			}
		};

		for (const auto& i : typeOrder)
		{
			const auto offset = find_rightmost_of(i);

			if (offset != invalid_token_index)
				return offset;
		}

		// If we don't find a operator, just go left to right and return the first symbol.
		for(int i = token_index_; i < this->size(); i++)
		{
			if(get_or_invalid(i).get_type() == token_type::symbol)
			{
				return i;
			}
		}
		
		return invalid_token_index;
	}

	void remove(std::uint32_t index)
	{
		if(get_or_invalid(index).is_valid())
		{
			tokens_.erase(tokens_.begin() + index);

			if (token_index_ >= tokens_.size() && token_index_ != invalid_token_index)
				token_index_--;
		}
	}

	bool is_type(std::uint32_t index, token_type type)
	{
		const auto foundToken = get_or_invalid(index);

		if (foundToken.is_valid() && foundToken.get_type() == type)
			return true;

		return false;
	}

	token* begin()
	{
		return tokens_.data();
	}

	token* end()
	{
		return tokens_.data() + (tokens_.size() - 1);
	}

	token& first()
	{
		return *begin();
	}

	token& last()
	{
		return *end();
	}

	void pop_front()
	{
		remove(0);
	}

	void pop_end()
	{
		remove(tokens_.size() - 1);
	}
	
private:
	token_array tokens_;
	token invalid_token_;
	std::uint32_t token_index_;
};

