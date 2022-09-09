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
#include "usdmar/registry.h"
#include "usdmar/debug.h"
#include "usdmar/utils.h"
#include "usdmar/config.h"



PXR_NAMESPACE_OPEN_SCOPE

AR_DEFINE_RESOLVER(MultiAssetResolver, ArResolver);

std::vector<std::string> uriSchemes = std::vector<std::string>{ RESOLVER_SCHEMES };

std::string _ExtractVarsFromAssetPath(const std::string& assetPath, std::string& scheme, std::string& stackName) {

	std::regex regex("^(" + TfStringJoin(uriSchemes, "|") + "):([\\w\\d_]+)?!?(.+)");
	std::smatch matches;

	if (std::regex_search(assetPath, matches, regex)) {
		
		scheme = matches[1].str();
		stackName = matches[2].str();
		return matches[3].str();
	}
	TF_WARN("Asset path %s is invalid\n", assetPath.c_str());
	return assetPath;

}

MultiAssetResolver::MultiAssetResolver() {
	USDMAR_DEBUG("MultiResolver::MultiResolver()\n");
	Registry& registry = Registry::GetInstance();
	registry.Initialize();
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

	Registry& registry = Registry::GetInstance();
	auto schemeDefaults = registry.GetSchemeDefaults();

	// If not specified, we fall back to the defaults
	if (stackName == "") {
		const auto findIter = schemeDefaults.find(scheme);
		if (findIter != schemeDefaults.end()) {
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