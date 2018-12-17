#ifndef RETURN_TYPE_CLASS_H
#define RETURN_TYPE_CLASS_H

#include <memory>
#include <tcp_error_defs.h>

// remarks

// adding a state to the constructor removes the simplicity of the automatic constructor call at the end of the function
// nothing() is not possible instead we need VON(nothing(), state...)
// 

struct Nothing {};

template <typename T>
class Returntype {

private:
	Returntype() : isSomething_(false), error_state_(error_category_def::ok){}
	
public:
    
	// default constructors
	Returntype(Nothing nothing, error_category_def category) : isSomething_(false), error_category_(category), nothing_(nothing) {}
	Returntype(T something) : isSomething_(true), error_category_(error_category_def::ok), something_(something) {}
	
	// copy constructors
	Returntype(Returntype& other) = delete;
	Returntype &operator=(const Returntype &other) {
		something_ = other.something_;
		isSomething_ = other.isSomething_;
		return *this;
	}

	// move constructors
	Returntype(Returntype &&other) : isSomething_(std::move(other.isSomething_)) , something_(std::move(other.something_))
	{
	other.isSomething_ = false;
	}

	Returntype &operator=(Returntype &&other)
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
	
	void print_error_state(){ std::cout << "error state" << std::endl; };

private:
	bool isSomething_;
	error_category_def error_category_;
	tcp_state_def error_state_;
	T something_;
	// added this because param nothing was unused in the option(nothing) constructor
	Nothing nothing_;
};


    
#endif

