#pragma once

#include <vector>
#include <string>
#include <vulkan\vulkan.h>

#include "App.h"

class ExtensionHelper {
public:
	ExtensionHelper(App& app);
	~ExtensionHelper();

	std::vector<VkExtensionProperties>& getExtensions() { return extensions; };

	bool isExtensionPresent(const char* name);
	bool areExtensionsPresent(const std::vector<const char*>& names);

private:
	App& app;

	void queryExtensions();

	std::vector<VkExtensionProperties> extensions;


};

