/*
 * ipc_senders.h
 *
 *  Created on: Mar 24, 2014
 *      Author: root
 */

#ifndef IPC_SENDERS_H_
#define IPC_SENDERS_H_

#include "ipc_base.h"

class pipe_sender : public ipc_sender
{
public:
	pipe_sender(int pipe_in_fd);
	int put(const char *buf, int size);
	bool is_full();
	~pipe_sender();
private:
	const int m_pipe_fd;
};

class named_pipe_sender : public ipc_sender
{
public:
	named_pipe_sender(const char * const pipe_path);
	int put(const char *buf, int size);
	bool is_full();
	~named_pipe_sender();
private:
	char m_pipe_path[128];
};

class msg_queue_sender : public ipc_sender
{
public:
	msg_queue_sender(int msg_key);
	int put(const char *buf, int size);
	bool is_full();
	~msg_queue_sender();
private:
	int m_msg_fd;
};

class shared_mem_sender : public ipc_sender
{
public:
	shared_mem_sender(int shm_id, int memsize);
	int put(const char *buf, int size);
	bool is_full();
	~shared_mem_sender();
private:
	int m_shm_id;
};

#endif /* IPC_SENDERS_H_ */
