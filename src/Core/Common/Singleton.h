#ifndef SINGLETON_HEADER
#define SINGLETON_HEADER

#pragma once

template<typename T>
class Singleton
{

public:
	virtual ~Singleton() {}

	static T& GetInstance()
	{
		static T instance;
		return instance;
	}

};


#endif