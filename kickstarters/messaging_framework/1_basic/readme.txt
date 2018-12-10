this builds on the thread example 4 where a simple single writer multiple readers example is implemented

this example here builds a more complex scenerio where the different actores are build as separate classes with the goal of:

1. seperating and building the classes for tcp (writer), queue, display (readers) as libraries
2. launching them as threads, using the queue to synchronize
3. implementing a global message object (singleton, global variable etc)
4. have the main program monitor the status and shutdown on request or error




