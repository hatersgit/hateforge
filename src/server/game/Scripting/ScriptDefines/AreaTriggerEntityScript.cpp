#include "AreaTriggerEntityScript.h"
#include "ScriptMgr.h"
#include "ScriptMgrMacros.h"

AreaTriggerEntityScript::AreaTriggerEntityScript(char const* name) :
    ScriptObject(name)
{
    ScriptRegistry<AreaTriggerEntityScript>::AddScript(this);
}

AreaTriggerEntityScript::~AreaTriggerEntityScript() = default;

AreaTriggerAI* ScriptMgr::GetAreaTriggerAI(AreaTrigger* areatrigger)
{
    ASSERT(areatrigger);

    auto retAI = GetReturnAIScript<AreaTriggerEntityScript, AreaTriggerAI>([areatrigger](AreaTriggerEntityScript* script)
        {
            return script->GetAI(areatrigger);
        });

    if (retAI)
    {
        return retAI;
    }

    auto tempScript = ScriptRegistry<AreaTriggerEntityScript>::GetScriptById(areatrigger->GetScriptId());
    return tempScript ? tempScript->GetAI(areatrigger) : nullptr;
}

bool ScriptMgr::CanCreateAreaTriggerAI(uint32 scriptId) const
{
    return !!ScriptRegistry<AreaTriggerEntityScript>::GetScriptById(scriptId);
}

template class AC_GAME_API ScriptRegistry<AreaTriggerEntityScript>;
