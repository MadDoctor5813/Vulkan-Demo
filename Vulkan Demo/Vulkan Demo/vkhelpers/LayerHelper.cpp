#include "LayerHelper.h"



LayerHelper::LayerHelper() {
	queryLayers();
}


LayerHelper::~LayerHelper() {
}

bool LayerHelper::isLayerPresent(const char* name) {
	for (auto layer : layers) {
		if (strcmp(layer.layerName, name)) {
			return true;
		}
	}
	return false;
}

bool LayerHelper::areLayersPresent(const std::vector<const char*>& names) {
	for (auto name : names) {
		if (!isLayerPresent(name)) {
			return false;
		}
	}
	return true;
}

void LayerHelper::queryLayers() {
	unsigned int layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	layers.resize(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
}
