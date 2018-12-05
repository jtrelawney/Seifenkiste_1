#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <vector>
#include <limits.h>
#include <sys/time.h>		//gettimeofday
#include <chrono> // for timing stuff
#include <iostream>
#include <mutex>

#include "opencv2/opencv.hpp"

// consts should move into the init file
const int TCP_HEADER_LENGTH  = 80;

// implement a global end flag
// the static member is initialized in the common.cpp
class end_flag_class {
private:
    static int self_counter_;
    bool end_flag;
    static std::mutex flag_mutex;
public:
    end_flag_class();//:end_flag(false){ std::cout << "constructing end flag " << self_counter_ << " , set to false" << std::endl; };
    virtual ~end_flag_class();//{ std::cout << "destructing end flag" << std::endl; };
    void set_flag();
    bool read_flag();
};


// cvmat has depths, channels which is a combination of type
// to rebuild the matrix on the other side of tcp the type needs to be know or both depth and channels
// this class extracts the params which are later required to rebuild a matrix
//https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
class cvMat_params_class {
private:
    cvMat_params_class();
    int cols_,rows_,type_,depth_,channels_;
public:
    cvMat_params_class(const cv::Mat &matrix) : cols_(-1),rows_(-1),type_(-1),depth_(-1),channels_(-1) {
        cols_ = matrix.cols; rows_ = matrix.rows; type_ = matrix.type(); depth_ = matrix.depth(); channels_ = matrix.channels();
    };
    int get_cols() { return cols_; };    
    int get_rows() { return rows_; };    
    int get_type() { return type_; };
    void get_params( int &cols, int &rows, int &type) { cols=cols_; rows=rows_;type=type_; };
    void print_cvMat_params(){ std::cout << "cvMat params     cols =" << cols_ << "   rows = " << rows_ << "  type = " << type_ << "  depth = " << depth_  << "  channels = " << channels_  << std::endl;};
    ~cvMat_params_class(){};
};

typedef unsigned int time_format;
time_format get_time_stamp();

// replace with above
std::time_t get_time();

#endif /* COMMON_DEFS_H_ */
