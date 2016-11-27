#pragma once

#include <vector>
#include <map>
#include <string>
#include <filesystem>

#include "graphics/Mesh.h"

namespace fs = std::tr2::sys;

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();

	void loadMeshes();
	Mesh getMesh(const std::string& name);

private:
	Mesh loadMesh(fs::path path);

	std::map<fs::path, Mesh> meshes;
};

