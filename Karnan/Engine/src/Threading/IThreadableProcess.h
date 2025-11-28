#pragma once
#include <string>
#include <thread>


class IThreadableProcess
{
public:
	bool IsThreadable = true;
	std::string ThreadName;
	std::thread* Thread = nullptr;

public:
	virtual void Process() = 0;

	virtual void BeginProcessAsSeperateThread() = 0;
	virtual void EndProcessThread() = 0;

private:

};