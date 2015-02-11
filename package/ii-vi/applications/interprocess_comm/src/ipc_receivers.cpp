/*
 * ipc_receivers.cpp
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
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
}
#endif

#include "ipc_receivers.h"

/***** Pipe receiver *****/
pipe_receiver::pipe_receiver(int pipe_out_fd) : m_pipe_fd(pipe_out_fd)
{	}

int pipe_receiver::get(char *buf, int size)
{
	/* caution: be aware of the max size of the pipe buffer! */
	read(m_pipe_fd, buf, size);
}

bool pipe_receiver::is_empty()
{ /* TODO: check if pipe is empty */ }

pipe_receiver::~pipe_receiver()
{
	close(m_pipe_fd);
}

/***** Named Pipe receiver *****/
named_pipe_receiver::named_pipe_receiver(const char * const pipe_path)
{
	memcpy(m_pipe_path, pipe_path, strlen(pipe_path));
}

int named_pipe_receiver::get(char *buf, int size)
{
	int fd = open(m_pipe_path, O_RDONLY);
	if (fd > 0)
	{
		read(fd, buf, size);
		close(fd);
	}
	return 0;
}

bool named_pipe_receiver::is_empty()
{ /* TODO: check if pipe is empty */ }

named_pipe_receiver::~named_pipe_receiver()
{	}

/***** Message Queue receiver *****/
msg_queue_receiver::msg_queue_receiver(int msg_key)
{
	m_msg_fd = msgget(msg_key, IPC_CREAT);
}

int msg_queue_receiver::get(char *buf, int size)
{
	struct msgbuf *rcvbuf = (struct msgbuf *)malloc(size);

	msgrcv(m_msg_fd, rcvbuf, size, 0, 0);
	strcpy(buf, rcvbuf->mtext);

	free(rcvbuf);
	return 0;
}

bool msg_queue_receiver::is_empty()
{ /* TODO: check if pipe is empty */ }

msg_queue_receiver::~msg_queue_receiver()
{	}

/***** Shared Memory receiver *****/
shared_mem_receiver::shared_mem_receiver(int shm_id, int memsize)
{
	/* TODO: no error handling code */
	m_shm_id = shmget(shm_id, memsize, IPC_CREAT);
}

int shared_mem_receiver::get(char *buf, int size)
{
	char *mem = (char *)shmat(m_shm_id, 0, 0);

	strcpy(buf, mem);

	shmdt(mem);
	return 0;
}

bool shared_mem_receiver::is_empty()
{ /* TODO: check if pipe is empty */ }

shared_mem_receiver::~shared_mem_receiver()
{	}


