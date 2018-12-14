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

#define use_global_debug_setting
const int GLOBAL_DEBUG_SETTING = 3;

// consts should move into the init file
const int TCP_HEADER_LENGTH  = 80;

const int BUFFER_DEBUG_LEVEL = 1;

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
    int cols_, rows_;       // as expected
    int mat_type_;          // opencv representation of the data type and channels
    int depth_;             // opendv representation of the datatype  eg how many bytes of a unsigned, signed, float
    int channels_;          // how many channels/layers/ ...
    int total_;             // cvmat represtantion of the size i.e. rows*cols
    int elem_size_;         // size of one matrix element in bytes (e.g. 32U = 4 bytes, 32F = 16 bytes
    int data_length_;       // how many bytes of data are in the matrix

public:
    cvMat_params_class();
    cvMat_params_class(const int &rows, const int &cols, const int &channels, const int &elem_size, const int &mat_type);
    // depth is not a measure it is a descriptor for the individual elements of the matrix
    std::vector<std::string> depth_defs_ {"CV_8U", "CV_8S", "CV_16U", "CV_16S", "CV_32S", "CV_32F", "CV_64F","undefined"};
    cvMat_params_class(const cv::Mat &matrix);
    int get_cols();    
    int get_rows();    
    int get_type();
    int get_data_length();
    void get_params( int &rows, int &cols, int &channels, int &elem_size, int &mat_type);
    void print_cvMat_params();
    void print_cvMat_data(const int &how_many=10);
    // should be const, but can't be bothered to track down function signatures after midnight ...
    bool operator==(cvMat_params_class &other);
    ~cvMat_params_class();
};

typedef unsigned int time_format;
time_format get_time_stamp();

// replace with above
std::time_t get_time();

#endif /* COMMON_DEFS_H_ */
