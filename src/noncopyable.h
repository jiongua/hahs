#pragma once
class noncopyable
{
	protected:
		noncopyable() = default;	//maybe protected
		~noncopyable() = default;	//maybe protected
	private:
		noncopyable(const noncopyable &rhs) = delete;
		noncopyable& operator=(const noncopyable &rhs) = delete;
};