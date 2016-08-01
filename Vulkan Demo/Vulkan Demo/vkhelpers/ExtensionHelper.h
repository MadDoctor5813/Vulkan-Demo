#pragma once

#include <vector>
#include <string>
#include <vulkan\vulkan.h>

class ExtensionHelper {
public:
	ExtensionHelper();
	~ExtensionHelper();

	std::vector<VkExtensionProperties>& getExtensions() { return extensions; };

	bool isExtensionPresent(const std::string& name);

private:
	void queryExtensions();

	std::vector<VkExtensionProperties> extensions;


};

