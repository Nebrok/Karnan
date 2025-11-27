#pragma once
#include "BaseMessage.h"

class MLSBaseMessage : public Message
{


public:
	MLSBaseMessage(Message::Type messageType, std::string messageInfo)
		: Message(System::MESH_LOADING, messageType, messageInfo)
	{
	}
	virtual ~MLSBaseMessage() override {};
};

class MLSLoadModelMessage : public MLSBaseMessage
{
public:
	std::string FilePath = "";

public:
	MLSLoadModelMessage(std::string filePath)
		: MLSBaseMessage(Message::Type::COMMAND, "Load Model"), FilePath(filePath)
	{
	}
	virtual ~MLSLoadModelMessage() override {};
};

class MLSGenerateBinaries : public MLSBaseMessage
{
public:
	MLSGenerateBinaries()
		: MLSBaseMessage(Message::Type::COMMAND, "Generate Binaries")
	{}
	virtual ~MLSGenerateBinaries() override {};
};