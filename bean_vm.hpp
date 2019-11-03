#pragma once
#include "utils.hpp"
#include "bean_ast.hpp"

class bean_vm
{
public:

	std::shared_ptr<bean_object> eval_result(const std::string script)
	{
		std::vector<std::string> lines;
		explode_string(script, lines, ";");

		if (lines.empty()) return std::make_shared<bean_object>(BeanObjectType::None);

		tokenizer token_gen;
		
		for(int i = 0; i < lines.size(); i++)
		{
			auto tokens = tokenizer::tokenize(lines[i]);

			if (tokens.empty()) continue;

			auto ast = ast_builder::parse(tokens, state);

			auto result = ast->eval(state);
			
			if (i == lines.size() - 1)
				return result;
		}

		return std::make_shared<bean_object>(BeanObjectType::None);
	}

	void eval(const std::string script)
	{
		eval_result(script);
	}
	
	bean_state& get_state()
	{
		return state;
	}
	

private:
	bean_state state;
};