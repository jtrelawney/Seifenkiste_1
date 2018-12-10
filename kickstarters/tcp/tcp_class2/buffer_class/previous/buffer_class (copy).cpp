#include "buffer_class.h"


buffer_class::buffer_class(char *b, int length){
    buffer.resize(length);
    for (int i=0; i<length; i++){
        //this->buffer.push_back(b[i]);
        this->buffer[i] = b[i];
    }
}

int buffer_class::size(){
    return buffer.size();
}


buffer_class::buffer_class(int size) : debug_level(BUFFER_DEBUG_LEVEL) {
    buffer = buffer_type(size,char(0));
    //std::cout << "size = " << buffer.size() << std::endl;
}

buffer_class::buffer_class(std::string input){
    buffer = buffer_type(input.begin(), input.end());
    std::cout << "\n\n\n\nIm fucking with your text .....  \n\n\n\n"<< std::endl;

}

buffer_class::buffer_class() : debug_level(BUFFER_DEBUG_LEVEL) {}
buffer_class::~buffer_class(){ buffer.clear(); }

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

    print_buffer_content(data(),size());
}

int buffer_class::resize(int new_size){
    if (new_size<0) return -1;
    if (new_size==size()) return new_size;

    //std::cout << "warning - buffer resizing operation" << std::endl;
    buffer.resize(new_size);
    return new_size;
}

int buffer_class::insert(int pos, unsigned int value){

    int s = size();
    if ( (pos<0) || (pos>s) ) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

    for (int i=0; i<l; i++){
        buffer[pos+i] = (value >> 8*i);
    }

    //for (int i=0; i<4; i++) std::cout << "i = " << i << " = " << (int) buffer[pos+i] << std::endl;
    
    return 0;
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

/*
int buffer_class::fill(int startpos, int end_pos, unsigned int value){

    int s = size();
    if ( (startpos<0) || (startpos>s) ) return -1;
    if ( (end_pos<0) || (end_pos>s) ) return -2;
    if (end_pos<startpos) return -3;

    for (int i=start_pos; i<end_pos; i++){
        
    }    

    std::string::iterator si = text.begin();
    buffer_type::iterator bi = buffer.begin();
    bi += pos;

    bool done = false;
    while (!done) {
        *(bi) = *(si);
        bi++;
        si++;
        bool d1 = ( bi == buffer.end() );
        bool d2 = ( si == text.end() );
        done = (d1 || d2); 
    }
    return 0;
}
*/

int buffer_class::insert(int pos, std::string text){
    int s = size();
    if ( (pos<0) || (pos>s) ) return -1;

    std::string::iterator si = text.begin();
    buffer_type::iterator bi = buffer.begin();
    bi += pos;

    bool done = false;
    while (!done) {
        *(bi) = *(si);
        bi++;
        si++;
        bool d1 = ( bi == buffer.end() );
        bool d2 = ( si == text.end() );
        done = (d1 || d2); 
    }

    return 0;
}

/*
buffer_class& buffer_class::operator=(const buffer_class& that)
{
    int t = that.buffer.size();
    std::cout << "not a real implementation of the assignment op -> do not use " << t << std::endl;
    return *this;
}
*/

