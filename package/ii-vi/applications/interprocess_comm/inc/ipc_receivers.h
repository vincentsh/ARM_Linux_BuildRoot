/*
 * ipc_receivers.h
 *
 *  Created on: Mar 24, 2014
 *      Author: root
 */

#ifndef IPC_RECEIVERS_H_
#define IPC_RECEIVERS_H_

#include "ipc_base.h"

class pipe_receiver : public ipc_receiver
{
public:
	pipe_receiver(int pipe_out_fd);
	int get(char *buf, int size);
	bool is_empty();
	virtual ~pipe_receiver();
private:
	int m_pipe_fd;
};

class named_pipe_receiver : public ipc_receiver
{
public:
	named_pipe_receiver(const char * const pipe_path);
	int get(char *buf, int size);
	bool is_empty();
	virtual ~named_pipe_receiver();
private:
	char m_pipe_path[128];
};

class msg_queue_receiver : public ipc_receiver
{
public:
	msg_queue_receiver(int msg_key);
	int get(char *buf, int size);
	bool is_empty();
	virtual ~msg_queue_receiver();
private:
	int m_msg_fd;
};

class shared_mem_receiver : public ipc_receiver
{
public:
	shared_mem_receiver(int shm_id, int memsize);
	int get(char *buf, int size);
	bool is_empty();
	virtual ~shared_mem_receiver();
private:
	int m_shm_id;
};

#endif /* IPC_RECEIVERS_H_ */
