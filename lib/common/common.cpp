/*
 * defs.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <common.h>

// initialize the global flag and set the static members
end_flag_class G_END_FLAG = end_flag_class();
std::mutex end_flag_class::flag_mutex;
int end_flag_class::self_counter_ = 0;

end_flag_class::end_flag_class():end_flag(false){ self_counter_++; std::cout << "constructing end flag, object count = " << self_counter_ << " , set to false" << std::endl; }
end_flag_class::~end_flag_class(){ self_counter_--; std::cout << "destructing end flag, objects left = " << self_counter_ << std::endl; }
void end_flag_class::set_flag(){ std::cout << "setting end_flag to true" << std::endl; std::lock_guard<std::mutex> lck (flag_mutex); end_flag = true; }
bool end_flag_class::read_flag(){ std::lock_guard<std::mutex> lck (flag_mutex); /*std::cout << "reading end_flag " << end_flag << std::endl;*/ return end_flag; }

cvMat_params_class::cvMat_params_class() : cols_(-1),rows_(-1),mat_type_(-1),depth_(-1),channels_(-1),total_(-1),elem_size_(-1),data_length_(-1) {}
cvMat_params_class::cvMat_params_class(const cv::Mat &matrix) {
        cols_ = matrix.cols;
        rows_ = matrix.rows;
        mat_type_ = matrix.type();
        depth_ = matrix.depth();
        channels_ = matrix.channels();
        total_ = matrix.total();
        elem_size_ = matrix.elemSize();
        data_length_ = cols_ * rows_ * elem_size_;
        //int bytes = Mat.total() * Mat.elemSize();
}

cvMat_params_class::cvMat_params_class(const int &rows, const int &cols, const int &channels, const int &elem_size, const int &mat_type) :
    cols_(cols),rows_(rows),mat_type_(mat_type),depth_(-1),channels_(channels),total_(-1),elem_size_(elem_size),data_length_(-1)
{
		// this is wrong: elemsize contains the datasize per pixel * channels, use elemsize1 and it would work
        //data_length_ = cols_ * rows_ * channels_ * elem_size_;
        data_length_ = cols_ * rows_ * elem_size_;
}

void print_cvMat_data(const int &how_many=10){
}


int cvMat_params_class::get_cols() { return cols_; }    
int cvMat_params_class::get_rows() { return rows_; }    
int cvMat_params_class::get_type() { return mat_type_; }
int cvMat_params_class::get_data_length() { return data_length_; }
void cvMat_params_class::get_params( int &rows, int &cols, int &channels, int &elem_size, int &mat_type) {
    rows=rows_; cols=cols_; channels=channels_;elem_size=elem_size_;mat_type=mat_type_;
}
void cvMat_params_class::print_cvMat_params(){
	std::cout << "cvMat params     rows =" << rows_ << "   cols = " << cols_ << "  channels = " << channels_ << std::endl;
    std::cout << "elem_size = " << elem_size_ << std::endl;
    int depth_def_index = depth_;
    if ( (depth_def_index<0) || (depth_def_index>(int)depth_defs_.size()-1 ) ) depth_def_index = depth_defs_.size()-1;
    std::cout << "depth index = " << depth_def_index << std::endl; 
    std::cout << "depth = " << depth_  << " = " << depth_defs_[depth_def_index] << "  mat type = " << mat_type_ << std::endl; 
    std::cout << "data length = " << data_length_ << "      with cvmat total = " << total_ << std::endl;
}

bool cvMat_params_class::operator==(cvMat_params_class &other){
    // extract the others params
    int rows,cols,channels,mat_type,elem_size;
    other.get_params( rows, cols, channels, elem_size, mat_type);
    
    bool result = ( (rows = rows_) && (cols == cols_) && (mat_type == mat_type_) );
    //std::cout << "rows = " << rows_ << "        other rows = " << rows << std::endl;
    //std::cout << "cols = " << cols_ << "        other cols = " << cols << std::endl;
    //std::cout << "mat_type = " << mat_type_ << "        other mat_type = " << mat_type << std::endl;
    //std::cout << "result = " << result << std::endl;

    // depth is not received via tcp, it is set by opencv when the mat is created, since it is initialized with -1 : the comparison would fail.
    //result = result && ( (elem_size = elem_size_) && (channels == channels_) );
    result = result && ( (elem_size = elem_size_) );
    //std::cout << "elem_size = " << elem_size_ << "        other elem_size = " << elem_size << std::endl;
    //std::cout << "result = " << result << std::endl;
    return result;
}

cvMat_params_class::~cvMat_params_class(){}



std::time_t get_time(){
    // this seems to give millis
    //return std::chrono::system_clock::to_time_t( std::chrono::high_resolution_clock::now() );
    // this is nanos
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return nanoseconds.count();
}


time_format get_time_stamp(){
	
    struct timeval start;
    long mtime;
    

    gettimeofday(&start, NULL);
    mtime = ((start.tv_sec) * 1000 + start.tv_usec/1000.0) + 0.5;

	//struct timeval start, end;
    //long mtime, seconds, useconds;    

    //usleep(20000);
    //gettimeofday(&end, NULL);

    //seconds  = end.tv_sec  - start.tv_sec;
    //useconds = end.tv_usec - start.tv_usec;

    //mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    //printf("Elapsed time: %ld milliseconds\n", mtime);

    return (unsigned long) mtime;	
}
