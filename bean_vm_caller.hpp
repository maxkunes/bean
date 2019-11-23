#pragma once
#include "bean_ast.hpp"

template<typename T> struct BoundBeanReturn
{
	inline static bean_object_ptr get(T value)
	{
		static_assert(false, "No support for return type in bound function.");
		return nullptr;
	}
};

template<> struct BoundBeanReturn<std::int32_t>
{
	inline static bean_object_ptr get(std::int32_t value)
	{
		return std::make_shared<bean_object_integer>(value);
	}
};


template<> struct BoundBeanReturn<double>
{
	inline static bean_object_ptr get(double value)
	{
		return std::make_shared<bean_object_double>(value);
	}
};

template<typename T> struct BoundBeanArgument
{
	inline static int get(bean_state& state, std::int32_t arg_idx)
	{
		static_assert(false, "No support for parameter type in bound function.");
		return 1;
	}
};

template<> struct BoundBeanArgument<std::int32_t>
{
	inline static std::int32_t& get(bean_state& state, std::int32_t arg_idx)
	{
		auto& param = state.parameter_stack[arg_idx];

		if (param->type() != BeanObjectType::INT)
		{
			throw std::exception("Invalid Parameter type!");
		}

		return param->as_int();
	}
};

template<> struct BoundBeanArgument<double>
{
	inline static double& get(bean_state& state, std::int32_t arg_idx)
	{
		auto& param = state.parameter_stack[arg_idx];

		if(param->type() != BeanObjectType::DOUBLE)
		{
			throw std::exception("Invalid Parameter type!");
		}

		return param->as_double();
	}
};


template<typename Ret, typename ...Args>
bean_function_caller bind_non_member_function(const std::string& function_name, std::function<Ret(Args...)> func)
{
	// the Ret type is set to the typename
	// the Args type is set to a number of types which are the function arguments
	// the above are set by template type deduction using the format Ret(*func)(Args...)
	// given the function double sum(int a, double b)
	// Ret type == double
	// Args is sorta like an array that is {int, double}

	// Create a caller lambda. Basically we return this function and that is used to call the function.
	bean_function_caller lambda = [func](bean_state& state) -> bean_object_ptr
	{
		// Gets the ammount of parameters of the function we want to call. Special sizeof... syntax.
		constexpr std::int32_t paramCount = sizeof...(Args);

		// Set argument index to the last parameter index.
		std::int32_t arg_idx = paramCount - 1;

		if constexpr (!std::is_same_v<Ret, void>) {
			Ret function_return;
			
			if constexpr(paramCount > 0)
				function_return = func(BoundBeanArgument<Args>::get(state, arg_idx--)...);
			else
				function_return = func();
			
			// ReSharper disable once CppAssignedValueIsNeverUsed
			return BoundBeanReturn<Ret>::get(function_return);
		}
		else
		{
			// no return

			if constexpr (paramCount > 0)
				func(BoundBeanArgument<Args>::get(state, arg_idx--)...);
			else
				func();
			
			return std::make_shared<bean_object_none>();
		}

		// above is where the magic happens, this can be broken up into a few steps
		// the first part, the cast turns the function addr into a function pointer using the above typedef.
		// the second part is the crazy part. we are calling that function with this (BoundBeanArgument<Args>::get(state, arg_idx--)...)
		// this is called unpacking noted by the (...) token . we are unpacking the variadic Args list so the resulting call looks like this:
		// function(BoundBeanArgument<FirstArgType>::get(state, arg_idx--), BoundBeanArgument<SecondArgType>::get(state, arg_idx--), BoundBeanArgument<NthArgType>::get(state, arg_idx--))
		// it basically repeats the same pattern but replaces the Args with you can think Arg[i]
		// we then use arg_idx to keep track of what arg we are calling ::get on.
		// All BoundBeanArgument<Type>::get does is essentially check get state.parameter_stack[arg_idx] object
		// and check if that object is of the type <Type> and if it is, return it.
	};

	return lambda;
}


template<typename Ret, typename ...Args>
bean_function_caller bind_non_member_function(const std::string& function_name, Ret(*func)(Args...))
{
	std::function<Ret(Args...)> stdfunc = func;
	return bind_non_member_function<Ret, Args...>(function_name, stdfunc);
}
