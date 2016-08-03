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

	bool isNamePresent(const char* name) {
		for (auto foundName : foundNames) {
			if (strcmp(getName(foundName), name) == 0) {
				return true;
			}
		}
		return false;
	}

	bool areNamesPresent(const std::vector<const char*>& names) {
		for (auto name : names) {
			if (!isNamePresent(name)) {
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


	void listNames(std::ostream& stream) {
		for (auto name : foundNames) {
			stream << getName(name) << std::endl;
		}
	}

private:

	std::vector<T> foundNames;

	const char* getName(VkExtensionProperties& props) {
		return props.extensionName;
	}

	const char* getName(VkLayerProperties& props) {
		return props.layerName;
	}

};

