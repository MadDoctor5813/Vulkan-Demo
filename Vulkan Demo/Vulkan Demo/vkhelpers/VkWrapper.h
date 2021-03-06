#pragma once

#include <vulkan\vulkan.h>
#include <functional>

template <typename T>
class VkWrapper {

public:
	VkWrapper() : deleteFunc([](T _) {}) {}

	VkWrapper(std::function<void(T, VkAllocationCallbacks*)> deletef) {
		deleteFunc = [deletef](T obj) { deletef(obj, nullptr); };
	}

	VkWrapper(const VkWrapper<VkInstance>& instance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef) {
		deleteFunc = [&instance, deletef](T obj) { deletef(instance, obj, nullptr); };
	}

	VkWrapper(const VkWrapper<VkDevice>& device, std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef) {
		deleteFunc = [&device, deletef](T obj) { deletef(device, obj, nullptr); };
	}

	~VkWrapper() {
		cleanup();
	}

	T* operator &() {
		cleanup();
		return &object;
	}

	T* replace() {
		cleanup();
		return &object;
	}

	operator T() const {
		return object;
	}

	void operator=(T rhs) {
		if (rhs != object) {
			cleanup();
			object = rhs;
		}
	}

	template <typename V>
	void operator==(V rhs) {
		return object = T(rhs);
	}

private:
	T object{ VK_NULL_HANDLE };
	std::function<void(T)> deleteFunc;

	void cleanup() {
		if (object != VK_NULL_HANDLE) {
			deleteFunc(object);
			object = VK_NULL_HANDLE;
		}
	}

};

