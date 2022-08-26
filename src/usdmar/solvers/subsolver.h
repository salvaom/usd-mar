#pragma once
#include <iostream>

#include <pxr/pxr.h>
#include <pxr/usd/ar/resolvedPath.h>
#include <pxr/base/js/json.h>

PXR_NAMESPACE_USING_DIRECTIVE

class SubSolver {
public:
	SubSolver() {};
	~SubSolver() = default;
	virtual const std::string GetName() = 0;
	virtual std::string Resolve(std::string assetPath) = 0;
	virtual void ConfigureFromJsObject(const JsObject& object) = 0;
};