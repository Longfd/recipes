#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class noncopyable 
{
protected:
	noncopyable() = default; // c++11 key words
	~noncopyable() = default;

	noncopyable(const noncopyable&) = delete; // c++11 key words
	noncopyable& operator=(const noncopyable&) = delete;
};

#endif //NONCOPYABLE_H

