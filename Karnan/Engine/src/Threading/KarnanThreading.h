#pragma once
#include "IThreadableProcess.h"

class KarnanThreading
{

public:


private:


public:
	static bool CheckThreadable(void* object);

	static void BeginThread(IThreadableProcess* process);
	static void EndThread(IThreadableProcess* process);

	static void EndAllAuxilliaryThreads();

private:


};