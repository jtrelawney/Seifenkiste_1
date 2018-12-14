#ifndef MESSAGE_CLASS_H_
#define MESSAGE_CLASS_H_

#include <iostream>
#include <unistd.h> //sleep

#include <vector>
#include <memory>

#include <mutex>
#include <condition_variable>

#include "opencv2/opencv.hpp"

#include <common.h>
#include <address_class.h>
#include <buffer_class.h>

// to manage ownership of messages, there can only be one
class message_class; 
typedef std::unique_ptr<message_class> unique_message_ptr;

// creates a test message with some comon parameters
// not only used in the unit test, but also for other tests
unique_message_ptr message_class_unit_test_create_test_message(int id = 1, int width=480, int height=640, int depth=3, int init=100, bool with_output=false);


#ifndef use_global_debug_setting
    const int MESSAGE_DEBUG_LEVEL = 5;
#else
    const int MESSAGE_DEBUG_LEVEL = GLOBAL_DEBUG_SETTING;
#endif

class message_class {
public:

    // message states
    // starts with initialized, when the header parameters are set
    // changes to complete when the message data has been set
    // is invalid when the constructor call invalid parameters are detected
    // is invalid (error_when_constructing_from_buffer) when buffer given to the constructor is not valid
    // becomes invalid (data_handed_over_waiting_to_be_marked_for_deletion) when the data has been passed on and all that is left is the meta data
    // later could be used to set the data again, but this is not implemented 
    enum message_state_def { initialized, complete, invalid, error_when_constructing_from_buffer, data_handed_over_waiting_to_be_marked_for_deletion };
    message_state_def get_state();

    // move paradigm
    message_class(message_class& other) = delete;
    message_class& operator= (message_class& other) = delete;
    message_class(message_class&& other) = default;
    message_class& operator= (message_class&& other) = default;

// ==============================================================================================================
// constructor section and related buffer and tcp functions

    // constructor, used when all meta and data is available (for example, when the sensor acquires data and passes it on)
    message_class(
    // message meta data
    unsigned int message_id, address_class recipient,
    // tcp meta data - if required will be set when the messagequeue dispatches to tcp
    // address_class sender, time_format sender_time,
    // sensor meta data
    address_class origin, time_format sensor_time,
    // data
    std::unique_ptr<cv::Mat> data_mat_ptr
    );

    // constructur with a header buffer, used by the tcp server when the header message has been received
    // this validates the buffer content and extracts all metadata into the message
    // the status afterwards is "initialized", the data is not yet available 
    message_class(buffer_class &HB);
    // after the data message has been received the data can be inserted into the message
    // this moves the cvmat from the DB into the message, which is then set to status complete
    int insert_data_buffer(buffer_class &&DB);
    // before a message can be received a buffer has to be provided to tcp to fill the data in
    // this function creates a data buffer with a properly sized cvmat contained, so that tcp can insert the data there
    // design decision was to let the message object create the buffer because it knows the expected data diemnsions from the header data
    buffer_class get_properly_sized_empty_data_buffer_according_to_header_data();

    /*
    delete when not required anymore
    // constructor, used when all meta and data is available (for example, by sensor)
    message_class(
    // message meta data
    unsigned int message_id, address_class recipient,
    // tcp meta data - if required will be set when the messagequeue dispatches to tcp
    // address_class sender, time_format sender_time,
    // sensor meta data
    address_class origin, time_format sensor_time,
    // data
    unsigned int data_length, int rows = 0, int cols = 0, int mattype = 0,
    std::unique_ptr<cv::Mat> data_mat_ptr = std::unique_ptr<cv::Mat> ( new cv::Mat(1,1,CV_8UC3, cv::Scalar(0,0,0)) )
    );
    */

    // to send messages over tcp the message has to be divided into a header and data part
    // these functions extract the relevant data and encapsulate it with access to it with char*, length
    buffer_class create_header_buffer();        // this creates a vetor buffer and inserts the meta data
    buffer_class create_data_buffer();          // this moves the cvmat into a buffer (ownership for cvmat changes , only one copy, mark as invalid??)

// ==============================================================================================================

    // to manage recipients
    void set_recipient_address(const address_class &recipient);
    address_class get_recipient_address();

    // extracts the unique ptr to the data mat (used by data recipient)
    std::unique_ptr<cv::Mat> fetch_data();

    // to set and get message parameters
	void set_id(unsigned int id);
	unsigned int get_id();

    time_format get_sensor_time();


    // below can maybe deleted

    void set_sender_address(const address_class sender);	
    void set_sender_time(time_format t);
   
    unsigned int get_data_size();


    // debuging
    void print_meta_data();
    void print_data(unsigned int howmany=10);
	int write_to_file(const char *fn="./test.jpeg");

     // destructor, free the memory for the data buffer and the spcial param buffer
	~message_class();
	
private:

    /*------------------------------------------------------------------------------*/
    // no access to standard constructor
    message_class();


    // message meta data
	unsigned int message_id_;
    address_class recipient_;

    // this will be filled by the messagequeue when it dispatches to tcp
    address_class sender_;
    time_format sender_time_;
   
    // sensor meta data
    address_class origin_;
	// sensor time
	time_format sensor_time_;

    // sensor data, set of params which describe the cvmat structure
    cvMat_params_class data_params_;

    // goal : delete those
	//unsigned int data_length_;
    //int rows_, cols_ , channels_;

	// ptr to data = cvmat
    std::unique_ptr<cv::Mat> data_ptr_;


    // object state management

	// keep the own state, desc see above
    message_state_def state_;
    int message_debug_level_;
};

#endif
