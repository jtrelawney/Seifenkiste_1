#include <message_class.h>

unique_message_ptr message_class_unit_test_create_test_message(int id, int width, int height, int depth, int init, bool with_output){

    std::cout << "\n\n\n=============================================================================================" << std::endl;
    std::cout << "unit test messaging class" << std::endl;
    std::cout << "gathering some test data" << std::endl;

    unsigned int message_id = id;

    // create the test cvmat
    char dim = CV_8UC3;
    if (depth == 3) dim = CV_8UC3;
    std::unique_ptr<cv::Mat> data_mat = std::unique_ptr<cv::Mat> ( new cv::Mat(width,height, dim, cv::Scalar(init,init,init) ) );
    cvMat_params_class test_params(*data_mat.get());
    if (with_output) test_params.print_cvMat_params();

    int rows, cols, channels, elem_size, type;
    test_params.get_params(rows, cols, channels, elem_size, type);

    // address business	
    address_class::platform_type_def platform;
    address_class::sensor_type_def sensor;
    address_class::process_type_def process;

    platform = address_class::platform_type_def::pc;
    sensor = address_class::sensor_type_def::undefined_sensor;
    process = address_class::process_type_def::cockpit;
    address_class recipient_addr(platform,sensor,process);

    platform = address_class::platform_type_def::arduino;
    sensor = address_class::sensor_type_def::imu1;
    process = address_class::process_type_def::undefined;
    address_class sender_addr(platform,sensor,process);
    time_format sender_time = get_time_stamp();

    platform = address_class::platform_type_def::rpi;
    sensor = address_class::sensor_type_def::camera1;
    process = address_class::process_type_def::undefined;
    address_class sensor_addr(platform,sensor,process);
	time_format sensor_time = sender_time - 1000;
    
    std::cout << "recipient = " << recipient_addr << std::endl;
    if (with_output) std::cout << "sender = " << sender_addr << std::endl;
    if (with_output) std::cout << "sensor = " << sensor_addr << std::endl;

    // now create the message and add sender info
    unique_message_ptr message = unique_message_ptr( new message_class(
        message_id, recipient_addr,
        sensor_addr, sensor_time,
        std::move(data_mat)
        )
    );

    message -> set_sender_address(sender_addr);
    message -> set_sender_time(sender_time);

    std::cout << "\n\ntest message data created" << std::endl;

    std::cout << "\n\n\n=============================================================================================" << std::endl;
    return std::move(message);
}

// this takes the data dimensions as defined by the header data and creates an empty cvmat buffer 
// used to pass to tcp receipt, which needs a proper cvmat buffer
buffer_class message_class::get_properly_sized_empty_data_buffer_according_to_header_data(){
    //std::unique_ptr<cv::Mat> data_mat = std::unique_ptr<cv::Mat> ( new cv::Mat( rows_, cols_, channels_ ) );
    
    if (message_debug_level_>0) std::cout << "message_class : get_properly_sized_empty_data_buffer_according_to_header_data : creating mat and buffer" << std::endl;
    std::cout << "message_class : get_properly_sized_empty_data_buffer_according_to_header_data : verify correct params for mat creation!!" << std::endl;
    int rows, cols, channels, elem_size, type;
    data_params_.get_params(rows, cols, channels, elem_size, type);
    std::unique_ptr<cv::Mat> data_mat = std::unique_ptr<cv::Mat> ( new cv::Mat( rows, cols, type) );
            
    return buffer_class(std::move(data_mat) );
}

std::unique_ptr<cv::Mat> message_class::fetch_data(){
    state_ = message_state_def::data_handed_over_waiting_to_be_marked_for_deletion;
    return std::move(data_ptr_);
}

void message_class::set_recipient_address(const address_class &recipient){ recipient_ = recipient; }
address_class message_class::get_recipient_address(){ return recipient_; }
void message_class::set_sender_address(const address_class sender){ sender_ = sender; }
void message_class::set_sender_time(time_format t){ sender_time_ = t; }
void message_class::set_id(unsigned int id){ message_id_ = id;}
unsigned int message_class::get_id() { return message_id_;}
time_format message_class::get_sensor_time(){ return sensor_time_;}


// default constructor, creates empty shell
message_class::message_class():
        message_id_(0), recipient_(address_class()),
        sender_(address_class()), sender_time_(0),
        origin_(address_class()), sensor_time_(0),
        //data_length_(0), rows_(0), cols_(0), channels_(0), data_ptr_(nullptr),
        state_(message_state_def::invalid), message_debug_level_(MESSAGE_DEBUG_LEVEL) {
    std::cout << "message_class : default constructor is private\n";
}

