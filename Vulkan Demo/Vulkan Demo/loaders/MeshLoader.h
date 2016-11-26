#pragma once

#include <vector>
#include <map>
#include <string>
#include <filesystem>

#include "graphics/Vertex.h"

namespace fs = std::tr2::sys;

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();

	void loadMeshes();
	std::vector<Vertex> getMesh(const std::string& name);

private:
	std::vector<Vertex> loadMesh(fs::path path);

	std::map<fs::path, std::vector<Vertex>> meshes;
};

