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
		Mesh mesh = loadMesh(iter->path());
		if (mesh.vertexes.size() != 0) {
			//only insert meshes that loaded successfully
			meshes.emplace(iter->path(), mesh);
		}
	}
}

Mesh MeshLoader::getMesh(const std::string & name)
{
	return meshes.at(fs::path(name));
}

Mesh MeshLoader::loadMesh(fs::path path)
{
	std::ifstream fStream;
	Mesh mesh;
	fStream.open(path.string(), std::ios::binary);
	char fileSig[4];
	for (int i = 0; i < 4; i++) {
		fileSig[i] = fStream.get();
	}
	if (strncmp(fileSig, "JMDL", 4) != 0) {
		std::cout << "File " << path.string() << " is an invalid file. Skipping." << std::endl;
		return Mesh();
	}
	int numVertexes;
	fStream.read((char*)&numVertexes, sizeof(numVertexes));
	mesh.vertexes.resize(numVertexes);
	fStream.read((char*)mesh.vertexes.data(), sizeof(Vertex) * numVertexes);
	int numIndexes;
	fStream.read((char*)&numIndexes, sizeof(numIndexes));
	mesh.indexes.resize(numIndexes);
	fStream.read((char*)mesh.indexes.data(), sizeof(int) * numIndexes);
	return mesh;
}
