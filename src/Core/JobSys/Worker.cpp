#include "Worker.h"
#include <chrono>
#include <string> 
#include <iostream>


Worker::Worker()
{
	//: m_thread(running) 
	m_thread = std::thread([&]() {
        running();
    });
}


//void Worker::assginJob(IWorkerJob * workingJob)
//{
//	m_workingJob = workingJob;
//}


void Worker::running()
{
	while(true)
	{
		if (m_workingJob)
		{
			m_workingJob->execute();
			m_workingJob = nullptr;
			if (m_scheduler) {
				// std::cout << "Worker Done" + std::to_string(m_workerId) + "\n";
				m_scheduler->jobDone(this);
			}
		}
		else 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if(bTerminate) break;
	}
}

void Worker::setScheduler(WorkerSchedulerInterface * scheduler)
{
	m_scheduler = scheduler;
}



