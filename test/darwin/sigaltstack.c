/*

 * Copyright (c) 2002-2003, Intel Corporation. All rights reserved.
 * Created by:  rusty.lynch REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this 
 * source tree.

  Test case for assertion #12 of the sigaction system call that verifies
  signal-catching functions are executed on the alternate stack if the
  SA_ONSTACK flag is set in the sigaction.sa_flags parameter to the
  sigaction function call, and an alternate stack has been setup with 
  sigaltstack().

  NOTE: This test case does not attempt to verify the proper operation
        of sigaltstack.
*/

#define _XOPEN_SOURCE 600

#define PTS_UNRESOLVED 1
#define PTS_PASS 0

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
//#include "posixtest.h"

stack_t alt_ss;

void handler(int signo)
{
  stack_t ss;

  printf("Caught SIGXFSZ\n");

  if (sigaltstack((stack_t *)0, &ss) == -1) {
    perror("Unexpected error while attempting to setup test "
	   "pre-conditions");
    exit(-1);
  } else {
    printf("INFO: %s sigaltstack((stack_t *)0, &ss)returned successfully\n",__func__);
  }

  
  if (ss.ss_sp != alt_ss.ss_sp || ss.ss_size != alt_ss.ss_size) {
    printf("Test FAILED\n");
    exit(-1);
  }
}

int main()
{
  struct sigaction act;
  
  act.sa_handler = handler;
  act.sa_flags = SA_ONSTACK;
  sigemptyset(&act.sa_mask);
  if (sigaction(SIGXFSZ,  &act, 0) == -1) {
    perror("Unexpected error while attempting to setup test "
	   "pre-conditions");
    return PTS_UNRESOLVED;
  } else {
    printf("INFO: sigaction((SIGXFSZ,  &act, 0) returned successfully\n");
  }


  if ((alt_ss.ss_sp = (void *)malloc(SIGSTKSZ)) == NULL) {
    perror("Unexpected error while attempting to setup test "
	   "pre-conditions");
    return PTS_UNRESOLVED;
  } else {
    printf("INFO: Successfully malloc'd alternative stack\n");
  }

  alt_ss.ss_size = SIGSTKSZ;
  alt_ss.ss_flags = 0;
  
  if (sigaltstack(&alt_ss, (stack_t *)0) == -1) {
    perror("Unexpected error while attempting to setup test "
	   "pre-conditions");
    return PTS_UNRESOLVED;
  } else {
    printf("INFO: sigaltstack(&alt_ss, (stack_t *)0) returned successfully\n");
  }


  if (raise(SIGXFSZ) == -1) {
    perror("Unexpected error while attempting to setup test "
	   "pre-conditions");
    return PTS_UNRESOLVED;
  } else {
    printf("INFO: raise(SGABRT) returned successfully\n");
  }


  printf("Test PASSED\n");

  return PTS_PASS;
}
