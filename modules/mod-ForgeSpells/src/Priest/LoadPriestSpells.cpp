#include "LoadForgeSpells.cpp"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum CustomPriestSpells
{
    // Ours
    SPELL_PRIEST_ATONEMENT_AURA = 110247,
    SPELL_PRIEST_ATONEMENT_HEAL = 110248,
};

// Atonement - 110248
class spell_pri_atonement : public AuraScript
{
    PrepareAuraScript(spell_pri_atonement);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ATONEMENT_AURA, SPELL_PRIEST_ATONEMENT_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamageInfo())
            return false;
        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        uint32 heal = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount());
        if (heal)
            caster->CastCustomSpell(SPELL_PRIEST_ATONEMENT_HEAL, SPELLVALUE_BASE_POINT0, heal, caster, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_atonement::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_atonement::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

class LoadPriestSpells : LoadForgeSpells
{
public:
    LoadPriestSpells() : LoadForgeSpells()
    {

    }

    void Load() override
    {
        RegisterSpellScript(spell_pri_atonement);
    }
};
