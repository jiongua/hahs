/******************************************************************************
 * Filename : Buffer.cpp
 * Copyright: 
 * Created  : 2018-1-22 by gongzenghao
 * Description: 
 ******************************************************************************/
#include "Buffer.h"
#include "SocketOps.h"

const char Buffer::kCRLF[] = "\r\n";//for findCRLF

//read data from socket write to buffer_
int Buffer::readFD(int fd)
{
	struct iovec iov;
	iov.iov_base = &buffer_[putIndex_];
	iov.iov_len = writeableBytes();
	int n = sockets::readv(fd, &iov, 1);
	if(n > 0){
		putIndex_ += n;
	}
	return n;
}

/*int Buffer::writeFD(int fd)
{
	int n = sockets::write(fd, &buffer_[getIndex_], readableBytes());
	if(n > 0){
		getIndex_ += n;
		if(getIndex_ == putIndex_)
		{
			getIndex_ = putIndex_ = BUFFER_RESERVED_LEN;
		}
	}
	return n;
}*/

