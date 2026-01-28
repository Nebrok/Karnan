#include "MeshLoadingSystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "EngineCore.h"
#include "KarnanUtils.h"
#include "MessagingSystem/Messages.h"
#include "AssetManagement/AssetManager.h"

//std libs
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <Windows.h>

//150mb
#define ALLOWABLE_LEFTOVER_MEMORY_SIZE 157286400

namespace std
{
	template<>
	struct hash<VertexBuffer::Vertex>
	{
		size_t operator()(VertexBuffer::Vertex const& vertex) const
		{
			size_t seed = 0;
			Karnan::HashCombine(seed, vertex.position, vertex.uv, vertex.normal);
			return seed;
		}
	};
}

MeshLoadingSystem* MeshLoadingSystem::Instance = nullptr;

MeshLoadingSystem* MeshLoadingSystem::StartMeshLoadingSystem()
{
	if (MeshLoadingSystem::Instance == nullptr)
	{
		Instance = DBG_NEW MeshLoadingSystem();
	}
	return Instance;
}

void MeshLoadingSystem::DestroyMeshLoadingSystem()
{
	MeshLoadingSystem::Instance->Thread->join();
	delete(MeshLoadingSystem::Instance->Thread);
	delete(MeshLoadingSystem::Instance);
}

void MeshLoadingSystem::LoadMesh(const std::string& filepath)
{
	std::filesystem::path meshpath{ filepath };
	if (std::string binaryFilepath = CheckForBinary(meshpath.stem().string()); binaryFilepath != "")
	{
		std::vector<VertexBuffer::Vertex>* vertices = DBG_NEW std::vector<VertexBuffer::Vertex>();
		std::vector<uint32_t>* indices = DBG_NEW std::vector<uint32_t>();

		auto currentTime = std::chrono::high_resolution_clock::now();
		LoadModelFromKMSH(binaryFilepath, *vertices, *indices);
		auto newTime = std::chrono::high_resolution_clock::now();
		double objLoadTime = std::chrono::duration<double, std::chrono::seconds::period>(newTime - currentTime).count();

		int verticesTotalByteSize = vertices->size() * sizeof(VertexBuffer::Vertex);
		int indicesTotalByteSize = indices->size() * sizeof(uint32_t);
		int totalBytesOfMesh = verticesTotalByteSize + indicesTotalByteSize;

		CreateMesh(filepath, vertices, indices);
		std::cout << filepath << " loaded successfully from kmsh with: " << '\n';
		std::cout << "Vertices: " << vertices->size() << ". Total memory usage: " << verticesTotalByteSize << "bytes." << '\n';
		std::cout << "Indices: " << indices->size() << ". Total memory usage: " << indicesTotalByteSize << "bytes." << '\n';
		std::cout << "Time to load file: " << objLoadTime << " seconds." << '\n';
		std::cout << "Mesh total memory usage: " << totalBytesOfMesh << "bytes." << '\n';
	}
	else
	{
		std::cout << "Could not find file: " + filepath + " when attempting to load mesh!" + '\n';
	}
}

std::string MeshLoadingSystem::CheckForBinary(const std::string& filename)
{
	std::string pathToAssetDir = "assets/bin";

	for (const auto& file : std::filesystem::directory_iterator("assets/bin"))
	{
		std::string pathFileName = file.path().filename().string();
		if (pathFileName.compare(filename + ".kmsh") == 0)
			return file.path().string();
	}
	return "";
}

void MeshLoadingSystem::LoadObj(const std::string& filename, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	vertices.clear();
	indices.clear();

	auto currentTime = std::chrono::high_resolution_clock::now();
	std::vector<glm::vec3> modelVertices;
	std::vector<glm::vec3> modelNormals;
	std::vector<glm::vec2> modelUVs;

	std::ifstream file(filename);
	std::string line;
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file: " + filename);
	}
	int lineCount = 0;

	std::unordered_map<VertexBuffer::Vertex, uint32_t> uniqueVertices{};

	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "v")
		{
			glm::vec3 modelVertex;
			iss >> modelVertex.x >> modelVertex.y >> modelVertex.z;
			modelVertices.push_back(modelVertex);
		}

		if (prefix == "vt")
		{
			glm::vec2 modelUV;
			iss >> modelUV.x >> modelUV.y;
			modelUVs.push_back(modelUV);
		}

		if (prefix == "vn")
		{
			glm::vec3 modelNormal;
			iss >> modelNormal.x >> modelNormal.y >> modelNormal.z;
			modelNormals.push_back(modelNormal);
		}

		if (prefix == "f")
		{
			std::vector<std::string> points;
			std::string point;

			while (iss >> point)
			{
				points.push_back(point);
			}

			if (points.size() > 4) // Ngons/Polygons
			{
				points = Triangularise(points);
			}
			else if (points.size() == 4) //Quads
			{
				points = Triangularise(points);
			}

			for (int i = 0; i < points.size(); i++)
			{
				std::vector<int> pointIndices = ParseFacePoint(points[i]);

				for (int index : pointIndices)
				{
					if (index < 0)
						throw std::runtime_error("Negative indexes in obj not accounted for yet.");
				}

				VertexBuffer::Vertex vertex;
				vertex.position = modelVertices[pointIndices[0] - 1];

				if (pointIndices[1] > 0)
					vertex.uv = modelUVs[pointIndices[1] - 1];
				else
					vertex.uv = { 0,0 };

				vertex.normal = modelNormals[pointIndices[2] - 1];


				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}

		}
		lineCount++;
	}
	file.close();
	auto newTime = std::chrono::high_resolution_clock::now();
	double objLoadTime = std::chrono::duration<double, std::chrono::seconds::period>(newTime - currentTime).count();

	std::cout << filename << " loaded successfully from obj with: " << '\n';
	std::cout << "Vertices: " << vertices.size() << '\n';
	std::cout << "Indices: " << indices.size() << '\n';
	std::cout << "Time to load file " + filename + ": " << objLoadTime << " seconds." << '\n';
}

