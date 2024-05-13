/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CreatureScript.h"
#include "GridNotifiers.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
#include "Unit.h"
/*
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 */

enum ShamanSpells
{
    SPELL_SHAMAN_ANCESTRAL_AWAKENING_DUMMY = 52759,
    SPELL_SHAMAN_GLYPH_OF_FERAL_SPIRIT          = 63271,
    SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC       = 52752,
    SPELL_SHAMAN_BIND_SIGHT                     = 6277,
    SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT         = 52025,
    SPELL_SHAMAN_EARTH_SHIELD_HEAL              = 379,
    SPELL_SHAMAN_ELEMENTAL_MASTERY              = 16166,
    SPELL_SHAMAN_ELECTRIFIED                    = 64930,
    SPELL_SHAMAN_EXHAUSTION                     = 57723,
    SPELL_SHAMAN_FIRE_NOVA_R1                   = 1535,
    SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1         = 8349,
    SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD          = 63279,
    SPELL_SHAMAN_GLYPH_OF_HEALING_STREAM_TOTEM  = 55456,
    SPELL_SHAMAN_GLYPH_OF_MANA_TIDE             = 55441,
    SPELL_SHAMAN_GLYPH_OF_THUNDERSTORM          = 62132,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD          = 23552,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE   = 27635,
    SPELL_SHAMAN_ITEM_MANA_SURGE                = 23571,
    SPELL_SHAMAN_LAVA_FLOWS_R1                  = 51480,
    SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1        = 64694,
    SPELL_SHAMAN_LIGHTNING_SHIELD_R1 = 26364,
    SPELL_SHAMAN_MANA_SPRING_TOTEM_ENERGIZE     = 52032,
    SPELL_SHAMAN_MANA_TIDE_TOTEM                = 39609,
    SPELL_SHAMAN_NATURE_GUARDIAN = 31616,
    SPELL_SHAMAN_SATED                          = 57724,
    SPELL_SHAMAN_STORM_EARTH_AND_FIRE           = 51483,
    SPELL_SHAMAN_TOTEM_EARTHBIND_EARTHGRAB      = 64695,
    SPELL_SHAMAN_TOTEM_EARTHBIND_TOTEM          = 6474,
    SPELL_SHAMAN_TOTEM_EARTHEN_POWER            = 59566,
    SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL      = 52042,
    SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1    = 51554,
    SPELL_SHAMAN_STORMSTRIKE                    = 17364,
    SPELL_SHAMAN_LAVA_LASH                      = 60103,
    SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD        = 45284,
    SPELL_SHAMAN_TOTEMIC_MASTERY = 38437,
    SPELL_SHAMAN_TIDAL_FORCE_CRIT = 55166,
    SPELL_SHAMAN_TOTEMIC_POWER_MP5 = 28824,
    SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER = 28825,
    SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER = 28826,
    SPELL_SHAMAN_TOTEMIC_POWER_ARMOR = 28827,
    SPELL_SHAMAN_WINDFURY_WEAPON_R1 = 8232,
    SPELL_SHAMAN_WINDFURY_ATTACK_MH = 25504,
    SPELL_SHAMAN_WINDFURY_ATTACK_OH = 33750,
    SPELL_SHAMAN_ENERGY_SURGE = 40465,
    SPELL_SHAMAN_POWER_SURGE = 40466,
    SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE_HEAL = 55533,
    SPELL_SHAMAN_SPIRIT_HUNT_HEAL = 58879,
    SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE = 71824,
    SPELL_SHAMAN_CHAINED_HEAL = 70809,
    SPELL_SHAMAN_TOTEM_OF_WRATH_SPELL_POWER = 63283,
    SPELL_SHAMAN_FREEZE = 63685,
    SPELL_SHAMAN_FLAMETONGUE_ATTACK = 10444,
    SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD_R1 = 45284,
    SPELL_SHAMAN_CHAIN_LIGHTNING_OVERLOAD_R1 = 45297,
    SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE_R1 = 26364,
    SPELL_SHAMAN_SHAMANISTIC_RAGE_PROC = 30824,
    SPELL_SHAMAN_MAELSTROM_POWER = 70831,
    SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS = 70832,

    SPELL_SHAMAN_FLOW_OF_THE_TIDES              = 999999999, // TODO: use real id

    SPELL_SEARING_BOLT = 3606, // hater updated searing totem spell
};

enum ShamanSpellIcons
{
    SHAMAN_ICON_ID_RESTORATIVE_TOTEMS           = 338,
    SHAMAN_ICON_ID_SHAMAN_LAVA_FLOW = 3087,
    SHAMAN_ICON_ID_TOTEM_OF_WRATH = 2019
};

// -51556 - Ancestral Awakening
class spell_sha_ancestral_awakening : public AuraScript
{
    PrepareAuraScript(spell_sha_ancestral_awakening);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ANCESTRAL_AWAKENING_DUMMY });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        int32 amount = CalculatePct(static_cast<int32>(healInfo->GetHeal()), aurEff->GetAmount());
        eventInfo.GetActor()->CastCustomSpell(SPELL_SHAMAN_ANCESTRAL_AWAKENING_DUMMY, SPELLVALUE_BASE_POINT0, amount, (Unit*)nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_awakening::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

enum SpellHelpers
{
    SPELL_MAGE_TEMPORAL_DISPLACEMENT            = 1280002
};

class spell_sha_totem_of_wrath : public SpellScript
{
    PrepareSpellScript(spell_sha_totem_of_wrath);

    void HandleAfterCast()
    {
        if (AuraEffect* aurEff = GetCaster()->GetAuraEffect(63280, EFFECT_0))
            if (Creature* totem = GetCaster()->GetMap()->GetCreature(GetCaster()->m_SummonSlot[1]))   // Fire totem summon slot
                if (SpellInfo const* totemSpell = sSpellMgr->GetSpellInfo(totem->m_spells[0]))
                {
                    int32 bp0 = CalculatePct(totemSpell->Effects[EFFECT_0].CalcValue(), aurEff->GetAmount());
                    int32 bp1 = CalculatePct(totemSpell->Effects[EFFECT_1].CalcValue(), aurEff->GetAmount());
                    GetCaster()->CastCustomSpell(GetCaster(), 63283, &bp0, &bp1, nullptr, true, nullptr, aurEff);
                }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sha_totem_of_wrath::HandleAfterCast);
    }
};

