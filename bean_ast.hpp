#pragma once
#include <iostream>
#include <memory>
#include <cstdint>
#include <map>
#include "bean_object.hpp"
#include "tokenizer.hpp"


class bean_state;
class ast;

class ast_builder
{
public:
	static void remove_first_last_token(token_iterator& iterator, const token_type left, const token_type right)
	{
		while (iterator.size() >= 2 && iterator.first().get_type() == left && iterator.last().get_type() == right)
		{
			int parenCount = 0;

			token_iterator paren_iterator(iterator.get_tokens());

			while (paren_iterator.next().is_valid())
			{
				auto parenToken = paren_iterator.here();

				if (parenToken.get_type() == left)
				{
					parenCount++;
				}
				else if (parenToken.get_type() == right)
				{
					parenCount--;
					if (paren_iterator.get_index() != iterator.size() - 1 && parenCount == 0)
					{
						// We are not at the end of the expression but we have no open parentheses, therefore the start and end parentheses are not linked.
						goto escapeParen;
					}
					else if (parenCount == 0)
					{
						// Last element.
						iterator.pop_front();
						iterator.pop_end();
					}
				}
			}
		}
	escapeParen:
		return;
	}

	static std::shared_ptr<ast> parse(const token_array& tokens, bean_state& state);
};

using bean_objects = std::vector<std::shared_ptr<bean_object>>;
using bean_function_decl = std::function<bean_objects(bean_objects&)>;


class bean_function
{
public:
	explicit bean_function(bean_function_decl& fun, const std::string name)
	{
		fun_ = std::move(fun);
		name_ = name;
		//parameter_types_ = parameter_types;
	}

	bean_objects operator()(bean_objects& params) const
	{
		return fun_(params);
	}

	[[nodiscard]] std::string get_name() const
	{
		return name_;
	}
	
private:
	bean_function_decl fun_;
	std::string name_;
	//std::vector<bean_object_type_descriptor> parameter_types_;
};


class bean_state
{
public:
	bean_state()
	{
		variables.clear();
	}

	std::shared_ptr<ast> get_function(const std::string& name)
	{
		for(auto& fun : functions)
		{
			if (fun.first == name)
				return fun.second;
		}

		return nullptr;
	}
	
	std::map<std::string, std::shared_ptr<bean_object>> variables;
	std::map<std::string, std::shared_ptr<ast>> functions;
	//std::vector<std::shared_ptr<bean_function>> functions;
};

class ast
{
public:

	ast()
	{
		children_.reserve(2);
	}
	
	virtual ~ast() = default;
	
	virtual std::shared_ptr<bean_object> eval(bean_state& state)
	{
		throw std::exception("not implemented");
	}

	void set_left(const std::shared_ptr<ast> left)
	{
		if (children_.empty())
			children_.resize(1);
		
		children_[0] = left;
	}

	void set_right(const std::shared_ptr<ast> right)
	{
		if (children_.size() < 2)
			children_.resize(2);

		children_[1] = right;
	}

	void set_children(const std::vector<std::shared_ptr<ast>> children)
	{
		children_ = children;
	}

	std::vector<std::shared_ptr<ast>> get_children() const
	{
		return children_;
	}

	bool has_left()
	{
		return children_.size() >= 1;
	}

	bool has_right()
	{
		return children_.size() >= 2;
	}
	
	[[nodiscard]] std::shared_ptr<ast> get_left() const
	{
		return children_[0];
	}

	[[nodiscard]] std::shared_ptr<ast> get_right() const
	{
		return children_[1];
	}

	void set_identifier(const std::string& identifier)
	{
		identifier_ = identifier;
	}

	[[nodiscard]] std::string get_identifier() const
	{
		return identifier_;
	}

protected:
	std::vector<std::shared_ptr<ast>> children_;
	std::string identifier_;
};

class ast_value_double final : public ast
{
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		double doubleValue;

		// TODO: Change to a proper str -> number conversion.
		std::stringstream stream;
		stream << identifier_;
		stream >> doubleValue;
		return std::make_shared<bean_object_double>(doubleValue);
	}
};

class ast_value_integer final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		std::int32_t intValue;

		// TODO: Change to a proper str -> number conversion.
		std::stringstream stream;
		stream << identifier_;
		stream >> intValue;
		return std::make_shared<bean_object_integer>(intValue);
	}
};



class ast_plus final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		return get_left()->eval(state)->lh_plus(get_right()->eval(state));
	}
};

class ast_minus final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		return get_left()->eval(state)->lh_minus(get_right()->eval(state));
	}
};

class ast_pow final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		return get_left()->eval(state)->lh_pow(get_right()->eval(state));
	}
};

