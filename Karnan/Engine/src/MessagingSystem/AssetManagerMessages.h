#pragma once
#include "BaseMessage.h"
#include "../GameObject.h"
#include "../KarnanMaterial.h"
#include "../AssetManagement/MaterialDataObject.h"

#include <vector>

class AMBaseCommandMessage : public Message
{


public:
	AMBaseCommandMessage(System callingSystem, std::string messageInfo)
		: Message(System::ASSET_MANAGER, callingSystem, Type::COMMAND, messageInfo)
	{
	}
	virtual ~AMBaseCommandMessage() override {};
};

class AMBaseReplyMessage : public Message
{


public:
	AMBaseReplyMessage(System callingSystem, std::string messageInfo)
		: Message(System::ASSET_MANAGER, callingSystem, Type::REPLY, messageInfo)
	{
	}
	virtual ~AMBaseReplyMessage() override {};
};


class AMCreateMeshMessage : public AMBaseCommandMessage
{
public:
	std::string Meshname;
	std::vector<VertexBuffer::Vertex>* Vertices;
	std::vector<uint32_t>* Indices;

public:
	AMCreateMeshMessage(System callingSystem, std::string meshname, std::vector<VertexBuffer::Vertex>* vertices, std::vector<uint32_t>* indices)
		: AMBaseCommandMessage(callingSystem, "Create Mesh"), Meshname(meshname), Vertices(vertices), Indices(indices)
	{
	}

	virtual ~AMCreateMeshMessage() override {};
};

class AMLoadMeshMessage : public AMBaseCommandMessage
{
public:
	std::string Filepath;
	GameObject* CallingGO;

public:
	AMLoadMeshMessage(System callingSystem, std::string filepath, GameObject* callingGO)
		: AMBaseCommandMessage(callingSystem, "Load Mesh"), Filepath(filepath), CallingGO(callingGO)
	{
	}

	virtual ~AMLoadMeshMessage() override {};
};

class AMReplyLoadMeshMessage : public AMBaseReplyMessage
{
public:
	Message* OriginalMessageKey;

public:
	AMReplyLoadMeshMessage(System callingSystem, Message* originalMessageKey)
		: AMBaseReplyMessage(callingSystem, "Load Mesh Reply"), OriginalMessageKey(originalMessageKey)
	{
	}

	virtual ~AMReplyLoadMeshMessage() override {};
};


class AMCreateMaterialMessage : public AMBaseCommandMessage
{
public:
	std::string MaterialDataObjectFilepath;
	GameObject* CallingGO;

public:
	AMCreateMaterialMessage(System callingSystem, std::string materialDataFilepath, GameObject* callingGO)
		: AMBaseCommandMessage(callingSystem, "Create Material"), MaterialDataObjectFilepath(materialDataFilepath), CallingGO(callingGO)
	{
	}
};

class AMUpdateGameobjectMaterialMessage : public AMBaseReplyMessage 
{
public:
	GameObject* CallingGO;

public:
	AMUpdateGameobjectMaterialMessage(System callingSystem, GameObject* callingGO)
		: AMBaseReplyMessage(callingSystem, "Update Gameobject Material"), CallingGO(callingGO)
	{
	}
};