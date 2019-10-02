#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
	int gettid();
}

#endif // CURRENT_THREAD_H

