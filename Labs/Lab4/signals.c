#include <stdio.h>	/* standard I/O functions                         */
#include <stdlib.h>	/* exit                                           */
#include <unistd.h>	/* standard unix functions, like getpid()         */
#include <signal.h>	/* signal name macros, and the signal() prototype */

/* first, define the Ctrl-C counter, initialize it with zero. */
int ctrl_c_count = 0;
int ctrl_z_count = 0;
int got_response = 0;
#define CTRL_C_THRESHOLD  5
#define CTRL_Z_THRESHOLD  5 

/* the Ctrl-C signal handler */
void catch_int(int sig_num)
{
	/* increase count, and check if threshold was reached */
	ctrl_c_count++;
	if (ctrl_c_count >= CTRL_C_THRESHOLD) {
		char answer[30];
		alarm(10);
		/* prompt the user to tell us if to really
		 * exit or not */
		printf("\nReally exit? [Y/n]: ");
		fflush(stdout);
		fgets(answer, sizeof(answer), stdin);
		fflush(stdout);
		if (answer[0] == 'n' || answer[0] == 'N') {
			printf("\nContinuing\n");
			fflush(stdout);
			/* 
			 * Reset Ctrl-C counter
			 */
			ctrl_c_count = 0;
			ctrl_z_count = 0;
		}
		else {
			printf("\nExiting...\n");
			fflush(stdout);
			exit(0);
		}
	}
}

/* the Ctrl-Z signal handler */
void catch_tstp(int sig_num)
{
	/* print the current Ctrl-C counter */
	printf("\n\nSo far, '%d' Ctrl-C presses were counted\n\n", ctrl_c_count);
	fflush(stdout);
	ctrl_z_count++;
	if(ctrl_z_count >= CTRL_Z_THRESHOLD) {
		printf("\nExiting...\n");
		fflush(stdout);
		exit(EXIT_SUCCESS);
	}
}

void catch_alarm(int sig_num)
{
	printf("User taking too long to respond.  Exiting...\n");
	exit(0);
}

int main(int argc, char* argv[])
{
	struct sigaction sa;
	sigset_t mask_set;  /* used to set a signal masking set. */

	/* setup mask_set */
	sigfillset(&mask_set);
	sigdelset(&mask_set, SIGALRM);

	/* set signal handlers */
	sa.sa_handler = &catch_int;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = &catch_tstp;
	sigaction(SIGTSTP, &sa, NULL);
	sa.sa_handler = &catch_alarm;
	sigaction(SIGALRM, &sa, NULL);
	
	while(1) {
		pause();
	}

	return 0;
}

