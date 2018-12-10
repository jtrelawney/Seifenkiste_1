#ifndef BUFFER_BASE_CLASS_H
#define BUFFER_BASE_CLASS_H

#include <memory>
#include <vector>
#include <iostream>

const int buffer_debug_level = 1;

typedef std::vector<char> buffer_type;
typedef std::unique_ptr<buffer_type> uptr_buffer_type;

// define the minimum structure a buffer class needs so that when
// passed to tcp object latter can access the char* and length informaation
// it is that data structure is vector char, but tcp send requires a char* and its size

class cvmat{
private:


public:
    /*
    data_buffer_class& operator=(const data_buffer_class&) = delete;
    data_buffer_class( const data_buffer_class&) = delete;
    data_buffer_class& operator=(data_buffer_class&&) = delete; // std copy is sufficient
    // need those for creating a message object with a previously tcp created buffer
    data_buffer_class( data_buffer_class&&) = default;
	*/

    int rows, cols, depth;
    uptr_buffer_type data;

    // copy stuff
    cvmat(cvmat& other) = delete;
    cvmat(const cvmat& other) = delete;
    cvmat& operator=(cvmat& other) = delete;
    cvmat& operator=(const cvmat& other) = delete;

    // move stuff
    //cvmat(cvmat&& other) = delete;
    cvmat(const cvmat&& other) = delete;
    cvmat& operator=(cvmat&& other) = delete;
    cvmat& operator=(const cvmat&& other) = delete;


    // move constructor used by sensor_data to create a cvmat test object and move that into the local member
    // this operation means taking control over of the tempoorary tets cvmat and not only taking the params, but also the unique ptr to the buffer data (vector char
    // this is the only plac this move constructor is used
    // it swaps all members including the uptr
    cvmat(cvmat&& other){
    	if (buffer_debug_level>0) std::cout << "\n\ncvmat using move operation" << std::endl;

    	if (buffer_debug_level>1) {
			std::cout << "self before : " << std::endl;
			print();

			std::cout << "other before :" << std::endl;
			other.print();
    	}

    	std::swap(rows,other.rows);
    	std::swap(cols,other.cols);
    	std::swap(depth,other.depth);
    	std::swap(data,other.data); // = std::move(other.data);

    	if (buffer_debug_level>1) {
			std::cout << "self now :" << std::endl;
			print();

			std::cout << "other now :" << std::endl;
			other.print();
    	}
    }

    cvmat():rows(0),cols(0),depth(0),data(nullptr){
    	if (buffer_debug_level>0) {
    		std::cout << "cvmat default destructor call" << std::endl;
    		std::cout << "cvmat data size = " << rows * cols * depth << std::endl;
    		if (data == nullptr) {
        		std::cout << "cvmat data ptr = nullptr" << std::endl;
    		} else {
    			std::cout << "cvmat data ptr = " << (void *) data->data() << std::endl;
    		}
    	}
    };

    // test init with 3 dims and a default value
    // this should create a cvmat like situation with access to size and a data ptr
    cvmat(int r, int c, int d, int val):rows(r),cols(c),depth(d),data(nullptr){
    	if (buffer_debug_level>0) std::cout << "cvmat constructor with params call" << std::endl;
    	data.reset(new buffer_type(r*c*d) );
    	for(buffer_type::iterator it = data->begin(); it != data->end(); ++it) {
    	    *it=char(val);
    	}
    }

    void print() const {
    	std::cout << "row,col,dep = " << rows << " , " << cols << " , " << depth << std::endl;
    	std::cout << "cvmat address = " << (void *) this << std::endl;

    	int size = rows*cols*depth;
	   	if (size>0) {

			// in cvmat the dta is a unique_ptr to vector<char>
			// to extract the pointer call the get function of the unique ptr
			std::vector<char> *buffer_address = data.get();
			if (buffer_address == nullptr) {
				std::cout << "buffer address = nullptr" << (void *) buffer_address << std::endl;
			} else {
				// in the vector the data pointer can be extracted with the data() function
				// we now have a pointer to the actual data
				char *finally_the_data = buffer_address->data();
				if (finally_the_data == nullptr) {
					std::cout << "buffer address = " << (void *) buffer_address << std::endl;
				} else {
					std::cout << "buffer address = " << (void *) buffer_address << std::endl;
					if (buffer_address == nullptr) {
						std::cout << "buffer address = nullptr" << (void *) buffer_address << std::endl;
					} else {
						std::cout << "buffer content (size = " << size << ")" << std::endl;
						for (int i=0; i<size; i++){
							//the ptr from above points to the array, use the counter as index
							//char content = finally_the_data[i];
							//ptr arithmetik works too
							char content = *finally_the_data;
							finally_the_data++;
							std::cout << int(content) << ",";
						}
						std::cout << std::endl;
					}
				}
			}
	   	}
    }

    unsigned int get_data_size() const {
    	if (buffer_debug_level>0) std::cout << "cvmat : get_data_size() call " << std::endl;
    	return (unsigned int) (rows*cols*depth);
    };

