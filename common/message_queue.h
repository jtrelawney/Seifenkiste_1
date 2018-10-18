/*
 * message_queue.h
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#ifndef COMMON_MESSAGE_QUEUE_H_
#define COMMON_MESSAGE_QUEUE_H_

#include <iostream>
#include <mutex>

#include <queue>
#include <memory> //shared pointer
#include <message_class.h>




using namespace std;


//typedef std::shared_ptr<message_class> p_message;
typedef std::shared_ptr<message_class> ptr_message;


class message_queue {
private:
	static message_queue &instance;
	int max_elements;

    std::queue <ptr_message> message_q; 

	// remove access to functions that could be used to create new instances / duplicates
	message_queue();
	~message_queue();
	message_queue(const message_queue&);
	const message_queue& operator=(const message_queue&);


public:

    /*
    - message list
    - unprocessed list
    - flag as processed
    - flag for deletion
    - export bag
    - import bag
    */

	static message_queue &getInstance();

    void add_message(ptr_message mp);
    ptr_message get_next_message();
    

	void print_block (int n, char c);
	int getData();
	void setData(int d);
};


#endif /* COMMON_MESSAGE_QUEUE_H_ */
