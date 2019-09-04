// from Scott Meyer

#ifndef SINGLETON2_H
#define SINGLETON2_H

#include "noncopyable.h"

template<typename T>
class Singleton : noncopyable
{
private:
	Singleton() = default; //c++11
	~Singleton() = default; //c++11

public: 
	static T& getInstance() {
		static T value;// thread safety: after c++11 or g++4.0
		return value;
	}
};

#endif

