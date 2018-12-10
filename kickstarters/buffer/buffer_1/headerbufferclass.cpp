#include "headerbufferclass.h"

header_buffer_class::header_buffer_class(header_buffer_class&& other){
	std::cout << "header_buffer_class : call to move constructor" << std::endl;
	data_length = other.data_length;
	other.data_length = 0;
	u_data_ptr = std::move(other.u_data_ptr);

	exit(0);
}

int header_buffer_class::extract_header(sensor_data_class &sensor_data){
    // extract the test header entries

	int result = 0;
	std::string a,b;

	result+= extract(0,3,a);	//con
	result+= extract(31,3,b);	//end
    if (debug_level>2) std::cout << "header qualifiers begin and end = " << a << " , " << b << std::endl;
    bool proper_header = (a=="con") && (b=="end");

    unsigned int mid=0;
    result+= extract(3,mid);
    int type=0;
    result+= extract(7,type);
    int platform=0;
    result+= extract(11,platform);
    unsigned int datalength;
    result+= extract(15,datalength);

    if (debug_level>2) {
    	std::cout << "information extracted " << (result==0) << std::endl;
    	std::cout << "proper header = " << proper_header << std::endl;
    }

    if (debug_level>1) {
		std::cout << "message id = " << mid << std::endl;
		std::cout << "type = " << type << std::endl;
		std::cout << "platform = " << platform << std::endl;
		std::cout << "datalength = " << datalength << std::endl;
    }

    sensor_data.message_id = mid;
    sensor_data.type = type;
    sensor_data.platform = platform;
    sensor_data.data.rows = datalength;
    sensor_data.data.cols = 1;
    sensor_data.data.depth = 1;


    // the result variable accumulates the extraction results
    // if all are ok (=0) we return 0 here , ok
    // if there is at least one failure then the result is negative
    // for a successful extraction we would have 0 as a total aggregated result.
    return result;
}

header_buffer_class::header_buffer_class(char *buffer, int length){
	if (debug_level>0) std::cout << "header buffer class con-structor call with buffer data" << std::endl;

	if (length!=TCP_HEADER_LENGTH) {
		std::cout << "header buffer class : constructor call with invalid header length os " << length << std::endl;
		if (length>TCP_HEADER_LENGTH) length = TCP_HEADER_LENGTH;
	}


	// this inserts the buffer data
	int result = insert_data(buffer,length);
	if (result<1) {
		std::cout << "header buffer class : error inserting data into base_class data structures" << std::endl;
		return;
	}

    if (debug_level>3) print_buffer();
}

int header_buffer_class::extract(int start_pos, int length, std::string &text){
    int s = get_data_size();
    if (start_pos<0) return -1;
    if (start_pos + length > s) return -1;

    text.resize(length);
    std::string::iterator si = text.begin();
    char *dp = get_data_ptr();
    dp+=start_pos;
    char *buffer_end = dp + s;
    //char *end = dp + (end_pos-start_pos);

    bool done = false;
    while (!done) {
        *(si) = *(dp);
        dp++;
        si++;
        bool d1 = ( dp == buffer_end );
        bool d2 = ( si == text.end() );
        //bool d3 = ( dp == end );
        done = (d1 || d2 ); //|| d3);
    }

    return 0;
}

int header_buffer_class::extract(int pos, int &var){
	unsigned int transfer = (unsigned int) var;
	int result = extract(pos, transfer );
	var = int(transfer);
	return result;
}

int header_buffer_class::extract(int pos, unsigned int &var){

    int s = get_data_size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

	char *buffer = get_data_ptr();
	var = (unsigned int )(
		(unsigned char)(buffer[pos+3]) << 24 |
		(unsigned char)(buffer[pos+2]) << 16 |
		(unsigned char)(buffer[pos+1]) << 8 |
		(unsigned char)(buffer[pos+0])
		);

	return 0;
}

header_buffer_class::header_buffer_class(const sensor_data_class &sensordata) {
	if (debug_level>1) std::cout << "header buffer class con-structor call with sensor data" << std::endl;

    unsigned int mid = sensordata.message_id;
    int type = sensordata.type;
    int platform = sensordata.platform;
    //unsigned int datalength = sensordata.data->rows * sensordata.data->cols * sensordata.data->depth * sizeof(int);
    unsigned int datalength = sensordata.get_data_size();

    if (debug_level>1) {
		std::cout << "given sensor data" << std::endl;
		std::cout << "message id = " << mid << std::endl;
		std::cout << "type = " << type << std::endl;
		std::cout << "platform = " << platform << std::endl;
		std::cout << "datalength = " << datalength << std::endl;
    }

    // create the buffer object and link the base pointer
    u_data_ptr = std::unique_ptr<buffer_type>( new buffer_type(TCP_HEADER_LENGTH,char(0)) );

    // create the test header entries
    insert(0,"con");
    insert(31,"end");
    //insert(54,"end");	//should return immediately
    //insert(32,"end");	// should return early when the end of the buffer is reached

    insert(3,mid);
    insert(7,type);
    insert(11,platform);
    insert(15,datalength);
}

header_buffer_class::~header_buffer_class(){
	if (debug_level>0) std::cout << "header buffer class de-structor call" << std::endl;

	if (u_data_ptr == nullptr){ if (debug_level>0) std::cout << "header_buffer_class : destructor - u_data_ptr == nullptr" << std::endl; }

	if (u_data_ptr) {
    	std::cout << "header_buffer_class : destructor - clearing the buffer vector" << std::endl;
    	u_data_ptr -> clear();
    	std::cout << "header_buffer_class : destructor - releasing the buffer vector" << std::endl;
    	u_data_ptr.release();
    }
}

int header_buffer_class::insert(int pos, int value){
	unsigned int transfer = (unsigned int) value;
	return insert(pos, transfer);
}

int header_buffer_class::insert(int pos, unsigned int value){

	int s = get_data_size();
    if (pos<0) return -1;
    if (pos>s) return -1;

    int l = sizeof(unsigned int);
    if (pos>s-l) return -2;

	char *buffer = get_data_ptr();


	buffer[pos + 3] = (value>>24) & 255; //0xFF;
	buffer[pos + 2] = (value>>16) & 255; //0xFF;
	buffer[pos + 1] = (value>>8) & 255; //0xFF;
	buffer[pos + 0] = (value>>0) & 255; //0xFF;

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
