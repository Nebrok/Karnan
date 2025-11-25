#pragma once

#include "Message.h"

//std libs
#include <vector>
#include <memory>

class IMessageSystem
{
public:

protected:
	std::vector<std::shared_ptr<Message>> _messages;

public:
	virtual void PollQueue() = 0;
	virtual void ProcessMessage(std::shared_ptr<Message> message) = 0;
	virtual void QueueMessage(std::shared_ptr<Message> message) = 0;

private:
	virtual ~IMessageSystem();

};