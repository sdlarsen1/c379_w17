# CMPUT 379 Assignment 2

## Client

## Server

To start the server, run the command

./wbs379 {portnumber} {-n number_of_entries | -f statefile}

where portnumber is the port you want clients to connect to,
number_of_entries is how many entries the whiteboard has or
statefile which follows the specifications in the assignment
description.

The process will fork a daemon and print it's process ID
to STDIO. When you want to exit the server, run the command

kill {PID}

where PID is the given ID of the daemon.

After exiting, the server will have produced two files:
whiteboard.all :
	this a statefile representing the state of the
	whiteboard at the time of exiting.

server.log :
	a log file with information about what the server had
	done while running.


### Code Reuse
Code taken from
-  http://stackoverflow.com/questions/9825114/gethostbyaddr-returns-0
-  http://stackoverflow.com/questions/22863977/dynamically-allocated-2d-array-of-strings-from-file
