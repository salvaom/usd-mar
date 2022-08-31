#include <iostream>
#include <fstream>

#include <pxr/base/js/json.h>

#include "usdmar/registry.h"
#include "usdmar/debug.h"

void Registry::RegisterStack(const std::string name, SubSolverStack stack)
{
	 std::shared_ptr<SubSolverStack> shared_ptr = std::make_shared<SubSolverStack>(stack);

	 if (_stackRegistry.find(name) != _stackRegistry.end()) {
		 TF_WARN("Subsolver stack with name %s already exists, overwriting it...\n", name.c_str());
	 }
	 USDMAR_DEBUG("Registering subsolver stack %s\n", name.c_str());
	 _stackRegistry[name] = shared_ptr;
}


void Registry::RegisterStacksFromJsObject(const JsObject& jsonDict)
{
	JsObject::const_iterator rootIter;
	rootIter = jsonDict.find("stacks");

	if (rootIter == jsonDict.end()) {
		TF_WARN("Could not find key 'stacks' key on file\n");
		return;
	}

	if (!rootIter->second.IsObject()) {
		TF_WARN("Key 'stacks' must be a dict\n");
		return;
	}

	Registry& registry = Registry::GetInstance();

	const JsObject& stacksDict = rootIter->second.GetJsObject();
	auto iter = stacksDict.begin();

	while (iter != stacksDict.end()) {
		const std::string& stackName = iter->first;
		const JsValue& stackData = iter->second;

		if (!stackData.IsObject()) {
			TF_WARN("Value of stack %s is not a dict\n", stackName.c_str());
			++iter;
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

std::shared_ptr<SubSolverStack> Registry::GetStack(std::string name)
{
	auto key = _stackRegistry.find(name);
	if (key == _stackRegistry.end()) {
		return nullptr;
	}
	return key->second;
}

void Registry::RegisterSubSolver(const std::shared_ptr<SubSolver> subsolverPtr)
{
	std::string name = subsolverPtr->GetName();
	if (_subsolverRegistry.find(name) != _subsolverRegistry.end()) {
		TF_WARN("Subsolver stack with name %s already exists, overwriting it...\n", name.c_str());
	}
	USDMAR_DEBUG("Registering subsolver %s\n", name.c_str());
	_subsolverRegistry[name] = subsolverPtr;
}


std::shared_ptr<SubSolver> Registry::GetSubSolver(std::string name)
{
	auto key = _subsolverRegistry.find(name);
	if (key == _subsolverRegistry.end()) {
		return nullptr;
	}
	return key->second;
}