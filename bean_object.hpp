#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include "fnv1a.hpp"
// double, integer

enum class BeanObjectType
{
	INVALID,
	None,
	REFERENCE,
	CUSTOM,
	DOUBLE,
	INT
};

inline bool BeanObjectTypeIsPrimitive(const BeanObjectType type)
{
	return type != BeanObjectType::INVALID && type != BeanObjectType::CUSTOM && type != BeanObjectType::REFERENCE;
}

class bean_object
{
public:
	bean_object() : type_(BeanObjectType::INVALID) {}
	bean_object(const BeanObjectType type) : type_(type) {}

	virtual bean_object_type_descriptor get_type_descriptor()
	{
		
	}
	
	virtual std::string to_string() {
		return "to_string not implmented for object type!";
	}

	virtual std::shared_ptr<bean_object> lh_plus(const std::shared_ptr<bean_object>& rh)
	{
		std::cout << "TODO: bean_object::lh_plus not implemented!";
		return std::make_shared<bean_object>(BeanObjectType::INVALID);
	}

	virtual std::shared_ptr<bean_object> lh_minus(const std::shared_ptr<bean_object>& rh)
	{
		std::cout << "TODO: bean_object::lh_minus not implemented!";
		return std::make_shared<bean_object>(BeanObjectType::INVALID);
	}

	virtual std::shared_ptr<bean_object> lh_pow(const std::shared_ptr<bean_object>& rh)
	{
		std::cout << "TODO: bean_object::lh_pow not implemented!";
		return std::make_shared<bean_object>(BeanObjectType::INVALID);
	}

	virtual std::shared_ptr<bean_object> lh_multiply(const std::shared_ptr<bean_object>& rh)
	{
		std::cout << "TODO: bean_object::lh_multiply not implemented!";
		return std::make_shared<bean_object>(BeanObjectType::INVALID);
	}

	virtual std::shared_ptr<bean_object> lh_divide(const std::shared_ptr<bean_object>& rh)
	{
		std::cout << "TODO: bean_object::lh_divide not implemented!";
		return std::make_shared<bean_object>(BeanObjectType::INVALID);
	}
	
	virtual std::shared_ptr<bean_object> rh_plus(const std::shared_ptr<bean_object>& lh)
	{
		std::cout << "TODO: bean_object::rh_plus not implemented!";
		return std::make_shared<bean_object>(BeanObjectType::INVALID);
	}

	virtual void set(const std::shared_ptr<bean_object>& value)
	{
		throw std::exception("Error: This object is not assignable!");
	}

	[[nodiscard]] BeanObjectType type() const
	{
		return type_;
	}

	std::int32_t as_int()
	{
		return as<std::int32_t>();
	}

	double as_double()
	{
		return as<double>();
	}

	template<typename T> T as()
	{
		return *static_cast<T*>(object_);
	}

	template<typename T> T as_ptr()
	{
		return (T*)object_;
	}

protected:
	BeanObjectType type_;
	void* object_;
};

/*
 Overly simple type descriptor class.
 */
class bean_object_type_descriptor
{
public:
	bean_object_type_descriptor(const std::string type_name)
	{
		type_name_ = type_name;
		type_name_hash_ = hash_32_fnv1a_const(type_name.c_str());
		valid_ = true;
	}

	bean_object_type_descriptor() : type_name_hash_(-1), valid_(false)
	{
		
	}
private:
	std::string type_name_;
	std::uint32_t type_name_hash_;
	bool valid_;
};

class bean_object_none : public bean_object
{
public:
	bean_object_none() : bean_object(BeanObjectType::None)
	{
	}

	virtual bean_object_type_descriptor get_type_descriptor() override
	{
		return bean_object_type_descriptor("None");
	}
};

class bean_object_reference : public bean_object
{
public:
	bean_object_reference(std::shared_ptr<bean_object> value) : bean_object(BeanObjectType::REFERENCE)
	{
		value_ = value;
		object_ = &value_;
	}

	virtual bean_object_type_descriptor get_type_descriptor() override
	{
		return bean_object_type_descriptor("Reference");
	}

	virtual std::string to_string() override 
	{
		return value_->to_string();
	}

	virtual void set(const std::shared_ptr<bean_object>& value)
	{
		value_ = value;
	}
	
	virtual std::shared_ptr<bean_object> lh_plus(const std::shared_ptr<bean_object>& rh) override
	{
		return value_->lh_plus(rh);
	}

	virtual std::shared_ptr<bean_object> lh_multiply(const std::shared_ptr<bean_object>& rh) override
	{
		return value_->lh_multiply(rh);
	}

