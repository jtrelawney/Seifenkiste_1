#include "opencv2/opencv.hpp"
#include "defs.h"
#include "message_class.h"
#include <unistd.h>

using namespace cv;

unsigned int image_counter;

void create_message(message_class **new_message, unsigned int message_id, time_format &sensor_time, unsigned int rows, unsigned int cols, unsigned int depth, char *image_data_buffer){

	//sender = rpi
	sender_type_def sender = rpi;
	//std::cout << "sender " << sender << std::endl;
	//  time sent = local time	
	time_format current_time = 0; //get_timestamp();
	//printf("message time = %lu\n",current_time);
	// sensor platform
	sender_type_def sensor_platform = rpi;
	// sensor type
	sensor_type_def sensortype= camera1;
	// sensor time
	// see parameter ::unsigned long sensor_time = get_timestamp()-1000;
	//printf("sensor time = %lu\n",sensor_time);
	// data length
	unsigned int data_length = rows*cols*depth;
	// ptr to databuffer
	//char *data_buffer = image_data_buffer;

	// initialize a new message
	message_class *test_message=new message_class(sender, current_time, sensor_platform, sensortype, sensor_time, data_length);
	test_message->set_id(message_id);
	char* ptr = test_message->get_data_buffer_ptr();
	memcpy(ptr,image_data_buffer,data_length);
	
	// prepare the special params and copy them into the message
	
	char *param_buffer = (char *) malloc(SPECIAL_PARAMS_BUFFER_LENGTH * sizeof(char));

	memcpy(&param_buffer[0],&rows,sizeof(unsigned int));
	memcpy(&param_buffer[4],&cols,sizeof(unsigned int));
	memcpy(&param_buffer[8],&depth,sizeof(unsigned int));
	//printf("param_buffer ptr     address = %lu   value = %lu\n", (unsigned int)&param_buffer, (unsigned int)param_buffer);
	int result = test_message->set_special_params(&param_buffer[0], SPECIAL_PARAMS_BUFFER_LENGTH);

	test_message->print_meta_data();
    //printf("new_message ptr     address = %lu   value = %lu\n", (unsigned int)&new_message, (unsigned int)new_message);
	//printf("message ptr address = %lu      message obj = %lu\n", (unsigned int)&test_message, (unsigned int)test_message);
	*new_message = test_message;
	//printf("new_message ptr     address = %lu   value = %lu\n", (unsigned int)&new_message, (unsigned int)new_message);
	//std::cout << "done with message" << std::endl;
	//delete test_message;
	//return;
}

void prep(unsigned int current_time, unsigned int image_counter, Mat &frame){
		// unsigned int elemsize, imgsize, 
		//determine image size
        //Mat frame1 = (frame.reshape(0,1));
        //elemsize = frame1.elemSize();
        //imgsize = frame1.total() * elemsize;
        unsigned int rows, cols, depth;
	    rows = frame.rows;
        cols = frame.cols;
        depth = frame.channels();        
        
        //std::cout << "\n\n\rows = " << rows << "   cols = " << cols << "   dep = " << depth << std::endl;
        //std::cout << "size = " << imgsize << "   calc = " << cols*rows*depth << std::endl << std::endl;
        
        // create the image message
        message_class *new_message=NULL;
        //printf("new_message ptr     address = %lu   value = %lu\n", (unsigned int)&new_message, (unsigned int)new_message);
		create_message(&new_message,image_counter, current_time, rows, cols, depth, (char *) frame.data);
        //printf("new_message ptr     address = %lu   value = %lu\n", (unsigned int)&new_message, (unsigned int)new_message);
		if (new_message != NULL) new_message->print_meta_data();
        if (new_message != NULL) delete new_message;
        //sleep(1);
        //printf("back here \n\n\n");        
}

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

	namedWindow("usb_camera",1);
    
	time_format current_time;
	unsigned int image_counter = 0;
	
	for(;;)
    {
		// catch next image and timestamp it
        Mat frame;
        cap >> frame; // get a new frame from camera
        current_time = get_timestamp();       
		image_counter++;
		
		//prep(current_time, image_counter, frame.clone());
		prep(current_time, image_counter, frame);
        
        imshow("usb_camera", frame);
		//printf("done displaying image\n\n\n");
		//sleep(1);
        //delete new_message;
        //printf("done deleting\n");
		//sleep(1);
        //printf("now lets see what happens when the mat goes ...\n");
        
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        //imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
