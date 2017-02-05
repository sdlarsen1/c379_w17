#include "findpattern.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <setjmp.h>


typedef unsigned char BYTE;

jmp_buf env;
enum {SUCCESS, FAILURE} status;	

/* This signal handler re-registers itself, then executes siglongjmp, 
	which will set the variable "status" to FAILURE*/
void segfault_handler(int sig, siginfo_t* sigInfo, void* ucontext)
{
    struct sigaction saSigSegV;     // re-register signal handler
    saSigSegV.sa_sigaction = (segfault_handler);
    saSigSegV.sa_flags = SA_RESTART | SA_SIGINFO | SA_RESETHAND;

    if(sigaction(SIGSEGV, &saSigSegV, NULL) < 0)
    {
            perror("ERROR: registering SigFault_Handler\n");
            exit(1);
    }
    siglongjmp(env, 1);
}

unsigned int findpattern(unsigned char *pattern,
			unsigned int patlength,
			struct patmatch *locations,
			unsigned int loclength)
{
	BYTE *mem_ptr, *page_ptr;
  	BYTE test, mode = 0;
  	unsigned int num_patterns_found = 0;

	// save old signal handler
	struct sigaction oldSigSegV;
	if(sigaction(SIGSEGV, NULL, &oldSigSegV) < 0)
  	{
    	        perror("ERROR: saving old sigsegv handler\n");
 	           exit(1);
 	}

    	struct sigaction saSigSegV;     // set up out signal handler
    	saSigSegV.sa_sigaction = (segfault_handler);
   	saSigSegV.sa_flags = SA_RESTART | SA_SIGINFO | SA_RESETHAND;

   	if(sigaction(SIGSEGV, &saSigSegV, NULL) < 0)
  	{
    	        perror("ERROR: registering SigFault_Handler\n");
 	           exit(1);
 	}

	for ( page_ptr = (void *) (unsigned long) 0; 
			(unsigned long) page_ptr < MAX_ADDRESS - getpagesize();
			page_ptr += (unsigned long)  getpagesize() )
	{
		mem_ptr = page_ptr;	// start of page

		status = sigsetjmp(env, 1);	// status == FAILURE when we segfault
		
		if (status == FAILURE) continue;// memory not readable? next page

		test = *mem_ptr;		// read test

		status = sigsetjmp(env, 1);     
		mode = MEM_RO;

		if (status == SUCCESS)
		{
			test = *mem_ptr;	// write test
			*mem_ptr  = 0;
			*mem_ptr = test;
			mode = MEM_RW;
		}

		//printf("Address %.8x\tMode %d\n", (int) (unsigned long) mem_ptr, mode);
		//------FOR DEBUG-------------
	
		// pattern matching section
		int num_char_match = 0;
		for (;mem_ptr < page_ptr + getpagesize(); mem_ptr++)
		{
		    if (*mem_ptr == pattern[num_char_match])
		    {
		        num_char_match++;


		        if (num_char_match == patlength) // found the pattern
		            {
			    // add the pattern
			    if (num_patterns_found < loclength)
			    {
				locations->location = (unsigned int) (mem_ptr - patlength) + 1;
				locations->mode = mode;
				locations++;
			    }
			    num_patterns_found++;
			    num_char_match = 0;
		        }
		    }
		    else    // pattern broke, but we need to check if
			  // this is the start of a new pattern.
		    {
		        num_char_match = 0;
		        if (*mem_ptr == pattern[num_char_match])
		        {
			    num_char_match++;

		            if (num_char_match == patlength)
		            {
			        // add the pattern
			        if (num_patterns_found < loclength)
			        {
				    locations->location = (unsigned int) (mem_ptr - patlength) + 1;
				    locations->mode = mode;
				    locations++;
			        }
			        num_patterns_found ++;
			        num_char_match = 0;
		            }
		        }
		    }
		}

	}
	if(sigaction(SIGSEGV, &oldSigSegV, NULL) < 0)
    	{
            perror("ERROR: restoring old signal handler\n");
            exit(1);
    	}

	return num_patterns_found;
}
