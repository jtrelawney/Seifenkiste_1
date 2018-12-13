#include <buffer_class.h>

buffer_class::buffer_class() :
        which_type_(buffer_type_def::invalid_buffer_type),
        buffer_(buffer_type(0)),
        data_buffer_( std::unique_ptr<cv::Mat> (new (cv::Mat)) ),
        buffer_debug_level_(BUFFER_DEBUG_LEVEL)
{
    if (buffer_debug_level_>0) if (buffer_debug_level_>0) std::cout << "buffer_class : standard constructor call" << std::endl;
}

buffer_class::buffer_class(const int &size) :
        which_type_(buffer_type_def::vector),
        buffer_(buffer_type(size)),
        data_buffer_( std::unique_ptr<cv::Mat> (new (cv::Mat)) ),
        buffer_debug_level_(BUFFER_DEBUG_LEVEL)
{
    if (buffer_debug_level_>0) std::cout << "buffer_class : constructor call with vector, size = " << size << std::endl;
}

buffer_class::buffer_class(std::unique_ptr<cv::Mat> &&mat) :
        which_type_(buffer_type_def::cvmat),
        buffer_(buffer_type(0)),
        data_buffer_(std::move(mat) ),
        buffer_debug_level_(BUFFER_DEBUG_LEVEL)
{
    if (data_buffer_ == nullptr) {
        if (buffer_debug_level_>0) std::cout << "buffer_class : constructor call with cvmat nullptr -> type set to invalid_buffer_type" << std::endl;
        which_type_ = buffer_type_def::invalid_buffer_type;        
    } else {
        if (buffer_debug_level_>0) std::cout << "buffer_class : constructor call with cvmat, size " << get_data_size() << std::endl;
        
    }
}

buffer_class::~buffer_class() {
    if (buffer_debug_level_>0) std::cout << "buffer_class : destructor call" << std::endl;
    if (which_type_ == buffer_type_def::vector) {
        if (buffer_debug_level_>2) std::cout << "buffer_class : destructing - buffertype vector" << std::endl;
        if (data_buffer_ != nullptr) {
            if (buffer_debug_level_>1)  std::cout << "data buffer ptr = " << (void *)&data_buffer_ << std::endl;
            if (buffer_debug_level_>1)  std::cout << "deleteing databuffer" << std::endl;
        } else {
            if (buffer_debug_level_>1)  std::cout << "dataptr = nullptr" << std::endl;
        }
    }
    if (which_type_ == buffer_type_def::cvmat) {
        if (buffer_debug_level_>2) std::cout << "buffer_class : destructing - buffertype cvmat" << std::endl;
        if (data_buffer_ != nullptr) {
            if (buffer_debug_level_>1)  std::cout << "data buffer ptr = " << (void *)&data_buffer_ << std::endl;
            if (buffer_debug_level_>1)  std::cout << "deleteing databuffer" << std::endl;
        } else {
            if (buffer_debug_level_>1)  std::cout << "dataptr = nullptr" << std::endl;
        }
    }
    if (which_type_ == buffer_type_def::invalid_buffer_type) {
        if (buffer_debug_level_>2) std::cout << "buffer_class : destructing - invalid buffertype" << std::endl;
        if (data_buffer_ != nullptr) {
            if (buffer_debug_level_>1)  std::cout << "surprisingly, data buffer ptr = " << (void *)&data_buffer_ << std::endl;
            if (buffer_debug_level_>1)  std::cout << "deleteing databuffer" << std::endl;
        } else {
            if (buffer_debug_level_>1)  std::cout << "dataptr = nullptr" << std::endl;
        }
    }
}

buffer_class::buffer_type_def buffer_class::get_buffer_type(){ return which_type_; }

