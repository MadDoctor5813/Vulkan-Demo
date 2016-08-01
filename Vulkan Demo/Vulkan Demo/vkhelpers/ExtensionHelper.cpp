#include "ExtensionHelper.h"

#include "App.h"

ExtensionHelper::ExtensionHelper(App& app) : app(app) {
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

PFN_vkVoidFunction ExtensionHelper::loadExtensionProc(const std::string & name) {
	return vkGetInstanceProcAddr(app.getInstance(), name.c_str());
}

void ExtensionHelper::queryExtensions() {
	unsigned int extCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
	extensions.resize(extCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());
}
