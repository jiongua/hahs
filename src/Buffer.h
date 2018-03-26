/******************************************************************************
 * Filename : Buffer.h
 * Copyright: 
 * Created  : 2018-1-22 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <assert.h>
#include <algorithm>

//客户代码不直接操控Buffer，仅供TcpConnection使用
//客户代码通过TcpConnection间接存取Buffer
class Buffer
{
public:
	typedef std::vector<char>::size_type pos;
	static const pos BUFFER_RESERVED_LEN = 8;
	static const pos BUFFER_INIT_LEN = 1024;

	explicit Buffer(pos buf_len = BUFFER_INIT_LEN)
	 :getIndex_(BUFFER_RESERVED_LEN),
	 putIndex_(BUFFER_RESERVED_LEN),
	 buffer_(buf_len + BUFFER_RESERVED_LEN)
	{
	}
	
	~Buffer()
	{
		
	}
	
	int readableBytes() const {return putIndex_ - getIndex_;} 
	int writeableBytes() const {return buffer_.size() - putIndex_;} 
	int prependableBytes() const {return getIndex_;}
	int internalCapacity() const {return buffer_.capacity();}
	const char* peek() const {return &buffer_[getIndex_];}
	const char* beginWrite() const {return &buffer_[putIndex_];}

	
	//查找"/r/n",返回
	const char* findCRLF()
	{
		//in range: [ peek(), beginWrite() )
		const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
		//如果成功，则返回*kCRLF在range中的位置
		return crlf == beginWrite() ? NULL : crlf;
	}
	bool setPutIndexOffset(int offset)
	{
		return setPutIndex(offset + putIndex_);
	}
	bool setGetIndexOffset(int offset)
	{
		return setGetIndex(offset + getIndex_);
	}
	
	int getIndex() const {return getIndex_;}
	int putIndex() const {return putIndex_;}

	//fixme：防止调用者误用
	bool setGetIndex(pos newIndex) 
	{	
		if(newIndex <= putIndex_)
		{
			getIndex_ = newIndex;
			if(getIndex_ == putIndex_)
			{
				getIndex_ = putIndex_ = BUFFER_RESERVED_LEN;
			}
			return true;
		}
		return false;
	} 

	bool setPutIndex(pos newIndex) 
	{	
		if(newIndex <= putIndex_)
		{
			getIndex_ = newIndex;
			if(getIndex_ == putIndex_)
			{
				getIndex_ = putIndex_ = BUFFER_RESERVED_LEN;
			}
			return true;
		}
		return false;
	} 

	/*void swap(Buffer &rhs)
	{
		buffer_.swap(rhs.buffer_);
		std::swap(getIndex_, rhs.getIndex_);
		std::swap(putIndex_, rhs.putIndex_);
	}*/
	
	//以string方式写入Buffer
	void putString(const std::string &data)
	{
		put(data.data(), data.size());
	}
	
	void putInt8(const int8_t data)
	{
		put(&data, sizeof(int8_t));
	}
	
	void putInt16(const int16_t data)
	{
		put(&data, sizeof(int16_t));
	}
	
	void putInt32(const int32_t data)
	{
		put(&data, sizeof(int32_t));
	}
	
	void putInt64(const int64_t data)
	{
		put(&data, sizeof(int64_t));
	}
	
	void put(const void *data, int len)
	{
		checkSpace(len);
		assert(len <= writeableBytes());
		memcpy(&buffer_[putIndex_], static_cast<const char *>(data), len);
		putIndex_ += len;
	}

	//读取所有可读数据
	std::string getAllAsString()
	{
		return getAsString(readableBytes());
	}
	
	std::string getAsString(int len)
	{
		assert(len <= readableBytes());
		std::string result(peek(), len);
		getIndex_ += len;
		if(getIndex_ == putIndex_)
		{
			getIndex_ = putIndex_ = BUFFER_RESERVED_LEN;
		}
		return result;
	}

	int8_t getAsInt8()
	{
		int8_t data;
		get(&data, sizeof(int8_t));
		return data;
	}

	int16_t getAsInt16()
	{
		int16_t data;
		get(&data, sizeof(int16_t));
		return data;
	}

	int32_t getAsInt32()
	{
		int32_t data;
		get(&data, sizeof(int32_t));
		return data;
	}

	int64_t getAsInt64()
	{
		int64_t data;
		get(&data, sizeof(int64_t));
		return data;
	}

	//从Buffer读出数据
	void get(void *data, int len)
	{
		assert(len <= readableBytes());
		//todo: use std::copy
		memcpy(static_cast<char *>(data), peek(), len);
		getIndex_ += len;
		if(getIndex_ == putIndex_)
		{
			getIndex_ = putIndex_ = BUFFER_RESERVED_LEN;
		}
	}

	int readFD(int fd);
	//int writeFD(int fd);

private:
	void checkSpace(int len)
	{
		if(len > writeableBytes())
		{
			if(getIndex_ - BUFFER_RESERVED_LEN + writeableBytes() >= (pos)len)
			{
				//可读部分挪到预留与可读位置之间
				//TODO: test
				//memmove可处理重叠
				memmove(&buffer_[BUFFER_RESERVED_LEN], peek(), readableBytes());
				int moveStep = getIndex_ - BUFFER_RESERVED_LEN;
				
				getIndex_ -= moveStep;
				putIndex_ -= moveStep;
				//assert(len <= writeableBytes());
			}
			else
			{
				//resize可能会重新分配内存
				buffer_.resize(len + putIndex_);
				
			}
		}
		//ok
	}

	pos getIndex_;	//从Buffer读取的位置
	pos putIndex_;	//向Buffer写入的位置
	std::vector<char> buffer_;
	static const char kCRLF[];
};
