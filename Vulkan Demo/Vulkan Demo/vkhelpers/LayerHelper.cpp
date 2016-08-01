#include "LayerHelper.h"



LayerHelper::LayerHelper() {
	queryLayers();
}


LayerHelper::~LayerHelper() {
}

bool LayerHelper::isLayerPresent(const std::string & name) {
	for (auto layer : layers) {
		if (name.compare(layer.layerName)) {
			return true;
		}
	}
	return false;
}

bool LayerHelper::areLayersPresent(const std::vector<std::string>& names) {
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
