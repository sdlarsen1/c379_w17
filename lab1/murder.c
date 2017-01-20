#include <signal.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {

	int pid = atoi(argv[1]);
	kill(pid, SIGKILL);

}
