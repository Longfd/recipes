#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class noncopyable 
{
protected:
	// c++11: default, delete
	noncopyable() = default; 
	~noncopyable() = default;

	noncopyable(const& noncopyable) = delete;
	noncopyable& operator=(const& noncopyable) = delete;
};

#endif
