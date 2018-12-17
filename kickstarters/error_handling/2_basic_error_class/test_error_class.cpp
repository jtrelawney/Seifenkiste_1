#include <error_class.h>

#include <iostream>

error_class error_object;


error_class::state_categories test_error_1(int threshold){
	
	error_class::state_categories return_value = error_class::state_categories::ok;
	if (threshold==0) return_value = error_class::state_categories::ok;
	if (threshold>=10) return_value = error_class::state_categories::ok;
	if (threshold>=20) return_value = error_class::state_categories::warning;
	if (threshold>=40) return_value = error_class::state_categories::error;
	if (threshold>=80) return_value = error_class::state_categories::shutdown;
	return return_value;
}

error_class::state_def recovery(int threshold){
	error_class::state_def result = error_class::state_def::header_message_error;	
	if (threshold ==4 ) result = error_class::state_def::client_shutdown_confirmed;
	return result;
}

int main(){

	int threshold = 20;
	error_class::state_categories result = test_error_1(threshold);

	if (result == error_class::state_categories::ok) {
		std::cout << "all is well " << std::endl;
		return 0;
	}

	if (result == error_class::state_categories::shutdown) {
		std::cout << "shutting down" << std::endl;
		threshold+=10;
	}

	if (result == error_class::state_categories::warning) {
		std::cout << "warning received , try to recover" << std::endl;
		threshold = 40;
	}
	
	if (result == error_class::state_categories::error) {
			std::cout << "error received, what is the matter" << std::endl;
			std::string message = error_object.get_error_message();
			return 0;
	}

	// ok and error, done
	// warning and shutdown, try to find out more and recover
	error_class::state_def result_recovery = recovery(threshold);
	
	if ( result_recovery !=  error_class::state_def::ok) {
		std::cout << "could not clear the warning" << std::endl;
	} else {
		std::cout << "recovery successful" << std::endl;
	}
	
	error_class::error_state_class e;
	e.error_state = error_class::state_def::ok;
	e.error_category = error_class::state_categories::ok;
	e.error_message = "supertest";
	
	std::cout << "size of shit = " << sizeof(error_class::error_state_class) << std::endl;
	std::cout << "size of e shit = " << sizeof(e) << std::endl;

    return 0;
}
