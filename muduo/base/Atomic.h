// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#ifndef ATOMIC_H
#define ATOMIC_H

#include "Noncopyable.h"

#include <stdint.h>

template<typename T>
class AtomicIntegerT : Noncopyable
{
	public:
		T get()
		{
			// in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
			return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
		}

		T getAndAdd(T x)
		{
			// in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
			return __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST);
		}

		T addAndGet(T x)
		{
			return getAndAdd(x) + x;
		}

		T incrementAndGet()
		{
			return addAndGet(1);
		}

		T decrementAndGet()
		{
			return addAndGet(-1);
		}

		void add(T x)
		{
			getAndAdd(x);
		}

		void increment()
		{
			incrementAndGet();
		}

		void decrement()
		{
			decrementAndGet();
		}

		T getAndSet(T newValue)
		{
			// in gcc >= 4.7: __atomic_exchange_n(&value, newValue, __ATOMIC_SEQ_CST)
			return __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST);
		}

	private:
		volatile T value_;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;


#endif // ATOMIC_H





















