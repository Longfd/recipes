#include "CurrentThread.h"


namespace CurrentThread
{
	__thread int t_cachedTid;

	void cacheTid()
	{
		t_cachedTid = syscall(SYS_gettid);
	}

	int gettid() 
	{
		// very likely exp != 0 -> t_cachedTid != 0  
		if ( __builtin_expect(t_cachedTid == 0, 0) )
			cacheTid();

		return t_cachedTid;
	}
}


