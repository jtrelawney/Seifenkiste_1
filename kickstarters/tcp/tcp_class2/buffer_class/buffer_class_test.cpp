#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#include <string.h>

#include "buffer_class.h"
#include "data_buffer_class.h"
#include "header_buffer_class.h"

/*
std::ostream & operator << (std::ostream &out, const buffer_class &buffer)
{
    for (buffer_type::iterator i = buffer.buffer.begin(); i != buffer.end(); ++i){ out << *i; }
    return out;
}
*/

void read_tcp(char *source, char *target, int length){

    std::cout << "addr source data " <<  (void *)source << std::endl;
    std::cout << "addr target data " <<  (void *)target << std::endl;
    memcpy(target, source, length);
}

std::ostream& operator<<(std::ostream& out, const buffer_class& buffer)  
{  
    buffer_type::const_iterator start = buffer.buffer.begin();
    buffer_type::const_iterator ende = buffer.buffer.end();
    for (auto i=start; i<ende; ++i){
        out << *i;
    }
    //buffer.print();  
    return out;  
}  

int main()
{
    std::cout << "test the buffer_class" << std::endl;

    buffer_base_class *bbc1 = new buffer_base_class();
    std::cout << "buffer base class size = " << bbc1 -> get_data_size() << std::endl;
    //conversion from base to dervied doesnt fly
    //data_buffer_class *dnw1 = bbc1;

    data_buffer_class *dbc1 = new data_buffer_class(37);
    std::cout << "data buffer class size = " << dbc1 -> get_data_size() << std::endl;
    std::cout << "addr = " << (void *)dbc1 -> get_data_ptr() << std::endl;

    buffer_base_class *bbc2 = dbc1;
    std::cout << "casted to base class , size = " << bbc2 -> get_data_size() << std::endl;
    std::cout << "addr = " << (void *)bbc2 -> get_data_ptr() << std::endl;

    std::cout << "\ncreating sensor test data" << std::endl;
    sensor_data_class sd = sensor_data_class(1,3);
    header_buffer_class *hbc1 = new header_buffer_class(sd);
    std::cout << "header buffer class size = " << hbc1 -> get_data_size() << std::endl;
    std::cout << "addr = " << (void *)hbc1 -> get_data_ptr() << std::endl;


    exit(0);
    // with a string
    std::string a("this is a test");
    buffer_class b1(a);
    //b1.print();
    std::cout << b1 << std::endl;

    // 0 terminated, so that strlen works ...
    char arra[] = "and another test";
    buffer_class b2(arra,strlen(arra));
    std::cout << b2 << std::endl;


    // 0 terminated, so that strlen works ...
    char *arrb = (char *) malloc (strlen(arra) * sizeof(char));
    memcpy(arrb,arra, 11);
    buffer_class b3(arrb,11);
    std::cout << b3 << std::endl;

    std::string tcp("and now testing a tcp read into the classes char buffer reference ...");
    int l = tcp.length();
    std::cout << tcp << "   l = " << l << std::endl;

    char *tcp_read = (char *) malloc (l * sizeof(char));
    memcpy(tcp_read,tcp.data(), l);
    std::cout << tcp_read << std::endl;

    buffer_class b4(l);
    char * b4d = b4.data();
    std::cout << "addr b4 data " <<  (void *)b4d<< std::endl;

    read_tcp(tcp_read, b4d, l);

    std::cout << "and the result is :"<< std::endl << b4 << std::endl;
    std::cout << "class print function : "<< std::endl;
    b4.print();
}
