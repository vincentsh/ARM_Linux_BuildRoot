#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <spawn.h>

#define P_SPAWN		"s"
#define P_FORK_EXEC	"f"

#if (PROCESS == m)

void test_fork_exec(const char* process)
{
	pid_t pid = fork();
	int status;
	switch (pid)
	{
		case -1:
		{
			perror("fork");
			break;
		}
		case 0:
		{
			execlp(process, (char *)0, NULL);
			perror("exec");
			break;
		}
		default:
		{
			printf("Child id: %i\n", pid);
			fflush(NULL);
//			if (waitpid(pid, &status, 0) != -1)
//			{
//				printf("Child exited with status %i\n", status);
//			}
//			else
//			{
//				perror("waitpid");
//			}
			break;
		}
	}
}

void test_spawn(const char * process)
{
	pid_t pid;
	int status;

	status = posix_spawnp(&pid, process, NULL, NULL, NULL, NULL);
	if (status == 0)
	{
		printf("Child id: %d\n", pid);
		fflush(NULL);
//		if (waitpid(pid, &status, 0) != 1)
//		{
//			printf("Child exited with status %i\n", status);
//		}
//		else
//		{
//			perror("waitpid");
//		}
	}
}

int main (int argc, char **argv)
{
	int argp = 1;
	int spawn_fork = 0;		// 0 for spawn, 1 for fork-exec

	if ((argc > 2) || (argc < 1))
	{
		fprintf(stderr, "Usage:%s [%s|%s]\n", argv[0], P_SPAWN, P_FORK_EXEC);
		exit(1);
	}
	else if (argc == 1)
	{
		spawn_fork = 0;
	}
	else
	{
		if (!strcmp(P_SPAWN, argv[argp]))
		{
			spawn_fork = 0;
		}
		else if (!strcmp(P_FORK_EXEC, argv[argp]))
		{
			spawn_fork = 1;
		}
		else
		{
			fprintf(stderr, "Usage:%s [%s|%s]\n", argv[0], P_SPAWN, P_FORK_EXEC);
			exit(1);
		}
	}

	if (spawn_fork)
	{
		printf("Testing fork/exec\n");
		fflush(NULL);
		test_fork_exec("demo_process1");
		test_fork_exec("demo_process2");
	}
	else
	{
		printf("Testing spawn\n");
		fflush(NULL);
		test_spawn("demo_process1");
		test_spawn("demo_process2");
	}

	while(1);
	return 0;
}
#elif (PROCESS == 1)
int main(void)
{
	int counter;
	int delay_counter;
	sleep(1);

	counter = 0;
	delay_counter = 0;
//	while (counter++ < 100)
	while(1)
	{
		printf("running process 1!\n");
		while (delay_counter++ < 10000);
		delay_counter = 0;
	}
	return 0;
}
#elif (PROCESS == 2)
int main(void)
{
	int counter;
	int delay_counter;
	sleep(1);

	counter = 0;
	delay_counter = 0;
//	while (counter++ < 100)
	while(1)
	{
		printf("running process 2!\n");
		while (delay_counter++ < 10000);
		delay_counter = 0;
	}
	return 0;
}
#endif
