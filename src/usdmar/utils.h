#pragma once
#include <iostream>
#include <regex>

#include <pxr/base/js/json.h>

PXR_NAMESPACE_USING_DIRECTIVE


std::string RegexReplace(const std::string& value, const std::regex& regex, std::string(*func)(const std::string&));
std::string ExpandString(const std::string& value);
std::string JoinString(const std::string& sep, const std::vector<std::string>& strings);

template <class T>
std::unique_ptr<T> _GetJsObjectValue(const JsObject& dict, std::string key) {
	JsObject::const_iterator findIter;
	findIter = dict.find(key);

	if (findIter == dict.end()) {
		return nullptr;
	}

	const JsValue& value = findIter->second;
	if (!value.Is<T>()) {
		return nullptr;
	}

	const T& finalValue = value.Get<T>();
	return std::make_unique<T>(finalValue);
}