// depending on the buffer type return the *char
// used for tcp which needs (*char, length)
char* buffer_class::get_data_ptr() {

    if (which_type_ == buffer_type_def::vector) {
        if (buffer_debug_level_>3) std::cout << "buffer_class : get_data_ptr()  - vector" << std::endl;
        return buffer_.data();
    }

    if (which_type_ == buffer_type_def::cvmat) {
        if (buffer_debug_level_>3) std::cout << "buffer_class : get_data_ptr()  - cvmat" << std::endl;
        cv::Mat *data = data_buffer_.get();
        unsigned char *d = data->data;
        return (char *) d;
    }

    if (which_type_ == buffer_type_def::invalid_buffer_type) {
            if (buffer_debug_level_>2) std::cout << "buffer_class : get_data_ptr() - buffer invalid (probably used the default constructor" << std::endl;
            return nullptr;
        }

    if (buffer_debug_level_>0) std::cout << "buffer_class : get_data_ptr() with undefined buffer type" << std::endl;
    return nullptr;
}

// depending on the buffer type return the buffer length
// used for tcp which needs (*char, length)
int buffer_class::get_data_size() {

    if (which_type_ == buffer_type_def::vector) {
        if (buffer_debug_level_>3) std::cout << "buffer_class : get_data_size()  - vector" << std::endl;
        return buffer_.size();
    }

    if (which_type_ == buffer_type_def::cvmat) {
        if (buffer_debug_level_>3) std::cout << "buffer_class : get_data_size()  - cvmat" << std::endl;
        cv::Mat *cvmat = data_buffer_.get();
        cvMat_params_class mat_params(*cvmat);
        return mat_params.get_data_length();

	    //cv::Mat *d = data_buffer_.get();
        //int data_size = d->rows * d->cols * d->channels();
        //return data_size;
    }

    if (which_type_ == buffer_type_def::invalid_buffer_type) {
            if (buffer_debug_level_>2) std::cout << "buffer_class : get_data_ptr() - buffer invalid (probably used the default constructor" << std::endl;
            return -1;
        }

    if (buffer_debug_level_>0) std::cout << "buffer_class : get_data_size() with undefined buffer type" << std::endl;
    return -2;
}

// when the buffer (i.e. the cvmat char* has been filled, the cvmat can be extracted and used otherwise
// this is move logic
std::unique_ptr<cv::Mat> buffer_class::get_cvmat_ptr(){
    if (which_type_ == buffer_type_def::cvmat) {
        if (buffer_debug_level_>1) std::cout << "buffer_class : get_cvmat_ptr() - handing over cvmat" << std::endl;
        return std::move(data_buffer_);
    } else {
        if (buffer_debug_level_>0) std::cout << "buffer_class : get_cvmat_ptr() - not a cvmat buffer, cant hand over cvmat" << std::endl;
    }
    return std::move(data_buffer_);
}

// ------------------------------------------------
// below are the function which print the buffer meta data depending on the type
// and the buffer content, with the default setting of 10 units
void buffer_class::print_buffer_meta_data(const bool &with_buffer_content){

    std::cout << "\n\nbuffer_class : print" << std::endl;
    if (which_type_ == buffer_type_def::invalid_buffer_type) {
        std::cout << "buffer_class : buffer invalid (probably used the default constructor or supplied nullptr cvmat" << std::endl;
        return;
    }

    if (which_type_ == buffer_type_def::vector) {
        std::cout << "buffer_class : this is a vector buffer" << std::endl;
        std::cout << "size = " << get_data_size() << std::endl;
        std::cout << "data ptr = " << (void *)get_data_ptr() << std::endl;
        if (with_buffer_content == true) print_buffer_content();
        return;
    }

    if (which_type_ == buffer_type_def::cvmat) {
        std::cout << "buffer_class : this is a cvmat buffer" << std::endl;
        std::cout << "size = " << get_data_size() << std::endl;
        std::cout << "data ptr = " << (void *)get_data_ptr() << std::endl;
        cv::Mat *data = data_buffer_.get();
        cvMat_params_class print_mat(*data);
        print_mat.print_cvMat_params();
        if (with_buffer_content == true) print_buffer_content();
        return;
    }

    std::cout << "buffer_class : print() undefined buffer type" << std::endl;
}

