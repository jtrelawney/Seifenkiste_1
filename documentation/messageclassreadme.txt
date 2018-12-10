this directory contains all libraries used in the project
the cmake file compile the libraries only and the unit test as well

Implemented libraries:

message_class

implements the container which is used to transport all meta data and sensor data to other processes or platforms
- contains message meta data
- contains sensor metadata
- contains sensor data
- will be refered to by a unique pointer so that messages cannot be duplicated (the current owner is responsible for keeping it together)
- allows processes / systems to exchange messages, i.e. need a sender receiver addressing mechanism

member functions:
1. constructor which creates a message with all information given at once (used by the sensor)
2. constructor which "initializes" a message with header data only
    - used by the tcp process wiht the header message available before the data message arrives
    - extracts meta info from a header message and populates the meta data fields
3. function which adds the sensor data to an initialized message

4. provides a status management facility  (initialized (only header / meta data) , complete (all data is contained), invalid (error or data has been passed on which violates the unique ptr concept)

6. implements a buffer type facility 
    - used to communicate between queue and tcp
    - sensor object creates message and equeues it with message queue, message queue sends it via tcp
    - tcp receives header message, inits a message, tcp receives data, completes message, inserts message into message queue

6. povides tools to insert and extract platform indepent information into / from the header buffer
    - inserts parameters (of type a,b,c) at a certain header location
    - extracts parameters (oftype a,b,c) from a certain header locaton
    - those can be used to manage the common parameters, but also to manage the sensor specific parameters
    - for latter the sensor object should call the interface, but is allowed to access a certain window within the header only
