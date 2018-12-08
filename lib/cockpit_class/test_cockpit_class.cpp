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

end_flag_class G_END_FLAG;

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
 
            /*            
            message = unique_message_ptr( new message_class(
                message_id, cockpit_addr,
                camera_addr, sensor_time,
                data_length, rows, cols, channels, std::move(data_mat)
                )
            );
            */

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
    G_MESSAGE_QUEUE_PTR->set_shut_down_flag();
    std::cout << "waiting for message queue to confirm readyness to shut down" << std::endl;
    while (G_MESSAGE_QUEUE_PTR -> get_shut_down_flag() == false) {};
    std::cout << "camera exiting has confirmed shutdownflag" << std::endl;
}

void dummy_thread(){

    address_class tcp_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::tcp);

    std::time_t ct;
    std::cout << "dummy_thread locks its mutex and will sleep" << std::endl;
    int reader_index = tcp_addr.get_process_index();
    std::unique_lock<std::mutex> lock_it(G_QUEUE_COORDINATION_VARS_OBJ.message_available_mutex[reader_index]);
    while(true) {
        //std::cout << "dummy_thread releases its mutex and sleeps" << std::endl;
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[reader_index].wait(lock_it);
        if (G_MESSAGE_QUEUE_PTR -> get_shut_down_flag() == true) break;
        ct = get_time();
        //std::cout << "dummy_thread = " << reader_index << " wakes up @ " << ct << std::endl;
        unique_message_ptr message = G_MESSAGE_QUEUE_PTR->dequeue(tcp_addr);
        //std::cout << "dummy_thread shoud not receive message, dropping the garbage" << std::endl;
    }
    ct = get_time();
    std::cout << "dummy_thread pid = " << tcp_addr << " verified the shutdown flag and is exiting @ " << ct << std::endl;
}

int main(int argc, char *argv[])
{
    int N = 10;
    if ( (argc>1) && (argc<3) )  N = atoi(argv[1]);
    std::cout << "running for approx " << N << " seconds" << std::endl;

    // create global message queue and endflag
    std::cout << "creating global message queue" << std::endl;
    G_MESSAGE_QUEUE_PTR = new message_queue_class();
    std::cout << "pulling global end flag" << std::endl;
    extern end_flag_class G_END_FLAG;

    // start the tcp server and client, the camera will use it send images, the cockpit will receive and display them
    std::cout << "creating dummy process for tcp address" << std::endl;
    std::thread dummy_process(dummy_thread);

    // start the camera process
    std::cout << "creating camera process" << std::endl;
    std::thread camera(&camera_thread);

    // create the cockpit process, it receives and displays images
    std::cout << "creating cockpit process" << std::endl;
    address_class cockpit_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit);
    // this doesn't work because it seems the object destructs right after the call and then there is no cockpit anymore to process
    //std::thread cockpit_process( &cockpit_class::keep_processing , cockpit_class(cockpit_addr) );
    // rather construct the cockpit and pass it as param for the processing, now there are 2 calls to the cockpit destructor ....

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

    // the camera thread is monitoring the end flag and shuts down the queue
    std::cout << "time is up, setting global end flag " << std::endl;
    G_END_FLAG.set_flag();

    // all other prcesses monitor the message queue state and shut down when the queue shuts down
    // wait for everybody to finish
    
    camera.join();
    dummy_process.join();
    cockpit_thread.join();
    std::cout << "processes ended" << std::endl;

    // clean up
    std::cout << "deleteting queue object" << std::endl;
    delete G_MESSAGE_QUEUE_PTR;

    std::cout << "main program done" << std::endl;

    /*
    cv::namedWindow("color_filter",2);
    cv::Mat filter = filter_image(frame,count);
    float factor = 0.8;
    distance = factor * distance + (1 - factor) * count;

    std::ostringstream oss;
    oss << "distance = " << distance;
    std::string output = oss.str();

    cv::putText(filter, output, cv::Point(30,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.8, cv::Scalar(200,200,250), 1, 16); //cv::CV_AA);
    cv::imshow("color_filter", filter);
    long count, distance=0;
    */
    

    return 0;

