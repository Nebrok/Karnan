#pragma once
#include <mutex>


class KarnanMutex
{
public:


private:
	std::mutex _mutex;
	bool _locked;


public:
	KarnanMutex();
	~KarnanMutex();

	bool WaitUntilLock();

	bool WaitForMaxLock(int maxMilliseconds);

	bool Unlock();

private:


};