// 3606
class spell_sha_totem_searing_bolt : public SpellScript
{
    PrepareSpellScript(spell_sha_totem_searing_bolt);

    void OnHit(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->IsTotem()) {
            auto owner = GetCaster()->GetOwner();
            if (auto effect = owner->GetDummyAuraEffect(SPELLFAMILY_SUMMON, 1812, EFFECT_0)) {
                auto amount = effect->GetAmount();
                owner->CastCustomSpell(GetExplTargetUnit(), 200002, &amount, nullptr, nullptr, true, 0, effect);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_totem_searing_bolt::OnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

class spell_sha_spirit_walk : public SpellScript
{
    PrepareSpellScript(spell_sha_spirit_walk);

    SpellCastResult CheckCast()
    {
        if (Unit* owner = GetCaster()->GetOwner())
            if (GetCaster()->IsWithinDist(owner, GetSpellInfo()->GetMaxRange(GetSpellInfo()->IsPositive())))
                return SPELL_CAST_OK;

        return SPELL_FAILED_OUT_OF_RANGE;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_spirit_walk::CheckCast);
    }
};

class spell_sha_t10_restoration_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t10_restoration_4p_bonus);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor() && eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 triggered_spell_id = 70809;
        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);

        HealInfo* healInfo = eventInfo.GetHealInfo();

        if (!healInfo || !triggeredSpell)
        {
            return;
        }

        int32 amount = CalculatePct(healInfo->GetHeal(), aurEff->GetAmount()) / triggeredSpell->GetMaxTicks();

        eventInfo.GetProcTarget()->CastDelayedSpellWithPeriodicAmount(GetTarget(), triggered_spell_id, SPELL_AURA_PERIODIC_HEAL, amount, EFFECT_0);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_t10_restoration_4p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_t10_restoration_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 38443 - Totemic Mastery (Tier 6 - 2P)
class spell_sha_totemic_mastery : public AuraScript
{
    PrepareAuraScript(spell_sha_totemic_mastery);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEMIC_MASTERY });
    }

    void HandleDummy(AuraEffect const* aurEff)
    {
        Unit* target = GetTarget();
        for (uint8 i = SUMMON_TYPE_TOTEM_FIRE; i < MAX_TOTEM_SLOT; ++i)
            if (!target->m_SummonSlot[i])
                return;

        target->CastSpell(target, SPELL_SHAMAN_TOTEMIC_MASTERY, aurEff);
        PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_totemic_mastery::HandleDummy, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

class spell_sha_feral_spirit_scaling : public AuraScript
{
    PrepareAuraScript(spell_sha_feral_spirit_scaling);

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: feral spirit inherits 40% of resistance from owner and 35% of armor
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            SpellSchoolMask schoolMask = SpellSchoolMask(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            int32 modifier = schoolMask == SPELL_SCHOOL_MASK_NORMAL ? 35 : 40;
            amount = CalculatePct(std::max<int32>(0, owner->GetResistance(schoolMask)), modifier);
        }
    }

    void CalculateStatAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: by default feral spirit inherits 30% of stamina
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            Stats stat = Stats(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            amount = CalculatePct(std::max<int32>(0, owner->GetStat(stat)), 30);
        }
    }

    void CalculateAPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: by default feral spirit inherits 30% of AP
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 modifier = 30;
            if (AuraEffect const* gofsEff = owner->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_FERAL_SPIRIT, EFFECT_0))
                modifier += gofsEff->GetAmount();

            amount = CalculatePct(std::max<int32>(0, owner->GetTotalAttackPowerValue(BASE_ATTACK)), modifier);
        }
    }

    void CalculateSPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: by default feral spirit inherits 30% of AP as SP
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 modifier = 30;
            if (AuraEffect const* gofsEff = owner->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_FERAL_SPIRIT, EFFECT_0))
                modifier += gofsEff->GetAmount();

            amount = CalculatePct(std::max<int32>(0, owner->GetTotalAttackPowerValue(BASE_ATTACK)), modifier);

            // xinef: Update appropriate player field
            if (owner->GetTypeId() == TYPEID_PLAYER)
                owner->SetUInt32Value(PLAYER_PET_SPELL_POWER, (uint32)amount);
        }
    }

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, aurEff->GetAuraType(), true, SPELL_BLOCK_TYPE_POSITIVE);
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_ATTACK_POWER)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ATTACK_POWER_PCT, true, SPELL_BLOCK_TYPE_POSITIVE);
        else if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, true, SPELL_BLOCK_TYPE_POSITIVE);
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 1 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT && (aurEff->GetMiscValue() == STAT_STAMINA || aurEff->GetMiscValue() == STAT_INTELLECT))
        {
            int32 currentAmount = aurEff->GetAmount();
            int32 newAmount = GetEffect(aurEff->GetEffIndex())->CalculateAmount(GetCaster());
            if (newAmount != currentAmount)
            {
                if (aurEff->GetMiscValue() == STAT_STAMINA)
                {
                    uint32 actStat = GetUnitOwner()->GetHealth();
                    GetEffect(aurEff->GetEffIndex())->ChangeAmount(newAmount, false);
                    GetUnitOwner()->SetHealth(std::min<uint32>(GetUnitOwner()->GetMaxHealth(), actStat));
                }
                else
                {
                    uint32 actStat = GetUnitOwner()->GetPower(POWER_MANA);
                    GetEffect(aurEff->GetEffIndex())->ChangeAmount(newAmount, false);
                    GetUnitOwner()->SetPower(POWER_MANA, std::min<uint32>(GetUnitOwner()->GetMaxPower(POWER_MANA), actStat));
                }
            }
        }
        else
            GetEffect(aurEff->GetEffIndex())->RecalculateAmount();
    }

    void Register() override
    {
        if (m_scriptSpellId == 35675)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_feral_spirit_scaling::CalculateResistanceAmount, EFFECT_ALL, SPELL_AURA_MOD_RESISTANCE);

        if (m_scriptSpellId == 35674)
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_feral_spirit_scaling::CalculateStatAmount, EFFECT_ALL, SPELL_AURA_MOD_STAT);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_feral_spirit_scaling::CalculateAPAmount, EFFECT_ALL, SPELL_AURA_MOD_ATTACK_POWER);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_feral_spirit_scaling::CalculateSPAmount, EFFECT_ALL, SPELL_AURA_MOD_DAMAGE_DONE);
        }

        OnEffectApply += AuraEffectApplyFn(spell_sha_feral_spirit_scaling::HandleEffectApply, EFFECT_ALL, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_sha_feral_spirit_scaling::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_feral_spirit_scaling::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

class spell_sha_fire_elemental_scaling : public AuraScript
{
    PrepareAuraScript(spell_sha_fire_elemental_scaling);

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: fire elemental inherits 40% of resistance from owner and 35% of armor
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            SpellSchoolMask schoolMask = SpellSchoolMask(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            int32 modifier = schoolMask == SPELL_SCHOOL_MASK_NORMAL ? 35 : 40;
            amount = CalculatePct(std::max<int32>(0, owner->GetResistance(schoolMask)), modifier);
        }
    }

    void CalculateStatAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: fire elemental inherits 30% of intellect / stamina
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            Stats stat = Stats(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            amount = CalculatePct(std::max<int32>(0, owner->GetStat(stat)), 30);
        }
    }

    void CalculateAPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: fire elemental inherits 300% / 150% of SP as AP
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 fire = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE);
            amount = CalculatePct(std::max<int32>(0, fire), (GetUnitOwner()->GetEntry() == NPC_FIRE_ELEMENTAL ? 300 : 150));
        }
    }

    void CalculateSPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: fire elemental inherits 100% of SP
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 fire = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE);
            amount = CalculatePct(std::max<int32>(0, fire), 100);

            // xinef: Update appropriate player field
            if (owner->GetTypeId() == TYPEID_PLAYER)
                owner->SetUInt32Value(PLAYER_PET_SPELL_POWER, (uint32)amount);
        }
    }

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, aurEff->GetAuraType(), true, SPELL_BLOCK_TYPE_POSITIVE);
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_ATTACK_POWER)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ATTACK_POWER_PCT, true, SPELL_BLOCK_TYPE_POSITIVE);
        else if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, true, SPELL_BLOCK_TYPE_POSITIVE);
    }

    void Register() override
    {
        if (m_scriptSpellId != 35665 && m_scriptSpellId != 65225)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_fire_elemental_scaling::CalculateResistanceAmount, EFFECT_ALL, SPELL_AURA_MOD_RESISTANCE);

        if (m_scriptSpellId == 35666 || m_scriptSpellId == 65226)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_fire_elemental_scaling::CalculateStatAmount, EFFECT_ALL, SPELL_AURA_MOD_STAT);

        if (m_scriptSpellId == 35665 || m_scriptSpellId == 65225)
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_fire_elemental_scaling::CalculateAPAmount, EFFECT_ALL, SPELL_AURA_MOD_ATTACK_POWER);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_fire_elemental_scaling::CalculateSPAmount, EFFECT_ALL, SPELL_AURA_MOD_DAMAGE_DONE);
        }

        OnEffectApply += AuraEffectApplyFn(spell_sha_fire_elemental_scaling::HandleEffectApply, EFFECT_ALL, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 52759 - Ancestral Awakening (Proc)
class spell_sha_ancestral_awakening_proc : public SpellScript
{
    PrepareSpellScript(spell_sha_ancestral_awakening_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() < 2)
            return;

        targets.sort(Acore::HealthPctOrderPred());

        WorldObject* target = targets.front();
        targets.clear();
        targets.push_back(target);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 damage = GetEffectValue();
        if (GetHitUnit())
            GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC, &damage, nullptr, nullptr, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_ancestral_awakening_proc::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnEffectHitTarget += SpellEffectFn(spell_sha_ancestral_awakening_proc::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 51474 - Astral Shift
class spell_sha_astral_shift : public AuraScript
{
    PrepareAuraScript(spell_sha_astral_shift);

    uint32 absorbPct;

    bool Load() override
    {
        absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
        return true;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // Set absorbtion amount to unlimited
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        // reduces all damage taken while stun, fear or silence
        if (GetTarget()->GetUnitFlags() & (UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED) || (GetTarget()->GetUnitFlags() & (UNIT_FLAG_STUNNED) && GetTarget()->HasAuraWithMechanic(1 << MECHANIC_STUN)))
            absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_astral_shift::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_astral_shift::Absorb, EFFECT_0);
    }
};

// 2825 - Bloodlust
class spell_sha_bloodlust : public SpellScript
{
    PrepareSpellScript(spell_sha_bloodlust);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SATED, SPELL_SHAMAN_EXHAUSTION, SPELL_MAGE_TEMPORAL_DISPLACEMENT });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_SATED));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_EXHAUSTION));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_MAGE_TEMPORAL_DISPLACEMENT));
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SHAMAN_SATED, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_bloodlust::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        AfterHit += SpellHitFn(spell_sha_bloodlust::ApplyDebuff);
    }
};

