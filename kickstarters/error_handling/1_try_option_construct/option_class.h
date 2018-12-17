//discivered at
//http://cwoodall.com/blog/2016/10/10/cpp-option.html

// extended version with copy ops and consturctor
//https://github.com/cwoodall/cpp-option/blob/master/include/option.h

#include <memory>
struct Nothing {};

template <typename T>
class Option {

public:
    
	// default constructors
	Option() : isSomething_(false) {}
	Option(Nothing nothing) : isSomething_(false), nothing_(nothing) {}
	Option(T something) : isSomething_(true), something_(something) {}
	
	// copy constructors
	Option(Option& other) = delete;
	Option &operator=(const Option &other) {
		something_ = other.something_;
		isSomething_ = other.isSomething_;
		return *this;
	}

	// move constructors
	Option(Option &&other) : isSomething_(std::move(other.isSomething_)) , something_(std::move(other.something_))
	{
	other.isSomething_ = false;
	}

	Option &operator=(Option &&other)
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