    // added const to the caller chain for this fucntion and here is where it blows up because the move requires a change in the return variable
    uptr_buffer_type get_data_ptr() {
    	if (buffer_debug_level>0) std::cout << "cvmat : get_data_ptr() call " << std::endl;

    	return move(data);
    };

    ~cvmat(){
    	std::cout << "cvmat destructor call" << std::endl;
			if (data) {
				std::cout << "cvmat deleting uptr do buffertype, addr = " << (void *) data.get() << std::endl;
				data.release();
			} else {
				std::cout << "cvmat : uptr to bufferype is nullptr" << std::endl;
			}
    	};

};

class sensor_data_class{
public:
	// metadata
    unsigned int message_id;
    int type;
    int platform;

    // data
    cvmat data;

    // copy stuff
    sensor_data_class(sensor_data_class& other) = delete;
    sensor_data_class(const sensor_data_class& other) = delete;
    sensor_data_class& operator=(sensor_data_class& other) = delete;
    sensor_data_class& operator=(const sensor_data_class& other) = delete;

    // move stuff
    sensor_data_class(sensor_data_class&& other) = default;
    sensor_data_class(const sensor_data_class&& other) = delete;
    sensor_data_class& operator=(sensor_data_class&& other) = delete;
    sensor_data_class& operator=(const sensor_data_class&& other) = delete;

    // pass the returnvalue at the end of function call

    /*
    sensor_data_class(sensor_data_class&& other){
    	std::cout << "sensordata class : move constructor call" << std::endl;
    	exit(0);
    }
    */

    sensor_data_class(){
    	std::cout << "sensordata class : private default constructor call" << std::endl;
    	print();
    };

    /*
    sensor_data_class(sensor_data_class& other){
    	std::cout << "\n\nsensordata move constructor" << std::endl;
    	exit(0);
    	std::cout << std::endl << std::endl;
    	std::cout << "current sensor data is = " << std::endl;
    	print();
    	std::cout << std::endl << std::endl;
    	std::cout << "given sensordata is = " << std::endl;
    	other.print();

    	//std::cout << "message id = " << message_id << " other = " << other.message_id << std::endl;
    	//std::cout << "message id = " << message_id << " other = " << other.message_id << std::endl;


    	//message_id = other.message_id;
    	std::swap(message_id,other.message_id);
    	std::swap(type,other.type);
    	std::swap(platform,other.platform);
    	//data = other.data;
    	//std::swap(data,other.data);


    	std::cout << std::endl << std::endl;
    	std::cout << "new sensor data is = " << std::endl;
    	print();
    	std::cout << std::endl << std::endl;
    	std::cout << "given sensordata is = " << std::endl;
    	other.print();
    	exit(0);
    }
*/


public:

    sensor_data_class(unsigned int messid, int senstype, char val):message_id(messid),type(senstype),platform(-3), data(std::move(cvmat(1,2,3,val))) {
    	std::cout << "sensordata constructor with data of rows , cols, depth, value = 1,2,3,69" << std::endl;
    };

    ~sensor_data_class(){
    	std::cout << "sensordata destructor call" << std::endl;
    };

    void print(){
    	std::cout << "own address = " << (void *) this << std::endl;
    	std::cout << "message_id = " << message_id << std::endl;
    	std::cout << "type = " << type << std::endl;
    	std::cout << "platform = " << platform << std::endl;
    	data.print();
    }

    unsigned int get_data_size() const {
    	std::cout << "buffer base class : call get_data_size()" << std::endl;
    	return data.get_data_size();
    };

    uptr_buffer_type get_data_ptr() {
    	std::cout << "buffer base class : call get_data_ptr()" << std::endl;
    	return data.get_data_ptr();
    };
};



class buffer_base_class {

private:
    // copy stuff
    buffer_base_class(buffer_base_class& other) = delete;
    buffer_base_class(const buffer_base_class& other) = delete;
    buffer_base_class& operator=(buffer_base_class& other) = delete;
    buffer_base_class& operator=(const buffer_base_class& other) = delete;

    // move stuff
    //buffer_base_class(buffer_base_class&& other) = delete;
    buffer_base_class(const buffer_base_class&& other) = delete;
    buffer_base_class& operator=(buffer_base_class&& other) = delete;
    buffer_base_class& operator=(const buffer_base_class&& other) = delete;


protected:

    // the derived class fills the data and its length
    // access to the base base class gives access to the data ptr and the length (tcp uses it as a buffer)

	uptr_buffer_type u_data_ptr;
    int data_length;

    int debug_level;

    // only derived classes should have access
    buffer_base_class();

public:



    // move result of function into returnvalue
    buffer_base_class(buffer_base_class&& other){
    	std::cout << "buffer base class : call to move constructor" << std::endl;
    	data_length = other.data_length;
    	other.data_length = 0;
    	u_data_ptr = std::move(other.u_data_ptr);
    }

    virtual int insert_data(char *buffer, int data_length);

    virtual ~buffer_base_class();
    virtual char* get_data_ptr();
    virtual unsigned int get_data_size();
    virtual void print_buffer();
};

#endif
