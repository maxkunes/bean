#pragma once
#include "bean_ast.hpp"
#include "json.hpp"


namespace bean
{

	std::string get_type(const std::shared_ptr<ast>& tree)
	{
		return typeid(*tree.get()).name();
	}


	nlohmann::json ast_to_json(const std::shared_ptr<ast>& node)
	{
		nlohmann::json output = {};

		if (!node)
		{
			return output;
		}
	
		output["type"] = get_type(node);

		if(!node->get_identifier().empty())
		{
			output["identifier"] = node->get_identifier();
		}
		
		auto& children = node->get_children();

		
		if(!children.empty())
		{
			output["children"] = {};
		}
		
		for(auto& child : children)
		{
			auto child_json = ast_to_json(child);
			
			output["children"].push_back(child_json);
		}

		

		return output;
	}
	
	nlohmann::json ast_to_json(const std::string& script)
	{
		bean_state state;
		
		const auto tokens = tokenizer::tokenize(script);

		auto res = ast_builder::parse(tokens, state);

		return ast_to_json(res);
	}

	
}