// -1064 - Chain Heal
class spell_sha_chain_heal : public SpellScript
{
    PrepareSpellScript(spell_sha_chain_heal);

    bool Load() override
    {
        firstHeal = true;
        riptide = false;
        return true;
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->HasSpell(SPELL_SHAMAN_FLOW_OF_THE_TIDES)) {
            if (firstHeal)
            {
                // Check if the target has Riptide
                if (AuraEffect* aurEff = GetHitUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_SHAMAN, 0, 0, 0x10, GetCaster()->GetGUID()))
                {
                    riptide = true;
                    // Consume it
                    GetHitUnit()->RemoveAura(aurEff->GetBase());
                }
                firstHeal = false;
            }
            // Riptide increases the Chain Heal effect by 25%
            if (riptide)
                SetHitHeal(GetHitHeal() * 1.25f);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_chain_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }

    bool firstHeal;
    bool riptide;
};

// 8171 - Cleansing Totem (Pulse)
class spell_sha_cleansing_totem_pulse : public SpellScript
{
    PrepareSpellScript(spell_sha_cleansing_totem_pulse);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 bp = 1;
        if (GetCaster() && GetHitUnit() && GetOriginalCaster())
            GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT, nullptr, &bp, nullptr, true, nullptr, nullptr, GetOriginalCaster()->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_cleansing_totem_pulse::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -974 - Earth Shield
class spell_sha_earth_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_earth_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_EARTH_SHIELD_HEAL, SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD });
    }

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
        {
            int32 baseAmount = amount;
            amount = caster->SpellHealingBonusDone(GetUnitOwner(), GetSpellInfo(), amount, HEAL, aurEff->GetEffIndex());
            // xinef: taken should be calculated at every heal
            //amount = GetUnitOwner()->SpellHealingBonusTaken(caster, GetSpellInfo(), amount, HEAL);

            // Glyph of Earth Shield
            //! WORKAROUND
            //! this glyphe is a proc
            if (AuraEffect* glyphe = caster->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD, EFFECT_0))
                AddPct(amount, glyphe->GetAmount());

            // xinef: Improved Shields
            if ((baseAmount = amount - baseAmount))
                if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_PCT_MODIFIER, SPELLFAMILY_SHAMAN, 19, EFFECT_1))
                {
                    ApplyPct(baseAmount, aurEff->GetAmount());
                    amount += baseAmount;
                }
        }
    }

    bool CheckProc(ProcEventInfo&  /*eventInfo*/)
    {
        return !GetTarget()->HasSpellCooldown(SPELL_SHAMAN_EARTH_SHIELD_HEAL);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo&  /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastCustomSpell(SPELL_SHAMAN_EARTH_SHIELD_HEAL, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, nullptr, aurEff, GetCasterGUID());
        GetTarget()->AddSpellCooldown(SPELL_SHAMAN_EARTH_SHIELD_HEAL, 0, 3500);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_earth_shield::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
        DoCheckProc += AuraCheckProcFn(spell_sha_earth_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_earth_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 6474 - Earthbind Totem - Fix Talent: Earthen Power
class spell_sha_earthbind_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_earthbind_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEM_EARTHBIND_TOTEM, SPELL_SHAMAN_TOTEM_EARTHEN_POWER });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetCaster())
            return;
        if (Player* owner = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself())
            if (AuraEffect* aur = owner->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, 2289, 0))
                if (roll_chance_i(aur->GetBaseAmount()))
                    GetTarget()->CastSpell((Unit*)nullptr, SPELL_SHAMAN_TOTEM_EARTHEN_POWER, true);
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;
        Player* owner = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner)
            return;
        // Storm, Earth and Fire
        if (AuraEffect* aurEff = owner->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_STORM_EARTH_AND_FIRE, EFFECT_1))
        {
            if (roll_chance_i(aurEff->GetAmount()))
                GetCaster()->CastSpell(GetCaster(), SPELL_SHAMAN_TOTEM_EARTHBIND_EARTHGRAB, false);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthbind_totem::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectApply += AuraEffectApplyFn(spell_sha_earthbind_totem::Apply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

class EarthenPowerTargetSelector
{
public:
    EarthenPowerTargetSelector() { }

    bool operator() (WorldObject* target)
    {
        if (!target->ToUnit())
            return true;

        if (!target->ToUnit()->HasAuraWithMechanic(1 << MECHANIC_SNARE))
            return true;

        return false;
    }
};

// 59566 - Earthen Power
class spell_sha_earthen_power : public SpellScript
{
    PrepareSpellScript(spell_sha_earthen_power);

    void FilterTargets(std::list<WorldObject*>& unitList)
    {
        unitList.remove_if(EarthenPowerTargetSelector());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_earthen_power::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// -51940 - Earthliving Weapon (Passive)
class spell_sha_earthliving_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_earthliving_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1 });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        auto chance = 20;
        Unit* caster = eventInfo.GetActor();
        if (!caster || !eventInfo.GetProcTarget())
        {
            return false;
        }

        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            if (spellInfo->Id == SPELL_SHAMAN_EARTH_SHIELD_HEAL)
            {
                return false;
            }
        }

        if (AuraEffect const* aurEff = caster->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1, EFFECT_1, caster->GetGUID()))
        {
            if (eventInfo.GetProcTarget()->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
            {
                chance += aurEff->GetAmount();
            }
        }

        return roll_chance_i(chance);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_earthliving_weapon::CheckProc);
    }
};

