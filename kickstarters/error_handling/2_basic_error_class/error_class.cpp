
#include<error_class.h>

error_class::error_class(){
	reset_error_state();
}

error_class::error_class(state_def status, std::string message) : state_(status), error_message_(message){
}

error_class::~error_class(){}

void error_class::set_error_state(const state_def status, const std::string &message){
	error_message_ = message; state_ = status;
}

void error_class::reset_error_state(){
	error_message_ = "ok"; state_ = state_def::ok;
}

std::string error_class::get_error_message() {
	//int index = static_cast<int>(state_);
	//return error_messages_[index];
	return error_message_;
}

error_class::state_categories error_class::get_error_category() {
	return state_categories::ok;
}

error_class::state_def error_class::get_error_state() {
	return state_;
}

/*
bool operator()(){
}

bool operator!(){
}
*/
