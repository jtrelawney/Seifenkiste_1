// this is to develop a error handling framework primarily for the tcp class

// idea:

// a called function returns the result but also set the new status
// the return value could be the error object or its substate and the result value could be a parameter reference
// the return value is the function result indicating the error state (e.g. -1 for an expected pos value / nullptr) and the error state will be questioned
// the return value is the function result and caller needs to question the error state in any case


// the error state is a simple structure to save a status, the corresponding message and a categoy for the caller to make a high level decision how to proceed
// A. status - enum with all states which can be observed
// B. message related to the status
// C. category - for each status the library and the caller can take a 
// 1. ok
// 2. warning - something is wrong but the error was handeld in the called library, and should be handled by the caller
//		- for example the header buffer was received but couldn't be validated, tcp is ready to continue, the caller should reset tcp and receive the next header
// 3. error - somehting went seriously wrong - tcp cannot continue
// 4. end of operation - tcp has received a shutdown request (e.g. client is shutting down / message queue is shutting down etc)

// object
// the error state can be a tcp member to be queried
// the error state object can be instiated and returned for each call

#ifndef ERROR_CLASS_H
#define ERROR_CLASS_H

#include <iostream>
#include <vector>
#include <string>

class error_class {

public:
	enum class state_categories : int {
		ok = 0,
		warning,
		error,
		shutdown
	};
	
	enum class state_def : int {
		ok = 0,
		header_message_received = 10,
		header_incomplete = 20,
		header_message_error = 40,
		client_shutdown_suspected = 80,
		client_shutdown_confirmed = 81
	};
	
	class error_state_class {
	public:
		error_class::state_def error_state;
		state_categories error_category;
		std::string error_message;
		error_state_class():error_state(state_def::ok), error_category(state_categories::ok),error_message(""){};
		~error_state_class(){};
	};
	
	error_class();
	error_class(state_def status, std::string message);
	~error_class();
	
	void set_error_state(const state_def status, const std::string &message);
	void reset_error_state();

	std::string get_error_message();
	state_categories get_error_category();
	state_def get_error_state();

	bool operator()();
	bool operator!();
	
private:
	state_def state_;
	std::string error_message_;
	
};
    
#endif
