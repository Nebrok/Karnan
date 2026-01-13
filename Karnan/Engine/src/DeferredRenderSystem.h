#pragma once
#include "KarnanDevice.h"


class DeferredRenderSystem
{

public:

private:
	KarnanDevice& _karnanDevice;


	int _maxFramesInFlight;



public:
	DeferredRenderSystem();
	~DeferredRenderSystem();

	DeferredRenderSystem(const DeferredRenderSystem&) = delete;
	DeferredRenderSystem& operator=(const DeferredRenderSystem&) = delete;

	KarnanDevice& GetDevice() { return _karnanDevice; }




private:


};