// -1535 - Fire Nova
class spell_sha_fire_nova : public SpellScript
{
    PrepareSpellScript(spell_sha_fire_nova);

    bool Validate(SpellInfo const* spellInfo) override
    {
        SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_FIRE_NOVA_R1);
        if (!firstRankSpellInfo || !spellInfo->IsRankOf(firstRankSpellInfo))
            return false;

        uint8 rank = spellInfo->GetRank();
        if (!sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1, rank, true))
            return false;
        return true;
    }

    SpellCastResult CheckFireTotem()
    {
        // fire totem
        Unit* caster = GetCaster();
        if (Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]))
        {
            if (!caster->IsWithinDistInMap(totem, caster->GetSpellMaxRangeForTarget(totem, GetSpellInfo())))
                return SPELL_FAILED_OUT_OF_RANGE;
            return SPELL_CAST_OK;
        }
        else
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_FIRE_TOTEM);
            return SPELL_FAILED_CUSTOM_ERROR;
        }
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]))
        {
            uint8 rank = GetSpellInfo()->GetRank();
            if (totem->IsTotem())
                caster->CastSpell(totem, 61655, true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_fire_nova::CheckFireTotem);
        OnEffectHitTarget += SpellEffectFn(spell_sha_fire_nova::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -8050 - Flame Shock
class spell_sha_flame_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_flame_shock);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_FLOWS_R1, SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1 });
    }

    void HandleDispel(DispelInfo* /*dispelInfo*/)
    {
        if (Unit* caster = GetCaster())
            // Lava Flows
            if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, SHAMAN_ICON_ID_SHAMAN_LAVA_FLOW, EFFECT_0))
            {
                if (SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_LAVA_FLOWS_R1))
                    if (!aurEff->GetSpellInfo()->IsRankOf(firstRankSpellInfo))
                        return;

                uint8 rank = aurEff->GetSpellInfo()->GetRank();
                caster->CastSpell(caster, sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1, rank), true);
            }
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_sha_flame_shock::HandleDispel);
    }
};

