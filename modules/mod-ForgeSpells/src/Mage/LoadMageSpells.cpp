#include "LoadForgeSpells.cpp"
#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

#include "AreaTriggerEntityScript.h"
#include "ScriptMgr.h"
#include "AreaTriggerAI.h"

enum MageSpells
{
    GA_CAN_FREEZE_AURA = 110302,
    CHILLED_GA_FREEZE = 100137,
    CHILLED_AURA = 100138,
    BURN_AURA = 100141,

    ELE_PULSE_FIRE = 100132,
    ELE_PULSE_FROST = 100130,
    ELE_PULSE_ARCANE = 100131,
    ELE_PULSE_FROST_AURA = 100139,
    ELE_PULSE_ARCANE_AURA = 100140,
    ELE_PULSE_MANA_INC = 110293,

};

class ai_mage_glacial_advance : public CreatureScript
{
public:
    ai_mage_glacial_advance() : CreatureScript("ai_mage_glacial_advance") {
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new glacialAdvanceAI(creature);
    }

    struct glacialAdvanceAI : public CreatureAI
    {
        glacialAdvanceAI(Creature* creature) : CreatureAI(creature), startedMovement(false) {}

        void UpdateAI(uint32 diff) override {
            if (!startedMovement)
                if (auto owner = me->GetCharmerOrOwner()) {
                    me->CastSpell(me, me->GetCreatureTemplate()->spells[0], true);
                    startedMovement = true;
                    me->GetMotionMaster()->MoveForwardsFromPosition(50.f, owner);
                }
        }

        bool UpdateVictim() {
            return false;
        }

        bool UpdateVictimWithGaze() {
            return false;
        }

        bool startedMovement = false;
    };

private:
    CreatureAI* _ai;
};

// 100136 - glacial assault damage
class spell_mage_glacial_advance : public SpellScript
{
    PrepareSpellScript(spell_mage_glacial_advance);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* unitTarget = GetHitUnit())
            if (auto chill = unitTarget->GetAura(CHILLED_AURA))
                if (chill->GetStackAmount() > 2 && GetCaster()->HasAura(GA_CAN_FREEZE_AURA))
                    GetCaster()->CastSpell(unitTarget, CHILLED_GA_FREEZE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_glacial_advance::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

class aura_mage_glacial_advance : public AuraScript
{
    PrepareAuraScript(aura_mage_glacial_advance);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (Unit* target = GetTarget())
            if (Unit* owner = target->GetOwner()) {
                auto position = target->GetPosition();
                owner->CastSpell(position.GetPositionX(), position.GetPositionY(), position.GetPositionZ(), 100136, true);
            }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_mage_glacial_advance::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 100133 - elemental pulse
class spell_mage_elemental_pulse : public SpellScript
{
    PrepareSpellScript(spell_mage_elemental_pulse);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        bool frost, fire = false;
        if (Unit* caster = GetCaster()) {
            if (caster->HasAura(ELE_PULSE_ARCANE_AURA)) {
                caster->CastSpell(caster, ELE_PULSE_ARCANE, true);
                caster->RemoveAura(ELE_PULSE_ARCANE_AURA);
            }
            else if (caster->HasAura(ELE_PULSE_FROST_AURA)) {
                caster->CastSpell(caster, ELE_PULSE_FROST, true);
                caster->CastSpell(caster, ELE_PULSE_ARCANE_AURA, true);
                caster->RemoveAura(ELE_PULSE_FROST_AURA);
            }
            else {
                caster->CastSpell(caster, ELE_PULSE_FIRE, true);
                caster->CastSpell(caster, ELE_PULSE_FROST_AURA, true);
            }

            if (caster->HasAura(110294))
                caster->CastSpell(caster, ELE_PULSE_MANA_INC, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_elemental_pulse::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class LoadMageSpells : LoadForgeSpells
{
public:
    LoadMageSpells() : LoadForgeSpells()
    {

    }

    void Load() override
    {
        new ai_mage_glacial_advance();
        RegisterSpellScript(spell_mage_glacial_advance);
        RegisterSpellScript(spell_mage_elemental_pulse);
        RegisterSpellScript(aura_mage_glacial_advance);
    }
};