void buffer_class::print_buffer_content(const int &num_elems){

    if ( !( (which_type_ == buffer_type_def::vector) || (which_type_ == buffer_type_def::cvmat) ) ) return;

    int size = get_data_size();
    char *start = get_data_ptr();
    char *endit = start + std::min(num_elems,size);
    int counter = 0;

    std::cout << "buffer content , size = " << size << std::endl;

    while (start<endit){
        std::cout << (char)(*start) << " ,  ";
        start++;
        counter++;
        if (counter>num_elems) break;
    }

    std::cout << std::endl;
    start = get_data_ptr();
    counter = 0;
    while (start<endit){
        std::cout << (int) (unsigned char)(*start) << " ,   ";
        start++;
        counter++;
        if (counter>num_elems) break;
    }
    std::cout << "\n\n" << std::endl;
}


// ------------------------------------------------
// below are the various insert and extract operations which allow a header to be constucted
// used to create header with all message infmation, transmit via tcp , reconstruct the header on the other side
int buffer_class::extract(const int &start_pos, const int &length, std::string &text) {
   
    int s = get_data_size();
    if (start_pos<0) return -1;
    if (start_pos + length > s) return -1;

    text.resize(length);
    std::string::iterator si = text.begin();
    char *dp = get_data_ptr();
    dp+=start_pos;
    char *buffer_end = dp + s;

    bool done = false;
    while (!done) {
        *(si) = *(dp);
        dp++;
        si++;
        bool d1 = ( dp == buffer_end );
        bool d2 = ( si == text.end() );
        done = (d1 || d2 );
    }
    return 0;
}

int buffer_class::extract(const int &pos, unsigned int &var) {

    int s = get_data_size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

	char *buffer = get_data_ptr();
    //std::cout << (int)(unsigned char)(buffer[pos+3]) << std::endl;
    //std::cout << (int)(unsigned char)(buffer[pos+2]) << std::endl;
    //std::cout << (int)(unsigned char)(buffer[pos+1]) << std::endl;
    //std::cout << (int)(unsigned char)(buffer[pos+0]) << std::endl;
	var = (unsigned int )(
		(unsigned char)(buffer[pos+3]) << 24 |
		(unsigned char)(buffer[pos+2]) << 16 |
		(unsigned char)(buffer[pos+1]) << 8 |
		(unsigned char)(buffer[pos+0])
		);

	return 0;
}

int buffer_class::extract(const int &pos, int &var) {
    
    int s = get_data_size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

	char *buffer = get_data_ptr();
    //std::cout << (int)(unsigned char)(buffer[pos+3]) << std::endl;
    //std::cout << (int)(unsigned char)(buffer[pos+2]) << std::endl;
    //std::cout << (int)(unsigned char)(buffer[pos+1]) << std::endl;
    //std::cout << (int)(unsigned char)(buffer[pos+0]) << std::endl;
	var = (unsigned int )(
		(unsigned char)(buffer[pos+3]) << 24 |
		(unsigned char)(buffer[pos+2]) << 16 |
		(unsigned char)(buffer[pos+1]) << 8 |
		(unsigned char)(buffer[pos+0])
		);

	return 0;
}

int buffer_class::insert(int pos, std::string text){
    int s = buffer_.size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    std::string::iterator si = text.begin();

    bool done = false;
    while (!done) {
        buffer_[pos] = *(si);
        pos++;
        si++;
        bool d1 = ( pos == s );
        bool d2 = ( si == text.end() );
        done = (d1 || d2);
    }

    return 0;
}

int buffer_class::insert(int pos, unsigned int value){

	int s = buffer_.size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

	buffer_[pos + 3] = (unsigned char) ( (value>>24) & 255);
	buffer_[pos + 2] = (unsigned char) ( (value>>16) & 255);
	buffer_[pos + 1] = (unsigned char) ( (value>>8) & 255);
	buffer_[pos + 0] = (unsigned char) ( (value>>0) & 255);
	return 0;
}

int buffer_class::insert(int pos, int value){

	int s = buffer_.size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

	buffer_[pos + 3] = (unsigned char) ( (value>>24) & 255);
	buffer_[pos + 2] = (unsigned char) ( (value>>16) & 255);
	buffer_[pos + 1] = (unsigned char) ( (value>>8) & 255);
	buffer_[pos + 0] = (unsigned char) ( (value>>0) & 255);

	return 0;
}
