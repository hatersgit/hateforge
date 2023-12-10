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

    auto tempScript = ScriptRegistry<AreaTriggerEntityScript>::GetScriptById(areatrigger->GetScriptId());
    return tempScript ? tempScript->GetAI(areatrigger) : nullptr;
}

template class AC_GAME_API ScriptRegistry<AreaTriggerEntityScript>;
