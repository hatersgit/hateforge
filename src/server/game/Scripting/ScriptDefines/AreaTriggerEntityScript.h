#pragma once

#include "AreaTriggerAI.h"
#include "ScriptObject.h"

class AreaTriggerEntityScript : public ScriptObject, UpdatableScript<AreaTrigger>
{
protected:
    AreaTriggerEntityScript(const char* name);

public:
    ~AreaTriggerEntityScript();
    virtual AreaTriggerAI* GetAI(AreaTrigger* at) const { return nullptr; }
};

template <class AI>
class GenericAreaTriggerEntityScript : public AreaTriggerEntityScript
{
public:
    GenericAreaTriggerEntityScript(char const* name) : AreaTriggerEntityScript(name) { }
    AreaTriggerAI* GetAI(AreaTrigger* me) const override { return new AI(me); }
};

#define RegisterAreaTriggerAI(ai_name) new GenericAreaTriggerEntityScript<ai_name>(#ai_name)

template <class AI, AI* (*AIFactory)(Creature*)>
class FactoryAreaTriggerEntityScript : public AreaTriggerEntityScript
{
public:
    FactoryAreaTriggerEntityScript(char const* name) : AreaTriggerEntityScript(name) { }
    AreaTriggerAI* GetAI(AreaTrigger* me) const override { return AIFactory(me); }
};

#define RegisterAreaTriggerAIWithFactory(ai_name, factory_fn) new FactoryAreaTriggerEntityScript<ai_name, &factory_fn>(#ai_name)
