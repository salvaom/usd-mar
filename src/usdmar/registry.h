#pragma once
#include <iostream>
#include <map>

#include "usdmar/solverStack.h"
#include "usdmar/solvers/subsolver.h"

class Registry
{
public:
    static Registry& GetInstance()
    {
        static Registry instance;
        return instance;
    }
    void Initialize();

    void RegisterStack(const std::string name, SubSolverStack stack);
    void RegisterStacksFromJsObject(const JsObject& jsonDict);
    void RegisterSchemeDefault(const std::string& scheme, const std::string& stack);
    std::shared_ptr<SubSolverStack> GetStack(std::string name);

    void RegisterSubSolver(const std::shared_ptr<SubSolver> subsolver);
    std::shared_ptr<SubSolver> GetSubSolver(std::string name);

    const std::map<std::string, std::string>& GetSchemeDefaults();

private:
    Registry() {};
    void _Initialize();
    std::atomic<bool> _initialized = false;
    std::map<std::string, std::string> _schemeDefaults;
    std::map<std::string, std::shared_ptr<SubSolverStack>> _stackRegistry;
    std::map<std::string, std::shared_ptr<SubSolver>> _subsolverRegistry;
};

