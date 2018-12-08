#include<address_class.h>
#include <sensor_camera_class.h>
#include "opencv2/opencv.hpp"


address_class define_camera_sensor_type(){
    address_class::platform_type_def platform;
    address_class::sensor_type_def sensor;
    address_class::process_type_def process;

    platform = address_class::platform_type_def::rpi;
    sensor = address_class::sensor_type_def::camera1;
    process = address_class::process_type_def::undefined;
    return address_class(platform,sensor,process);
}

int main()
{
    cv::namedWindow("usb_camera",1);
    sensor_camera_class *camera = new sensor_camera_class();
    address_class sensor_type = define_camera_sensor_type();
    camera -> set_sensor_type(sensor_type);

    if ( camera->init_sensor() < 0 ) {
        std::cout << "error initializing camera" << std::endl;
        return -1;
    }

	for(;;)
    {
        cv::Mat frame;
        camera->acquire_data();

        if (camera->fetch_current_frame(frame) < 0 ) std::cout << "frame invalid" << std::endl;
        else cv::imshow("usb_camera", frame);
		
		//prep(current_time, image_counter, frame.clone());
		//prep(current_time, image_counter, frame);
        
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
        //if (image_counter>1000) break;
        if( cv::waitKey(30) >= 0) break;
    }

    return 0;
}
