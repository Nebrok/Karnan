#pragma once
#include <string>

class Message
{
public:
	enum class System
	{
		NONE,
		MESH_LOADING,
	};


	enum class Type
	{
		NONE,
		LOG,
		COMMAND,
		REQUEST,
	};

private:
	System _recipientSystem;
	Type _messageType;
	std::string _messageInfo;

public:
	Message(System recipientSystem, Type messageType, std::string messageInfo)
		: _recipientSystem(recipientSystem), _messageType(messageType), _messageInfo(messageInfo)
	{
	}
	virtual ~Message() {};

	Type GetMessageType() { return _messageType; };
	std::string MessageInfo() { return _messageInfo; };

private:

};

