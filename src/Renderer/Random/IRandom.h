#pragma once



class IRandom {
public:
	~IRandom() {}
	virtual float random() = 0;
};