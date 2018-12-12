//#include <sstream>
//#include <string.h>

#include <iostream>       // std::cout
#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>

#include "opencv2/opencv.hpp"
//#include <common.h>
#include <message_class.h>
#include <sensor_camera_class.h>
#include <tcp_client.h>


void send_messages(tcp_client &client, const int N){

    address_class recipient_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit);
    address_class camera_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::camera1,address_class::process_type_def::undefined);

    // create the camera object
    sensor_camera_class *camera = new sensor_camera_class();
    if ( camera->init_sensor() < 0 ) {
        std::cout << "error initializing camera" << std::endl;
        return;
    }
    
    // now create and send N camera images
    int frame_count(0);
    unique_message_ptr message;

    while(frame_count < N) {
        cv::Mat frame;
        camera->acquire_data();
        if (camera->fetch_current_frame(frame) < 0 ) std::cout << "frame invalid" << std::endl;
        else {
            unsigned int message_id = (unsigned int) frame_count;
	        time_format sensor_time = get_time_stamp();
            time_format sender_time = get_time_stamp();
        
            //std::unique_ptr<cv::Mat> data_mat = std::unique_ptr<cv::Mat> ( new cv::Mat(3,3, CV_8UC3, cv::Scalar(0,128,255) ) );
            std::unique_ptr<cv::Mat> data_mat = std::unique_ptr<cv::Mat> ( new cv::Mat(frame) );

            message = unique_message_ptr( new message_class(
                message_id, recipient_addr,
                camera_addr, sensor_time,
                std::move(data_mat)
                )
            );

            message -> set_sender_address(camera_addr);
            message -> set_sender_time(sender_time);

            std::cout << "send the test message" << std::endl;
            int result = client.send_message(std::move(message));
            if (result<0) std::cout << "error sending message via TCP" << std::endl; //client.print_status();

            frame_count++;
        }
    }
    std::cout << "camera done , current framecount = " << frame_count << std::endl;
}

int main(int argc, char *argv[])
{
    // standard is 100 images, unless provided differently
    int N = 100;
    if ( (argc>1) && (argc<3) )  N = atoi(argv[1]);
    std::cout << "sending " << N << " camera images" << std::endl;


    // start the client
    std::cout << "\n\n starting the tcp client" << std::endl;
    tcp_client client;
    client.start_up();
    
    // now send messages
    send_messages(client, N);
    
    std::cout << "\n\n shutting down the tcp client in 2s" << std::endl;
    sleep(2);
    client.shut_down();

    return 0;
}
