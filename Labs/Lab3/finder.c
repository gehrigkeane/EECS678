#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define BSIZE 256
#define WRITE 1
#define READ 0

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"

int main(int argc, char *argv[])
{
	int status;
	pid_t pid_1, pid_2, pid_3, pid_4;

	// Initialize Pipes
	int p1[2];
	pipe(p1);

	int p2[2];
	pipe(p2);

	int p3[2];
	pipe(p3);

	// Error handling
	if ( pipe(p1) == -1 )
	{
		perror("PIPE1 ERROR");
		exit(-1);
	}
	if ( pipe(p2) == -1 )
	{
		perror("PIPE2 ERROR");
		exit(-1);
	}
	if ( pipe(p3) == -1 )
	{
		perror("PIPE3 ERROR");
		exit(-1);
	}

	// Argument validation
	if (argc != 4) {
		printf("usage: finder DIR STR NUM_FILES\n");
		exit(0);
	}

	pid_1 = fork();
	if (pid_1 == 0)
	{
		// First Child
		dup2(p1[WRITE], STDOUT_FILENO);
		char cmdbuf[BSIZE];
		bzero(cmdbuf, BSIZE);
		sprintf(cmdbuf, "%s %s -name \'*\'.[ch]", FIND_EXEC, argv[1]);

		// Close unneeded pipes
		close(p1[WRITE]);
		close(p1[READ]);
		close(p2[WRITE]);
		close(p2[READ]);
		close(p3[WRITE]);
		close(p3[READ]);

		// Execute FIND
		if ( ( execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0) ) < 0 )
		{
				fprintf(stderr, "\n--Error executing FIND. Error #%d\n", errno);
				return EXIT_FAILURE;
		}
		exit(0);
	}

	pid_2 = fork();
	if (pid_2 == 0) {
		// Second Child
		char cmdbuf[BSIZE];
		bzero(cmdbuf, BSIZE);
		dup2(p1[READ], STDIN_FILENO);
		dup2(p2[WRITE], STDOUT_FILENO);
		sprintf(cmdbuf, "%s %s -c %s", XARGS_EXEC, GREP_EXEC, argv[2]);

		// Close unneeded pipes
		close(p1[WRITE]);
		close(p1[READ]);
		close(p2[WRITE]);
		close(p2[READ]);
		close(p3[WRITE]);
		close(p3[READ]);

		if( ( execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0) ) < 0 )
		{
				fprintf(stderr, "\n--Error executing FIND - Error inside XARGS GREP.  Error #%d", errno);
				return EXIT_FAILURE;
		}
		exit(0);
	}

	pid_3 = fork();
	if (pid_3 == 0) {
		// Third Child
		char cmdbuf[BSIZE];
		bzero(cmdbuf, BSIZE);
		dup2(p2[READ], STDIN_FILENO);
		dup2(p3[WRITE], STDOUT_FILENO);
		sprintf(cmdbuf, "%s -t : +1.0 -2.0 --numeric --reverse", SORT_EXEC);

		// Close unneeded pipes
		close(p1[WRITE]);
		close(p1[READ]);
		close(p2[WRITE]);
		close(p2[READ]);
		close(p3[WRITE]);
		close(p3[READ]);

		if( ( execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0) ) < 0 )
		{
				fprintf(stderr, "\n--Error executing SORT.  Error #%d", errno);
				return EXIT_FAILURE;
		}
		exit(0);
	}

	pid_4 = fork();
	if (pid_4 == 0) {
		// Fourth Child
		char cmdbuf[BSIZE];
		bzero(cmdbuf, BSIZE);
		dup2(p3[READ], STDIN_FILENO);
		sprintf(cmdbuf, "%s --lines=%s", HEAD_EXEC, argv[3]);
		
		// Close unneeded pipes
		close(p1[WRITE]);
		close(p1[READ]);
		close(p2[WRITE]);
		close(p2[READ]);
		close(p3[WRITE]);
		close(p3[READ]);

		if ( ( execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0) ) < 0 )
		{
				fprintf(stderr, "Error executing HEAD. Error #%d", errno);
				return EXIT_FAILURE;
		}
		exit(0);
	}

	// Close all pipes
	close(p1[WRITE]);
	close(p1[READ]);
	close(p2[WRITE]);
	close(p2[READ]);
	close(p3[WRITE]);
	close(p3[READ]);

	if ( (waitpid(pid_1, &status, 0) ) == -1 )
	{
		fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}
	if ( (waitpid(pid_2, &status, 0) ) == -1 )
	{
		fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}
	if ( (waitpid(pid_3, &status, 0) ) == -1 )
	{
		fprintf(stderr, "Process 3 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}
	if ( (waitpid(pid_4, &status, 0) ) == -1 )
	{
		fprintf(stderr, "Process 4 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}

	return 0;
}
