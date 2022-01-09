#include "JobScheduler.h"
#include <mutex>
#include <iostream>
#include <thread>
#include "Worker.h"
#include <string>


JobScheduler::~JobScheduler(void)
{
	for (int i = 0; i < workers.size(); ++i)
	{
		workers[i]->setScheduler(nullptr);
		workers[i]->terminate();
		delete workers[i];
	}

}

void JobScheduler::addJob(const Job & job)
{
	jobs.push_back(job);
	tryDispatchJob();
}


Job JobScheduler::schedule(std::function<void()> func)
{
	return internal_schedule(func, nullptr);
}

Job JobScheduler::schedule(std::function<void()> func, const Job & dependency)
{
	return internal_schedule(func, &dependency);
}

Job JobScheduler::internal_schedule(std::function<void()> func, const Job * dependency)
{
	Job job = {};
	job.setId({getId()});
	job.setTask(func);
	if (dependency != nullptr) job.addDependency(*dependency);

	jobs.push_back(job);
	tryDispatchJob();


	return job;	
}


void JobScheduler::tryDispatchJob()
{
	std::lock_guard<std::mutex> lock(m_dispatchLock);
	if (jobs.size() == 0) return;

	bool bAnyWork = true;
	while (bAnyWork)
	{
		bAnyWork = false;

		std::lock_guard<std::mutex> lock(m_workerLock);
		if (freeWorkers.size() > 0 )
		{
			FindJobResult fjr = popJob();
			if (fjr.bFind == false) return;
			Job job = fjr.job;

			int idx = freeWorkers[freeWorkers.size()-1];
			freeWorkers.pop_back();
			Worker * pWorker = workers[idx];

			WorkerJob * ji = new WorkerJob();
			ji->setJob(job);

			pWorker->assginJob(ji);

			bAnyWork = true;
		}
	}
}


JobScheduler::FindJobResult JobScheduler::popJob()
{
	FindJobResult result = {};

	std::lock_guard<std::mutex> lock(m_lock);
	int id = decideNextJobIdx();

	if (id < 0) {result.bFind = false; return result;}
	


	Job j = jobs[id];
	std::swap(jobs[id], jobs[jobs.size()-1]);
	jobs.pop_back();

	result.bFind = true;
	result.job = j;

	return result;
}


int JobScheduler::decideNextJobIdx()
{
	for(int i = 0; i < jobs.size(); ++i)
	{
		Job & job = jobs[i];
		if (checkJob(i)) return i;
	}

	return -1;
}

bool JobScheduler::checkJob(int & jobIdx)
{
	if (jobIdx >= jobs.size()) return false;
	Job & job = jobs[jobIdx];

	auto dependencies = job.getDependencies();

	for(int i = 0; i < dependencies.size(); ++i) 
	{
		if (findJob(dependencies[i].getId())) {
			//std::cout << "Job pass" << std::endl;
			return false;
		}
	}


	return true;
}


bool JobScheduler::findJob(const JobIdentifier & id)
{
	for (int i = 0; i < jobs.size(); i++)
	{
		if (id.id == jobs[i].getId().id) return true;	
	}

	return false;
}


void JobScheduler::jobDone(const Worker * pWorker)
{
	// std::cout << "Worker Done(JobScheduler)" + std::to_string(pWorker->getWorkerId()) + "\n";
	for (int i = 0; i < workers.size(); ++i)
	{
		if (pWorker == workers[i]) 
		{
			{
				std::lock_guard<std::mutex> lock(m_workerLock);
				freeWorkers.push_back(i);

				workerStatus[i].taskNum++;
			}
			tryDispatchJob();
			break;
		}
	}

	// std::cout << "Worker Done(end JobScheduler)" + std::to_string(pWorker->getWorkerId()) + "\n";
}


void JobScheduler::setWorkerCount(int num)
{
	num = std::max(1, num);

	for (int i = 0; i < workers.size(); ++i)
	{
		workers[i]->setScheduler(nullptr);
	}

	workerStatus = {};
	workerStatus.resize(num);
	workers = std::vector<Worker*>();
	workers.resize(num);
	freeWorkers = {};
	freeWorkers.resize(num);

	for (int i = 0; i < num; ++i)
	{
		workers[i] = new Worker();
		workers[i]->setWorkerId(i);
		workers[i]->setScheduler(this);
		freeWorkers[i] = i;

		workerStatus[i].id = i;
		workerStatus[i].taskNum = 0;
	}
}

int JobScheduler::getMaxWorkerCount()
{
	return std::thread::hardware_concurrency();
}


