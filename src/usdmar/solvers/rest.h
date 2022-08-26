#pragma once

#include <iostream>
#include "usdmar/solvers/subsolver.h"

class RESTSubSolver : public SubSolver {

public:
	RESTSubSolver();
	~RESTSubSolver();

	virtual const std::string GetName() override;
	void SetHost(std::string host);
	const std::string GetHost();
	void SetEntryPoint(std::string entryPoint);
	virtual std::string Resolve(std::string assetPath) override;
	virtual void ConfigureFromJsObject(const JsObject& object) override;

private:
	std::string _host;
	std::string _entryPoint;
	std::string _name = "rest";
};
