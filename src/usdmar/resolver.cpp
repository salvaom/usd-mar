#include <iostream>
#include <regex>
#include <fstream>

#include <pxr/pxr.h>
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/ar/defineResolver.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/base/arch/env.h>
#include <pxr/base/arch/fileSystem.h>
#include <pxr/base/tf/fileUtils.h>

#include "usdmar/resolver.h"
#include "usdmar/solvers/rest.h"
#include "usdmar/registry.h"
#include "usdmar/debug.h"
#include "usdmar/utils.h"

PXR_NAMESPACE_OPEN_SCOPE

AR_DEFINE_RESOLVER(MultiAssetResolver, ArResolver);

// TODO: Allow this to be define at configure time
std::string uriSchemes = "mr";

std::string _ExtractVarsFromAssetPath(const std::string& assetPath, std::string& scheme, std::string& stackName) {
	std::vector<std::string> schemes = TfStringTokenize(uriSchemes);

	std::regex regex("^(" + JoinString("|", schemes) + "):([\\w\\d_]+)?!?(.+)");
	std::smatch matches;

	if (std::regex_search(assetPath, matches, regex)) {
		
		// We know we have only three groups on the regex
		for (size_t i = 1; i < matches.size(); ++i) {
			auto match = matches[i].str();
			switch (i)
			{
			case 1:
				scheme = match;
				break;
			case 2:
				stackName = match;
				break;
			case 3:
				return match;
			default:
				break;
			}
		}
	}
	TF_WARN("Asset path %s is invalid\n", assetPath.c_str());
	return assetPath;

}

MultiAssetResolver::MultiAssetResolver() {

	USDMAR_DEBUG("MultiResolver::MultiResolver()\n");
	SubsolverRegistry& registry = SubsolverRegistry::GetInstance();

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

	registry.RegisterFromJsObject(rootDict);

	auto defaultsPtr = _GetJsObjectValue<JsObject>(rootDict, "defaults");
	if (defaultsPtr != nullptr) {
		auto iter = defaultsPtr->begin();
		while (iter != defaultsPtr->end()) {
			if (!iter->second.IsString()) {
				TF_WARN("'default' key '%s' is not a string", iter->first.c_str());
				++iter;
				continue;
			}
			_schemeDefaults.insert(
				std::pair<std::string, std::string>(
					iter->first,
					iter->second.GetString()
				)
			);
			++iter;
		}
	}

	
}
MultiAssetResolver::~MultiAssetResolver() = default;

std::string
MultiAssetResolver::_CreateIdentifier(const std::string& assetPath, const ArResolvedPath& anchorAssetPath) const
{
	USDMAR_DEBUG("MultiResolver::_CreateIdentifier('%s', '%s')\n", assetPath.c_str(), anchorAssetPath.GetPathString().c_str());
	return assetPath;
}

std::string
MultiAssetResolver::_CreateIdentifierForNewAsset(const std::string& assetPath, const ArResolvedPath& anchorAssetPath) const
{
	USDMAR_DEBUG("MultiResolver::_CreateIdentifierForNewAsset('%s', '%s')\n", assetPath.c_str(), anchorAssetPath.GetPathString().c_str());
	return assetPath;
}

ArResolvedPath
MultiAssetResolver::_Resolve(const std::string& assetPath) const
{

	USDMAR_DEBUG("MultiResolver::_Resolve('%s')\n", assetPath.c_str());
	std::string scheme, stackName;
	auto cleanAssetPath = _ExtractVarsFromAssetPath(assetPath, scheme, stackName);
	USDMAR_DEBUG("Extracted asset path: %s\n", cleanAssetPath.c_str());

	SubsolverRegistry& registry = SubsolverRegistry::GetInstance();

	// If not specified, we fall back to the defaults
	if (stackName == "") {
		const auto findIter = _schemeDefaults.find(scheme);
		if (findIter != _schemeDefaults.end()) {
			stackName = findIter->second;
			USDMAR_DEBUG("No stack name specified, defaulting to %s\n", stackName.c_str());
		}
	}

	std::shared_ptr<SubSolverStack> stack = registry.GetStack(stackName);
	if (stack == nullptr) {
		TF_WARN("Stack name '%s' could not be found", stackName.c_str());
		return ArResolvedPath(assetPath);
	}
	return ArResolvedPath(stack->Resolve(cleanAssetPath));
}

ArResolvedPath
MultiAssetResolver::_ResolveForNewAsset(const std::string& assetPath) const
{
	USDMAR_DEBUG("MultiResolver::_ResolveForNewAsset('%s')\n", assetPath.c_str());
	return ArResolvedPath();
}

std::shared_ptr<ArAsset>
MultiAssetResolver::_OpenAsset(const ArResolvedPath& resolvedPath) const
{
	USDMAR_DEBUG("MultiResolver::_OpenAsset('%s')\n", resolvedPath.GetPathString().c_str());
	return _defaultResolver.OpenAsset(resolvedPath);
}

std::shared_ptr<ArWritableAsset>
MultiAssetResolver::_OpenAssetForWrite(const ArResolvedPath& resolvedPath, WriteMode writeMode) const
{
	USDMAR_DEBUG("MultiResolver::_OpenAssetForWrite('%s', %d)\n", resolvedPath.GetPathString().c_str(), static_cast<int>(writeMode));
	return _defaultResolver.OpenAssetForWrite(resolvedPath, writeMode);
}

ArResolverContext
MultiAssetResolver::_CreateDefaultContext() const
{
	USDMAR_DEBUG("MultiResolver::_CreateDefaultContext()\n");
	return ArResolverContext();
}

ArResolverContext
MultiAssetResolver::_CreateDefaultContextForAsset(const std::string& assetPath) const
{
	USDMAR_DEBUG("MultiResolver::_CreateDefaultContextForAsset('%s')\n", assetPath.c_str());
	return _CreateDefaultContext();
}

PXR_NAMESPACE_CLOSE_SCOPE