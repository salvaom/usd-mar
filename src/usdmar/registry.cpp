#include <iostream>
#include <fstream>

#include <pxr/base/js/json.h>

#include "usdmar/registry.h"
#include "usdmar/debug.h"

void SubsolverRegistry::RegisterStack(const std::string name, SubSolverStack stack)
{
	 std::shared_ptr<SubSolverStack> shared_ptr = std::make_shared<SubSolverStack>(stack);

	 if (_registry.find(name) != _registry.end()) {
		 TF_WARN("Subsolver stack with name %s already exists, overwriting it...\n", name.c_str());
	 }
	 USDMAR_DEBUG("Registering %s\n", name.c_str());
	 _registry[name] = shared_ptr;
}


void SubsolverRegistry::RegisterFromJsObject(const JsObject& jsonDict)
{
	JsObject::const_iterator rootIter;
	rootIter = jsonDict.find("resolvers");

	if (rootIter == jsonDict.end()) {
		TF_WARN("Could not find key 'resolvers' key on file\n");
		return;
	}

	if (!rootIter->second.IsObject()) {
		TF_WARN("Key 'resolvers' must be a dict\n");
		return;
	}

	SubsolverRegistry& registry = SubsolverRegistry::GetInstance();

	const JsObject& stacksDict = rootIter->second.GetJsObject();
	auto iter = stacksDict.begin();

	while (iter != stacksDict.end()) {
		const std::string& stackName = iter->first;
		const JsValue& stackData = iter->second;

		if (!stackData.IsObject()) {
			TF_WARN("Value of stack %s is not a dict\n", stackName.c_str());
			continue;
		}

		const JsObject& dict = stackData.GetJsObject();
		std::unique_ptr<SubSolverStack> stack = SubSolverStack::CreateFromJsObject(dict);
		if (stack != nullptr) {
			registry.RegisterStack(stackName, *stack);
		}
		++iter;
	}
}

std::shared_ptr<SubSolverStack> SubsolverRegistry::GetStack(std::string name)
{
	auto key = _registry.find(name);
	if (key == _registry.end()) {
		return nullptr;
	}
	return key->second;
}


