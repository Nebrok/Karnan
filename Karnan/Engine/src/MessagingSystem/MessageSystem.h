#pragma once

#include "BaseMessage.h"

//std libs
#include <vector>
#include <queue>
#include <memory>
#include <mutex>


class IMessageSystem
{
public:
	std::mutex MessageQueueMutex;


protected:
	std::queue<std::shared_ptr<Message>> _messages;

public:
	virtual ~IMessageSystem() {};

	virtual void QueueMessage(std::shared_ptr<Message> message) = 0;

protected:
	virtual void ProcessMessage(std::shared_ptr<Message> message) = 0;

	//Returns top message in queue and removes message from queue
	virtual std::shared_ptr<Message> PollQueue() = 0;

private:

};