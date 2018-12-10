this is a pilot to provide a buffer class structure which:

- allows to hand a basic buffer to the tcp to either write char*, size or fill it with the received data char*, size
- this basic buffer only has 2 data structurs    - data_size and data_ptr, which both tcp and the derived classes use as the interface to exchange the buffer
- this char buffer is implemented as a vector<char> to intruduce the abstraction from char * and operate without ptrs to the data structrures

the derived classes use this interface to either write the headerinformation and data to tcp or are handed back a biffer from which they extract the header and data and create the higher level data structure
- there are 2 derived classes:   header_buffer and data_buffer
- for testing purposes a helper class sensor_data provides a higher level data structure, it implements the meta data and has access to a data container cvmat
- as a data container a second helper class cvmat stores the data data structure vector<int> and its related parameters, here rows,cols,depth (the plan is to use the real cvmat later as a generic container for all sensor data such as images, imu data etc).
- the sensor data object can be passed arround and it contains all the information (meta and sensordata) needed by other processes

the buffer is implemented as vector<char> and for this pilot i want to ensure that there exists only one instance of each data container - hence the unique ptr on buffer level
typedef std::vector<char> buffer_type;
typedef std::unique_ptr<buffer_type> uptr_buffer_type;
the cvmat contains the meta data (such as sensortype) and holds a cvmat with the data parameters and a unique ptr to the buffer structure

the header buffer object
- to send over tcp it takes the sensor data and inserts the relevant meta data into a newly created buffer of fixed length
- to receive from tcp it takes the tcp buffer and extracts the relevant meta data into a newly created sensor data object

the data buffer object
- to send over tcp it takes the sensor data and extracts the data size and data ptr from cv mat and stores those in its data members
- the basic buffer object with its primitive functions now has access to the char*, size informaiton
- to receive from tcp it takes the tcp buffer and and creates a cvmat from it and attaches it to the sensor data object



Test:
the test creates a sensor data object of size 1,2,3 to create a vector<int> filled with 6 times a test value
this object is passed to the header buffer which inserts the meta data and sends the buffer to the tcp stub (which prints the buffer)
next the sensor data is passed to the data buffer which extracts the size and the data ptr and stores it before sending the data over the tcp stub.

for both the resulting char tcp buffer is fed back into a new header buffer, data buffer and the end result is a new sensor data object



Conclusion:
it works.
however: it was not a good idea to use a unique ptr to the lowest data entity in the cvmat object
i.e. there can only be one unqiue pointer to a vector<char> which holds the data
it was a good idea but it means that copy and move operators have to be managed at the lowest level of the object hierarchy, which means
that the higher levels need those too

furthermore the number of objects to manage along the chain:

send process:
main -> test object -> tcp  -> extract meta and create header buffer -> send header buffer
                            -> extract the data and create data buffer -> send data buffer
latter requires to gain access to the unqiue pointer and its data and giving access to it to the tcp buffer

receive process:
main -> tcp -> base buffer -> header buffer -> extract meta data -> insert into sensor data object
            -> base buffer -> data buffer -> extract the data -> insert into sensor data object

again latter requires to create the unqiue pointer and its data moving ownership to the sensor data object 


Overall the high level process looks clean but at the lower level the copy and move process is not fully obvious
the goal must be to reduce the number of objects and the complexity related to the move process and the unique pointer

1. use a unique pointer for a particular sensor data object instead of a unqiue pointer to the sensor data
the unqiue pointer managment then happens on the highest level because it is the sensordata ownership which is passed along the chain
that means a move for the sensor object passes on the resonsibility for the data
and as long as the object is handled as one unit, i.e. the current owners do not copy and leak data , the process should become simpler

2. reduce the buffer complexity, either
- remove the buffer base class and use the header buffer and the data buffer
- or remove the data buffer and use the base class to send data of the objects
- or find a way to use one object which handles the header and data send process at once
- or find a way to combine the header and data into one tcp send process and create one buffer object for that

