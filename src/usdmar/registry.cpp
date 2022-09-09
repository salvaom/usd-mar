#include <iostream>
#include <fstream>

#include <pxr/base/js/json.h>
#include <pxr/base/arch/env.h>
#include <pxr/base/tf/fileUtils.h>

#include "usdmar/registry.h"
#include "usdmar/debug.h"
#include "usdmar/solvers/env.h"
#include "usdmar/solvers/format.h"
#include "usdmar/solvers/symlink.h"
#ifdef USDMAR_REST
#include "usdmar/solvers/rest.h"
#endif
#ifdef USDMAR_SUBPROCESS
#include "usdmar/solvers/subprocess.h"
#endif
#include <shared_mutex>


std::shared_mutex initializeMutex;
std::shared_mutex schemeDefaultsMutex;
std::shared_mutex stackMutex;
std::shared_mutex subsolverMutex;


void Registry::Initialize() {
	initializeMutex.lock();
	_Initialize();
	initializeMutex.unlock();
}
void Registry::_Initialize() {

	if (_initialized) {
		return;
	}

	// Register all subsolvers
#ifdef USDMAR_REST
	RegisterSubSolver(std::make_shared<RESTSubSolver>());
#endif
#ifdef USDMAR_SUBPROCESS
	RegisterSubSolver(std::make_shared<SubprocessSubSolver>());
#endif
	RegisterSubSolver(std::make_shared<EnvSubSolver>());
	RegisterSubSolver(std::make_shared<FormatterSubSolver>());
	RegisterSubSolver(std::make_shared<SymlinkSubSolver>());


	// TODO: Move this environment variable to a USD registered one
	std::string configFilePath = ArchGetEnv("USDMAR_CONFIG_PATH");
	if (!TfIsFile(configFilePath)) {
		TF_WARN("USDMAR config path '%s' does not exist\n", configFilePath.c_str());
		return;
	}

	USDMAR_DEBUG("Reading config file '%s'\n", configFilePath.c_str());

	std::ifstream configStream(configFilePath);
	if (!configStream) {
		TF_WARN("Could not open %s\n", configFilePath.c_str());
		return;
	}

	JsParseError err;
	const JsValue configJsValue = JsParseStream(configStream, &err);

	if (configJsValue.IsNull()) {
		TF_WARN("Error parsing JSON %s\n"
			"line: %d, col: %d ->\n\t%s.\n",
			configFilePath.c_str(),
			err.line, err.column,
			err.reason.c_str());
		return;
	}
	const JsObject& rootDict = configJsValue.GetJsObject();

	RegisterStacksFromJsObject(rootDict);

	auto defaultsPtr = _GetJsObjectValue<JsObject>(rootDict, "defaults");
	if (defaultsPtr != nullptr) {
		auto iter = defaultsPtr->begin();
		while (iter != defaultsPtr->end()) {
			if (!iter->second.IsString()) {
				TF_WARN("'default' key '%s' is not a string", iter->first.c_str());
				++iter;
				continue;
			}
			RegisterSchemeDefault(iter->first, iter->second.GetString());
			++iter;
		}
	}
	_initialized = true;
}

void Registry::RegisterStack(const std::string name, SubSolverStack stack)
{

	
	 std::shared_ptr<SubSolverStack> shared_ptr = std::make_shared<SubSolverStack>(stack);
	 stackMutex.lock_shared();
	 if (_stackRegistry.find(name) != _stackRegistry.end()) {
		 TF_WARN("Subsolver stack with name %s already exists, overwriting it...\n", name.c_str());
	 }
	 stackMutex.unlock_shared();

	 USDMAR_DEBUG("Registering subsolver stack %s\n", name.c_str());
	 stackMutex.lock();
	 _stackRegistry[name] = shared_ptr;
	 stackMutex.unlock();
}

const std::map<std::string, std::string>& Registry::GetSchemeDefaults() {
	return _schemeDefaults;
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
			RegisterStack(stackName, *stack);
		}
		++iter;
	}
}

void Registry::RegisterSchemeDefault(const std::string& scheme, const std::string& stack)
{
	schemeDefaultsMutex.lock();
	_schemeDefaults.insert(std::pair<std::string, std::string>(scheme, stack));
	schemeDefaultsMutex.unlock();
}

std::shared_ptr<SubSolverStack> Registry::GetStack(std::string name)
{
	stackMutex.lock_shared();
	auto key = _stackRegistry.find(name);
	if (key == _stackRegistry.end()) {
		return nullptr;
	}
	stackMutex.unlock_shared();
	return key->second;
}

void Registry::RegisterSubSolver(const std::shared_ptr<SubSolver> subsolverPtr)
{
	std::string name = subsolverPtr->GetName();
	if (_subsolverRegistry.find(name) != _subsolverRegistry.end()) {
		TF_WARN("Subsolver stack with name %s already exists, overwriting it...\n", name.c_str());
	}
	USDMAR_DEBUG("Registering subsolver %s\n", name.c_str());
	subsolverMutex.lock();
	_subsolverRegistry[name] = subsolverPtr;
	subsolverMutex.unlock();
}


std::shared_ptr<SubSolver> Registry::GetSubSolver(std::string name)
{
	subsolverMutex.lock_shared();
	auto key = _subsolverRegistry.find(name);
	if (key == _subsolverRegistry.end()) {
		return nullptr;
	}
	subsolverMutex.unlock_shared();
	return key->second;
}