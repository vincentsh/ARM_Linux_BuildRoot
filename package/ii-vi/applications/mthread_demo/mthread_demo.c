#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <linux/string.h>

#define msleep(n)		usleep(n*1000)
#define NUM_TRHEADS		(2)
#define MTX_CMD			"m"
#define SYNC_CMD		"s"

static pthread_t g_thread[NUM_TRHEADS];
static pthread_mutex_t g_mtx;		// mutex
static pthread_cond_t g_cond;		// condition for thread synchronization

void * threadx_yell (void *param)
{
	int t_id = *(int*)param;
	while(1)
	{
		pthread_mutex_lock(&g_mtx);
		printf("mutex demo: thread %d entered mutex!\n", t_id);
		msleep(500);
		pthread_mutex_unlock(&g_mtx);
		printf("mutex demo: thread %d left mutex!\n", t_id);
		msleep(500);
	}
}

void * thread1_yell (void *param)
{
	int t_id = *(int*)param;
	while(1)
	{
		printf("sync demo: thread %d wait for signal!\n", t_id);
		pthread_cond_wait(&g_cond, &g_mtx);
		printf("sync demo: thread %d released!\n", t_id);
		sleep(1);
		printf("sync demo: thread %d send the signal!\n", t_id);
		pthread_cond_signal(&g_cond);
	}
}

void * thread2_yell (void *param)
{
	int t_id = *(int*)param;
	while(1)
	{
		sleep(1);
		printf("sync demo: thread %d send the signal!\n", t_id);
		pthread_cond_signal(&g_cond);
		printf("sync demo: thread %d wait for signal!\n", t_id);
		pthread_cond_wait(&g_cond, &g_mtx);
		printf("sync demo: thread %d released!\n", t_id);
	}
}

int main(int argc, char **argv)
{
	int argp = 1;
	int t_id1 = 1;
	int t_id2 = 2;
	int mtx_sync = 0; // 0 for mutex demo; 1 for sync demo

	if ((argc > 2) || (argc < 1))
	{
		fprintf(stderr, "Usage:%s [m|s]\n", argv[0]);
		exit(1);
	}
	else if (argc == 1)
	{
		mtx_sync = 0;
	}
	else
	{
		if (!strcmp(MTX_CMD, argv[argp]))
		{
			mtx_sync = 0;
		}
		else if (!strcmp(SYNC_CMD, argv[argp]))
		{
			mtx_sync = 1;
		}
		else
		{
			fprintf(stderr, "Usage:%s [m|s]\n", argv[0]);
			exit(1);
		}
	}

	pthread_cond_init(&g_cond, NULL);
	pthread_mutex_init(&g_mtx, NULL);

	printf("===== linux multi-thread programming demo =====\n");

	if(mtx_sync)	// sync demo
	{
		pthread_create(&g_thread[0], NULL, thread1_yell, (void *)&t_id1);
		pthread_create(&g_thread[1], NULL, thread2_yell, (void *)&t_id2);

		pthread_join(g_thread[0], NULL);
		pthread_join(g_thread[1], NULL);
	}
	else			// mutex demo
	{
		pthread_create(&g_thread[0], NULL, threadx_yell, (void *)&t_id1);
		pthread_create(&g_thread[1], NULL, threadx_yell, (void *)&t_id2);

		pthread_join(g_thread[0], NULL);
		pthread_join(g_thread[1], NULL);
	}

	pthread_cond_destroy(&g_cond);
	pthread_mutex_destroy(&g_mtx);

	return 0;
}

