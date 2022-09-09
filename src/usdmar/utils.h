#pragma once
#include <iostream>
#include <regex>

#include <pxr/base/js/json.h>

PXR_NAMESPACE_USING_DIRECTIVE

using dict = std::map<std::string, std::string>;

std::string ExpandString(const std::string& value);

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

/// I created this class because I don't know how to pass capturing
/// lambdas as arguments, so instead we can subclass this class and
/// store all required data within.
class _RegexReplacer {
public:
	_RegexReplacer();
	~_RegexReplacer() = default;
	std::string Replace(const std::string& value, const std::regex& regex);

private:
	virtual std::string GetReplacement(const std::string& key) = 0;
};

class _EnvRegexReplacer : public _RegexReplacer {
private:
	// Inherited via _RegexReplacer
	virtual std::string GetReplacement(const std::string& key) override;
};

class MapRegexReplacer : public _RegexReplacer {
public:
	MapRegexReplacer(std::shared_ptr<dict> mappingPtr) {
		_mappingPtr = mappingPtr;
	}
private:
	std::shared_ptr<dict> _mappingPtr;
	// Inherited via _RegexReplacer
	virtual std::string GetReplacement(const std::string& key) override;

};