	virtual std::shared_ptr<bean_object> lh_divide(const std::shared_ptr<bean_object>& rh) override
	{
		return value_->lh_divide(rh);
	}

	virtual std::shared_ptr<bean_object> lh_pow(const std::shared_ptr<bean_object>& rh) override
	{
		return value_->lh_pow(rh);
	}
private:
	std::shared_ptr<bean_object> value_;
};

class bean_object_double;

class bean_object_integer : public bean_object
{
public:
	bean_object_integer(std::int32_t value) : bean_object(BeanObjectType::INT), value_(0xCCCCCCCC)
	{
		value_ = value;
		object_ = &value_;
	}

	virtual bean_object_type_descriptor get_type_descriptor() override
	{
		return bean_object_type_descriptor("Integer");
	}

	virtual std::string to_string() override
	{
		return std::to_string(value_);
	}

	virtual std::shared_ptr<bean_object> lh_pow(const std::shared_ptr<bean_object>& rh) override;
	
	virtual std::shared_ptr<bean_object> lh_multiply(const std::shared_ptr<bean_object>& rh) override;

	virtual std::shared_ptr<bean_object> lh_divide(const std::shared_ptr<bean_object>& rh) override;
	
	virtual std::shared_ptr<bean_object> lh_plus(const std::shared_ptr<bean_object>& rh) override;


	virtual std::shared_ptr<bean_object> lh_minus(const std::shared_ptr<bean_object>& rh) override;
private:
	std::int32_t value_;
};

class bean_object_double : public bean_object
{
public:
	bean_object_double(double value) : bean_object(BeanObjectType::DOUBLE), value_(std::numeric_limits<double>::quiet_NaN())
	{
		value_ = value;
		object_ = &value_;
	}

	virtual bean_object_type_descriptor get_type_descriptor() override
	{
		return bean_object_type_descriptor("Double");
	}
	
	virtual std::string to_string() override
	{
		return std::to_string(value_);
	}

	virtual std::shared_ptr<bean_object> lh_pow(const std::shared_ptr<bean_object>& rh) override
	{
		switch (rh->type())
		{
		case BeanObjectType::INT:
		{
			const auto rh_double = double(rh->as<std::int32_t>());
			const auto lh_double = double(value_);
			auto pow_result = pow(lh_double, rh_double);

			return std::make_shared<bean_object_double>(pow_result);
		}
		break;
		case BeanObjectType::DOUBLE:
		{
			const auto rh_double = double(rh->as<double>());
			const auto lh_double = double(value_);
			auto pow_result = pow(lh_double, rh_double);

			return std::make_shared<bean_object_double>(pow_result);
		}
		break;
		default:
			throw std::exception("bean_object_integer::lh_pow cannot deal with right hand object of type");
			break;
		}
	}
	
	virtual std::shared_ptr<bean_object> lh_divide(const std::shared_ptr<bean_object>& rh) override
	{
		switch (rh->type())
		{
		case BeanObjectType::INT:
		{
			const auto rhAsDouble = static_cast<double>(rh->as<std::int32_t>());

			return std::make_shared<bean_object_double>(value_ / rhAsDouble);
		}
		break;
		case BeanObjectType::DOUBLE:
		{
			return std::make_shared<bean_object_double>(value_ / rh->as<double>());
		}
		break;
		default:
			throw std::exception("bean_object_double::lh_divide cannot deal with right hand object of type");
			break;
		}
	}

	virtual std::shared_ptr<bean_object> lh_multiply(const std::shared_ptr<bean_object>& rh) override
	{
		switch (rh->type())
		{
		case BeanObjectType::INT:
		{
			const auto rhAsDouble = static_cast<double>(rh->as<std::int32_t>());

			return std::make_shared<bean_object_double>(value_ * rhAsDouble);
		}
		break;
		case BeanObjectType::DOUBLE:
		{
			return std::make_shared<bean_object_double>(value_ * rh->as<double>());
		}
		break;
		default:
			throw std::exception("bean_object_double::lh_multiply cannot deal with right hand object of type");
			break;
		}
	}
	
	virtual std::shared_ptr<bean_object> lh_plus(const std::shared_ptr<bean_object>& rh) override
	{
		switch (rh->type())
		{
		case BeanObjectType::INT:
		{
			const auto rhAsDouble = static_cast<double>(rh->as<std::int32_t>());
				
			return std::make_shared<bean_object_double>(value_ + rhAsDouble);
		}
		break;
		case BeanObjectType::DOUBLE:
		{
			return std::make_shared<bean_object_double>(value_ + rh->as<double>());
		}
		break;
		default:
			throw std::exception("bean_object_double::plus cannot deal with right hand object of type");
			break;
		}
	}

