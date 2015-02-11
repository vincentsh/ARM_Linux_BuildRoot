/*
 * ipc_demo.cpp
 *
 *  Created on: Mar 25, 2014
 *      Author: root
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#ifdef __cplusplus
}
#endif

#include "ipc_receivers.h"
#include "ipc_senders.h"

enum IPC_METHOD { ipc_pipe, ipc_named_pipe, ipc_msg_queue, ipc_shared_mem };

#define MAX_LEN				(128)

#define IPC_PIPE			"p"
#define IPC_NAMED_PIPE		"n"
#define IPC_MSG_QUEUE		"m"
#define IPC_SHARED_MEM		"s"

#define IPC_MSG_KEY			(1234)
#define IPC_SHM_ID			(4321)
#define SHM_SIZE			(1024)

static pthread_t slave_thread_t;
void * slave_thread(void *param);

int pipeFd[2];
IPC_METHOD test;
ipc_sender *snd = NULL;
ipc_receiver *rcv = NULL;

const char *named_pipe_path = "/tmp/ipc_demo_pipe.tmp";

int main(int argc, char **argv)
{
	char write_buf[MAX_LEN];
	const char *sender_name = NULL;

	if (argc != 2)
	{
		fprintf(stderr, "Usage:%s (%s)ipe|(%s)amed pipe|(%s)sg queue|(%s)hared mem\n", argv[0], IPC_PIPE, IPC_NAMED_PIPE, IPC_MSG_QUEUE, IPC_SHARED_MEM);
		exit(1);
	}

	if (!strcmp(argv[1], IPC_PIPE))
	{
		printf("===== Pipe Test =====\n");
		test = ipc_pipe;

		if (pipe(pipeFd) == 0)
		{
			snd = new pipe_sender(pipeFd[1]);
		}
		else
		{
			printf("!error: pipe creation failed\n");
			exit(1);
		}
	}
	else if (!strcmp(argv[1], IPC_NAMED_PIPE))
	{
		printf("===== Named Pipe Test =====\n");
		test = ipc_named_pipe;
		snd = new named_pipe_sender(named_pipe_path);
	}
	else if (!strcmp(argv[1], IPC_MSG_QUEUE))
	{
		printf("===== Message Queue Test =====\n");
		test = ipc_msg_queue;
		snd = new msg_queue_sender(IPC_MSG_KEY);
	}
	else if (!strcmp(argv[1], IPC_SHARED_MEM))
	{
		printf("===== Shared Memory Test =====\n");
		test = ipc_shared_mem;
		snd = new shared_mem_sender(IPC_SHM_ID, SHM_SIZE);
	}
	else
	{
		fprintf(stderr, "Usage:%s %s|%s|%s|%s [msg]\n", argv[0], IPC_PIPE, IPC_NAMED_PIPE, IPC_MSG_QUEUE, IPC_SHARED_MEM);
		exit(1);
	}

	printf("enter the message to send: ");
	scanf("%s", write_buf);

	switch(test)
	{
		case ipc_pipe:
			sender_name = "pipe";
			break;
		case ipc_named_pipe:
			sender_name = "named pipe";
			break;
		case ipc_msg_queue:
			sender_name = "msg queue";
			break;
		case ipc_shared_mem:
			sender_name = "shared mem";
			break;
	}
	printf("%s sender put: %s\n", sender_name, write_buf);
	pthread_create(&slave_thread_t, NULL, slave_thread, (void *)&test);
	snd->put(write_buf, strlen(write_buf));

	pthread_join(slave_thread_t, NULL);

	delete snd;
}

/* Demo Implementation */
void * slave_thread(void *param)
{
	IPC_METHOD test;
	memcpy(&test, param, sizeof(test));

	char read_buf[MAX_LEN];
	const char *receiver_name = NULL;

	switch(test)
	{
		case ipc_pipe:
		{
			rcv = new pipe_receiver(pipeFd[0]);
			receiver_name = "pipe";
			break;
		}
		case ipc_named_pipe:
		{
			rcv = new named_pipe_receiver(named_pipe_path);
			receiver_name = "named pipe";
			break;
		}
		case ipc_msg_queue:
		{
			rcv = new msg_queue_receiver(IPC_MSG_KEY);
			receiver_name = "msg queue";
			break;
		}
		case ipc_shared_mem:
		{
			rcv = new shared_mem_receiver(IPC_SHM_ID, SHM_SIZE);
			receiver_name = "shared mem";
			break;
		}
	}

	rcv->get(read_buf, MAX_LEN);
	printf("%s receiver get: %s\n", receiver_name, read_buf);
	delete rcv;

	return NULL;
}