// constructor, used when all meta and data is available (for example, when the sensor acquires data and passes it on)
message_class::message_class(
    unsigned int message_id, address_class recipient,
    address_class origin, time_format sensor_time,
    std::unique_ptr<cv::Mat> data_mat_ptr
    ) : message_id_(message_id), recipient_(recipient),
        sender_(address_class()), sender_time_(0),
        origin_(origin), sensor_time_(sensor_time),
        data_ptr_(std::move(data_mat_ptr)),
        state_(message_state_def::initialized), message_debug_level_(MESSAGE_DEBUG_LEVEL)
{   
    if (message_debug_level_>0) std::cout << "message_class : constructor with data ..." << std::endl;
    if (data_ptr_ == nullptr) {
        if (message_debug_level_>0) std::cout << "message_class : constructor : data is nullptr - invalid" << std::endl;
        state_ = message_state_def::invalid;
    } else {
        if (message_debug_level_>1) std::cout << "message_class : constructor : extracting data params" << std::endl;
        data_params_ = cvMat_params_class(*data_ptr_.get());
        state_ = message_state_def::complete;
    }
}

message_class::~message_class() {
	if (message_debug_level_>0) std::cout << "\n\nmessage class - releasing message buffer, object = " << (unsigned long) this << std::endl;
}

// creates a data buffer from the existing message data
buffer_class message_class::create_data_buffer(){

    buffer_class data_buffer(std::move(data_ptr_));

    if (state_ != complete) {
        //if (message_debug_level_>0) std::cout << "message_class : create_data_buffer : failed buffer creation, incomplete message" << std::endl;
        std::cout << "message_class : create_data_buffer : failed buffer creation, incomplete message" << std::endl;
        return data_buffer;
    }
    
    if (data_buffer.get_buffer_type() == buffer_class::buffer_type_def::cvmat) {
        if (message_debug_level_>0) std::cout << "message_class : create_data_buffer : buffer creation successful" << std::endl;
        state_ = message_state_def::data_handed_over_waiting_to_be_marked_for_deletion;
        return data_buffer;
    } else {
        if (message_debug_level_>0) std::cout << "message_class : create_data_buffer : failed buffer creation" << std::endl;
    }

    return data_buffer;
}

// push all metadata into the header
buffer_class message_class::create_header_buffer(){

    buffer_class header_buffer(TCP_HEADER_LENGTH);
    header_buffer.insert(0,"head");
    header_buffer.insert(4,message_id_);
    header_buffer.insert(8,(int)recipient_.get_platform());
    header_buffer.insert(12,(int)recipient_.get_sensor());
    header_buffer.insert(16,(int)recipient_.get_process());
    header_buffer.insert(20,sender_.get_platform());
    header_buffer.insert(24,sender_.get_sensor());
    header_buffer.insert(28,sender_.get_process());
    header_buffer.insert(32,sender_time_);
    header_buffer.insert(36,origin_.get_platform());
    header_buffer.insert(40,origin_.get_sensor());
    header_buffer.insert(44,origin_.get_process());
    header_buffer.insert(48,sensor_time_);
    // data params
    int rows,cols,channels,mat_type,elem_size;
    data_params_.get_params( rows, cols, channels, elem_size, mat_type);

    header_buffer.insert(52,rows);
    header_buffer.insert(56,cols);
    header_buffer.insert(60,channels);
    header_buffer.insert(64,elem_size);
    header_buffer.insert(68,mat_type);
    header_buffer.insert(72,"ende");
    return std::move(header_buffer);
}

