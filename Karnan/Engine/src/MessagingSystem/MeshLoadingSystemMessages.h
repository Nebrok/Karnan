#pragma once
#include "BaseMessage.h"

class MLSBaseMessage : public Message
{


public:
	MLSBaseMessage(System callingSystem, Message::Type messageType, std::string messageInfo)
		: Message(System::MESH_LOADING, callingSystem, messageType, messageInfo)
	{
	}
	virtual ~MLSBaseMessage() override {};
};

class MLSLoadModelMessage : public MLSBaseMessage
{
public:
	std::string FilePath = "";

public:
	MLSLoadModelMessage(System callingSystem, std::string filePath)
		: MLSBaseMessage(callingSystem, Message::Type::COMMAND, "Load Model"), FilePath(filePath)
	{
	}
	virtual ~MLSLoadModelMessage() override {};
};

class MLSGenerateBinaries : public MLSBaseMessage
{
public:
	MLSGenerateBinaries(System callingSystem)
		: MLSBaseMessage(callingSystem, Message::Type::COMMAND, "Generate Binaries")
	{}
	virtual ~MLSGenerateBinaries() override {};
};

class MLSTerminateThreadProcess : public MLSBaseMessage
{
public:
	MLSTerminateThreadProcess(System callingSystem)
		:MLSBaseMessage(callingSystem, Message::Type::COMMAND, "Terminate Process")
	{}
	virtual ~MLSTerminateThreadProcess() override {};

};