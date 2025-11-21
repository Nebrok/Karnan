#pragma once


//std libs
#include <vector>


class MessageSystem
{
public:
	static MessageSystem* Instance;

private:


public:
	void InitialiseMessageSystem();
	void ShutdownMessageSystem();


	void RegisterSystem();

	void PollQueue();


private:
	MessageSystem();
	~MessageSystem();

};