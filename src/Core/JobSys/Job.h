#ifndef JOB_HEADER
#define JOB_HEADER

//#pragma once

#include <vector>
#include "JobScheduler.h"
#include <iostream>
#include <sstream>
#include <functional>

struct JobIdentifier
{
	int id;
};


class Job
{

public:
	Job(){};

	void execute() { 
		taskFunction();
	}

	int value;

	//void execute(const JobScheduler & scheduler ) {}
	JobIdentifier getId() {return m_id;}
	void setId(const JobIdentifier & id) {m_id = id;}

	void addDependency(const Job & job) {m_dependency.push_back(job);}
	std::vector<Job> getDependencies() {return m_dependency; }

	void setTask(std::function<void()> & func) {taskFunction = func;}
private:
	std::function<void()> taskFunction = [] {};
	JobIdentifier m_id;
	std::vector<Job> m_dependency;

};


#endif