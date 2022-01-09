
#ifndef WORKER_HEADER
#define WORKER_HEADER

//#pragma once

//#include "Job.h"
#include <thread>
#include <memory>


class Job;
class Worker;

class IWorkerJob
{
public:
	virtual ~IWorkerJob() {}
	virtual void execute() {}
};

class WorkerSchedulerInterface
{
public:
	virtual ~WorkerSchedulerInterface() {}
	virtual void jobDone(const Worker * pWorker) {}
};


class Worker
{
public:
	Worker();

	void assginJob(IWorkerJob * workingJob) {m_workingJob = workingJob;}
	void setWorkerId(int id) {m_workerId = id;}
	int getWorkerId() const {return m_workerId;}
	void setScheduler(WorkerSchedulerInterface * scheduler);
	void join() {m_thread.join();}
	void terminate() {bTerminate = true; join();};
private:
	int m_workerId;
	void running();
	bool bTerminate = false;

	IWorkerJob * m_workingJob = nullptr;
	WorkerSchedulerInterface * m_scheduler = nullptr;
	std::thread m_thread;
};

#endif
