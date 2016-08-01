#include "ExtensionHelper.h"

ExtensionHelper::ExtensionHelper() {
	queryExtensions();
}


ExtensionHelper::~ExtensionHelper() {
}

bool ExtensionHelper::isExtensionPresent(const std::string & name) {
	for (auto ext : extensions) {
		if (name.compare(ext.extensionName)) {
			return true;
		}
	}
	return false;
}

bool ExtensionHelper::areExtensionsPresent(const std::vector<std::string>& names) {
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