// -10400 - Flametongue Weapon (Passive)
class spell_sha_flametongue_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_flametongue_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FLAMETONGUE_ATTACK });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player)
            return false;

        Item* item = player->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!item || !item->IsEquipped())
            return false;

        WeaponAttackType attType = static_cast<WeaponAttackType>(player->GetAttackBySlot(item->GetSlot()));
        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();
        Unit* target = eventInfo.GetProcTarget();
        WeaponAttackType attType = BASE_ATTACK;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            attType = OFF_ATTACK;

        Item* item = ASSERT_NOTNULL(player->GetWeaponForAttack(attType));

        float const basePoints = GetSpellInfo()->Effects[aurEff->GetEffIndex()].CalcValue();

        // Flametongue max damage is normalized based on a 4.0 speed weapon
        // Tooltip says max damage = BasePoints / 25, so BasePoints / 25 / 4 to get base damage per 1.0s AS
        float fireDamage = basePoints / 100.0f;
        float const attackSpeed = player->GetAttackTime(attType) / 1000.f;
        fireDamage *= attackSpeed;

        // clip value between (BasePoints / 77) and (BasePoints / 25) as the tooltip indicates
        RoundToInterval(fireDamage, basePoints / 77.0f, basePoints / 25.0f);

        // Calculate Spell Power scaling
        float spellPowerBonus = player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE) + target->SpellBaseDamageBonusTaken(SPELL_SCHOOL_MASK_FIRE);
        float const spCoeff = 0.03811f;
        spellPowerBonus *= spCoeff * attackSpeed;

        // All done, now proc damage
        int32 amount = static_cast<int32>(fireDamage + spellPowerBonus);
        player->CastCustomSpell(SPELL_SHAMAN_FLAMETONGUE_ATTACK, SPELLVALUE_BASE_POINT0, amount, target, true, item, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flametongue_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_flametongue_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -63373 - Frozen Power
class spell_sha_frozen_power : public AuraScript
{
    PrepareAuraScript(spell_sha_frozen_power);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FREEZE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (!roll_chance_i(aurEff->GetAmount()))
            return;

        Unit* caster = eventInfo.GetActor();
        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_FREEZE);
        float minDistance(spellInfo->GetEffect(EFFECT_0).CalcValue(caster));

        Unit* target = eventInfo.GetProcTarget();
        if (caster->GetDistance(target) < minDistance)
            return;

        caster->CastSpell(target, SPELL_SHAMAN_FREEZE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_frozen_power::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 63279 - Glyph of Earth Shield
class spell_sha_glyph_of_earth_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_earth_shield);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* earthShield = eventInfo.GetSpellInfo();
        if (!earthShield)
            return;

        AuraEffect* earthShieldEffect = eventInfo.GetProcTarget()->GetAuraEffect(earthShield->Id, EFFECT_0, eventInfo.GetActor()->GetGUID());
        if (!earthShieldEffect)
            return;

        int32 amount = earthShieldEffect->GetAmount();
        AddPct(amount, aurEff->GetAmount());
        earthShieldEffect->SetAmount(amount);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_earth_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 55440 - Glyph of Healing Wave
class spell_sha_glyph_of_healing_wave : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_healing_wave);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        if (caster == eventInfo.GetProcTarget())
            return;

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        int32 amount = CalculatePct(static_cast<int32>(healInfo->GetHeal()), aurEff->GetAmount());
        caster->CastCustomSpell(SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE_HEAL, SPELLVALUE_BASE_POINT0, amount, (Unit*)nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_healing_wave::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 63280 - Glyph of Totem of Wrath
class spell_sha_glyph_of_totem_of_wrath : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_totem_of_wrath);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEM_OF_WRATH_SPELL_POWER });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Totem of Wrath shares family flags with other totems
        // filter by spellIcon instead
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || spellInfo->SpellIconID != SHAMAN_ICON_ID_TOTEM_OF_WRATH)
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();

        // Fire totem summon slot
        Creature* totem = ObjectAccessor::GetCreature(*caster, caster->m_SummonSlot[1]);
        if (!totem)
            return;

        SpellInfo const* totemSpell = sSpellMgr->GetSpellInfo(totem->m_spells[0]);
        if (!totemSpell)
            return;

        int32 bp0 = CalculatePct(totemSpell->Effects[EFFECT_0].CalcValue(caster), aurEff->GetAmount());
        int32 bp1 = CalculatePct(totemSpell->Effects[EFFECT_1].CalcValue(caster), aurEff->GetAmount());
        caster->CastCustomSpell((Unit*)nullptr, SPELL_SHAMAN_TOTEM_OF_WRATH_SPELL_POWER, &bp0, &bp1, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_glyph_of_totem_of_wrath::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_totem_of_wrath::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 52041, 52046, 52047, 52048, 52049, 52050, 58759, 58760, 58761 - Healing Stream Totem
class spell_sha_healing_stream_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_stream_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_GLYPH_OF_HEALING_STREAM_TOTEM, SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL });
    }

    void HandleDummy(SpellEffIndex effIndex)
    {
        int32 damage = GetEffectValue();
        SpellInfo const* triggeringSpell = GetTriggeringSpell();
        if (Unit* target = GetHitUnit())
        {
            if (Unit* caster = GetCaster())
            {
                if (Unit* owner = caster->GetOwner())
                {
                    if (triggeringSpell)
                        damage = int32(owner->SpellHealingBonusDone(target, triggeringSpell, damage, HEAL, effIndex));
                    // Restorative Totems
                    if (AuraEffect* dummy = owner->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_SHAMAN, SHAMAN_ICON_ID_RESTORATIVE_TOTEMS, 1))
                        AddPct(damage, dummy->GetAmount());

                    // Glyph of Healing Stream Totem
                    if (AuraEffect const* aurEff = owner->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_HEALING_STREAM_TOTEM, EFFECT_0))
                        AddPct(damage, aurEff->GetAmount());

                    damage = int32(target->SpellHealingBonusTaken(owner, triggeringSpell, damage, HEAL));
                }
                caster->CastCustomSpell(target, SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL, &damage, 0, 0, true, 0, 0, GetOriginalCaster()->GetGUID());
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_healing_stream_totem::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 32182 - Heroism
class spell_sha_heroism : public SpellScript
{
    PrepareSpellScript(spell_sha_heroism);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_EXHAUSTION });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_EXHAUSTION));
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SHAMAN_EXHAUSTION, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_heroism::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);

        AfterHit += SpellHitFn(spell_sha_heroism::ApplyDebuff);
    }
};

