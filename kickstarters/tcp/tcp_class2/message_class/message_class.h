#ifndef MESSAGE_CLASS_H
#define MESSAGE_CLASS_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <vector>

#include <buffer_class.h>
#include <tcp_class.h>


#define MESSAGE_DEBUG_LEVEL 5

class message_class {

private:
    unsigned int message_id;

    buffer_class header;
    void *data;
    unsigned int data_length;

    // control move/copy constructors / assignement operators
    message_class& operator=(const message_class& mess) = delete;
    message_class& operator=(const message_class&& mess) = delete;
    message_class( const message_class&) = delete;
    message_class( const message_class&&) = delete;

    // locked away
    message_class();


    int debug_level;
public:

    // very basic message class, constructor takes an id and then fills the message with some test data
    message_class(unsigned int mid);    
    message_class(buffer_class &header);
    ~message_class();  


    friend std::ostream& operator<<(std::ostream& os, const message_class& mess);
    //const message_class& mess);

    void print();

    std::ostream& myostream(std::ostream& out) const;

    /*
    message_class(int size);
    message_class(char *buffer, int length);

    message_class(std::string input);

    // output / debugigging
    void print();
    friend std::ostream& operator<<(std::ostream& os, const buffer_class& buffer);
    void set_debug_level(int level);

    // retrieve a pointer where to store data
    // make sure when writing to it that the correct length/size is used and use resize() to adjust if necessary
    char* data();

    // get and change the size of the buffer    
    int size();
    int resize(int new_size);
    */

};


#endif
