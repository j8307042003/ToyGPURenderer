#pragma once


enum class EWindowEvent
{
	KeyPressed,
	KeyReleased,
	KeyDown,
	MousePressed,
	MouseReleased,
};

class WindowEvent
{
public:
	virtual ~WindowEvent() = default;
	virtual EWindowEvent GetEventType() = 0;
};

class KeyPressedEvent : public WindowEvent
{
public:
	int keyCode;

public:
	virtual EWindowEvent GetEventType() { return EWindowEvent::KeyPressed;}
};

class KeyDownEvent : public WindowEvent
{
private:
	int keyCode;

public:
	virtual EWindowEvent GetEventType() { return EWindowEvent::KeyDown;}
};


class KeyReleaseEvent : public WindowEvent
{
public:
	int keyCode;

public:
	virtual EWindowEvent GetEventType() { return EWindowEvent::KeyReleased;}
};

class MousePressedEvent : public WindowEvent
{
public:
	int mouseCode;

public:
	virtual EWindowEvent GetEventType() { return EWindowEvent::MousePressed;}
};

class MouseReleaseEvent : public WindowEvent
{
public:
	int mouseCode;

public:
	virtual EWindowEvent GetEventType() { return EWindowEvent::MouseReleased;}
};