void MeshLoadingSystem::LoadObjToBinary(const std::filesystem::path& filepath)
{
	std::vector<VertexBuffer::Vertex> vertices;
	std::vector<uint32_t> indices;
	
	LoadObj(filepath.string(), vertices, indices);

	SerialiseMesh("assets/bin/" + filepath.stem().string() + ".kmsh", vertices, indices);
}

void MeshLoadingSystem::CreateMesh(const std::string& modelName, std::vector<VertexBuffer::Vertex>* vertices, std::vector<uint32_t>* indices)
{
	std::shared_ptr<AMCreateMeshMessage> createMessage = std::shared_ptr<AMCreateMeshMessage>(
		DBG_NEW AMCreateMeshMessage(Message::System::ASSET_MANAGER, modelName, vertices, indices));

	std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
	AssetManager::Instance->QueueMessage(createMessage);
	messageQueueLock.unlock();
}

void MeshLoadingSystem::Process()
{
	while (!_terminateProcess)
	{
		std::unique_lock<std::mutex> messageQueueLock(MessageQueueMutex);
		std::shared_ptr<Message> message = PollQueue();
		messageQueueLock.unlock();
		while (message != nullptr)
		{
			ProcessMessage(message);
			std::unique_lock<std::mutex> messageQueueLock(MessageQueueMutex);
			message = PollQueue();
			messageQueueLock.unlock();
		}
	}
}

void MeshLoadingSystem::BeginProcessAsSeperateThread()
{
	Thread = DBG_NEW std::thread(&MeshLoadingSystem::Process, this);
	ThreadName = "MeshLoadingSystemThread";
}

void MeshLoadingSystem::EndProcessThread()
{
	std::shared_ptr<MLSTerminateThreadProcess> message = std::shared_ptr<MLSTerminateThreadProcess>(DBG_NEW MLSTerminateThreadProcess(Message::System::MESH_LOADING));
	std::unique_lock<std::mutex> messageQueueLock(MessageQueueMutex);
	QueueMessage(message);
	messageQueueLock.unlock();
}

MeshLoadingSystem::MeshLoadingSystem()
	: _karnanDevice(EngineCore::Instance->Device())
{
}

void MeshLoadingSystem::SerialiseMesh(const std::string& outputPath, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	/* 
	* VertexBuffer::Vertex format:
	*		glm::vec3
	*		glm::vec3
	*		glm::vec2          
	* 
	* glm::vec3 == 3 * 4bytes or 12 bytes total
	* glm::vec2 == 2 * 4bytes or 8 bytes total
	* 
	* Vertex == 32 bytes
	* 
	* custom file format == ".kmsh" 
	*/

	std::fstream outfile;
	outfile.open(outputPath.c_str(), std::fstream::out | std::fstream::binary);
	if (outfile.fail())
	{
		std::cout << "Failed to open file: " << outputPath << '\n';
	}
	size_t numberVertices = vertices.size();
	outfile.write((char*)&numberVertices, sizeof(size_t));

	size_t numberIndices = indices.size();
	outfile.write((char*)&numberIndices, sizeof(size_t));

	outfile.write((char*)vertices.data(), sizeof(VertexBuffer::Vertex) * numberVertices);

	if (numberIndices != 0)
	{
		outfile.write((char*)indices.data(), sizeof(uint32_t) * numberIndices);
	}

	outfile.close();
}

void MeshLoadingSystem::LoadModelFromKMSH(const std::string& filepath, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	vertices.clear();
	indices.clear();

	std::fstream infile;
	infile.open(filepath.c_str(), std::fstream::in | std::fstream::binary);
	size_t numberVertices;
	infile.read((char*)&numberVertices, sizeof(size_t));

	size_t numberIndices = indices.size();
	infile.read((char*)&numberIndices, sizeof(size_t));

	int verticesTotalByteSize = numberVertices * sizeof(VertexBuffer::Vertex);
	int indicesTotalByteSize = numberIndices * sizeof(uint32_t);
	int totalBytesOfMesh = verticesTotalByteSize + indicesTotalByteSize;

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	if ((statex.ullAvailPhys - ALLOWABLE_LEFTOVER_MEMORY_SIZE - totalBytesOfMesh) <= 0)
	{
		throw std::runtime_error("Error reading kmsh file: not enough memory to load model");
	}


	if (numberVertices < 100000000)
	{
		vertices.resize(numberVertices);
		infile.read((char*)vertices.data(), sizeof(VertexBuffer::Vertex) * numberVertices);
	}
	else
	{
		throw std::runtime_error("Error reading kmsh file: too many vertices to be read");
	}

	if (numberIndices > 0 && numberIndices < 100000000)
	{
		indices.resize(numberIndices);
		infile.read((char*)indices.data(), sizeof(uint32_t) * numberIndices);
	}
	else if (numberIndices < 100000000)
	{
		throw std::runtime_error("Error reading kmsh file: too many indices to be read");
	}

	infile.close();


}

