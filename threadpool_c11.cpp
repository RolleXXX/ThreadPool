#include<vector>
#include <queue>
#include <iostream>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <condition_variable>
#include<unistd.h>
#include<functional>

using namespace std;

template <class T>
class threadpool {
public:
	threadpool(int thread_number);
	~threadpool();
	void addTask(T* request);
	void run();//不能设置为static
	int size();
private:
	vector<thread>m_threads;
	queue<T*>m_task_queue;
	int m_thread_number;
	mutex queueMutex;
	condition_variable queueCond;
	bool m_stop;
};

template<class T>
threadpool<T>::threadpool(int thread_number):m_thread_number(thread_number),m_stop(false)
{
	
	for (int i = 0; i < thread_number; i++) {
		m_threads.emplace_back(bind(&threadpool::run,this));//empalce_back直接构造一个thread对象，用bind将函数调用对象绑定
	}
}

template<class T>
threadpool<T>::~threadpool()
{
	{
		unique_lock<mutex> lock(queueMutex);//用一个类来管理 RAII
		m_stop = true; 
	}

	queueCond.notify_all();//唤醒所有工作线程。因为它们都在等待任务队列中有任务，但现在没有任务需要执行，我们通过发送信号来告诉它们现在可以停止了。

	for(auto &thread:m_threads){
		thread.join();//等待所有工作线程完成它们的任务并退出
	}

}

template<class T>
void threadpool<T>::addTask(T * request)
{
	{
		unique_lock<mutex> lock(queueMutex);
		m_task_queue.emplace(request);
	}
	queueCond.notify_one();
}


template<class T>
void threadpool<T>::run()
{
	//todo
	while (!m_stop)
	{
		T* task = nullptr;
		{
			unique_lock<mutex>lock(queueMutex);
			if (!m_task_queue.empty()) {
				task = m_task_queue.front();
				m_task_queue.pop();
			}
			else {
				queueCond.wait(lock);//等待队列有任务 被notify_one())或notify_broadcast()唤醒
			}
		}
		if (task != nullptr) {
			task->process();
			delete task;
		}
	}
}

template<class T>
int threadpool<T>::size()
{
	return m_task_queue.size();
}

class Task {
public:
	Task(int id) :m_id(id) {}

	void process() {
		cout << "Task " << m_id << " is doing" << endl;
	}
private:
	int m_id;
};

int main() {
	threadpool<Task> pool(5);

	// 向线程池中添加 10 个任务
	for (int i = 0; i < 10; i++) {
		Task* task = new Task(i);
		pool.addTask(task);
	}

	while (true) {
		if (pool.size() == 0) {
			cout << "now i finish all tasks" << endl;
			return 0;
		}
		sleep(2);
	}
}