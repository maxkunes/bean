#pragma once
#include "utils.hpp"
#include "bean_ast.hpp"
#include <sstream>
#include <algorithm>
#include <string>
#include <fstream>
#include <streambuf>

class bean_vm
{
public:

	std::shared_ptr<bean_object> eval_result(const std::string& script)
	{

		if (script.empty()) return std::make_shared<bean_object>(BeanObjectType::None);

		tokenizer token_gen;

		const auto tokens = tokenizer::tokenize(script);
		
		auto res = ast_builder::parse(tokens, state);

		return res->eval(state);
	}

	void eval(const std::string& script)
	{
		eval_result(script);
	}

	std::shared_ptr<bean_object>  eval_file_result(const std::string& file_path)
	{
		std::ifstream t(file_path);

		const std::string str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		
		return eval_result(str);
	}
	
	void eval_file(const std::string& file_path)
	{
		eval_file_result(file_path);
	}
	
	bean_state& get_state()
	{
		return state;
	}
	

private:
	bean_state state;
};