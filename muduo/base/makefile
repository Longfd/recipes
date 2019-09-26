all: dead_lock rcu_demo rcu_demo2

dead_lock: dead_lock.cpp
	g++ -std=c++11 -g dead_lock.cpp -lpthread -o dead_lock

rcu_demo: read_copy_update.cpp
	g++ -std=c++11 -g read_copy_update.cpp -lpthread -o rcu_demo

rcu_demo2: read_copy_update2.cpp
	g++ -std=c++11 -g read_copy_update2.cpp -lpthread -o rcu_demo2

clean:
	rm -rf dead_lock rcu_demo rcu_demo2

