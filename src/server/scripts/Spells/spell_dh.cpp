#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "AreaTriggerTemplate.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "ScriptMgr.h"

enum DemonHunterSpells
{
    SPELL_DH_DESPAIR_DEBUFF                 = 1410014,
    SPELL_DH_DESPAIR_TALENT                 = 1410013,
    SPELL_DH_SIGIL_OF_FLAME_EXPLOSION       = 1410004,
    SPELL_DH_SIGIL_OF_FLAME_ENERGIZE        = 1410005,
    SPELL_DH_SIGIL_OF_MISERY_EXPLOSION      = 1410007,

};

// 1410003 - Sigil of Flame
struct at_dh_sigil_of_flame : AreaTriggerAI
{
    at_dh_sigil_of_flame(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnRemove() override
    {
        Unit* caster = at->GetCaster();

        if (!caster)
            return;

        caster->CastSpell(at->GetPositionX(), at->GetPositionY(), at->GetPositionZ(), SPELL_DH_SIGIL_OF_FLAME_EXPLOSION, true);
        caster->CastSpell(caster, SPELL_DH_SIGIL_OF_FLAME_ENERGIZE, true);
    }
};

// 1410006 - Sigil of Misery
struct at_dh_sigil_of_misery : AreaTriggerAI
{
    at_dh_sigil_of_misery(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnRemove() override
    {
        Unit* caster = at->GetCaster();

        if (!caster)
            return;

        caster->CastSpell(at->GetPositionX(), at->GetPositionY(), at->GetPositionZ(), SPELL_DH_SIGIL_OF_MISERY_EXPLOSION, true);

        if (caster->HasAura(SPELL_DH_DESPAIR_TALENT))
            caster->CastSpell(at->GetPositionX(), at->GetPositionY(), at->GetPositionZ(), SPELL_DH_DESPAIR_DEBUFF, true);
    }
};

void AddSC_demonhunter_spell_scripts()
{
    RegisterAreaTriggerAI(at_dh_sigil_of_flame);
    RegisterAreaTriggerAI(at_dh_sigil_of_misery);
}