// this initializes a message from the given header buffer
// first extract the header paramaters and validate the content
// if they are extracted directly into the message members this may cause issues later when the message data is inserted or the message is processed otherwise
// when all parameters are fine create the message
message_class::message_class(buffer_class &header_buffer) : message_class(){

    if (message_debug_level_>0) std::cout << "message_class : constructor with header-buffer" << std::endl;
    if (message_debug_level_>1) header_buffer.print_buffer_meta_data();

    int result = 0;

	std::string a,b;
	result+= header_buffer.extract(0,4,a);
	result+= header_buffer.extract(72,4,b);
    bool proper_header = (a=="head") && (b=="ende");

    if (message_debug_level_>1) {
        std::cout << "header qualifiers begin and end = " << a << " , " << b << std::endl;
     	std::cout << "proper header = " << proper_header << std::endl;
    }

    //helper
    int value;
    address_class::platform_type_def platform;
    address_class::sensor_type_def sensor;
    address_class::process_type_def process;

    // id
    result+= header_buffer.extract(4, message_id_);
    // recipient
    result+= header_buffer.extract(8, value);
    platform = (address_class::platform_type_def) value;
    result+= header_buffer.extract(12, value);
    sensor = (address_class::sensor_type_def) value;
    result+= header_buffer.extract(16, value);
    process = (address_class::process_type_def) value;
    recipient_ = address_class(platform,sensor,process);
    //sender
    result+= header_buffer.extract(20, value);
    platform = (address_class::platform_type_def) value;
    result+= header_buffer.extract(24, value);
    sensor = (address_class::sensor_type_def) value;
    result+= header_buffer.extract(28, value);
    process = (address_class::process_type_def) value;
    sender_ = address_class(platform,sensor,process);
    // snder time
    result+= header_buffer.extract(32, sender_time_);
    //origin
    result+= header_buffer.extract(36, value);
    platform = (address_class::platform_type_def) value;
    result+= header_buffer.extract(40, value);
    sensor = (address_class::sensor_type_def) value;
    result+= header_buffer.extract(44, value);
    process = (address_class::process_type_def) value;
    origin_ = address_class(platform,sensor,process);
    // sensortime
    result+= header_buffer.extract(48, sensor_time_);
    // data params
    int rows,cols,channels,mat_type,elem_size;
    result+= header_buffer.extract(52, rows);
    result+= header_buffer.extract(56, cols);
    result+= header_buffer.extract(60, channels);
    result+= header_buffer.extract(64, elem_size);
    result+= header_buffer.extract(68, mat_type);

    if (message_debug_level_>1) std::cout << "message_class : constructor with header buffer : creating data parameters" << std::endl;
    data_params_ = cvMat_params_class(rows,cols,channels,elem_size,mat_type);
    data_params_.print_cvMat_params();
    // the result variable accumulates the extraction results, they are 0 for success and <0 for issues
    // if all are ok (=0) the header data was valid and has been successfully extracted
    if (result<0) state_ = message_state_def::error_when_constructing_from_buffer;
    else state_ = message_state_def::initialized;
}


// after receiving the data message over tcp a buffer with the data can be inserted into the message to make it complete
int message_class::insert_data_buffer(buffer_class &&data_buffer){
    
    if (message_debug_level_>1) std::cout << "message_class : insert_data_buffer - storing data from buffer into message" << std::endl;

    // from the buffer extract the cvmat (move logic to take owner ship)
    data_ptr_ = data_buffer.get_cvmat_ptr();

    // compare the cvmat parameters
    cvMat_params_class new_data_params(*data_ptr_.get());
    bool result = (data_params_ == new_data_params);
    std::cout << "received result = " << result << std::endl;

    if (message_debug_level_>1) {
        std::cout << "message_class : insert_data_buffer : compare the expected and the received data params" << std::endl;
        if (message_debug_level_>2) {
            std::cout << "\n\nexpected:" << std::endl;
            data_params_.print_cvMat_params();
            std::cout << "\n\nreceived:" << std::endl;
            new_data_params.print_cvMat_params();
        }
    }

    if (result != true) {
        if (message_debug_level_>1) std::cout << "message_class : insert_data_buffer - buffer data does not hhave correct parameters" << std::endl;
        state_ = message_state_def::error_when_constructing_from_buffer;
        return -1;
    }

    if (message_debug_level_>1) std::cout << "message_class : insert_data_buffer - buffer data has correct parameters" << std::endl;
    data_params_ = new_data_params;
    state_ = message_state_def::complete;

    return 0;
}

message_class::message_state_def message_class::get_state(){ return state_; }

