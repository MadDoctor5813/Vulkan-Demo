#pragma once

#include <vector>
#include <string>
#include <functional>
#include <vulkan\vulkan.h>

class App;

template <typename T>
class VkQueryHelper {

public:
	VkQueryHelper() {

	}

	~VkQueryHelper() {
	}

	std::vector<T>& getNames() { return foundNames; };

	bool isNamePresent(const char* name, std::vector<VkExtensionProperties> exts) {
		for (auto ext : exts) {
			if (strcmp(ext.extensionName, name) == 0) {
				return true;
			}
		}
		return false;
	}

	bool isNamePresent(const char* name, std::vector<VkLayerProperties> layers) {
		for (auto layer : layers) {
			if (strcmp(layer.layerName, name) == 0) {
				return true;
			}
		}
		return false;
	}

	bool areNamesPresent(const std::vector<const char*>& names) {
		for (auto name : names) {
			if (!isNamePresent(name, foundNames)) {
				return false;
			}
		}
		return true;
	}

	void query(std::function<VkResult(unsigned int*, T*)> queryFunc) {
		unsigned int count;
		queryFunc(&count, nullptr);
		foundNames.resize(count);
		queryFunc(&count, foundNames.data());
	}

private:

	std::vector<T> foundNames;

};

