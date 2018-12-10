this is a working version of the base tcp class and the derived client and server classes.

the test program sends some text from the client to the server.

The client transfers strings and the server prints the result on the screen.

the loggin is detailed to show which steps happen.

Currentlt the received data cannot be transfered out of the server object, this is the next step to implement passing on the data.

Furthermore the server loop is started in a process and while its child process terminates when the client signals the end of transmission the parent process is detached and runs in an endless loop.
One step would be to signal the server when to exit.
