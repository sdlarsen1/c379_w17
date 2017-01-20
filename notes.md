## Basic Signal Flow
1. OS becomes aware of an event for a process
2. OS can
  * ignore the signal
  * let the process catch the signal with custom handler
  * let the default action take place
3. some signals cannot be ignored or handled

## Signal handler Flow
1. handlers can be created and specified for a given event
2. when the signal is received, calls this handler
3. handler executes to completion

Common Use Cases
* Error conditions
  - segmentation fault
  - float processor errors
  - illegal instructions
* explicitly sent from one process to another
  - inter-process communication

## System Calls
- functions that can be called from within a C program
- executed within the kernel space
- considered "privileged" code, user has no control over its execution
- handing things off to the kernel to do "sensitive" operations

Signal Handling
* each signal has default handler
  - most default handlers exit process
- program can install its own handler for signals of any type, but there are exceptions:
  - SIGKILL
  - SIGSTOP

How to Handle Signals
- signal()
```
#include <signal.h>

void (*signal(int sig, void (*func) (int))) (int);
```
signal() returns a function which is the previous value of the function set up to handle the signal
