/*
 * message_queue.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <message_queue.h>

std::mutex mtx1;           // mutex for critical section

//ptr_message test = new(message_class);

message_queue& message_queue::getInstance() {
	mtx1.lock();
	printf("call to message_queue.getInstance\n");
    static message_queue instance;
    mtx1.unlock();
	return instance;
}

void message_queue::add_message(ptr_message mp){
    message_q.push(mp);
}

ptr_message message_queue::get_next_message(){
    return message_q.front();
}

message_queue::message_queue():max_elements(256) {}
message_queue::~message_queue() {}



//remove below, testing only


void message_queue::print_block (int n, char c) {
  // critical section (exclusive access to std::cout signaled by locking mtx):
  for (int i=0; i<n; ++i) { std::cout << c; }
  std::cout << '\n';

}

int message_queue::getData() { return this->max_elements; }
void message_queue::setData(int d){this->max_elements = d; };