std::vector<std::string> MeshLoadingSystem::Triangularise(const std::vector<std::string>& polygon)
{
	std::vector<std::string> expanded;
	if (polygon.size() < 5)
	{
		expanded.push_back(polygon[0]);
		expanded.push_back(polygon[1]);
		expanded.push_back(polygon[3]);

		expanded.push_back(polygon[1]);
		expanded.push_back(polygon[2]);
		expanded.push_back(polygon[3]);
	}
	else
	{
		for (int i = 1; i < polygon.size() - 1; i++)
		{
			expanded.push_back(polygon[0]);
			expanded.push_back(polygon[i]);
			expanded.push_back(polygon[i + 1]);
		}
	}

	return expanded;
}

std::vector<int> MeshLoadingSystem::ParseFacePoint(const std::string& point)
{
	std::vector<int> indices;
	std::istringstream ss(point);
	std::string part;

	// Extract v
	if (std::getline(ss, part, '/'))
		indices.push_back(std::stoi(part));

	// Extract vt
	if (std::getline(ss, part, '/'))
		indices.push_back(part.empty() ? 0 : std::stoi(part));   // vt may be missing

	// Extract vn
	if (std::getline(ss, part, '/'))
		indices.push_back(std::stoi(part));

	return indices;
}

void MeshLoadingSystem::QueueMessage(std::shared_ptr<Message> message)
{
	_messages.push(message);
}

std::shared_ptr<Message> MeshLoadingSystem::PollQueue()
{
	if (_messages.empty())
		return nullptr;

	std::shared_ptr<Message> message = _messages.front();
	_messages.pop();
	return message;
}

void MeshLoadingSystem::ProcessMessage(std::shared_ptr<Message> message)
{
	if (message->GetMessageType() == Message::Type::COMMAND)
	{
		if ((message->MessageInfo()).compare("Terminate Process") == 0)
		{
			_terminateProcess = true;
		}

		if ((message->MessageInfo()).compare("Generate Binaries") == 0)
		{
			GenerateBinaries();
		}

		if ((message->MessageInfo()).compare("Load Model") == 0 && message->GetCallingSystem() == Message::System::ASSET_MANAGER)
		{
			std::string filepathToLoad = dynamic_cast<MLSLoadModelMessage*>(message.get())->FilePath;
			if (filepathToLoad.compare("") != 0)
				LoadMesh(filepathToLoad);
			std::shared_ptr<AMReplyLoadMeshMessage> loadReplyMessage = std::shared_ptr<AMReplyLoadMeshMessage>(
				DBG_NEW AMReplyLoadMeshMessage(Message::System::MESH_LOADING, message->ReplyKey));
			std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
			AssetManager::Instance->QueueMessage(loadReplyMessage);
			messageQueueLock.unlock();

		}
		else if ((message->MessageInfo()).compare("Load Model") == 0)
		{
			std::string filepathToLoad = dynamic_cast<MLSLoadModelMessage*>(message.get())->FilePath;
			if (filepathToLoad.compare("") != 0)
				LoadMesh(filepathToLoad);
		}
	}
}

void MeshLoadingSystem::GenerateBinaries()
{
	std::filesystem::path rootFolder{ "./assets" };
	for (std::filesystem::path& filepath : FindAllModelFilepaths(rootFolder))
	{
		std::string filename = filepath.stem().string();
		if (CheckForBinary(filename) == "")
		{
			LoadObjToBinary(std::filesystem::path(rootFolder.string() + "\\" + filepath.string()));
		}
	}
}

std::vector<std::filesystem::path> MeshLoadingSystem::FindAllModelFilepaths(std::filesystem::path rootPath)
{
	std::vector<std::filesystem::path> objFiles;

	if (!std::filesystem::exists(rootPath) || !std::filesystem::is_directory(rootPath)) {
		std::cerr << "Invalid directory: " << rootPath.string() << std::endl;
		return objFiles;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
		if (entry.is_regular_file() && (entry.path().extension() == ".obj" || entry.path().extension() == ".OBJ")) {
			// Store the path relative to the root folder
			objFiles.push_back(
				std::filesystem::relative(entry.path(), rootPath)
			);
		}
	}

	return objFiles;
}

std::string MeshLoadingSystem::StripFilenameFromFilepath(std::string filepath)
{
	std::filesystem::path path(filepath);
	std::string modelName = path.filename().string();
	return modelName;
}
