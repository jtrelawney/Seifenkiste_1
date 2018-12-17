#ifndef RETURN_TYPE_CLASS_H
#define RETURN_TYPE_CLASS_H

#include <memory>

// remarks

// adding a state to the constructor removes the simplicity of the automatic constructor call at the end of the function
// nothing() is not possible instead we need VON(nothing(), state...)
// 

struct Nothing {};

template <typename T>
class Return_type {

private:
	Return_type() : isSomething_(false){}
	
public:
    
	// default constructors
	Return_type(Nothing nothing) : isSomething_(false), nothing_(nothing) {}
	Return_type(T &&something) : isSomething_(true), something_(std::move(something)) {}
	
	// copy constructors
	Return_type(Return_type& other) = delete;
	Return_type &operator=(const Return_type &other) = delete;
	/*
	{
		something_ = other.something_;
		isSomething_ = other.isSomething_;
		return *this;
	}
	*/
	
	// move constructors
	Return_type(Return_type &&other) : isSomething_(std::move(other.isSomething_))
	{
		other.isSomething_ = false;
	}

	/*
	Return_type(Return_type &&other) : isSomething_(std::move(other.isSomething_)) , something_(std::move(other.something_))
	{
		other.isSomething_ = false;
	}
	*/

	Return_type &operator=(Return_type &&other)
	{
		something_ = std::move(other.something_);
		isSomething_ = std::move(other.isSomething_);
		other.isSomething_ = false;
		return *this;
	}

	// comparisons
	inline constexpr explicit operator bool(void) const { return isSomething_; }
	inline constexpr bool is_something(void) const { return isSomething_; }
	inline constexpr bool is_nothing(void) const { return !isSomething_; }

	// unpack the data
	inline T unwrap(void) const { return something_; }
	
private:
	bool isSomething_;
	T something_;
	// added this because param nothing was unused in the option(nothing) constructor
	Nothing nothing_;
};


    
#endif