class ast_multiply final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		return get_left()->eval(state)->lh_multiply(get_right()->eval(state));
	}
};

class ast_divide final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		return get_left()->eval(state)->lh_divide(get_right()->eval(state));
	}
};

class ast_define_var final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		const auto varName = identifier_;
		
		state.variables[varName] = std::make_shared<bean_object>(BeanObjectType::None);

		return std::make_shared<bean_object>(BeanObjectType::None);
	}
};

class ast_variable_reference final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		const auto variableName = identifier_;
		
		return state.variables[variableName];
	}
};

class ast_return final : public ast
{
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		return get_left()->eval(state);
	}
};

class ast_define_and_set_var final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		const auto varName = identifier_;

		state.variables[varName] = get_left()->eval(state);

		return std::make_shared<bean_object>(BeanObjectType::None);
	}
};

class ast_set_var final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		const auto varName = identifier_;
		
		if (state.variables.count(varName) == 0)
			throw std::exception("Invalid variable name!");

		state.variables[varName] = get_right()->eval(state);
		
		return std::make_shared<bean_object>(BeanObjectType::None);
	}
};

class ast_function final : public ast {
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		const auto function_name = identifier_;

		state.functions[function_name] = get_left();

		return std::make_shared<bean_object>(BeanObjectType::None);
	}
};

class ast_function_script_call final : public ast {
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		const auto function_name = identifier_;
		const auto target_function = state.get_function(function_name);

		return target_function->eval(state);
	}
};

class ast_statement_list final : public ast
{
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		for(auto it = children_.begin(); it != children_.end(); ++it)
		{
			auto res = (*it)->eval(state);

			if (it == children_.end() - 1)
				return res;
		}

		return std::make_shared<bean_object_none>();
	}
};