/*
    std::cout << "main program connecting to message queue " << std::endl;
    message_queue &m_q = message_queue::getInstance();
    m_q.debug_level = 0;


	//int count;

    std::cout << "main program starting tcp server" << std::endl;
    //tcp_loop();
    std::thread tcp_loop_thread (tcp_loop);

    std::cout << "main program starting loop" << std::endl;
    while(1){

        int count = m_q.get_count();
    	std::cout << "Message queue count : " << count << std::endl;
        //unique_message_ptr deleter = m_q.dequeue_message();
        //if (deleter != NULL) deleter.reset(); 
        sleep(1);
    }

*/
    /*    

    // keep receiving messages, the processing happens in the function call where
    // the data is received and inserted in a message
    // this message is then inserted into the message queue
	printf("starting server connection management loop\n");
	while(1){

        count = m_q.get_count();
    	std::cout << "Message queue count : " << count << std::endl;
        sleep(2);
 
        if (false) {
            result = tcp.connect_and_receive();
		    if (result<0)
			    printf("error connecting to port\n");
        }

//		else
//			printf("received message, length = %lu\n",message_length);
	}
    */
 
   //tcp_loop_thread.join();


	return 0;
}



/*
    printf("the main creates a global message controller queue and several readers one of which is a 'writer' process\n");
    printf("the 'writer process' regularly queues a message to a random target reader and does not receive message itself\n");
    printf("all other reader proocess sometimes create a response to a random other reader\n");
    printf("the processes run for SEC secs, a watcher thread sets the global the_end flag to true\n");

    printf("the main watches the global end flag and once it occurs it calls the message queue shutdown function\n");
    printf("it then joins the terminating processes and afterwards deletes the message controller\n");

    printf("the global controller manages the message queue, all operations are implemented with a lockguard\n");
    printf("new messages are queued and then for the intended receipient of the first messgage in the queue the condition variable is notified\n");
    printf("each reader has its own condition variable (array)  and the target id is used as the index for the condition variable array\n");
    printf("when a reader tries to read the first element in the queue and it is not for the reader, -1 is returned\n");
    printf("and the condition variable of the target id is called\n");
    printf("the controller is not a process rather a global object - so it doesn't monitor the the_end-flag\n");
    printf("it provides a shutdown function which the main calls to end the process\n");
    printf("in the shutdown function the controller notifies the reader for the first message in the qeueu, until\n");
    printf("the queue is empty. Then the controller sets teh shutdonw flag and notifies all readers so that they can see the shutdown flag is set and terminate\n");

    printf("message ids are created sequentially by the controller, so they are unique but they maybe queued out of order or displayed out of order\n");
    printf("since the shutdown is synchronized all messages should be completely printed\n");

    printf("each reader has a unique integer id which is used to identify its condition variable and allows to select the correct messages\n");
    printf("when the readers gets notified and holds the lock, it first checks the shutdown flag and starts the exit process if that is the case\n");
    printf("in its loop the reader dequeues the next massage, it can assume it is for him since it was notified. if this is not the case -1 is returned\n");
    printf("if this element is for the reader the value is reutrned and the element is removed from the queue\n");
    printf("the controller ensures that the queue will be emptied at shutdown, so it will notify the process to read their remaining message\n");
    printf("if during this process a reader wants to send a response it will be rejected (at this point the shutdown flag is not yet set)\n");
    printf("when the shutdown flag is set the queue will have been emptied so when the reader sees the flag it can exit\n");

    printf("compilation with the debug and threadsnaitizer, if the interprocess communication is wrong warnings will be issued\n");
    printf("run with ./7_queue_mpmc_cond_vars > test.txt 2>&1 /  to catch the warnings");
    printf("to test run with ./7_queue_mpmc_cond_vars > test.txt    then compare\n");
    printf("cat test.txt | grep q ing \n");
    printf("cat test.txt | grep reads | grep -v -1\n");

*/

