#include <message_class.h>

message_class::message_class(int d): data(d){}

void message_class::set_id(unsigned int this_id){ message_id = this_id; }
unsigned int message_class::get_id(){ return message_id;}

void message_class::set_data(unsigned int d){ data = d; }
unsigned int message_class::get_data(){ return data;}


void message_class::print_meta_data(){ printf("\nmessage class : content dump : message id = %u            data = %i\n",message_id,data);}


message_class::~message_class(){}
