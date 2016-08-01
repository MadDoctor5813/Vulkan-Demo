#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class LayerHelper {
public:
	LayerHelper();
	~LayerHelper();

	bool isLayerPresent(const std::string& name);
	bool areLayersPresent(const std::vector<std::string>& names);
	std::vector<VkLayerProperties> getLayers() { return layers; };

private:
	std::vector<VkLayerProperties> layers;

	void queryLayers();

};

