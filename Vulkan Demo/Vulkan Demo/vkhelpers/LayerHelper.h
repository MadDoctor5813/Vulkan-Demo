#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class LayerHelper {
public:
	LayerHelper();
	~LayerHelper();

	bool isLayerPresent(const char* name);
	bool areLayersPresent(const std::vector<const char*>& names);
	std::vector<VkLayerProperties> getLayers() { return layers; };

private:
	std::vector<VkLayerProperties> layers;

	void queryLayers();

};

