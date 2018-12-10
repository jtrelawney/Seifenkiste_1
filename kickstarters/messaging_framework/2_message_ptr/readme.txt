

--------------------------------------------------------------------------------------------------------------------------------------

use the 1_basic framework andintroduce message pointers
the major change is the unique ptr to refer to messages throughtout the process
the simple message structure may hide the need for a custom move operator

valgrind complains about some memory which is still accessiable
however all messages are deleted at the end

next step is to swap in functionality in small portions and see what works

--------------------------------------------------------------------------------------------------------------------------------------

this builds on the thread example 4 where a simple single writer multiple readers example is implemented

this example here builds a more complex scenerio where the different actores are build as separate classes with the goal of:

1. seperating and building the classes for tcp (writer), queue, display (readers) as libraries
2. launching them as threads, using the queue to synchronize
3. implementing a global message object (singleton, global variable etc)
4. have the main program monitor the status and shutdown on request or error




