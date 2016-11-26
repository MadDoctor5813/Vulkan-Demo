#include "MeshLoader.h"

#include <fstream>
#include <iostream>

MeshLoader::MeshLoader()
{
}


MeshLoader::~MeshLoader()
{
}

void MeshLoader::loadMeshes()
{
	for (auto iter = fs::directory_iterator("meshes/"); iter != fs::directory_iterator(); iter++) {
		std::vector<Vertex> mesh = loadMesh(iter->path());
		if (mesh.size() != 0) {
			//only insert meshes that loaded successfully
			meshes.emplace(iter->path(), mesh);
		}
	}
}

std::vector<Vertex> MeshLoader::getMesh(const std::string & name)
{
	return meshes.at(fs::path(name));
}

std::vector<Vertex> MeshLoader::loadMesh(fs::path path)
{
	std::ifstream fStream;
	std::vector<Vertex> mesh;
	fStream.open(path.string(), std::ios::binary);
	char fileSig[4];
	for (int i = 0; i < 4; i++) {
		fileSig[i] = fStream.get();
	}
	if (strncmp(fileSig, "JMDL", 4) != 0) {
		std::cout << "File " << path.string() << " is an invalid file. Skipping." << std::endl;
		return std::vector<Vertex>();
	}
	int numVertexes;
	fStream.read((char*)&numVertexes, sizeof(numVertexes));
	mesh.resize(numVertexes);
	fStream.read((char*)mesh.data(), sizeof(Vertex) * numVertexes);
	return mesh;
}
