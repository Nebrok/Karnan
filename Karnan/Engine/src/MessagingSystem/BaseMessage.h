#pragma once
#include <string>

class Message
{
public:
	enum class System
	{
		NONE,
		MESH_LOADING,
		ASSET_MANAGER,
	};

	enum class Type
	{
		NONE,
		LOG,
		COMMAND,
		REQUEST,
		REPLY,
	};

	Message* ReplyKey;

private:
	System _recipientSystem;
	System _callingSystem;
	Type _messageType;
	std::string _messageInfo;

public:
	Message(System recipientSystem, System callingSystem, Type messageType, std::string messageInfo)
		: _recipientSystem(recipientSystem), _messageType(messageType), _messageInfo(messageInfo)
	{
	}
	virtual ~Message() {};

	System GetRecipientSystem() { return _recipientSystem; };
	System GetCallingSystem() { return _callingSystem; };
	Type GetMessageType() { return _messageType; };
	std::string MessageInfo() { return _messageInfo; };

private:

};

