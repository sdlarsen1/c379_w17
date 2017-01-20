#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void interrupt(int sig) {
  printf("\nThis is the signal handler for SIGINT\n");
  printf("The signal number received is %d\n", sig);
  exit(1);
  //(void) signal(SIGINT, SIG_DFL);
}

void stop(int sig) {
  printf("\nThis is the signal handler for SIGTSTP\n");
  printf("The signal number received is %d\n", sig);
  exit(1);
  //(void) signal(SIGTSTP, SIG_DFL);
}

int main(int argc, char* argv[]) {
  int n = atoi(argv[1]);

  (void) signal(SIGINT, interrupt);
  (void) signal(SIGTSTP, stop);

  for (int i = 0; i < n; i++) {
    printf("%d\n", i);
    sleep(1);
  }
}
