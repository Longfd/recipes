#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class Noncopyable 
{
protected:
	Noncopyable() = default; // c++11 key words
	~Noncopyable() = default;

	Noncopyable(const Noncopyable&) = delete; // c++11 key words
	Noncopyable& operator=(const Noncopyable&) = delete;
};

#endif //NONCOPYABLE_H