// -324 - Lightning Shield
class spell_sha_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_lightning_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_SHAMAN_LIGHTNING_SHIELD_R1))
            return false;
        return true;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActionTarget())
            return true;
        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 triggerSpell = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LIGHTNING_SHIELD_R1, aurEff->GetSpellInfo()->GetRank());

        eventInfo.GetActionTarget()->CastSpell(eventInfo.GetActor(), triggerSpell, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_lightning_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 52031, 52033, 52034, 52035, 52036, 58778, 58779, 58780 - Mana Spring Totem
class spell_sha_mana_spring_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_mana_spring_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_MANA_SPRING_TOTEM_ENERGIZE });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 damage = GetEffectValue();
        if (Unit* target = GetHitUnit())
            if (Unit* caster = GetCaster())
                if (target->getPowerType() == POWER_MANA)
                    caster->CastCustomSpell(target, SPELL_SHAMAN_MANA_SPRING_TOTEM_ENERGIZE, &damage, 0, 0, true, 0, 0, GetOriginalCaster()->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_mana_spring_totem::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 39610 - Mana Tide Totem
class spell_sha_mana_tide_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_mana_tide_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_GLYPH_OF_MANA_TIDE, SPELL_SHAMAN_MANA_TIDE_TOTEM });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* unitTarget = GetHitUnit())
            {
                if (unitTarget->getPowerType() == POWER_MANA)
                {
                    int32 effValue = GetEffectValue();
                    // Glyph of Mana Tide
                    if (Unit* owner = caster->GetOwner())
                        if (AuraEffect* dummy = owner->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_MANA_TIDE, 0))
                            effValue += dummy->GetAmount();
                    // Regenerate 6% of Total Mana Every 3 secs
                    int32 effBasePoints0 = int32(CalculatePct(unitTarget->GetMaxPower(POWER_MANA), effValue));
                    caster->CastCustomSpell(unitTarget, SPELL_SHAMAN_MANA_TIDE_TOTEM, &effBasePoints0, nullptr, nullptr, true, nullptr, nullptr, GetOriginalCaster()->GetGUID());
                }
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_mana_tide_totem::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -30881 - Nature's Guardian
class spell_sha_nature_guardian : public AuraScript
{
    PrepareAuraScript(spell_sha_nature_guardian);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_SHAMAN_NATURE_GUARDIAN))
            return false;
        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        int32 healthpct = aurEff->GetSpellInfo()->Effects[EFFECT_1].CalcValue(); // %s2 - the 30% threshold for health

        if (Unit* target = eventInfo.GetActionTarget())
        {
            if (target->HealthBelowPctDamaged(healthpct, eventInfo.GetDamageInfo()->GetDamage()))
            {

                uint32 bp = CalculatePct(target->GetMaxHealth(), aurEff->GetAmount());
                target->CastCustomSpell(SPELL_SHAMAN_NATURE_GUARDIAN, SPELLVALUE_BASE_POINT0, bp, target, true, nullptr, aurEff);

                // @TODO: FIX ME PLEASE
                // Threat reduction is around 10% confirmed in retail and from wiki
//                Unit* attacker = eventInfo.GetActor();
//                if (attacker->IsAlive())
//                    attacker->GetThreatManager().modifyThreatPercent(target, -10);
            }
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_nature_guardian::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 6495 - Sentry Totem
class spell_sha_sentry_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_sentry_totem);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_BIND_SIGHT });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->GetTypeId() == TYPEID_PLAYER)
                caster->ToPlayer()->StopCastingBindSight();
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_sha_sentry_totem::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// -51490 - Thunderstorm
class spell_sha_thunderstorm : public SpellScript
{
    PrepareSpellScript(spell_sha_thunderstorm);

    void HandleKnockBack(SpellEffIndex effIndex)
    {
        // Glyph of Thunderstorm
        if (GetCaster()->HasAura(SPELL_SHAMAN_GLYPH_OF_THUNDERSTORM))
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_thunderstorm::HandleKnockBack, EFFECT_2, SPELL_EFFECT_KNOCK_BACK);
    }
};

// -16257 - SpellName
class spell_sha_flurry_proc : public AuraScript
{
    PrepareAuraScript(spell_sha_flurry_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Should not proc from Windfury Attack, Stormstrike and Lava Lash
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            constexpr std::array<uint32, 2> spellIcons = {SPELL_SHAMAN_STORMSTRIKE, SPELL_SHAMAN_LAVA_LASH};
            const auto found = std::find(std::begin(spellIcons), std::end(spellIcons), spellInfo->Id);

            if ((spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN && (spellInfo->SpellFamilyFlags[0] & 0x00800000) != 0) || found != std::end(spellIcons))
            {
                return false;
            }
        }

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flurry_proc::CheckProc);
    }
};

// 64928 - Item - Shaman T8 Elemental 4P Bonus
class spell_sha_t8_electrified : public AuraScript
{
    PrepareAuraScript(spell_sha_t8_electrified);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELECTRIFIED });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        // Do not proc from Lightning Overload (patch 3.1~)
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            if (spellInfo->Id == SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD)
            {
                return;
            }
        }

        SpellInfo const* electrifiedDot = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_ELECTRIFIED);
        int32 amount = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()) / electrifiedDot->GetMaxTicks());

        eventInfo.GetProcTarget()->CastDelayedSpellWithPeriodicAmount(eventInfo.GetActor(), SPELL_SHAMAN_ELECTRIFIED, SPELL_AURA_PERIODIC_DAMAGE, amount);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t8_electrified::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

class spell_sha_astral_shift_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_astral_shift_aura);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            if (spellInfo->GetAllEffectsMechanicMask() & ((1 << MECHANIC_SILENCE) | (1 << MECHANIC_STUN) | (1 << MECHANIC_FEAR)))
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_astral_shift_aura::CheckProc);
    }
};

// -16180 - Improved Water Shield
class spell_sha_imp_water_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_imp_water_shield);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        // If we're here, we've already passed initial aura roll
        // So just chance based on 100%

        // Default chance for Healing Wave and Riptide
        int32 chance = 100;
        // Lesser Healing Wave - 0.6 of default
        if (spellInfo->SpellFamilyFlags[0] & 0x00000080)
            chance = 60;
        // Chain heal - 0.3 of default
        else if (spellInfo->SpellFamilyFlags[0] & 0x00000100)
            chance = 30;

        if (!roll_chance_i(chance))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        // Get Water Shield
        AuraEffect const* waterShield = caster->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x00000000, 0x00000020, 0x00000000, caster->GetGUID());
        if (!waterShield)
            return;

        uint32 spellId = waterShield->GetSpellInfo()->Effects[waterShield->GetEffIndex()].TriggerSpell;
        caster->CastSpell((Unit*)nullptr, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_imp_water_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_imp_water_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -30675 - Lightning Overload
class spell_sha_lightning_overload : public AuraScript
{
    PrepareAuraScript(spell_sha_lightning_overload);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD_R1,
                SPELL_SHAMAN_CHAIN_LIGHTNING_OVERLOAD_R1
            });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId;

        // Lightning Bolt
        if (spellInfo->SpellFamilyFlags[0] & 0x00000001)
            spellId = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD_R1, spellInfo->GetRank());
        // Chain Lightning
        else
        {
            // Chain lightning has [LightOverload_Proc_Chance] / [Max_Number_of_Targets] chance to proc of each individual target hit.
            // A maxed LO would have a 33% / 3 = 11% chance to proc of each target.
            // LO chance was already "accounted" at the proc chance roll, now need to divide the chance by [Max_Number_of_Targets]
            float chance = 100.0f / spellInfo->GetEffect(EFFECT_0).ChainTarget;
            if (!roll_chance_f(chance))
                return;

            spellId = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_CHAIN_LIGHTNING_OVERLOAD_R1, spellInfo->GetRank());
        }

        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_lightning_overload::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 23551 - Lightning Shield T2 Bonus
