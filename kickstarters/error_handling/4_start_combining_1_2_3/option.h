struct Nothing {};

template <typename T>
class Option {
 public:
  Option() : isSomething_(false) {}

  Option(Nothing nothing) : isSomething_(false) {}

  Option(T something) : isSomething_(true), something_(something) {}

  // The copy/move functions have been left out see the github link for details.

  /**
   * An explicit conversion to bool, which makes boolean comparisons of the
   * Option type possible
   *
   * @return isSomething_ (true if it is something, false otherwise)
   */
  inline constexpr explicit operator bool(void) const { return isSomething_; }

  /**
   * Get the value stored inside of the Option<T> type
   *
   * @return something_;
   */
  inline T unwrap(void) const { return something_; }

 private:
  bool isSomething_;  ///< @brief stores whether Some(T) or Nothing is stored
  T something_;       ///< @brief temporary storage for the something object
};