	virtual std::shared_ptr<bean_object> lh_minus(const std::shared_ptr<bean_object>& rh) override
	{
		switch (rh->type())
		{
		case BeanObjectType::INT:
		{
			const auto rhAsDouble = static_cast<double>(rh->as<std::int32_t>());

			return std::make_shared<bean_object_double>(value_ - rhAsDouble);
		}
		break;
		case BeanObjectType::DOUBLE:
		{
			return std::make_shared<bean_object_double>(value_ - rh->as<double>());
		}
		break;
		default:
			throw std::exception("bean_object_double::plus cannot deal with right hand object of type");
			break;
		}
	}
private:
	double value_;
};


inline std::shared_ptr<bean_object> bean_object_integer::lh_multiply(const std::shared_ptr<bean_object>& rh)
{
	switch (rh->type())
	{
	case BeanObjectType::INT:
	{
		return std::make_shared<bean_object_integer>(value_ * rh->as<std::int32_t>());
	}
	break;
	case BeanObjectType::DOUBLE:
	{
		const auto double_value = static_cast<double>(value_);
		return std::make_shared<bean_object_double>(double_value * rh->as<double>());
	}
	break;
	default:
		throw std::exception("bean_object_integer::lh_multiply cannot deal with right hand object of type");
		break;
	}
}

inline std::shared_ptr<bean_object> bean_object_integer::lh_divide(const std::shared_ptr<bean_object>& rh)
{
	switch (rh->type())
	{
	case BeanObjectType::INT:
	{
		const auto rh_double = double(rh->as<std::int32_t>());
		const auto lh_double = double(value_);
		double div_result = lh_double / rh_double;
			
		return std::make_shared<bean_object_double>(div_result);
	}
	break;
	case BeanObjectType::DOUBLE:
	{
		const auto double_value = static_cast<double>(value_);
		return std::make_shared<bean_object_double>(double_value / rh->as<double>());
	}
	break;
	default:
		throw std::exception("bean_object_integer::lh_divide cannot deal with right hand object of type");
		break;
	}
}

inline std::shared_ptr<bean_object> bean_object_integer::lh_pow(const std::shared_ptr<bean_object>& rh)
{
	switch (rh->type())
	{
	case BeanObjectType::INT:
	{
		const auto rh_double = double(rh->as<std::int32_t>());
		const auto lh_double = double(value_);
		double pow_result = pow(lh_double, rh_double);
			
		return std::make_shared<bean_object_double>(pow_result);
	}
	break;
	case BeanObjectType::DOUBLE:
	{
		const auto rh_double = double(rh->as<double>());
		const auto lh_double = double(value_);
		auto pow_result = pow(lh_double, rh_double);
			
		return std::make_shared<bean_object_double>(pow_result);
	}
	break;
	default:
		throw std::exception("bean_object_integer::lh_pow cannot deal with right hand object of type");
		break;
	}
}

inline std::shared_ptr<bean_object> bean_object_integer::lh_minus(const std::shared_ptr<bean_object>& rh)
{
	switch (rh->type())
	{
	case BeanObjectType::INT:
	{
		return std::make_shared<bean_object_integer>(value_ - rh->as<std::int32_t>());
	}
	break;
	case BeanObjectType::DOUBLE:
	{
		const auto rh_double = double(rh->as<double>());
		const auto lh_double = double(value_);
		double pow_result = lh_double - rh_double;

		return std::make_shared<bean_object_double>(pow_result);
	}
	break;
	default:
		throw std::exception("bean_object_integer::lh_minus cannot deal with right hand object of type");
		break;
	}
}

inline std::shared_ptr<bean_object> bean_object_integer::lh_plus(const std::shared_ptr<bean_object>& rh)
{
	switch (rh->type())
	{
	case BeanObjectType::INT:
	{
		return std::make_shared<bean_object_integer>(value_ + rh->as<std::int32_t>());
	}
	break;
	case BeanObjectType::DOUBLE:
	{
		const auto rh_double = double(rh->as<double>());
		const auto lh_double = double(value_);
		double pow_result = lh_double + rh_double;

		return std::make_shared<bean_object_double>(pow_result);
	}
	break;
	default:
		throw std::exception("bean_object_integer::plus cannot deal with right hand object of type");
		break;
	}
}