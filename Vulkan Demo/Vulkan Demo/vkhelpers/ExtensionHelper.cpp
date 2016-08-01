#include "ExtensionHelper.h"

ExtensionHelper::ExtensionHelper() {
	queryExtensions();
}


ExtensionHelper::~ExtensionHelper() {
}

bool ExtensionHelper::isExtensionPresent(const char* name) {
	for (auto ext : extensions) {
		if (strcmp(ext.extensionName, name) == 0) {
			return true;
		}
	}
	return false;
}

bool ExtensionHelper::areExtensionsPresent(const std::vector<const char*>& names) {
	for (auto name : names) {
		if (!isExtensionPresent(name)) {
			return false;
		}
	}
	return true;
}

void ExtensionHelper::queryExtensions() {
	unsigned int extCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
	extensions.resize(extCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());
}