class spell_sha_item_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23552 - Lightning Shield T2 Bonus
class spell_sha_item_lightning_shield_trigger : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield_trigger::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23572 - Mana Surge
class spell_sha_item_mana_surge : public AuraScript
{
    PrepareAuraScript(spell_sha_item_mana_surge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_MANA_SURGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        int32 mana = spellInfo->CalcPowerCost(GetTarget(), eventInfo.GetSchoolMask());
        int32 damage = CalculatePct(mana, 35);

        GetTarget()->CastCustomSpell(SPELL_SHAMAN_ITEM_MANA_SURGE, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_mana_surge::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 40463 - Shaman Tier 6 Trinket
class spell_sha_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_sha_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_ENERGY_SURGE,
                SPELL_SHAMAN_POWER_SURGE
            });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId;
        int32 chance;

        // Lesser Healing Wave
        if (spellInfo->SpellFamilyFlags[0] & 0x00000080)
        {
            spellId = SPELL_SHAMAN_ENERGY_SURGE;
            chance = 10;
        }
        // Lightning Bolt
        else if (spellInfo->SpellFamilyFlags[0] & 0x00000001)
        {
            spellId = SPELL_SHAMAN_ENERGY_SURGE;
            chance = 15;
        }
        // Stormstrike
        else if (spellInfo->SpellFamilyFlags[1] & 0x00000010)
        {
            spellId = SPELL_SHAMAN_POWER_SURGE;
            chance = 50;
        }
        else
            return;

        if (roll_chance_i(chance))
            eventInfo.GetActor()->CastSpell((Unit*)nullptr, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70811 - Item - Shaman T10 Elemental 2P Bonus
class spell_sha_item_t10_elemental_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_item_t10_elemental_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELEMENTAL_MASTERY });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Player* target = GetTarget()->ToPlayer())
            target->ModifySpellCooldown(SPELL_SHAMAN_ELEMENTAL_MASTERY, -aurEff->GetAmount());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_t10_elemental_2p_bonus::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 60103 - Lava Lash
class spell_sha_lava_lash : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_lash);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            int32 damage = GetEffectValue();
            int32 hitDamage = GetHitDamage();
            if (Item* offhand = caster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
            {
                // Damage is increased by 25% if your off-hand weapon is enchanted with Flametongue.
                if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_SHAMAN, 0x200000, 0, 0))
                    if (aurEff->GetBase()->GetCastItemGUID() == offhand->GetGUID())
                        AddPct(hitDamage, damage);
                SetHitDamage(hitDamage);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_lava_lash::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }

};

// 53817 - Maelstrom Weapon
class spell_sha_maelstrom_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_maelstrom_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_MAELSTROM_POWER,
                SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS
            });
    }

    void HandleBonus(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() < GetSpellInfo()->StackAmount)
            return;

        Unit* caster = GetUnitOwner();
        AuraEffect const* maelstrom = caster->GetAuraEffect(SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS, EFFECT_0);
        if (!maelstrom || !roll_chance_i(maelstrom->GetAmount()))
            return;

        caster->CastSpell((Unit*)nullptr, SPELL_SHAMAN_MAELSTROM_POWER, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_maelstrom_weapon::HandleBonus, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_CHANGE_AMOUNT);
    }
};

// 30823 - Shamanistic Rage
class spell_sha_shamanistic_rage : public AuraScript
{
    PrepareAuraScript(spell_sha_shamanistic_rage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SHAMANISTIC_RAGE_PROC });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* target = GetTarget();
        int32 amount = CalculatePct(static_cast<int32>(target->GetTotalAttackPowerValue(BASE_ATTACK)), aurEff->GetAmount());
        target->CastCustomSpell(SPELL_SHAMAN_SHAMANISTIC_RAGE_PROC, SPELLVALUE_BASE_POINT0, amount, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_shamanistic_rage::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 58877 - Spirit Hunt
class spell_sha_spirit_hunt : public AuraScript
{
    PrepareAuraScript(spell_sha_spirit_hunt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SPIRIT_HUNT_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        Unit* caster = eventInfo.GetActor();
        Unit* target = caster->GetOwner();
        if (!target)
            return;

        int32 amount = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount());
        caster->CastCustomSpell(SPELL_SHAMAN_SPIRIT_HUNT_HEAL, SPELLVALUE_BASE_POINT0, amount, caster, true, nullptr, aurEff);
        caster->CastCustomSpell(SPELL_SHAMAN_SPIRIT_HUNT_HEAL, SPELLVALUE_BASE_POINT0, amount, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_spirit_hunt::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -51525 - Static Shock
class spell_sha_static_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_static_shock);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE_R1 });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();

        // Get Lightning Shield
        AuraEffect const* lightningShield = caster->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x00000400, 0x00000000, 0x00000000, caster->GetGUID());
        if (!lightningShield)
            return;

        uint32 spellId = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE_R1, lightningShield->GetSpellInfo()->GetRank());
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), spellId, true, nullptr, aurEff);
        lightningShield->GetBase()->DropCharge();
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_static_shock::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 55198 - Tidal Force
class spell_sha_tidal_force_dummy : public AuraScript
{
    PrepareAuraScript(spell_sha_tidal_force_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TIDAL_FORCE_CRIT });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->RemoveAuraFromStack(SPELL_SHAMAN_TIDAL_FORCE_CRIT);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_tidal_force_dummy::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 28823 - Totemic Power
