#ifndef BUFFER_CLASS_H
#define BUFFER_CLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include <vector>

//#include <defs.h>

#define BUFFER_DEBUG_LEVEL 5

typedef std::vector<char> buffer_type;

class buffer_class {

private:
    int debug_level;
    
    buffer_type buffer;

    // control move/copy constructors / assignement operators
    buffer_class& operator=(const buffer_class&) = delete;
    buffer_class( const buffer_class&) = delete;
    buffer_class& operator=(buffer_class&&) = delete; // std copy is sufficient

    // constructor / destructor
    // avoid 0-terminated mess by privatising the related constructors    
    buffer_class();


public:

    // constructor / destructor
    buffer_class(int size);
    buffer_class(char *buffer, int length);
    ~buffer_class();    

    // need those for creating a message object with a previously tcp created buffer
    buffer_class( buffer_class&&) = default;

    buffer_class(std::string input);

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
    int fill(int startpos, int end_pos, unsigned int value);

    int insert(int pos, std::string text);
    int insert(int pos, unsigned int value);

    unsigned int extract_ui(int pos);


};


#endif
