#include <cockpit_class.h>

// stores the address and creates the main display window
// keep_processing will receive the data, process it and display it
// currently on ly the display is implemented
cockpit_class::cockpit_class(const address_class &whoami) :
    event_counter_(0), 
    whoami_(whoami),
    cockpit_debug_level_(COCKPIT_DEBUG_LEVEL)
{
    std::cout << "cockpit class : constructor, who am i : " << whoami_ << std::endl;
    cockpit_process_id_ = G_MESSAGE_QUEUE_PTR -> register_process(whoami_);
    cv::namedWindow("PC_Cockpit",1);
}

// destroys teh display window
cockpit_class::~cockpit_class(){
    std::cout << "cockpit class : de - constructor" << std::endl;
    cv::destroyWindow("PC_Cockpit");
}

// should not be called
cockpit_class::cockpit_class() : event_counter_(0),
            whoami_(address_class(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit))
{
    std::cout << "cockpit class : private default constructor, who am i : " << whoami_ << std::endl;
}

// returns a thread which calls the main processing loop
std::thread cockpit_class::operator()() {
    return std::thread(&cockpit_class::keep_processing,this);
}

// the main processing loop
// sleeps on the condition variable, which is activated by the message queue
// check the global end flag and the shtudown flag of the message queue to find out when to shut down
void cockpit_class::keep_processing(){

    std::time_t ct;

    std::time_t t_last_image(0);
    unsigned int camera_frequency(0);

    // get lock then in loop sleep until notified
    std::cout << "cockpit locks its mutex" << std::endl;
    std::unique_lock<std::mutex> lock_communication_mutex(G_MESSAGE_QUEUE_PTR -> message_available_mutex[cockpit_process_id_]);
    while(G_END_FLAG.read_flag()==false) {
        event_counter_++;

        //std::cout << "\n\n===========================================" << std::endl;
        //std::cout << "cockpit releases its mutex and sleeps" << std::endl;
        G_MESSAGE_QUEUE_PTR -> message_available_condition[cockpit_process_id_].wait(lock_communication_mutex);
        // after waking up the process holds the lock
        // check for spuriuos wakeup and process message

        // also check if the shutdown flag was set (message queue only sets shutdown flag if the queue is empty)
        // if (G_MESSAGE_QUEUE_PTR -> get_shut_down_flag() == true) break;

        // check if message is available, if not it is either a spurious wakeup or a the queue is shutting down
        if ( G_MESSAGE_QUEUE_PTR -> message_available_flag[cockpit_process_id_] == true ) {

            ct = get_time();
            if (cockpit_debug_level_>1) std::cout << "cockpit  = " << cockpit_process_id_ << " wakes up @ " << ct << std::endl;

            // fetch and process the message
            unique_message_ptr message = G_MESSAGE_QUEUE_PTR->dequeue(whoami_);

            std::time_t t_this_image = message -> get_sensor_time();
            if (event_counter_ > 1) {
                camera_frequency = (t_this_image-t_last_image);
            }
            t_last_image = t_this_image;

            //std::cout << "\nmessage received " << message->get_id() << std::endl;
            if (message!=nullptr) {
                std::unique_ptr<cv::Mat> frame = message->fetch_data();
                //cv::Mat frame = uframe -> get();
                std::string output = "dt=" + std::to_string(camera_frequency);
                //std::cout << "dt = " << camera_frequency << "   " << output << std::endl;
                cv::putText(*frame, output, cv::Point(30,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(255,0,0), 1, cv::LINE_AA);
                cv::imshow("PC_Cockpit", *frame);
                //test_message -> print_meta_data();
            }

            // message processed -> set com flag
            G_MESSAGE_QUEUE_PTR -> message_available_flag[cockpit_process_id_] = false;
            // check if the shutdown flag was set (message queue only sets shutdown flag if the queue is empty)
            // do that after message has been processed,so that the flag has been set to false, else the queueu thinks the message needs to still be processed
            // if (G_MESSAGE_QUEUE_PTR -> get_shut_down_flag() == true) break;

            if( cv::waitKey(1) >= 0) break;
        } else {
            // if woken up, but the data flag is false then check for shutdown, else it is spurious wake up
            if (G_MESSAGE_QUEUE_PTR -> get_shut_down_flag() == true) break;
            else std::cout << "cockpit : keep_processing : wake up , no data, no shutdown -> spurious wake up?" << ct << std::endl;
        }
    }
    ct = get_time();
    std::cout << "\ncockpit pid = " << cockpit_process_id_ << " verified the shutdown flag and is exiting @ " << ct << " with count = " << event_counter_ << std::endl;

    bool result = G_MESSAGE_QUEUE_PTR -> deregister_process(whoami_);
    std::cout << "\ncockpit deregistering from message queue , result = " << result << std::endl;
}