class ast_function_call final : public ast
{
public:
	virtual std::shared_ptr<bean_object> eval(bean_state& state) override
	{
		/*
		const auto functionName = left_->get_value().get_text();
		const auto targetFunction = state.get_function(functionName);
		
		if (!targetFunction) {
			throw std::exception("Function to call does not exist!");
		}
		bean_objects params;
		
		if (right_) {
 			params.push_back(right_->eval(state));
		}

		auto function_return = targetFunction->operator()(params);

		if (function_return.size() == 0)
			function_return.push_back(std::make_shared<bean_object>(BeanObjectType::INVALID));

		return function_return[0];
		*/
		return std::make_shared<bean_object>(BeanObjectType::None);
	}
};
inline std::shared_ptr<ast> ast_builder::parse(const token_array& tokens, bean_state& state)
{
	token_iterator iterator(tokens);

	remove_first_last_token(iterator, token_type::lparen, token_type::rparen);
	remove_first_last_token(iterator, token_type::lbrace, token_type::rbrace);


	std::vector<std::shared_ptr<ast>> ast_list;
	ast_list.reserve(1);
	std::uint32_t last_expresssion_end = 0;
	
	std::shared_ptr<ast> resulting_ast;
	
	while (iterator.next().is_valid())
	{
		auto details = iterator.here_details();
		auto& token = std::get<0>(details);
		auto& token_type = std::get<1>(details);
		auto& token_text = std::get<2>(details);
		
		switch (token_type)
		{
		case token_type::symbol:
		{
			if (token_text == "fun")
			{
				details = iterator.next_details();

				if (token_type != token_type::symbol)
				{
					throw std::exception("Expected name proceeding 'fun' token.");
				}

				const auto function_name = token_text;  // NOLINT(performance-unnecessary-copy-initialization)

				details = iterator.next_details();

				if (token_type != token_type::lbrace)
				{
					throw std::exception("Expected body proceeding function name.");
				}

				resulting_ast = std::make_shared<ast_function>();
				resulting_ast->set_identifier(function_name);


				
				iterator.before();
				// +1 to skip over name
				auto body_end = iterator.find_last_pos_of_open_close(token_type::lbrace, token_type::rbrace) + 1;
				auto function_body = iterator.splice(iterator.get_index() + 1, body_end);
				iterator.next();

				state.functions[function_name] = nullptr;
				
				resulting_ast->set_left(parse(function_body.get_tokens(), state));
				ast_list.push_back(resulting_ast);
				
				// jump iterator till the end of block.
				iterator.jump_to(body_end - 1);
				last_expresssion_end = body_end;
			}
			else if(token_text == "var")
			{
				details = iterator.next_details();

				if (token_type != token_type::symbol)
				{
					throw std::exception("Expected name proceeding 'var' token.");
				}

				const auto var_name = token_text;  // NOLINT(performance-unnecessary-copy-initialization)

				details = iterator.next_details();

				if (token_type != token_type::equal)
				{
					throw std::exception("Expected equal token proceeding variable token.");
				}

				details = iterator.next_details();

				auto assignment_body_end = iterator.find_first_of(token_type::semicolon);
				auto assignment_body = iterator.splice(iterator.get_index(), assignment_body_end);

				
				resulting_ast = std::make_shared<ast_define_and_set_var>();

				state.variables[var_name] = std::make_shared<bean_object_none>();
				
				resulting_ast->set_identifier(var_name);
				resulting_ast->set_left(parse(assignment_body.get_tokens(), state));
				ast_list.push_back(resulting_ast);

				// jump iterator till end of block.
				iterator.jump_to(assignment_body_end - 1);
				last_expresssion_end = assignment_body_end;
			}
			else if(token_text == "return")
			{
				details = iterator.next_details();

				auto return_body_end = iterator.find_first_of(token_type::semicolon);

				if (return_body_end == invalid_token_index)
					return_body_end = iterator.size();
				
				auto return_body = iterator.splice(iterator.get_index(), return_body_end);

				resulting_ast = std::make_shared<ast_return>();
				resulting_ast->set_left(parse(return_body.get_tokens(), state));
				ast_list.push_back(resulting_ast);

				// jump iterator till end of block.
				iterator.jump_to(return_body_end - 1);
				last_expresssion_end = return_body_end;
			}
			else if (iterator.size() > 1)
			{
				const auto token_index = iterator.find_rightmost_of_pemdas();

				if (token_index == invalid_token_index)
				{
					throw std::exception("No token found to parse expression!");
				}

				iterator.jump_to(token_index);

				details = iterator.here_details();

				// Parse mathematical expression
				if (token_type != token_type::symbol) {

					switch (token_type)
					{
					case token_type::plus:
						resulting_ast = std::make_shared<ast_plus>();
						break;
					case token_type::minus:
						resulting_ast = std::make_shared<ast_minus>();
						break;
					case token_type::asterisk:
						resulting_ast = std::make_shared<ast_multiply>();
						break;
					case token_type::forward_slash:
						resulting_ast = std::make_shared<ast_divide>();
						break;
					case token_type::carrot:
						resulting_ast = std::make_shared<ast_pow>();
						break;
					case token_type::equal:
						resulting_ast = std::make_shared<ast_set_var>();
						break;
					default:
						throw std::exception("No handler for mathematical token.");
						break;
					}


					auto expression_end = iterator.find_first_of(token_type::semicolon);

					if (expression_end == invalid_token_index)
						expression_end = iterator.size();

					auto expression_left = iterator.splice(last_expresssion_end, iterator.get_index());
					auto expression_right = iterator.splice(iterator.get_index() + 1, expression_end);


					resulting_ast->set_left(parse(expression_left.get_tokens(), state));
					resulting_ast->set_right(parse(expression_right.get_tokens(), state));
					ast_list.push_back(resulting_ast);

					// jump iterator till end of block.
					iterator.jump_to(expression_end - 1);
					last_expresssion_end = expression_end;

				}
				else
				{
					// Some expression that contains no mathematical operators but has more than one symbol.

					if(state.functions.count(token_text))
					{
						// Possible function call.
						details = iterator.next_details();

						if(token_type == token_type::lparen)
						{
							// go back to function name to capture it
							iterator.before();
							details = iterator.here_details();
							
							// Function call.
							const auto function_name = token_text;  // NOLINT(performance-unnecessary-copy-initialization)

							auto parameter_body_end = iterator.find_last_pos_of_open_close(token_type::lparen, token_type::rparen);
							auto parameter_body = iterator.splice(iterator.get_index() + 2, parameter_body_end);

							std::vector<token_iterator> parameter_iterators;
							
							if(!parameter_body.empty())
							{
								// We have parameters!
								/* TODO: Implement don't peak for split as to allow something like print(some_func(arg1, arg2))*/
								parameter_iterators = parameter_body.split(token_type::comma);
							}
							

							resulting_ast = std::make_shared<ast_function_script_call>();
							resulting_ast->set_identifier(function_name);

							for(auto& param : parameter_iterators)
							{
								resulting_ast->get_children().push_back(parse(param.get_tokens(), state));
							}
							
							ast_list.push_back(resulting_ast);

							iterator.jump_to(parameter_body_end - 1);
							last_expresssion_end = parameter_body_end;
						}
						
					}
					
				}
			}
			else if(iterator.size() == 1)
			{
				std::shared_ptr<ast> ast_node;

				auto textual_representation = iterator.here().get_text();

				char* p = nullptr;

				auto i = std::strtol(textual_representation.data(), &p, 10);
				if (p == textual_representation.data() + textual_representation.size())
				{
					ast_node = std::make_shared<ast_value_integer>();
				}

				if (!ast_node) {
					auto f = std::strtod(textual_representation.data(), &p);
					if (p == textual_representation.data() + textual_representation.size())
					{
						ast_node = std::make_shared<ast_value_double>();
					}
				}

				if (!ast_node) {
					if (state.variables.count(textual_representation) > 0)
					{
						ast_node = std::make_shared<ast_variable_reference>();
					}
					else
					{
						throw std::exception("Unable to parse single symbol.");
					}
				}

				if (!ast_node)
				{
					throw std::exception("No known type for symbol");
				}

				ast_node->set_identifier(textual_representation);

				return ast_node;
			}
		}
		break;
		break;
		default:
			break;
		}

	}

	if (ast_list.empty())
	{
		throw std::exception("Failed to parse statement!");
	}
	if(ast_list.size() == 1)
		return ast_list[0];
	else
	{
		resulting_ast = std::make_shared<ast_statement_list>();
		resulting_ast->set_children(ast_list);
		return resulting_ast;
	}
}


