#include <iostream>       // std::cout
#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>

#include "opencv2/opencv.hpp"
#include <common.h>
#include <message_class.h>
#include <sensor_camera_class.h>


#include <cockpit_class.h>

// message_queue_class.h imports the coordination variables and mutex

#include<message_queue_class.h>

// define a global queue object for all processes / objects to refer to using extern 
message_queue_class *G_MESSAGE_QUEUE_PTR;

extern end_flag_class G_END_FLAG;

void camera_thread(){

    // whoami and recipient
    address_class camera_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::camera1,address_class::process_type_def::undefined);
    address_class cockpit_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit);

    sensor_camera_class *camera = new sensor_camera_class();
    
    if ( camera->init_sensor() < 0 ) {
        std::cout << "error initializing camera" << std::endl;
        return;
    }

    unsigned int frame_count(0);
    unique_message_ptr message;

    while(G_END_FLAG.read_flag()==false) {
        cv::Mat frame;
        camera->acquire_data();
        if (camera->fetch_current_frame(frame) < 0 ) std::cout << "frame invalid" << std::endl;
        else {
            unsigned int message_id = (unsigned int) frame_count;
	        time_format sensor_time = get_time_stamp();
            time_format sender_time = get_time_stamp();
        
            std::unique_ptr<cv::Mat> data_mat = std::unique_ptr<cv::Mat> ( new cv::Mat(frame) );

            message = unique_message_ptr( new message_class(
                message_id, cockpit_addr,
                camera_addr, sensor_time,
                std::move(data_mat)
                )
            );

            message -> set_sender_address(camera_addr);
            message -> set_sender_time(sender_time);

            G_MESSAGE_QUEUE_PTR->enqueue(std::move(message));
            frame_count++;
        }
    }

    std::cout << "camera done , current framecount = " << frame_count << std::endl;
}

int main(int argc, char *argv[])
{
    int N = 3;
    if ( (argc>1) && (argc<3) )  N = atoi(argv[1]);
    std::cout << "running for approx " << N << " seconds" << std::endl;

    // create global message queue and pull in the global endflag
    std::cout << "creating global message queue" << std::endl;
    G_MESSAGE_QUEUE_PTR = new message_queue_class(address_class::platform_type_def::pc);
    std::cout << "pulling global end flag" << std::endl;
    extern end_flag_class G_END_FLAG;

    // start the camera process
    std::cout << "creating camera process" << std::endl;
    std::thread camera(&camera_thread);

    // create the cockpit process, it receives and displays images
    std::cout << "creating cockpit process" << std::endl;
    address_class cockpit_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit);

    //cockpit_class cockpit_obj(cockpit_addr);
    //std::thread cockpit_process( &cockpit_class::keep_processing , cockpit_obj );
    std::thread cockpit_thread = cockpit_class(cockpit_addr)();

    // run for N secs then set the global end flag
    int counter(0);
    while(counter<N){
        counter++;
        std::cout << "running for " << counter << " s" << std::endl;
        sleep(1);
    }

    // all threads which are communicating via the queue object will be informed by the queue and shut down
    std::cout << "time is up, shutting down" << std::endl;

    std::cout << "request queue shutdown" << std::endl;
    G_MESSAGE_QUEUE_PTR -> request_shut_down();

    // the camera thread is monitoring the end flag and shuts down the queue
    std::cout << "set global end flag, camera should see that and exit" << std::endl;
    G_END_FLAG.set_flag();

    // queue is going to inform her clients
    std::cout << "now do the queue shutdown" << std::endl;
    G_MESSAGE_QUEUE_PTR -> shut_down();


    // wait for everybody to finish
    camera.join();
    cockpit_thread.join();
    std::cout << "processes ended" << std::endl;

    // clean up
    std::cout << "deleteting queue object" << std::endl;
    delete G_MESSAGE_QUEUE_PTR;

    std::cout << "main program done" << std::endl;

    return 0;
}
