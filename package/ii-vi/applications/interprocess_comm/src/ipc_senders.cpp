/*
 * ipc_senders.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: root
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
}
#endif

#include "ipc_senders.h"

/***** Pipe sender *****/
pipe_sender::pipe_sender(int pipe_in_fd) : m_pipe_fd(pipe_in_fd)
{	}

int pipe_sender::put(const char *buf, int size)
{
	/* caution: be aware of the max size of the pipe buffer! */
	write(m_pipe_fd, buf, size);
	return 0;
}

bool pipe_sender::is_full()
{ /* TODO: check if pipe is full */ }

pipe_sender::~pipe_sender()
{
	close(m_pipe_fd);
}

/***** Named Pipe sender *****/
named_pipe_sender::named_pipe_sender(const char * const pipe_path)
{
	memcpy(m_pipe_path, pipe_path, strlen(pipe_path));

	int err = mkfifo(m_pipe_path, 0666);
	if (err != 0)
	{
		// probably file already exists, try removing and recreating it
		unlink(m_pipe_path);
		err = mkfifo(m_pipe_path, 0666);
		if (err != 0)
		{
			fprintf(stderr, "!error: cannot create named pipe file\n");
			exit(1);
		}
	}
}

int named_pipe_sender::put(const char *buf, int size)
{
	int fd = open(m_pipe_path, O_WRONLY);
	if (fd > 0)
	{
		write(fd, buf, size);
		close(fd);
	}
	return 0;
}

bool named_pipe_sender::is_full()
{ /* TODO: check if pipe is full */ }


named_pipe_sender::~named_pipe_sender()
{
	unlink(m_pipe_path);
}

/***** Message Queue sender *****/
msg_queue_sender::msg_queue_sender(int msg_key)
{
	/* TODO: no error handling code */
	m_msg_fd = msgget(msg_key, IPC_CREAT);
}

int msg_queue_sender::put(const char *buf, int size)
{
	struct msgbuf *sndbuf = (struct msgbuf *)malloc(size);

	sndbuf->mtype = 1;
	strcpy(sndbuf->mtext, buf);

	msgsnd(m_msg_fd, sndbuf, size, 0);

	free(sndbuf);
	return 0;
}

bool msg_queue_sender::is_full()
{ /* TODO: check if pipe is full */ }

msg_queue_sender::~msg_queue_sender()
{
	msgctl(m_msg_fd, IPC_RMID, NULL);
}

/***** Shared Memory sender *****/
shared_mem_sender::shared_mem_sender(int shm_id, int memsize)
{
	/* TODO: no error handling code */
	m_shm_id = shmget(shm_id, memsize, IPC_CREAT);
}

int shared_mem_sender::put(const char *buf, int size)
{
	char *mem = (char *)shmat(m_shm_id, 0, 0);

	strcpy(mem, buf);

	shmdt(mem);
	return 0;
}

bool shared_mem_sender::is_full()
{ /* TODO: check if pipe is full */ }

shared_mem_sender::~shared_mem_sender()
{
	shmctl(m_shm_id, IPC_RMID, NULL);
}
