#include "header_buffer_class.h"


header_buffer_class::header_buffer_class(const sensor_data_class &sensordata) {
    //buffer_type test(size,char(0));
    //std::unique_ptr<buffer_type> t( new buffer_type(size,char(0)) );
    u_data_ptr = std::unique_ptr<buffer_type>( new buffer_type(TCP_HEADER_LENGTH,char(0)) );

    unsigned int mid = sensordata.message_id;
    int type = sensordata.type;
    int platform = sensordata.platform;
    unsigned int datalength = sensordata.data->rows * sensordata.data->cols * sensordata.data->depth * sizeof(int);


    std::cout << "constructur header buffer class , size = " << get_data_size() << std::endl;
    std::cout << "message id = " << mid << std::endl;
    std::cout << "type = " << type << std::endl;
    std::cout << "platform = " << platform << std::endl;
    std::cout << "datalength = " << datalength << std::endl;

    insert(0,"con");
    insert(31,"end");
    //insert(54,"end");
    //insert(32,"end");

    insert(3,mid);
    insert(7,type);
    insert(11,platform);
    insert(15,datalength);

    std::cout << "buffer = " << std::endl;
    print_buffer();
    
}

header_buffer_class::~header_buffer_class(){
    std::cout << "destructur header buffer class , size = " << get_data_size() << std::endl;
    if (u_data_ptr) u_data_ptr -> clear();
}

int header_buffer_class::insert(int pos, unsigned int value){

    int s = get_data_size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

    char *buffer = get_data_ptr();

    for (int i=0; i<l; i++){
        buffer[pos+i] = (value >> 8*i);
    }

    //for (int i=0; i<4; i++) std::cout << "i = " << i << " = " << (int) buffer[pos+i] << std::endl;
    
    return 0;
}

// inserts the string at the requested position
// god for general messages or responses that contain keywords
int header_buffer_class::insert(int pos, std::string text){
    int s = get_data_size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    std::string::iterator si = text.begin();
    char *dp = get_data_ptr();
    char *end = dp + s;
    dp+=pos;

    bool done = false;
    while (!done) {
        *(dp) = *(si);
        dp++;
        si++;
        bool d1 = ( dp == end );
        bool d2 = ( si == text.end() );
        done = (d1 || d2); 
    }

    return 0;
}

/*
char* data_buffer_class::get_data_ptr() {
    if (u_data_ptr==nullptr) return nullptr;
    if (u_data_ptr) return u_data_ptr->data();
};
*/







// below not adapated yet
// -----------------------------------------------------------------------

/*
data_buffer_class::data_buffer_class(char *b, int length){
    buffer.resize(length);
    for (int i=0; i<length; i++){
        //this->buffer.push_back(b[i]);
        this->buffer[i] = b[i];
    }
}






buffer_class::buffer_class(std::string input){
    buffer = buffer_type(input.begin(), input.end());
    std::cout << "\n\n\n\nIm fucking with your text .....  \n\n\n\n"<< std::endl;

}

buffer_class::buffer_class() : debug_level(BUFFER_DEBUG_LEVEL) {}

char* buffer_class::data(){
    char *bd = buffer.data();
    if (debug_level>0) std::cout << "bufferclass vector data address " <<  (void *)bd << std::endl;
    return bd;
}

void buffer_class::print(){
    int l = buffer.size();
    std::cout << "buffer length = " << l << std::endl;
    for (buffer_type::iterator i = buffer.begin(); i != buffer.end(); ++i){ std::cout << *i; }
    std::cout << std::endl;
    for (buffer_type::iterator i = buffer.begin(); i != buffer.end(); ++i){ std::cout << (unsigned char) (*i) << " (" << char(*i) << ") , "; }
    std::cout << std::endl;

    //print_buffer_content(data(),size());
}

int buffer_class::resize(int new_size){
    if (new_size<0) return -1;
    if (new_size==size()) return new_size;

    //std::cout << "warning - buffer resizing operation" << std::endl;
    buffer.resize(new_size);
    return new_size;
}



unsigned int buffer_class::extract_ui(int pos){

    int s = size();
    if ( (pos<0) || (pos>s) ) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

    unsigned int result=0;
    for (int i=3; i>=0; i--){
        unsigned char d = buffer[pos+i];
        result = (result << 8) + d;
        std::cout << "i = " << i << " = " << d << " = " << (int) d << "   -----    " << result << std::endl;
    }

    return result;

    //for (int i=0; i<4; i++) std::cout << "i = " << i << " = " << (int) buffer[pos+i] << std::endl;
}



*/

