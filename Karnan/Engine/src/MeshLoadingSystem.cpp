#include "MeshLoadingSystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "EngineCore.h"
#include "KarnanUtils.h"
#include "MessagingSystem/Messages.h"

//std libs
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <filesystem>

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
	size_t lastSlash = filepath.rfind('/');
	std::string modelName = filepath.substr(lastSlash + 1);
	if (auto search = _meshMap.find(filepath); search != _meshMap.end())
	{
		std::cout << "Model: " << filepath << " is already loaded." << '\n';
	}
	else if (std::string binaryFilepath = CheckForBinary(modelName); binaryFilepath != "")
	{
		std::vector<VertexBuffer::Vertex> vertices;
		std::vector<uint32_t> indices;

		auto currentTime = std::chrono::high_resolution_clock::now();
		LoadModelFromKMSH(binaryFilepath, vertices, indices);
		auto newTime = std::chrono::high_resolution_clock::now();
		double objLoadTime = std::chrono::duration<double, std::chrono::seconds::period>(newTime - currentTime).count();

		CreateMesh(filepath, vertices, indices);
		std::cout << filepath << " loaded successfully from kmsh with: " << '\n';
		std::cout << "Vertices: " << vertices.size() << '\n';
		std::cout << "Indices: " << indices.size() << '\n';
		std::cout << "Time to load file: " << objLoadTime << " seconds." << '\n';
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
			glm::vec3 modelUV;
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

void MeshLoadingSystem::LoadObjToBinary(const std::string& filename)
{
	std::vector<VertexBuffer::Vertex> vertices;
	std::vector<uint32_t> indices;
	
	LoadObj(filename, vertices, indices);

	size_t lastSlash = filename.rfind('/');
	std::string modelName = filename.substr(lastSlash);
	SerialiseMesh("assets/bin/" + modelName + ".kmsh", vertices, indices);
}

void MeshLoadingSystem::CreateMesh(const std::string& modelName, std::vector<VertexBuffer::Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(_karnanDevice));
	vertexBuffer->CreateVertexBuffers(vertices);

	std::shared_ptr<BasicMesh> mesh;
	if (indices.size() > 0)
	{
		std::shared_ptr<IndexBuffer> indexBuffer(DBG_NEW IndexBuffer(_karnanDevice));
		indexBuffer->CreateIndexBuffers(indices);
		mesh = std::shared_ptr<BasicMesh>(DBG_NEW BasicMesh(modelName, vertexBuffer, indexBuffer));
	}
	else
	{
		mesh = std::shared_ptr<BasicMesh>(DBG_NEW BasicMesh(modelName, vertexBuffer, nullptr));
	}

	_meshMap.insert({ modelName, mesh });
}

std::shared_ptr<BasicMesh> MeshLoadingSystem::GetMesh(const std::string& filename)
{
	if (auto search = _meshMap.find(filename); search != _meshMap.end())
	{
		return _meshMap.at(filename);
	}
	return nullptr;
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
	std::shared_ptr<MLSTerminateThreadProcess> message = std::shared_ptr<MLSTerminateThreadProcess>(DBG_NEW MLSTerminateThreadProcess());
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

		if ((message->MessageInfo()).compare("Load Model") == 0)
		{
			std::string filepathToLoad = dynamic_cast<MLSLoadModelMessage*>(message.get())->FilePath;
			if (filepathToLoad.compare("") != 0)
				LoadMesh(filepathToLoad);
		}
	}
}

void MeshLoadingSystem::GenerateBinaries()
{
	std::string rootFolder{ "./assets" };
	for (auto filepath : FindAllModelFilepaths(rootFolder))
	{
		std::string filename = StripFilenameFromFilepath(filepath);
		if (CheckForBinary(filename) == "")
		{
			LoadObjToBinary(rootFolder + "/" + filepath);
		}
	}
}

std::vector<std::string> MeshLoadingSystem::FindAllModelFilepaths(std::string& rootFolder)
{
	std::vector<std::string> objFiles;
	std::filesystem::path rootPath(rootFolder);

	if (!std::filesystem::exists(rootPath) || !std::filesystem::is_directory(rootPath)) {
		std::cerr << "Invalid directory: " << rootFolder << std::endl;
		return objFiles;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
		if (entry.is_regular_file() && (entry.path().extension() == ".obj" || entry.path().extension() == ".OBJ")) {
			// Store the path relative to the root folder
			objFiles.push_back(
				std::filesystem::relative(entry.path(), rootPath).string()
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
