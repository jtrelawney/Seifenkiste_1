
version 2:
basic message class stub and basic buffer class to test the tcp process
both have been developed further , and on this way the buffer stub was changed
for example away from a virutal base buffer class
hence no interest in figuring out the issues, or continue to test further kickstart versions, 
rather finalize the tcp class


==========================================================================================================================================================================================================

Version 4 (not done):
Implement a handshake on message level , i.e. sending confirmation that a message has been received (based on message_id).
evaluate a message handshake on complete message vs handshake on header and data.

==========================================================================================================================================================================================================

Version 3 (not done)
continously sending N messages in quick sucession.
add the data transfer mechanism and confirm both header and messages are correctly transfered

==========================================================================================================================================================================================================

Version 2:
use a buffer class to encapsulate the char* representation of the tcp interface.
implement a simple message class which allows to insert and extract information from a message header
test the transfer to confirm the insert/extract mechanism is working and test that 0 value bytes are properly handled (ie. potential strings reads will terminate at the first 0).

==========================================================================================================================================================================================================

Version 1:

this is a working version of the base tcp class and the derived client and server classes.

the test program sends some text from the client to the server.

The client transfers strings and the server prints the result on the screen.

the loggin is detailed to show which steps happen.

Currentlt the received data cannot be transfered out of the server object, this is the next step to implement passing on the data.

Furthermore the server loop is started in a process and while its child process terminates when the client signals the end of transmission the parent process is detached and runs in an endless loop.
One step would be to signal the server when to exit.
