
#ifndef JOBSCHEDULER_HEADER
#define JOBSCHEDULER_HEADER
//#pragma once

#include "../Common/Singleton.h"
#include <vector>
#include <mutex>
#include "Job.h"
#include "Worker.h"
#include <functional>

class JobScheduler : public WorkerSchedulerInterface// : public Singleton<JobScheduler>
{
public:
	struct FindJobResult
	{
		bool bFind;
		Job job;
	};


	JobScheduler() {}
	~JobScheduler(void);

	int size() {return jobs.size();}
	int empty() {return size() == 0 && freeWorkers.size() == workers.size(); }
	FindJobResult popJob();
	void addJob(const Job & job);
	int getId() {
		if (jobIds.size() != 0)
		{
			int id = jobIds[jobIds.size()-1];
			jobIds.pop_back();
			return id;
		}

		return ++m_idMax;
	}

	Job schedule(std::function<void()> func);
	Job schedule(std::function<void()> func, const Job & dependency);
private:
	Job internal_schedule(std::function<void()> func, const Job * dependency);

private:
	int m_idMax = 0;
	std::vector<int> jobIds = {};
	std::mutex m_lock;
	std::mutex m_dispatchLock;
	std::mutex m_workerLock;

	std::vector<Job> jobs = {};
	int decideNextJobIdx();
	bool checkJob(int & jobIdx);
	bool findJob(const JobIdentifier & id);
	void tryDispatchJob();

//Worker
public:
	struct WorkerStatus
	{
		int id;
		int taskNum;
	};

	int workerCount() const {return workers.size();}
	void setWorkerCount(int num);
	static int getMaxWorkerCount();
	virtual void jobDone(const Worker * pWorker);

	std::vector<WorkerStatus> getStatusData() {return workerStatus;}
private:
	std::vector<Worker*> workers;
	std::vector<int> freeWorkers;
	std::vector<WorkerStatus> workerStatus;


};




class WorkerJob : public IWorkerJob
{
public:
	virtual ~WorkerJob() {}
	virtual void execute() {m_job.execute();}

	void setJob(Job & job) {m_job = job;}

private:
	Job m_job;
};



#endif