#pragma once
#include "bean_ast.hpp"
#include "json.hpp"


namespace bean
{

	std::string get_type(const std::shared_ptr<ast>& tree)
	{
		return typeid(*tree.get()).name();
	}


	nlohmann::json ast_to_json(const std::shared_ptr<ast>& node, bean_state& state)
	{
		nlohmann::json output = {};

		if (!node)
		{
			return output;
		}
	
		output["type"] = node->to_string();

		auto str_rep = node->eval(state)->to_string();

		//if(strstr(str_rep.c_str(), "Identifier : to_string") < 0)
		output["identifier"] = node->eval(state)->to_string();
		
		auto& children = node->get_children();

		
		if(!children.empty())
		{
			output["children"] = {};
		}
		
		for(auto& child : children)
		{
			auto child_json = ast_to_json(child, state);
			
			output["children"].push_back(child_json);
		}

		

		return output;
	}
	
	nlohmann::json ast_to_json(const std::string& script)
	{
		bean_state state;
		
		const auto tokens = tokenizer::tokenize(script);

		auto res = ast_builder::parse(tokens, state);

		return ast_to_json(res, state);
	}

	
}