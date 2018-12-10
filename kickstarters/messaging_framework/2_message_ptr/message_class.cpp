#include <message_class.h>

message_class::message_class(unsigned int this_id): message_id(this_id) {
    buffer = NULL;
    constructor++;
    //printf("message_class : Constructor call - %p\n", (void*) this );
}

void message_class::allocate_buffer(){
    buffer = (char *)malloc(sizeof(int));
}

void message_class::set_id(unsigned int this_id){ message_id = this_id; }
unsigned int message_class::get_id(){ return message_id;}

void message_class::set_data(int d){
    data = d;
    if (buffer == NULL) allocate_buffer();
    memcpy(&buffer[0],&data,sizeof(int));
    //printf("message_class : set data buffer - %p\n", (void*) buffer );
}
    
int message_class::get_data(){
    int test=0;
    if (buffer!=0) memcpy(&test, &buffer[0], sizeof(int));
    //if (test == data) printf("ok");
    return data;
}

void message_class::print_meta_data(){
    printf("\nmessage class : content dump :\n");
    printf("message id = %i\n",message_id);
    printf("data = %i\n",data);
    printf("message_class : buffer addr - %p\n", (void*) buffer );
}

message_class::~message_class(){
    //printf("message_class : Destructor call  - %p\n", (void*) this );
    destructor++;
    if (buffer != NULL) {
        //printf("delete buffer at - %p\n", (void*) buffer );
        free(buffer);
    }
    printf("con vs des %i\n",constructor - destructor);
    
    //else { printf("buffer is NULL\n"); }

}
    
