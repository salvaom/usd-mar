#pragma once
#include <iostream>

#include <pxr/pxr.h>
#include <pxr/usd/ar/resolvedPath.h>
#include <pxr/base/js/json.h>

#include "usdmar/utils.h"

PXR_NAMESPACE_USING_DIRECTIVE

class SubSolver {
public:
	SubSolver() {};
	~SubSolver() = default;

	virtual const std::string GetName() = 0;
	virtual std::string Resolve(std::string assetPath) = 0;
	virtual void ConfigureFromJsObject(const JsObject& object) = 0;
	virtual std::shared_ptr<SubSolver> CreateNew() const = 0;

	void _ConfigureFromJsObjectInternal(const JsObject& object) {
		std::unique_ptr<bool> evalCachePtr = _GetJsObjectValue<bool>(object, "eval_cache");
		if (evalCachePtr != nullptr) {
			_evaluatesCache = *evalCachePtr;
		}
	}

	bool EvaluatesCache() {
		return _evaluatesCache;
	}

private:
	bool _evaluatesCache = false;
};