void message_class::print_meta_data(){

	std::cout << "\n-------------------------\n";
    std::cout << "\n\nmessage metadata:\n";
	
    std::cout << "message ptr" << (unsigned long)this << std::endl;	
    if (state_ == initialized) std::cout << "message status = initialized\n" << std::endl;	 
    if (state_ == complete) std::cout << "message status = complete\n" << std::endl;
    if (state_ == invalid) std::cout << "message status = invalid  (corrupted during construction or move operator applied)\n" << std::endl;
    if (state_ == error_when_constructing_from_buffer) std::cout << "message status = error_when_constructing_from_buffer\n" << std::endl;
    if (state_ == data_handed_over_waiting_to_be_marked_for_deletion) std::cout << "message status = data_handed_over_waiting_to_be_marked_for_deletion\n" << std::endl;

	std::cout << "message ID = " << message_id_ << std::endl;
	std::cout << "recipient : " << recipient_ << std::endl;
	std::cout << "sender : " << sender_ << std::endl;
    std::cout << "sender time " << sender_time_ << std::endl;
	std::cout << "sensor = " << origin_ << std::endl;
    std::cout << "sensor_time time " << sensor_time_ << std::endl;
    unsigned long address = (unsigned long) data_ptr_.get();
    std::cout << "cvmat address = " << (void*)address << std::endl;
    std::cout << "cvmat data params" << std::endl;
    data_params_.print_cvMat_params();
    if (state_ != complete) {
        std::cout << "message not complete yet, data not available" << std::endl;
    } else {
        print_data(20);
    }
    
    std::cout << "\n-------------------------\n";
}

void message_class::print_data(unsigned int howmany){
    std::cout << "\nmessage data" << std::endl;
    std::cout << "\n\n\n message_class : print_data ()  - how and when to use that" << std::endl;

    cv::Mat *data = data_ptr_.get();
    unsigned char *d = data->data;
    if (data == nullptr) {
        std::cout << "data nullptr" << std::endl;
        return;
    } else {
        for (unsigned int i=0; i<howmany; i++){
            std::cout << (int)d[i] << " , ";
        }
        std::cout << std::endl;
    }
}

/*
// this creates a cvmat from the buffer, with data size paramters as determined by the header (and already available
int message_class::push_data(buffer_class &data_buffer){

    //remove
    if (data_buffer.get_data_size()<0) return 0;

    int result = 0;
    if (state_ != message_state_def::initialized) return -1;
    
    // the empty buffer is created by the owner, tcp then reads the data massage with the proper data size
    // then tcp creates and attaches the cvmat to this buffer
    // this cvmat needs to be taken over and stored as the data ptr for the message

    std::cout << "message class , push data from buffer" << std::endl;

    // first get the data - it is a cvmat
    char* get_data_ptr();  // vector.data()
    int get_data_size();
    //data_ptr_ = std::move(data_buffer);

    state_ = message_state_def::complete;
    return result;
}




//int insert_data_buffer(buffer_type &data_buffer_to_fill){



// with a given headerbuffer, extract the meta data and initialize the message
message_class::message_class(buffer_class &header_buffer) : message_class() {

    if (message_debug_level_>0) std::cout << "message_class : constructor with header buffer" << std::endl;

    int result = 0;

	std::string a,b;
	result+= header_buffer.extract(0,4,a);	    //con
	result+= header_buffer.extract(34,4,b);	    //end
    bool proper_header = (a=="head") && (b=="ende");

    if (message_debug_level_>1) {
        std::cout << "header qualifiers begin and end = " << a << " , " << b << std::endl;
     	std::cout << "proper header = " << proper_header << std::endl;
    }

    int rows;
    unsigned int type_def_value;

    result+= header_buffer.extract(4, message_id_);          // pos 3 size 4
    result+= header_buffer.extract(8, type_def_value);       // pos 7 size 1
    sender_ = (platform_type_def) type_def_value;
    result+= header_buffer.extract(12, sender_time_);         // pos 8 size 4
    result+= header_buffer.extract(16, type_def_value); //sensor_platform_);    // pos 12 size 1
    sensor_platform_ = (platform_type_def) type_def_value;
    result+= header_buffer.extract(20, type_def_value); //sensor_type_);        // pos 13 size 1
    sensor_type_ = (sensor_type_def) type_def_value;
    result+= header_buffer.extract(24, sensor_time_);        // pos 14 size 4
    result+= header_buffer.extract(28, data_length_);        // pos 18 size 4
    result+= header_buffer.extract(32,rows_);                // pos 22 size 4
    result+= header_buffer.extract(36,cols_);                // pos 22 size 4
    result+= header_buffer.extract(40,channels_);                // pos 22 size 4
    result+= header_buffer.extract(44, rows);                // pos 22 size 4

    if (result<0) state_ = message_state_def::error_when_constructing_from_buffer;
    else state_ = message_state_def::initialized;
    

    // the result variable accumulates the extraction results
    // if all are ok (=0) we return 0 here , ok
    // if there is at least one failure then the result is negative
    // for a successful extraction we would have 0 as a total aggregated result.
    //return result;
}





void message_class::set_id(unsigned int id){ message_id_ = id; }



*/
