/*
 * message_class.h
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#ifndef BUFFER_CLASS_H
#define BUFFER_CLASS_H

#include <iostream>
#include <vector>
#include <memory>

#include "opencv2/opencv.hpp"
#include <common.h>

void buffer_class_unit_test();
extern const int BUFFER_DEBUG_LEVEL;
//const int BUFFER_DEBUG_LEVEL = 1;


// basic buffer typemade of vector char
typedef std::vector<char> buffer_type;

// tools to manipulate a char buffer which is used to communicate with TCP to send / receive
class buffer_class {

public:
    // possible buffer types
    // 1. vector<char>
    // 2. ptr to cvmat
    // everything else is invalid
    enum buffer_type_def{ vector , cvmat , invalid_buffer_type };

protected:
    buffer_type_def which_type_;            
    buffer_type buffer_;
    std::unique_ptr<cv::Mat> data_buffer_;
    int buffer_debug_level_;

public:

    // only want vector or cvmat buffer, hence make it private
    // however the Returntype class when returning Nothing() calls the constructor
    // so make it public and set the status to invalid
    buffer_class();

    // print the current buffer content
    void print_buffer_meta_data(const bool &with_buffer_content = false);
    void print_buffer_content(const int &num_elems = 10);

    // control move/copy constructors / assignement operators
    //buffer_class( buffer_class&);
    buffer_class& operator=(buffer_class&) = delete;
    buffer_class( buffer_class&&) = default;
    buffer_class& operator=(buffer_class&&) = delete;

    virtual ~buffer_class();
    buffer_type_def get_buffer_type();

    // creates a buffer of given size (i.e. header message)
    buffer_class(const int &size);
    // this moves the cvmat into the buffer
    // the buffer size is calculated from the cvmat size
    buffer_class(std::unique_ptr<cv::Mat>&& mat);

    // when the buffer has been created those give access to the size and to the data, depending on the buffer type
    char* get_data_ptr();    // if vector -> vector.data();
                             // if cvmat -> cvmat.data;
    int get_data_size();     // if vector -> size_  (given to constructor) or alternativley vector.size();
                             // if cvmat -> size_  (which is given to the constructor)

    // this returns the cvmat of the databuffer with move logic, used when the mat ownership changes
    std::unique_ptr<cv::Mat> get_cvmat_ptr();

    // tools to shape and insert data into the the buffer
    int insert(int pos, std::string text);
    int insert(int pos, unsigned int value);
    int insert(int pos, int value);
    int extract(const int &pos, const int &length, std::string &text);
    int extract(const int &pos, unsigned int &var);
    int extract(const int &pos, int &var);
};

#endif
