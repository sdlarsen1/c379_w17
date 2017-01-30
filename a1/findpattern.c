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

void segfault_handler(int sig, siginfo_t* sigInfo, void* ucontext);

void register_handler(void)
{


        return;

}


void segfault_handler(int sig, siginfo_t* sigInfo, void* ucontext)
{
    struct sigaction saSigSegV;     // re-register
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

    struct sigaction saSigSegV;     // re-register
    saSigSegV.sa_sigaction = (segfault_handler);
    saSigSegV.sa_flags = SA_RESTART | SA_SIGINFO | SA_RESETHAND;

    if(sigaction(SIGSEGV, &saSigSegV, NULL) < 0)
    {
            perror("ERROR: registering SigFault_Handler\n");
            exit(1);
    }

	for ( page_ptr = (void *) (unsigned long) 0; (unsigned long) page_ptr < MAX_ADDRESS;
			page_ptr += (unsigned long)  getpagesize() )
	{
		mem_ptr = page_ptr;

		status = sigsetjmp(env, 1);	// status == SUCCESS
		printf("Address %x\tStatus %d\n", (int) (unsigned long) mem_ptr, status);


		if (status == FAILURE) continue;// memory not readable

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

		int num_match = 0;
		for (;mem_ptr < page_ptr + getpagesize(); mem_ptr++)
		{
		  if (*mem_ptr == pattern[num_match])
		  {
		    num_match += 1;

		    if (num_match == patlength)
		      {
			// add the pattern
			locations->location = (unsigned int) mem_ptr - patlength;
			locations->mode = mode;
			locations++;
		      }
		  }
		  else
		  {
		    num_match = 0;
		  }
		}

	}

	return 0;
}