class spell_sha_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_TOTEMIC_POWER_ARMOR,
                SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER,
                SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER,
                SPELL_SHAMAN_TOTEMIC_POWER_MP5
            });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 spellId;
        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        switch (target->getClass())
        {
        case CLASS_PALADIN:
        case CLASS_PRIEST:
        case CLASS_SHAMAN:
        case CLASS_DRUID:
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_MP5;
            break;
        case CLASS_MAGE:
        case CLASS_WARLOCK:
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER;
            break;
        case CLASS_HUNTER:
        case CLASS_ROGUE:
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER;
            break;
        case CLASS_WARRIOR:
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_ARMOR;
            break;
        default:
            return;
        }

        caster->CastSpell(target, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70817 - Item - Shaman T10 Elemental 4P Bonus
class spell_sha_t10_elemental_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t10_elemental_4p_bonus);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        // try to find spell Flame Shock on the target
        AuraEffect* flameShock = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, 0x10000000, 0x00000000, 0x00000000, caster->GetGUID());
        if (!flameShock)
            return;

        Aura* flameShockAura = flameShock->GetBase();

        int32 maxDuration = flameShockAura->GetMaxDuration();
        int32 newDuration = flameShockAura->GetDuration() + aurEff->GetAmount() * IN_MILLISECONDS;

        flameShockAura->SetDuration(newDuration);
        // is it blizzlike to change max duration for FS?
        if (newDuration > maxDuration)
            flameShockAura->SetMaxDuration(newDuration);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t10_elemental_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 33757 - Windfury Weapon (Passive)
class spell_sha_windfury_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_windfury_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_WINDFURY_WEAPON_R1,
                SPELL_SHAMAN_WINDFURY_ATTACK_MH,
                SPELL_SHAMAN_WINDFURY_ATTACK_OH
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player)
            return false;

        Item* item = player->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!item || !item->IsEquipped())
            return false;

        WeaponAttackType attType = static_cast<WeaponAttackType>(player->GetAttackBySlot(item->GetSlot()));
        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();

        uint32 spellId = 0;
        WeaponAttackType attType = BASE_ATTACK;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)
            spellId = SPELL_SHAMAN_WINDFURY_ATTACK_MH;

        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
        {
            spellId = SPELL_SHAMAN_WINDFURY_ATTACK_OH;
            attType = OFF_ATTACK;
        }

        Item* item = ASSERT_NOTNULL(player->GetWeaponForAttack(attType));

        int32 enchantId = static_cast<int32>(item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
        int32 extraAttackPower = 0;
        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_WINDFURY_WEAPON_R1);
        while (spellInfo)
        {
            if (spellInfo->Effects[EFFECT_0].MiscValue == enchantId)
            {
                extraAttackPower = spellInfo->Effects[EFFECT_1].CalcValue(player);
                break;
            }
            spellInfo = spellInfo->GetNextRankSpell();
        }

        if (!extraAttackPower)
            return;

        // Value gained from additional AP
        int32 amount = static_cast<int32>(extraAttackPower / 14.f * player->GetAttackTime(attType) / 1000.f);

        // Attack twice
        for (uint8 i = 0; i < 2; ++i)
            player->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetProcTarget(), true, item, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_windfury_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_windfury_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_shaman_spell_scripts()
{
    RegisterSpellScript(spell_sha_ancestral_awakening);
    RegisterSpellScript(spell_sha_totem_of_wrath);
    RegisterSpellScript(spell_sha_spirit_walk);
    RegisterSpellScript(spell_sha_t10_restoration_4p_bonus);
    RegisterSpellScript(spell_sha_totemic_mastery);
    RegisterSpellScript(spell_sha_feral_spirit_scaling);
    RegisterSpellScript(spell_sha_fire_elemental_scaling);
    RegisterSpellScript(spell_sha_ancestral_awakening_proc);
    RegisterSpellScript(spell_sha_astral_shift);
    RegisterSpellScript(spell_sha_bloodlust);
    RegisterSpellScript(spell_sha_chain_heal);
    RegisterSpellScript(spell_sha_cleansing_totem_pulse);
    RegisterSpellScript(spell_sha_earth_shield);
    RegisterSpellScript(spell_sha_earthbind_totem);
    RegisterSpellScript(spell_sha_earthen_power);
    RegisterSpellScript(spell_sha_earthliving_weapon);
    RegisterSpellScript(spell_sha_fire_nova);
    RegisterSpellScript(spell_sha_flame_shock);
    RegisterSpellScript(spell_sha_healing_stream_totem);
    RegisterSpellScript(spell_sha_heroism);
    RegisterSpellScript(spell_sha_item_lightning_shield);
    RegisterSpellScript(spell_sha_item_lightning_shield_trigger);
    RegisterSpellScript(spell_sha_item_mana_surge);
    RegisterSpellScript(spell_sha_item_t10_elemental_2p_bonus);
    RegisterSpellScript(spell_sha_lava_lash);
    RegisterSpellScript(spell_sha_lightning_shield);
    RegisterSpellScript(spell_sha_mana_spring_totem);
    RegisterSpellScript(spell_sha_mana_tide_totem);
    RegisterSpellScript(spell_sha_nature_guardian);
    RegisterSpellScript(spell_sha_sentry_totem);
    RegisterSpellScript(spell_sha_thunderstorm);
    RegisterSpellScript(spell_sha_flurry_proc);
    RegisterSpellScript(spell_sha_t8_electrified);
    RegisterSpellScript(spell_sha_flametongue_weapon);
    RegisterSpellScript(spell_sha_frozen_power);
    RegisterSpellScript(spell_sha_glyph_of_earth_shield);
    RegisterSpellScript(spell_sha_glyph_of_healing_wave);
    RegisterSpellScript(spell_sha_glyph_of_totem_of_wrath);
    RegisterSpellScript(spell_sha_healing_stream_totem);
    RegisterSpellScript(spell_sha_astral_shift_aura);
    RegisterSpellScript(spell_sha_imp_water_shield);
    RegisterSpellScript(spell_sha_lightning_overload);
    RegisterSpellScript(spell_sha_item_t6_trinket);
    RegisterSpellScript(spell_sha_maelstrom_weapon);
    RegisterSpellScript(spell_sha_shamanistic_rage);
    RegisterSpellScript(spell_sha_spirit_hunt);
    RegisterSpellScript(spell_sha_static_shock);
    RegisterSpellScript(spell_sha_tidal_force_dummy);
    RegisterSpellScript(spell_sha_t3_6p_bonus);
    RegisterSpellScript(spell_sha_t10_elemental_4p_bonus);
    RegisterSpellScript(spell_sha_windfury_weapon);

    RegisterSpellScript(spell_sha_totem_searing_bolt);
}

