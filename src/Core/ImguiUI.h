#pragma once



class ImguiUI
{
public:
	virtual ~ImguiUI() = default;
	virtual void OnGUI() = 0;
};