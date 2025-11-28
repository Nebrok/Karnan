#include "KarnanMutex.h"

#include <chrono>

KarnanMutex::KarnanMutex()
{
}

KarnanMutex::~KarnanMutex()
{
}

bool KarnanMutex::WaitUntilLock()
{
	return false;
}

bool KarnanMutex::WaitForMaxLock(int maxMilliseconds)
{
	return false;
}

bool KarnanMutex::Unlock()
{
	return false;
}
