# CMPUT 379 Assignment 2

## Client
The client is activated by running:

    $ ./wbc379 <ip address> <port number> <keyfile>

* Currently the client has issues when receiving, then attempting to decrypt encrypted entries.
* The client cannot run without a keyfile

The client provides a simple UI which allows the user to specify their query type, guides them through data entry, then formats their response and sends it to the server following the C379 Whiteboard protocol.
When getting a response from the server, the client will display the message header (eg. `Header: !12p44`) as well as, if it exists, the message (eg. `Message: I like toast!`). Upon completion of a successful POST or clear request, the client will display only the header of the success message (eg. `Header: !12e0`).

## Server

### Code Reuse
Code taken from
-  http://stackoverflow.com/questions/9825114/gethostbyaddr-returns-0
-  http://stackoverflow.com/questions/22863977/dynamically-allocated-2d-array-of-strings-from-file