/*
inline std::vector<std::shared_ptr<ast>> ast_builder::parse(const token_array& tokens, bean_state& state)
{
	token_iterator iterator(tokens);
	iterator.next();

	int brace_stack = 0;
	int paren_stack = 0;

	int start = 0;

	std::vector<std::shared_ptr<ast>> trees;
	
	while (iterator.here().is_valid())
	{
		auto details = iterator.here_details();
		auto& token = std::get<0>(details);
		auto& token_type = std::get<1>(details);

		if (token_type == token_type::lbrace)
		{
			brace_stack++;
		}
		else if (token_type == token_type::lparen)
		{
			paren_stack++;
		}
		else if (token_type == token_type::rbrace)
		{
			brace_stack--;

			if(brace_stack == 0 && paren_stack == 0)
			{
				auto sub_trees = parse(iterator.splice(start, iterator.get_index() + 1).get_tokens(), state);
				trees.insert(trees.end(), sub_trees.begin(), sub_trees.end());
				start = iterator.get_index() + 1;
			}
		}
		else if (token_type == token_type::rparen)
		{
			paren_stack--;
		}
		else if (token_type == token_type::semicolon)
		{
			// Ensure we are not inside of {} or ()
			if (brace_stack == 0 && paren_stack == 0)
			{
				auto sub_trees = parse(iterator.splice(start, iterator.get_index()).get_tokens(), state);
				trees.insert(trees.end(), sub_trees.begin(), sub_trees.end());
				start = iterator.get_index() + 1;
			}
		}
		else if(!iterator.peek_next().is_valid())
		{
			
		}

		iterator.next();
		details = iterator.here_details();
	}

	return trees;
}
*/
/*
inline std::vector<std::shared_ptr<ast>> ast_builder::parse_internal(const token_array& tokens, bean_state& state)
{
	token_iterator iterator(tokens);
	iterator.next();
	
	if (iterator.here().is_valid())
	{
		auto details = iterator.here_details();
		auto& token = std::get<0>(details);
		auto& token_type = std::get<1>(details);
		auto& token_text = std::get<2>(details);

		std::shared_ptr<ast> resulting_node;

		switch (token_type)
		{
		case token_type::plus:
			resulting_node = std::make_shared<ast_plus>();
			break;
		case token_type::minus:
			resulting_node = std::make_shared<ast_minus>();
			break;
		case token_type::asterisk:
			resulting_node = std::make_shared<ast_multiply>();
			break;
		case token_type::forward_slash:
			resulting_node = std::make_shared<ast_divide>();
			break;
		case token_type::carrot:
			resulting_node = std::make_shared<ast_pow>();
			break;
		case token_type::equal:
			resulting_node = std::make_shared<ast_set_var>();
			break;
		case token_type::var:
		{
			iterator.next();
			details = iterator.here_details();

			if (token_type != token_type::symbol)
			{
				throw std::exception("Expecting variable name after var token.");
			}

			auto variable_name = token_text;

			iterator.next();
			details = iterator.here_details();

			if (token_type != token_type::equal)
			{
				throw std::exception("Expecting assignment for var.");
			}

			auto node = std::make_shared<ast_define_and_set_var>();
			node->set_right(parse_internal(iterator.get_rest().get_tokens(), state));
			node->set_left(parse_internal(variable_name, state));
		}
		default:
			break;
		}

	}
}
*/