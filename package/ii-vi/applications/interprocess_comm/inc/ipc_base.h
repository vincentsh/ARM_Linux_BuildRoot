/*
 * ipc_base.h
 *
 *  Created on: Mar 24, 2014
 *      Author: root
 *
 *	This is the base class for some Inter-Process Communication (IPC) method
 *	implementation classes
 */

#ifndef IPC_BASE_H_
#define IPC_BASE_H_

class ipc_receiver
{
public:
	ipc_receiver() {	}
	virtual int get(char *buf, int size) = 0;
	virtual bool is_empty() = 0;
	virtual ~ipc_receiver() {	}
};

class ipc_sender
{
public:
	ipc_sender() {	}
	virtual int put(const char *buf, int size) = 0;
	virtual bool is_full() = 0;
	virtual ~ipc_sender() {	}
};

#endif /* IPC_RECEIVER_H_ */
