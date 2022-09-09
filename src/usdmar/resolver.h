#pragma once
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/ar/defaultResolver.h>

#include "usdmar/solverStack.h"

PXR_NAMESPACE_OPEN_SCOPE

class MultiAssetResolver : public ArResolver {
	
public:
	MultiAssetResolver();
	virtual ~MultiAssetResolver();
protected:
	// Inherited via ArResolver
	virtual std::string _CreateIdentifier(const std::string& assetPath, const ArResolvedPath& anchorAssetPath) const override;
	virtual std::string _CreateIdentifierForNewAsset(const std::string& assetPath, const ArResolvedPath& anchorAssetPath) const override;
	virtual ArResolvedPath _Resolve(const std::string& assetPath) const override;
	virtual ArResolvedPath _ResolveForNewAsset(const std::string& assetPath) const override;
	virtual std::shared_ptr<ArAsset> _OpenAsset(const ArResolvedPath& resolvedPath) const override;
	virtual std::shared_ptr<ArWritableAsset> _OpenAssetForWrite(const ArResolvedPath& resolvedPath, WriteMode writeMode) const override;
	ArResolverContext _CreateDefaultContext() const final;
	ArResolverContext _CreateDefaultContextForAsset(const std::string& assetPath) const final;
private:
	ArDefaultResolver _defaultResolver;
	SubSolverStack _stack;
};

PXR_NAMESPACE_CLOSE_SCOPE