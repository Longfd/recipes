#include "Thread.h"

pid_t gettid()
{
	return syscall(SYS_gettid);
}

