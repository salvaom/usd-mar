#pragma once
#include <iostream>
#include <map>

#include "usdmar/solverStack.h"

class SubsolverRegistry
{
public:

    void RegisterStack(const std::string name, SubSolverStack stack);

    static SubsolverRegistry& GetInstance()
    {
        static SubsolverRegistry instance;
        return instance;
    }

    void RegisterFromJsObject(const JsObject& jsonDict);

    std::shared_ptr<SubSolverStack> GetStack(std::string name);

private:
    SubsolverRegistry() {}
    std::map<std::string, std::shared_ptr<SubSolverStack>> _registry;
};

