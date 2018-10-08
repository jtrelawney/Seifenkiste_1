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


//std::mutex mtx1;           // mutex for critical section

using namespace std;


class message_queue {
private:
	static message_queue &instance;
	int max_elements;

	// remove access to functions that could be used to create new instances / duplicates
	message_queue();
	~message_queue();
	message_queue(const message_queue&);
	const message_queue& operator=(const message_queue&);

public:


	void print_block (int n, char c) {
	  // critical section (exclusive access to std::cout signaled by locking mtx):

	  for (int i=0; i<n; ++i) { std::cout << c; }
	  std::cout << '\n';

	}

	static message_queue &getInstance() {
		//mtx1.lock();
		printf("call to message_queue.getInstance\n");
        static message_queue instance;
        //mtx1.unlock();
		return instance;
	   }

	int getData() {
	      return this->max_elements;
	   }

	void setData(int d){this->max_elements = d; };


	//message_queue(const int max_elems):max_elements(max_elems){};
};


#endif /* COMMON_MESSAGE_QUEUE_H_ */
