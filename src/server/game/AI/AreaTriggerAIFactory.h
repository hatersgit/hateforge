#ifndef ACORE_GAMEOBJECTAIFACTORY_H
#define ACORE_GAMEOBJECTAIFACTORY_H

#include "FactoryHolder.h"
#include "AreaTriggerAI.h"
#include "ObjectRegistry.h"

typedef FactoryHolder<AreaTriggerAI, AreaTrigger> AreaTriggerAICreator;

struct SelectableAreaTriggerAI : public AreaTriggerAICreator, public Permissible<AreaTrigger>
{
    SelectableAreaTriggerAI(std::string const& name) : AreaTriggerAICreator(name), Permissible<AreaTrigger>() { }
};

template<class REAL_AT_AI>
struct AreaTriggerAIFactory : public SelectableAreaTriggerAI
{
    AreaTriggerAIFactory(std::string const& name) : SelectableAreaTriggerAI(name) { }

    AreaTriggerAI* Create(AreaTrigger* go) const override
    {
        return new REAL_AT_AI(go);
    }

    int32 Permit(GameObject const* go) const override
    {
        return REAL_AT_AI::Permissible(go);
    }
};

typedef AreaTriggerAICreator::FactoryHolderRegistry AreaTriggerAIRegistry;

#define sAreaTriggerAIRegistry AreaTriggerAIRegistry::instance()

#endif
