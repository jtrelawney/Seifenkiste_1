#ifndef BUFFER_BASE_CLASS_H
#define BUFFER_BASE_CLASS_H


#include <memory>
#include <vector>

#define BUFFER_DEBUG_LEVEL 5

// define the minimum structure a buffer class needs so that when
// passed to tcp object latter can access the char* and length informaation 
// it is that data structure is vector char, but tcp send requires a char* and its size

class cvmat{
private:
    std::vector<std::vector<std::vector<int> > > *mymatrix;
public:

    int rows, cols, depth;

    cvmat(int rows, int cols, int depth, int val){
        this -> rows = rows;
        this -> cols = cols;
        this -> depth = depth;
        std::vector<int> d = std::vector<int>(cols, val);
        std::vector<std::vector<int> > c = std::vector<std::vector<int> > (rows, d);
        std::vector<std::vector<std::vector<int> > > r = std::vector<std::vector<std::vector<int> > > (depth, c);
        mymatrix = &r;
    }

    ~cvmat(){ delete mymatrix;};
    
};

class sensor_data_class{
public:
    unsigned int message_id;
    int type;
    int platform;
    cvmat *data;

    sensor_data_class(unsigned int mid, int t):message_id(mid),type(t),platform(5){ data = new cvmat(3,4,1,0); };
    ~sensor_data_class(){};
};



typedef std::vector<char> buffer_type;

class buffer_base_class {

protected:
    
    // the derivec class fills the data and its length
    // the entity which accesses the underlying baseclass can then ask for a ptr to the data and its length
    std::unique_ptr<buffer_type> u_data_ptr;
    int data_length;

    int debug_level;

public:

    buffer_base_class():u_data_ptr(nullptr),data_length(0), debug_level(BUFFER_DEBUG_LEVEL){
        std::cout << "buffer base class constructor call" << std::endl;
    };

    virtual ~buffer_base_class(){
        if (u_data_ptr) u_data_ptr.release();
    };

    char* get_data_ptr(){
        return u_data_ptr->data();
    };
    
    virtual int get_data_size(){
        if (u_data_ptr==nullptr) return -1;
        return u_data_ptr->size();
    };

    void print_buffer(){
        int s = get_data_size();
        char *pos = get_data_ptr();

        if (s>50) s = 50;
        for (int i=0; i<s; i++){
            char inhalt = *(pos);
            std::cout << "i=" << i << "     " << int(inhalt) << "       " << inhalt << std::endl; //(unsigned int) (inhalt) << "       " << inhalt << std::endl;
            pos++;
        }
    };
};

#endif
