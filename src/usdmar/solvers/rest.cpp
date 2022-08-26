#include <iostream>

#include <cpr/cpr.h>
#include <pxr/base/tf/diagnostic.h>

#include "usdmar/solvers/rest.h"
#include "usdmar/utils.h"
#include "usdmar/debug.h"


RESTSubSolver::RESTSubSolver() {}
RESTSubSolver::~RESTSubSolver() = default;

 

// Name
const std::string RESTSubSolver::GetName() {
	return _name;
}

// Host
void RESTSubSolver::SetHost(std::string host) {
	_host = ExpandString(host);
}
const std::string RESTSubSolver::GetHost() {
	return _host;
}

// Entry Point
void RESTSubSolver::SetEntryPoint(std::string entryPoint) {
	_entryPoint = ExpandString(entryPoint);
}

std::string RESTSubSolver::Resolve(std::string assetPath)
{
	std::string url;

	// 
	if (_host != "") {
		url = _host;
		if (url.size() && url.back() == '/') {
			url.pop_back();
		}
	}
	if (_entryPoint != "") {
		url += (_entryPoint.at(0) == '/' ? "" : "/") + _entryPoint;
	}
	url = url + assetPath;

	USDMAR_DEBUG("[%s] Making GET request to url '%s'\n", TF_FUNC_NAME().c_str(), url.c_str());

	cpr::Response r = cpr::Get(cpr::Url{ url });
	if (r.status_code != 200) {
		TF_WARN("URL %s responded with code %d\n", url.c_str(), r.status_code);
		return assetPath;
	}
	USDMAR_DEBUG("[%s] Response: %s\n", TF_FUNC_NAME().c_str(), r.text.c_str());
	return r.text;
}

void RESTSubSolver::ConfigureFromJsObject(const JsObject& object)
{
	std::unique_ptr<std::string> host = _GetJsObjectValue<std::string>(object, "host");
	std::unique_ptr<std::string> entryPoint = _GetJsObjectValue<std::string>(object, "entryPoint");

	if (host != nullptr) {
		SetHost(*host);
	}
	if (entryPoint != nullptr) {
		SetEntryPoint(*entryPoint);
	}
}

