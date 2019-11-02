#pragma once
#include <iostream>
#include <memory>

// double, integer

enum class ObjectType
{
	INVALID,
	None,
	REFERENCE,
	CUSTOM,
	DOUBLE,
	INT
};

inline bool ObjectTypeIsPrimitive(const ObjectType type)
{
	return type != ObjectType::INVALID && type != ObjectType::CUSTOM && type != ObjectType::REFERENCE;
}

class ast_object
{
public:
	ast_object() : type_(ObjectType::INVALID) {}
	ast_object(const ObjectType type) : type_(type) {}

	virtual std::shared_ptr<ast_object> lh_plus(const std::shared_ptr<ast_object>& rh)
	{
		std::cout << "TODO: ast_object::lh_plus not implemented!";
		return std::make_shared<ast_object>(ObjectType::INVALID);
	}

	virtual std::shared_ptr<ast_object> rh_plus(const std::shared_ptr<ast_object>& lh)
	{
		std::cout << "TODO: ast_object::rh_plus not implemented!";
		return std::make_shared<ast_object>(ObjectType::INVALID);
	}

	virtual void set(const std::shared_ptr<ast_object>& value)
	{
		throw std::exception("Error: This object is not assignable!");
	}

	[[nodiscard]] ObjectType type() const
	{
		return type_;
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
	ObjectType type_;
	void* object_;
};

class ast_object_reference : public ast_object
{
public:
	ast_object_reference(std::shared_ptr<ast_object> value) : ast_object(ObjectType::REFERENCE)
	{
		value_ = value;
		object_ = &value_;
	}

	virtual void set(const std::shared_ptr<ast_object>& value)
	{
		value_ = value;
	}
	
	virtual std::shared_ptr<ast_object> lh_plus(const std::shared_ptr<ast_object>& rh) override
	{
		return value_->lh_plus(rh);
	}
private:
	std::shared_ptr<ast_object> value_;
};

class ast_object_integer : public ast_object
{
public:
	ast_object_integer(std::int32_t value) : ast_object(ObjectType::INT), value_(0xCCCCCCCC)
	{
		value_ = value;
		object_ = &value_;
	}

	virtual std::shared_ptr<ast_object> lh_plus(const std::shared_ptr<ast_object>& rh) override
	{
		switch (rh->type())
		{
		case ObjectType::INT:
		{
			return std::make_shared<ast_object_integer>(value_ + rh->as<std::int32_t>());
		}
		break;
		case ObjectType::DOUBLE:
		{
			const auto double_value = floorf(static_cast<double>(value_) + rh->as<double>());
			auto int_value = static_cast<std::int32_t>(double_value);

			return std::make_shared<ast_object_integer>(int_value);
		}
		break;
		default:
			throw std::exception("ast_object_integer::plus cannot deal with right hand object of type");
			break;
		}
	}
private:
	std::int32_t value_;
};

class ast_object_double : public ast_object
{
public:
	ast_object_double(double value) : ast_object(ObjectType::DOUBLE), value_(std::numeric_limits<double>::quiet_NaN())
	{
		value_ = value;
		object_ = &value_;
	}

	virtual std::shared_ptr<ast_object> lh_plus(const std::shared_ptr<ast_object>& rh) override
	{
		switch (rh->type())
		{
		case ObjectType::INT:
		{
			const auto rhAsDouble = static_cast<double>(rh->as<double>());
				
			return std::make_shared<ast_object_double>(value_ + rhAsDouble);
		}
		break;
		case ObjectType::DOUBLE:
		{
			return std::make_shared<ast_object_double>(value_ + rh->as<double>());
		}
		break;
		default:
			throw std::exception("ast_object_double::plus cannot deal with right hand object of type");
			break;
		}
	}
private:
	double value_;
};