#pragma once
#define OUT

#include <string>
#include "AreaTriggerDataStore.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "DatabaseEnv.h"
#include "SharedDefines.h"
#include "Gamemode.h"
#include "GameTime.h"
#include <unordered_map>
#include <list>
#include <tuple>

#include <random>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

enum CharacterPointType
{
    TALENT_TREE = 0,
    FORGE_SKILL_TREE = 1,
    PRESTIGE_TREE = 3,
    RACIAL_TREE = 4,
    SKILL_PAGE = 5,
    PRESTIGE_COUNT = 6,
    CLASS_TREE = 7,
    PET_TALENT = 8,
};

enum NodeType
{
    AURA = 0,
    SPELL = 1,
    CHOICE = 2
};

enum ForgeSettingIndex
{
    FORGE_SETTING_SPEC_SLOTS = 0
};

#define FORGE_SETTINGS "ForgeSettings"
#define ACCOUNT_WIDE_TYPE CharacterPointType::PRESTIGE_TREE
#define ACCOUNT_WIDE_KEY 0xfffffffe

enum class SpecVisibility
{
    PRIVATE = 0,
    FRIENDS = 1,
    GUILD = 2,
    PUBLIC = 3
};

enum class PereqReqirementType
{
    ALL = 0,
    ONE = 1
};

struct ForgeCharacterPoint
{
    CharacterPointType PointType;
    uint32 SpecId;
    uint32 Sum;
    uint32 Max;
};

struct ClassSpecDetail
{
    std::string Name;
    uint32 SpellIconId;
    uint32 SpecId;
};

struct ForgeCharacterTalent
{
    uint32 SpellId;
    uint32 TabId;
    uint8 CurrentRank;
    uint8 type;
};

struct ForgeTalentPrereq
{
    uint32 reqId;
    uint32 Talent;
    uint32 TalentTabId;
    uint32 RequiredRank;
};

struct ForgeTalentChoice
{
    uint32 spellId;
    bool active;
};

struct ForgeTalent
{
    uint32 SpellId;
    uint32 TalentTabId;
    uint32 ColumnIndex;
    uint32 RowIndex;
    uint8 RankCost;
    uint16 TabPointReq;
    uint8 RequiredLevel;
    CharacterPointType TalentType;
    NodeType nodeType;
    uint8 nodeIndex;

    uint8 NumberOfRanks;
    PereqReqirementType PreReqType;
    std::list<ForgeTalentPrereq*> Prereqs;
    std::map<uint8 /*index*/, ForgeTalentChoice*> Choices;
    std::list<uint32> UnlearnSpells;
    // rank number, spellId
    std::unordered_map<uint32, uint32> Ranks;
    // SpellId, Rank Number
    std::unordered_map<uint32, uint32> RanksRev; // only in memory
};

struct ForgeTalentTab
{
    uint32 Id;
    uint32 ClassMask;
    uint32 RaceMask;
    std::string Name;
    uint32 SpellIconId;
    std::string Background;
    std::string Description;
    uint8 Role;
    std::string SpellString;
    CharacterPointType TalentType;
    uint32 TabIndex;
    std::unordered_map<uint32 /*spellId*/, ForgeTalent*> Talents;
};

// XMOG
struct ForgeCharacterXmog
{
    std::string name;
    std::unordered_map<uint8, uint32> slottedItems;
};

// hater: custom stats
#define MAINSTATS 3
#define TANKDIST 0

enum RewardItems {
    TOKEN_OF_PRESTIGE = 1049426,
    REROLL = 1000000,
    SOUL_DUST = 1045624
};

//////// PERK ////////

struct Perk {
    uint32 spellId;
    uint8 maxRank;
    uint32 associatedClass;
    bool isAura;
    uint32 mutexGroup;
    std::string tags;
    bool operator () (const Perk& m) const
    {
        return m.spellId == m.spellId;
    }
};

struct CharacterSpecPerk {
    Perk* spell;
    std::string uuid;
    uint8 rank;
    uint8 carryover;
    bool operator () (const CharacterSpecPerk* m) const
    {
        return m->spell->spellId == m->spell->spellId;
    }
};

struct ForgeCharacterSpec
{
    uint32 Id;
    ObjectGuid CharacterGuid;
    std::string Name;
    std::string Description;
    bool Active;
    uint32 SpellIconId;
    SpecVisibility Visability;
    uint32 CharacterSpecTabId; // like holy ret pro
    // TabId, Spellid
    std::unordered_map<uint32, std::unordered_map<uint32, ForgeCharacterTalent*>> Talents;
    // tabId
    std::unordered_map<uint32, uint8> PointsSpent;
    std::unordered_map<uint32 /*node id*/, uint32/*spell picked*/> ChoiceNodesChosen;

    // hater: perks
    std::unordered_map<uint32, CharacterSpecPerk*> perks;
    std::unordered_map<uint32, Perk*> groupPerks;
    std::vector<CharacterSpecPerk*> prestigePerks;
    std::unordered_map<std::string, std::vector<CharacterSpecPerk*>> perkQueue;

};

class ForgeCache : public DatabaseScript
{
public:

    static ForgeCache* instance()
    {
        static ForgeCache* cache;

        if (cache == nullptr)
            cache = new ForgeCache();

        return cache;
    }

    ForgeCache() : DatabaseScript("ForgeCache")
    {
        RACE_LIST =
        {
            RACE_HUMAN,
            RACE_ORC,
            RACE_DWARF,
            RACE_NIGHTELF,
            RACE_UNDEAD_PLAYER,
            RACE_TAUREN,
            RACE_GNOME,
            RACE_TROLL,
            RACE_BLOODELF,
            RACE_DRAENEI
        };

        CLASS_LIST =
        {
            CLASS_WARRIOR,
            CLASS_PALADIN,
            CLASS_HUNTER,
            CLASS_ROGUE,
            CLASS_PRIEST,
            CLASS_DEATH_KNIGHT,
            CLASS_SHAMAN,
            CLASS_MAGE,
            CLASS_WARLOCK,
            CLASS_DRUID,
            CLASS_SHAPESHIFTER,
        };

        TALENT_POINT_TYPES =
        {
            CharacterPointType::TALENT_TREE,
            CharacterPointType::CLASS_TREE,
            CharacterPointType::FORGE_SKILL_TREE,
            CharacterPointType::PRESTIGE_TREE,
            CharacterPointType::RACIAL_TREE
        };
    }

    bool IsDatabaseBound() const override
    {
        return true;
    }

    void OnAfterDatabasesLoaded(uint32 updateFlags) override
    {
        BuildForgeCache();
    }

    bool TryGetTabIdForSpell(Player* player, uint32 spellId, OUT uint32& tabId)
    {
        auto tabItt = SpellToTalentTabMap.find(spellId);

        if (tabItt == SpellToTalentTabMap.end())
            return false;

        tabId = tabItt->second;
        return true;
    }

    bool TryGetSpellIddForTab(Player* player, uint32 tabId, OUT uint32& skillId)
    {
        auto tabItt = TalentTabToSpellMap.find(tabId);

        if (tabItt == TalentTabToSpellMap.end())
            return false;

        skillId = tabItt->second;
        return true;
    }

    bool TryGetCharacterTalents(Player* player, uint32 tabId, OUT std::unordered_map<uint32, ForgeCharacterTalent*>& spec)
    {
        ForgeCharacterSpec* charSpec;

        if (!TryGetCharacterActiveSpec(player, charSpec))
            return false;

        auto tabItt = charSpec->Talents.find(tabId);

        if (tabItt == charSpec->Talents.end())
            return false;

        spec = tabItt->second;
        return true;
    }

    bool TryGetAllCharacterSpecs(Player* player, OUT std::list<ForgeCharacterSpec*>& specs)
    {
        auto charSpecItt = CharacterSpecs.find(player->GetGUID());

        if (charSpecItt == CharacterSpecs.end())
            return false;

        for (auto& specKvp : charSpecItt->second)
            specs.push_back(specKvp.second);

        return true;
    }

    bool TryGetCharacterActiveSpec(Player* player, OUT ForgeCharacterSpec*& spec)
    {
        auto cas = CharacterActiveSpecs.find(player->GetGUID());

        if (cas == CharacterActiveSpecs.end())
            return false;

        return TryGetCharacterSpec(player, cas->second, spec);
    }

    bool TryGetCharacterSpec(Player* player, uint32 specId, OUT ForgeCharacterSpec*& spec)
    {
        auto charSpecItt = CharacterSpecs.find(player->GetGUID());

        if (charSpecItt == CharacterSpecs.end())
            return false;

        auto sp = charSpecItt->second.find(specId);

        if (sp == charSpecItt->second.end())
            return false;

        spec = sp->second;
        return true;
    }

    ForgeCharacterTalent* GetTalent(Player* player, uint32 spellId)
    {
        auto tabItt = SpellToTalentTabMap.find(spellId);

        if (tabItt == SpellToTalentTabMap.end())
            return nullptr;

        auto* tab = TalentTabs[tabItt->second];

        ForgeCharacterSpec* spec;
        if (TryGetCharacterActiveSpec(player, spec))
        {
            auto talTabItt = spec->Talents.find(tab->Id);

            if (talTabItt == spec->Talents.end())
                return nullptr;

            auto spellItt = talTabItt->second.find(spellId);

            if (spellItt == talTabItt->second.end())
                return nullptr;

            return spellItt->second;
        }

        return nullptr;
    }

    ForgeCharacterPoint* GetSpecPoints(Player* player, CharacterPointType pointType)
    {
        ForgeCharacterSpec* spec;

        if (TryGetCharacterActiveSpec(player, spec))
        {
            return GetSpecPoints(player, pointType, spec->Id);
        }

        AddCharacterSpecSlot(player);

        return GetSpecPoints(player, pointType);
    }

    ForgeCharacterPoint* GetSpecPoints(Player* player, CharacterPointType pointType, uint32 specId)
    {
        if (ACCOUNT_WIDE_TYPE == pointType || pointType == CharacterPointType::PRESTIGE_COUNT)
        {
            auto ptItt = AccountWidePoints.find(player->GetSession()->GetAccountId());

            if (ptItt != AccountWidePoints.end())
            {
                auto p = ptItt->second.find(pointType);

                if (p != ptItt->second.end())
                    return p->second;
            }

            return CreateAccountBoundCharPoint(player, pointType);
        }

        auto charGuid = player->GetGUID();
        auto cpItt = CharacterPoints.find(charGuid);

        if (cpItt != CharacterPoints.end())
        {
            auto ptItt = cpItt->second.find(pointType);

            if (ptItt != cpItt->second.end())
            {
                auto talItt = ptItt->second.find(specId);

                if (talItt != ptItt->second.end())
                    return talItt->second;
            }
        }


        ForgeCharacterPoint* fcp = new ForgeCharacterPoint();
        fcp->PointType = pointType;
        fcp->SpecId = specId;


        UpdateCharPoints(player, fcp);

        return fcp;
    }

    ForgeCharacterPoint* GetCommonCharacterPoint(Player* player, CharacterPointType pointType)
    {
        if (pointType != ACCOUNT_WIDE_TYPE && pointType != CharacterPointType::PRESTIGE_COUNT)
            return GetSpecPoints(player, pointType, UINT_MAX);
        else
            return GetSpecPoints(player, pointType);
    }

    bool TryGetTabPointType(uint32 tabId, CharacterPointType& pointType)
    {
        auto fttItt = TalentTabs.find(tabId);

        if (fttItt == TalentTabs.end())
            return false;

        pointType = fttItt->second->TalentType;
        return true;
    }

    bool TryGetTalentTab(Player* player, uint32 tabId, OUT ForgeTalentTab*& tab)
    {
        tab = TalentTabs[tabId];
        return true;
    }

    bool TryGetForgeTalentTabs(Player* player, CharacterPointType cpt, OUT std::list<ForgeTalentTab*>& talentTabs)
    {
        auto race = player->getRace();
        auto pClass = player->getClass();

        auto charRaceItt = RaceAndClassTabMap.find(race);

        if (charRaceItt == RaceAndClassTabMap.end())
            return false;

        auto charClassItt = charRaceItt->second.find(pClass);

        if (charClassItt == charRaceItt->second.end())
            return false;

        auto ptItt = CharacterPointTypeToTalentTabIds.find(cpt);

        if (ptItt == CharacterPointTypeToTalentTabIds.end())
            return false;

        for (auto iter : charClassItt->second)
        {
            if (ptItt->second.find(iter) != ptItt->second.end())
                talentTabs.push_back(TalentTabs[iter]);
        }

        return true;
    }


    void AddCharacterSpecSlot(Player* player)
    {
        uint32 act = player->GetSession()->GetAccountId();
        uint8 num = player->GetSpecsCount();

        num = num + 1;
        player->UpdatePlayerSetting(FORGE_SETTINGS, FORGE_SETTING_SPEC_SLOTS, num);

        ForgeCharacterSpec* spec = new ForgeCharacterSpec();
        spec->Id = num;
        spec->Active = num == 1;
        spec->CharacterGuid = player->GetGUID();
        spec->Name = "Specialization " + std::to_string(num);
        spec->Description = "Skill Specilization";
        spec->Visability = SpecVisibility::PRIVATE;
        spec->SpellIconId = 133743;
        spec->CharacterSpecTabId = 0;

        player->SetSpecsCount(num);

        if (spec->Active)
            player->ActivateSpec(num);

        auto actItt = AccountWideCharacterSpecs.find(act);

        if (actItt != AccountWideCharacterSpecs.end())
        {
            for (auto& talent : actItt->second->Talents)
                for (auto& tal : talent.second)
                    spec->Talents[talent.first][tal.first] = tal.second;


            for (auto& pointsSpent : actItt->second->PointsSpent)
                spec->PointsSpent[pointsSpent.first] = pointsSpent.second;
        }

        std::list<ForgeTalentTab*> tabs;
        if (TryGetForgeTalentTabs(player, CharacterPointType::TALENT_TREE, tabs)) {
            for (auto tab : tabs) {
                for (auto talent : tab->Talents) {
                    if (talent.second) {
                        ForgeCharacterTalent* ct = new ForgeCharacterTalent();
                        ct->CurrentRank = 0;
                        ct->SpellId = talent.second->SpellId;
                        ct->TabId = tab->Id;
                        ct->type = talent.second->nodeType;

                        spec->Talents[tab->Id][ct->SpellId] = ct;
                    }
                }
            }
        }

        AddCharacterPointsToAllSpecs(player, CharacterPointType::TALENT_TREE, 1);

        for (auto pt : TALENT_POINT_TYPES)
        {
            if (ACCOUNT_WIDE_TYPE == pt || pt == CharacterPointType::PRESTIGE_COUNT)
            {
                if (actItt == AccountWideCharacterSpecs.end() || AccountWidePoints[act].find(pt) == AccountWidePoints[act].end())
                    CreateAccountBoundCharPoint(player, pt);

                continue;
            }

            ForgeCharacterPoint* fpt = GetCommonCharacterPoint(player, pt);
            ForgeCharacterPoint* maxCp = GetMaxPointDefaults(pt);

            ForgeCharacterPoint* newFp = new ForgeCharacterPoint();
            newFp->Max = maxCp->Max;
            newFp->PointType = pt;
            newFp->SpecId = spec->Id;
            newFp->Sum = maxCp->Sum;

            UpdateCharPoints(player, newFp);
        }

        UpdateCharacterSpec(player, spec);
    }

    void ReloadDB()
    {
        BuildForgeCache();
    }

    void UpdateCharPoints(Player* player, ForgeCharacterPoint*& fp)
    {
        auto charGuid = player->GetGUID();
        auto acct = player->GetSession()->GetAccountId();
        bool isNotAccountWide = ACCOUNT_WIDE_TYPE != fp->PointType && fp->PointType != CharacterPointType::PRESTIGE_COUNT;

        if (isNotAccountWide)
            CharacterPoints[charGuid][fp->PointType][fp->SpecId] = fp;
        else
        {
            AccountWidePoints[acct][fp->PointType] = fp;

            auto pItt = PlayerCharacterMap.find(acct);

            if (pItt != PlayerCharacterMap.end())
                for (auto& ch : pItt->second)
                    for (auto& spec : CharacterSpecs[ch])
                        CharacterPoints[ch][fp->PointType][spec.first] = fp;
        }

        auto trans = CharacterDatabase.BeginTransaction();

        if (isNotAccountWide)
            trans->Append("INSERT INTO `forge_character_points` (`guid`,`type`,`spec`,`sum`,`max`) VALUES ({},{},{},{},{}) ON DUPLICATE KEY UPDATE `sum` = {}, `max` = {}", charGuid.GetCounter(), (int)fp->PointType, fp->SpecId, fp->Sum, fp->Max, fp->Sum, fp->Max);
        else
            trans->Append("INSERT INTO `forge_character_points` (`guid`,`type`,`spec`,`sum`,`max`) VALUES ({},{},{},{},{}) ON DUPLICATE KEY UPDATE `sum` = {}, `max` = {}", acct, (int)fp->PointType, ACCOUNT_WIDE_KEY, fp->Sum, fp->Max, fp->Sum, fp->Max);

        CharacterDatabase.CommitTransaction(trans);
    }

    void UpdateCharacterSpec(Player* player, ForgeCharacterSpec* spec)
    {
        uint32 charId = player->GetGUID().GetCounter();
        uint32 acct = player->GetSession()->GetAccountId();

        auto trans = CharacterDatabase.BeginTransaction();

        if (spec->Active)
            CharacterActiveSpecs[player->GetGUID()] = spec->Id;

        UpdateForgeSpecInternal(player, trans, spec);

        for (auto& tabIdKvp : spec->Talents)
            for (auto& tabTypeKvp : tabIdKvp.second)
                UpdateCharacterTalentInternal(acct, charId, trans, spec->Id, tabTypeKvp.second->SpellId, tabTypeKvp.second->TabId, tabTypeKvp.second->CurrentRank);

        CharacterDatabase.CommitTransaction(trans);
    }

    void UpdateCharacterSpecDetailsOnly(Player* player, ForgeCharacterSpec*& spec)
    {
        uint32 charId = player->GetGUID().GetCounter();
        auto trans = CharacterDatabase.BeginTransaction();
        UpdateForgeSpecInternal(player, trans, spec);

        CharacterDatabase.CommitTransaction(trans);
    }

    void ApplyAccountBoundTalents(Player* player)
    {
        ForgeCharacterSpec* currentSpec;

        if (TryGetCharacterActiveSpec(player, currentSpec))
        {
            uint32 playerLevel = player->getLevel();
            auto modes = Gamemode::GetGameMode(player);

            for (auto charTabType : TALENT_POINT_TYPES)
            {
                if (ACCOUNT_WIDE_TYPE != charTabType)
                    continue;

                std::list<ForgeTalentTab*> tabs;
                if (TryGetForgeTalentTabs(player, charTabType, tabs))
                    for (auto* tab : tabs)
                    {
                        auto talItt = currentSpec->Talents.find(tab->Id);

                        for (auto spell : tab->Talents)
                        {

                            if (playerLevel != 80 && modes.size() == 0 && talItt != currentSpec->Talents.end())
                            {
                                auto spellItt = talItt->second.find(spell.first);

                                if (spellItt != talItt->second.end())
                                {
                                    uint32 currentRank = spell.second->Ranks[spellItt->second->CurrentRank];

                                    for (auto rank : spell.second->Ranks)
                                        if (currentRank != rank.second)
                                            player->removeSpell(rank.second, SPEC_MASK_ALL, false);

                                    if (!player->HasSpell(currentRank))
                                        player->learnSpell(currentRank, false, false);
                                }
                            }
                        }
                    }
            }

            player->SendInitialSpells();
        }
    }

    ForgeCharacterPoint* GetMaxPointDefaults(CharacterPointType cpt)
    {
        auto fpd = MaxPointDefaults.find(cpt);

        // Get default skill max and current. Happens at level 10. Players start with 10 forge points. can be changed in DB with UINT_MAX entry.
        if (fpd == MaxPointDefaults.end())
        {
            ForgeCharacterPoint* fp = new ForgeCharacterPoint();
            fp->PointType = cpt;
            fp->SpecId = UINT_MAX;
            fp->Max = 0;

            if (cpt == CharacterPointType::FORGE_SKILL_TREE)
                fp->Sum = GetConfig("level10ForgePoints", 30);
            else
                fp->Sum = 0;

            return fp;
        }
        else
            return fpd->second;
    }

    void AddCharacterPointsToAllSpecs(Player* player, CharacterPointType pointType, uint32 amount)
    {
        ForgeCharacterPoint* m = GetMaxPointDefaults(pointType);
        ForgeCharacterPoint* ccp = GetCommonCharacterPoint(player, pointType);

        if (m->Max > 0)
        {
            auto maxPoints = amount + ccp->Sum;

            if (maxPoints >= m->Max)
            {
                maxPoints = maxPoints - m->Max;
                amount = amount - maxPoints;
            }
        }

        if (amount > 0)
        {
            ccp->Sum += amount;
            UpdateCharPoints(player, ccp);

            if (pointType != ACCOUNT_WIDE_TYPE)
                for (auto& spec : CharacterSpecs[player->GetGUID()])
                {
                    ForgeCharacterPoint* cp = GetSpecPoints(player, pointType, spec.first);
                    cp->Sum += amount;
                    UpdateCharPoints(player, cp);
                }

            ChatHandler(player->GetSession()).SendSysMessage("|cff8FCE00You have been awarded " + std::to_string(amount) + " " + GetpointTypeName(pointType) + " point(s).");
        }
    }

    std::string GetpointTypeName(CharacterPointType t)
    {
        switch (t)
        {
        case CharacterPointType::PRESTIGE_TREE:
            return "Prestige";
        case CharacterPointType::TALENT_TREE:
            return "Talent";
        case CharacterPointType::RACIAL_TREE:
            return "Racial";
        case CharacterPointType::FORGE_SKILL_TREE:
            return "Forge";
        default:
            return "";
        }
    }

    bool TryGetClassSpecilizations(Player* player, OUT std::vector<ClassSpecDetail*>& list)
    {
        auto raceItt = ClassSpecDetails.find(player->getRace());


        if (raceItt == ClassSpecDetails.end())
            return false;

        auto classItt = raceItt->second.find(player->getClass());

        if (classItt == raceItt->second.end())
            return false;

        list = classItt->second;
        return true;
    }

    void DeleteCharacter(ObjectGuid guid, uint32 accountId)
    {
        if (GetConfig("PermaDeleteForgecharacter", 0) == 1)
        {
            auto trans = CharacterDatabase.BeginTransaction();
            trans->Append("DELETE FROM forge_character_specs WHERE guid = {}", guid.GetCounter());
            trans->Append("DELETE FROM character_modes WHERE guid = {}", guid.GetCounter());
            trans->Append("DELETE FROM forge_character_points WHERE guid = {} AND spec != {}", guid.GetCounter(), ACCOUNT_WIDE_KEY);
            trans->Append("DELETE FROM forge_character_talents WHERE guid = {} AND spec != {}", guid.GetCounter(), ACCOUNT_WIDE_KEY);
            trans->Append("DELETE FROM forge_character_talents_spent WHERE guid = {} AND spec != {}", guid.GetCounter(), ACCOUNT_WIDE_KEY);
            trans->Append("DELETE FROM character_spec_perks WHERE guid = {}", guid.GetCounter());
            trans->Append("DELETE FROM character_perk_selection_queue WHERE guid = {}", guid.GetCounter());
            trans->Append("DELETE FROM character_prestige_perk_carryover WHERE guid = {}", guid.GetCounter());
            trans->Append("DELETE FROM character_worldtier WHERE `char` = {}", guid.GetCounter());
            // hater: perks

            CharacterDatabase.CommitTransaction(trans);
        }
    }

    uint32 GetConfig(std::string key, uint32 defaultValue)
    {
        auto valItt = CONFIG.find(key);

        if (valItt == CONFIG.end())
        {
            auto trans = WorldDatabase.BeginTransaction();
            trans->Append("INSERT INTO forge_config (`cfgName`,`cfgValue`) VALUES ('{}',{})", key, defaultValue);
            WorldDatabase.CommitTransaction(trans);

            CONFIG[key] = defaultValue;
            return defaultValue;
        }

        return valItt->second;
    }


    bool isNumber(const std::string& s)
    {
        return std::ranges::all_of(s.begin(), s.end(), [](char c) { return isdigit(c) != 0; });
    }

    void UpdateCharacters(uint32 account, Player* player)
    {
        if (PlayerCharacterMap.find(account) != PlayerCharacterMap.end())
            PlayerCharacterMap.erase(account);

        QueryResult query = CharacterDatabase.Query("SELECT guid, account FROM characters where account = {}", account);

        if (!query)
            return;

        do
        {
            Field* field = query->Fetch();
            PlayerCharacterMap[field[1].Get<uint32>()].push_back(ObjectGuid::Create<HighGuid::Player>(field[0].Get<uint32>()));

        } while (query->NextRow());
    }

    ForgeCharacterPoint* CreateAccountBoundCharPoint(Player* player, const CharacterPointType& pt)
    {
        ForgeCharacterPoint* maxCp = GetMaxPointDefaults(pt);

        ForgeCharacterPoint* newFp = new ForgeCharacterPoint();
        newFp->Max = maxCp->Max;
        newFp->PointType = pt;
        newFp->SpecId = ACCOUNT_WIDE_KEY;
        newFp->Sum = 0;

        UpdateCharPoints(player, newFp);
        return newFp;
    }


    std::string BuildXmogSetsMsg(Player* player) {
        std::string out = "";

        auto sets = XmogSets.find(player->GetGUID().GetCounter());
        if (sets != XmogSets.end())
            for (auto set : sets->second)
                out += std::to_string(set.first) + "^" + set.second->name + ";";
        else
            out += "empty";

        return out;
    }

    void SaveXmogSet(Player* player, uint32 setId) {
        auto sets = XmogSets.find(player->GetGUID().GetCounter());
        if (sets != XmogSets.end()) {
            auto set = sets->second.find(setId);
            if (set != sets->second.end()) {
                for (int i : xmogSlots)
                    if (auto item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                        set->second->slottedItems[i] = item->GetTransmog();
                    else
                        set->second->slottedItems[i] = 0;

                SaveXmogSetInternal(player, setId, set->second);
            }
        }
    }

    void AddXmogSet(Player* player, uint32 setId, std::string name) {
        ForgeCharacterXmog* xmog = new ForgeCharacterXmog();
        xmog->name = name;

        auto newSetId = FirstOpenXmogSlot(player);

        for (int i : xmogSlots)
            if (auto item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                xmog->slottedItems[i] = item->GetTransmog();
            else
                xmog->slottedItems[i] = 0;

        XmogSets[player->GetGUID().GetCounter()][newSetId] = xmog;
        SaveXmogSetInternal(player, newSetId, xmog);
    }

    std::string BuildXmogFromEquipped(Player* player) {
        std::string out = "noname^";
        for (auto slot : xmogSlots) {
            auto item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            out += item == nullptr ? "0^" : std::to_string(item->GetTransmog()) + "^";
        }
        return out + ";";
    }

    std::string BuildXmogFromSet(Player* player, uint8 setId) {
        std::string out = "";
        auto sets = XmogSets.find(player->GetGUID().GetCounter());

        if (sets != XmogSets.end()) {
            auto set = sets->second.find(setId);
            if (set != sets->second.end()) {
                out += set->second->name + "^";
                for (auto slot : xmogSlots) {
                    out += std::to_string(set->second->slottedItems[slot]) + "^";
                }
                out += ";";

                return out;
            }
        }

        return BuildXmogFromEquipped(player);
    }

    void UnlearnFlaggedSpells(Player* player) {
        auto found = FlaggedForUnlearn.find(player->GetGUID().GetCounter());
        if (found != FlaggedForUnlearn.end()) {
            if (player->HasSpell(found->second))
                player->postCheckRemoveSpell(found->second);
        }
    }

    void LearnExtraSpellsIfAny(Player* player, uint32 spell) {
        auto found = SpellLearnedAdditionalSpells.find(spell);
        if (found != SpellLearnedAdditionalSpells.end()) {
            for (auto spell : found->second) {
                if (!player->HasSpell(spell))
                    player->learnSpell(spell);
            }
        }
    }

    void LoadLoadoutActions(Player* player)
    {
        ForgeCharacterSpec* spec;
        if (TryGetCharacterActiveSpec(player, spec)) {
            auto foundActions = _playerActiveTalentLoadouts.find(player->GetGUID().GetCounter());
            if (foundActions != _playerActiveTalentLoadouts.end()) {
                // SELECT button, action, type FROM forge_character_action WHERE guid = ? AND spec = ? and loadout = ? ORDER BY button;
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_ACTIONS_SPEC_LOADOUT);
                stmt->SetData(0, player->GetGUID().GetCounter());
                stmt->SetData(1, spec->CharacterSpecTabId);
                stmt->SetData(2, foundActions->second->id);

                WorldSession* mySess = player->GetSession();
                mySess->GetQueryProcessor().AddCallback(CharacterDatabase.AsyncQuery(stmt)
                    .WithPreparedCallback([mySess](PreparedQueryResult result)
                        {
                            // safe callback, we can't pass this pointer directly
                            // in case player logs out before db response (player would be deleted in that case)
                            if (Player* thisPlayer = mySess->GetPlayer())
                                thisPlayer->LoadActions(result);
                        }));
            }
        }
    }

    std::vector<uint32> RACE_LIST;
    std::vector<uint32> CLASS_LIST;
    std::vector<CharacterPointType> TALENT_POINT_TYPES;

    // tabId
    std::unordered_map<uint32, ForgeTalentTab*> TalentTabs;

    // choiceNodeId is the id of the node in forge_talents
    std::unordered_map<uint32 /*nodeid*/, std::vector<uint32/*choice spell id*/>> _choiceNodes;
    std::unordered_map<uint32 /*choice spell id*/, uint32 /*nodeid*/> _choiceNodesRev;
    std::unordered_map<uint8, uint32> _choiceNodeIndexLookup;

    uint32 GetChoiceNodeFromindex(uint8 index) {
        auto out = _choiceNodeIndexLookup.find(index);
        if (out != _choiceNodeIndexLookup.end())
            return out->second;

        return 0;
    }

    std::unordered_map<uint8 /*class*/, std::unordered_map<uint32 /*racemask*/, std::unordered_map<uint8/*level*/, std::vector<uint32 /*spell*/>>>> _levelClassSpellMap;

    /* hater: cached tree meta data */
    struct NodeMetaData {
        uint32 spellId;
        uint8 row;
        uint8 col;
        uint8 pointReq;
        uint8 nodeIndex;
        std::vector<NodeMetaData*> unlocks;
    };
    struct TreeMetaData {
        uint32 TabId;
        uint8 MaxXDim = 0;
        uint8 MaxYDim = 0;
        std::unordered_map<uint8/*row*/, std::unordered_map<uint8 /*col*/, NodeMetaData*>> nodes;
        std::unordered_map<uint32/*spellId*/, NodeMetaData*> nodeLocation;
    };
    std::unordered_map<uint32 /*tabId*/, TreeMetaData*> _cacheTreeMetaData;

    void ForgetTalents(Player* player, ForgeCharacterSpec* spec, CharacterPointType pointType) {
        for (auto tt : TalentTabs) {
            if (auto tab = tt.second) {
                for (auto spell : tab->Talents) {
                    if (spell.second) {
                        if (spell.second->nodeType == NodeType::CHOICE) {
                            for (auto choice : _choiceNodesRev)
                                if (player->HasSpell(choice.first))
                                    player->removeSpell(choice.first, player->GetActiveSpecMask(), false);
                        }
                        else
                            for (auto rank : spell.second->Ranks)
                                if (auto spellInfo = sSpellMgr->GetSpellInfo(rank.second)) {
                                    if (spellInfo->HasEffect(SPELL_EFFECT_LEARN_SPELL))
                                        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i) {
                                            auto toUnlearn = spellInfo->Effects[i].TriggerSpell;
                                            if (auto unlearnSpellInfo = sSpellMgr->GetSpellInfo(toUnlearn)) {
                                                if (unlearnSpellInfo->HasEffect(SPELL_EFFECT_LEARN_SPELL))
                                                    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i) {
                                                        auto subToUnlearn = unlearnSpellInfo->Effects[i].TriggerSpell;
                                                        player->RemoveAura(subToUnlearn);
                                                        player->removeSpell(subToUnlearn, player->GetActiveSpecMask(), false);
                                                    }
                                                else {
                                                    player->RemoveAura(toUnlearn);
                                                    player->removeSpell(toUnlearn, player->GetActiveSpecMask(), false);
                                                }
                                            }
                                        }

                                    player->RemoveAura(rank.second);
                                    player->removeSpell(rank.second, player->GetActiveSpecMask(), false);
                                }
                        auto talent = spec->Talents[tab->Id].find(spell.first);
                        if (talent != spec->Talents[tab->Id].end())
                            talent->second->CurrentRank = 0;
                    }
                }
                spec->PointsSpent[tab->Id] = 0;
            }
        }
        ForgeCharacterPoint* fcp = GetSpecPoints(player, pointType, spec->Id);

        auto amount = 0;
        auto level = player->getLevel();
        if (level > 10)
            level -= 9;

        switch (pointType) {
        case CharacterPointType::TALENT_TREE:
            if (level > 1) {
                int div = level / 2;
                amount = div;
            }
            else
                if (level % 2)
                    amount = 1;
            break;
        case CharacterPointType::CLASS_TREE:
            if (level > 1) {
                int rem = level % 2;
                int div = level / 2;
                if (rem)
                    div += 1;

                amount = div;
            }
            break;
        default:
            break;
        }

        fcp->Sum = amount;

        UpdateCharPoints(player, fcp);
    }

    std::unordered_map<uint32 /*tabid*/, std::unordered_map<uint8 /*node*/, uint32 /*spell*/>> _cacheSpecNodeToSpell;
    std::unordered_map<uint32 /*class*/, std::unordered_map<uint8 /*node*/, uint32 /*spell*/>> _cacheClassNodeToSpell;
    std::unordered_map<uint32, uint32> _cacheClassNodeToClassTree;

    const std::string base64_char = "|ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // | is to offset string to base 1

    const uint8 LOADOUT_NAME_MAX = 64;
    const uint8 META_PREFIX = 3;
    const uint8 MAX_LOADOUTS_PER_SPEC = 7;

    struct PlayerLoadout {
        bool active;
        uint8 id;
        std::string name;
        std::string talentString;
    };
    // hater: player loadout storage
    std::unordered_map<uint32 /*guid*/, std::unordered_map<uint8 /*id*/, PlayerLoadout*>> _playerTalentLoadouts;
    std::unordered_map<uint32 /*guid*/, PlayerLoadout*> _playerActiveTalentLoadouts;

    std::unordered_map<uint32 /*class*/, uint32 /*spec*/> _playerClassFirstSpec;

    // hater: custom items
    std::unordered_map<InventoryType, float /*value*/> _forgeItemSlotValues;
    std::unordered_map<InventoryType, uint8 /*slots*/> _genItemSlotsForEquipSlot;
    std::unordered_map<ItemModType, float /*value*/> _forgeItemStatValues;
    std::unordered_map<uint8, std::vector<ItemModType>> _forgeItemSecondaryStatPools;

    std::unordered_map<ItemClass, std::unordered_map<uint8 /*subclass*/, std::unordered_map<uint8 /*invtype*/, std::unordered_map<uint32 /*displayid*/, std::string /*name*/>>>> _genItemNamesAndDisplays;

    void AddDefaultLoadout(Player* player)
    {
        ForgeTalentTab* tab;
        if (TryGetTalentTab(player, player->getClassMask(), tab)) {
            std::string loadout = "A";
            loadout += base64_char.substr(player->getClass(), 1);
            loadout += base64_char.substr(64, 1);

            auto classMap = _cacheSpecNodeToSpell[player->getClass()];
            for (int i = 1; i <= classMap.size(); i++)
                loadout += base64_char.substr(1, 1);

            auto guid = player->GetGUID().GetCounter();

            PlayerLoadout* plo = new PlayerLoadout();
            plo->active = true;
            plo->id = 1;
            plo->name = "Default";
            plo->talentString = loadout;

            _playerTalentLoadouts[guid][plo->id] = plo;
            _playerActiveTalentLoadouts[guid] = plo;

            CharacterDatabase.Execute("insert into `forge_character_talent_loadouts` (`guid`, `id`, `name`, `talentString`, `active`) values ({}, {}, '{}', '{}', {})",
                guid, plo->id, plo->name, loadout, true);
        }
    }

    void SaveLoadout(Player* player, std::string loadout) {
        auto active = _playerActiveTalentLoadouts[player->GetGUID().GetCounter()];

        auto guid = player->GetGUID().GetCounter();
        auto plos = _playerTalentLoadouts.find(guid);

        if (plos != _playerTalentLoadouts.end())
        {
            auto exists = plos->second.find(active->id);
            if (exists != plos->second.end()) {

                exists->second->talentString = loadout;
                active->talentString = loadout;

                CharacterDatabase.DirectExecute("UPDATE `forge_character_talent_loadouts` set `talentString` = '{}' WHERE `guid` = {} and `id` = {}",
                    loadout, guid, active->id);
            }
        }
    }

private:
    std::unordered_map<ObjectGuid, uint32> CharacterActiveSpecs;
    std::unordered_map<std::string, uint32> CONFIG;

    // charId, specId
    std::unordered_map<ObjectGuid, std::unordered_map<uint32, ForgeCharacterSpec*>> CharacterSpecs;
    std::unordered_map<uint32, ForgeCharacterSpec*> AccountWideCharacterSpecs;

    // charId, PointType, specid
    std::unordered_map<ObjectGuid, std::unordered_map<CharacterPointType, std::unordered_map<uint32, ForgeCharacterPoint*>>> CharacterPoints;
    std::unordered_map<CharacterPointType, ForgeCharacterPoint*> MaxPointDefaults;
    std::unordered_map < uint32, std::unordered_map<uint32, ForgeCharacterPoint*>> AccountWidePoints;
    // skillid
    std::unordered_map<uint32, uint32> SpellToTalentTabMap;

    // skillid
    std::unordered_map<uint32, uint32> TalentTabToSpellMap;
    std::unordered_map<CharacterPointType, std::unordered_set<uint32>> CharacterPointTypeToTalentTabIds;

    // Race, class, tabtype
    std::unordered_map<uint8, std::unordered_map<uint8, std::unordered_set<uint32>>> RaceAndClassTabMap;

    // Race, class
    std::unordered_map<uint8, std::unordered_map<uint8, std::vector<ClassSpecDetail*>>> ClassSpecDetails;


    std::unordered_map<uint32, std::vector<ObjectGuid>> PlayerCharacterMap;

    // Flagged for spec reset
    std::vector<uint32 /*guid*/> FlaggedForReset;
    std::unordered_map<uint32 /*guid*/, uint32 /*spell*/> FlaggedForUnlearn;
    std::unordered_map<uint32 /*guid*/, std::vector<uint32 /*spell*/>> SpellLearnedAdditionalSpells;

    // hater: perks
    std::unordered_map<uint32 /*class*/, std::vector<Perk*>> Perks;
    std::unordered_map<uint32 /*class*/, std::unordered_map<uint32 /*level*/, std::vector<Perk*>>> Archetypes;
    std::unordered_map<uint32 /*id*/, Perk*> AllPerks;

    // xmog
    std::unordered_map<uint32 /*char*/, std::unordered_map<uint8 /*setId*/, ForgeCharacterXmog*>> XmogSets;
    uint8 xmogSlots[14] = { EQUIPMENT_SLOT_HEAD, EQUIPMENT_SLOT_SHOULDERS, EQUIPMENT_SLOT_BODY, EQUIPMENT_SLOT_CHEST,
        EQUIPMENT_SLOT_WAIST, EQUIPMENT_SLOT_LEGS, EQUIPMENT_SLOT_FEET, EQUIPMENT_SLOT_WRISTS, EQUIPMENT_SLOT_HANDS,
        EQUIPMENT_SLOT_BACK, EQUIPMENT_SLOT_MAINHAND, EQUIPMENT_SLOT_OFFHAND, EQUIPMENT_SLOT_RANGED, EQUIPMENT_SLOT_TABARD };

    void BuildForgeCache()
    {
        CharacterActiveSpecs.clear();
        CharacterSpecs.clear();
        CharacterPoints.clear();
        MaxPointDefaults.clear();
        SpellToTalentTabMap.clear();
        TalentTabToSpellMap.clear();
        TalentTabs.clear();
        RaceAndClassTabMap.clear();
        ClassSpecDetails.clear();
        PRESTIGE_IGNORE_SPELLS.clear();
        CONFIG.clear();
        CharacterPointTypeToTalentTabIds.clear();

        for (const auto& race : RACE_LIST)
        {
            for (const auto& wowClass : CLASS_LIST)
            {
                for (const auto& ptType : TALENT_POINT_TYPES)
                    RaceAndClassTabMap[race][wowClass];
            }
        }

        GetCharacters();
        GetConfig();
        AddTalentTrees();
        AddTalentsToTrees();
        AddLevelClassSpellMap();
        AddTalentPrereqs();
        AddTalentChoiceNodes();
        AddTalentRanks();
        AddTalentUnlearn();
        AddCharacterSpecs();
        AddTalentSpent();
        AddCharacterTalents();
        AddCharacterChoiceNodes();

        AddPlayerTalentLoadouts();

        LOG_INFO("server.load", "Loading characters points...");
        AddCharacterPointsFromDB();
        AddCharacterClassSpecs();
        AddCharacterXmogSets();

        LOG_INFO("server.load", "Loading m+ difficulty multipliers...");
        sObjectMgr->LoadInstanceDifficultyMultiplier();
        LOG_INFO("server.load", "Loading m+ difficulty level scales...");
        sObjectMgr->LoadMythicLevelScale();
        LOG_INFO("server.load", "Loading m+ minion values...");
        sObjectMgr->LoadMythicMinionValue();
        LOG_INFO("server.load", "Loading m+ keys...");
        sObjectMgr->LoadMythicDungeonKeyMap();
        LOG_INFO("server.load", "Loading m+ affixes...");
        sObjectMgr->LoadMythicAffixes();

        LOG_INFO("server.load", "Loading npc sounds...");
        sObjectMgr->LoadNpcSounds();

        LOG_INFO("server.load", "Loading character spell unlearn flags...");
        AddSpellUnlearnFlags();
        LOG_INFO("server.load", "Loading character spell learn additional spells...");
        AddSpellLearnAdditionalSpells();

        LOG_INFO("server.load", "Loading curves...");
        sObjectMgr->LoadATCurves();
        LOG_INFO("server.load", "Loading curve points...");
        sObjectMgr->LoadATCurvePoints();

        // hater: CUSTOM ITEMS
        AddItemSlotValue();
        AddItemStatValue();
        AddItemStatPools();
        AddGemSlotsForItemSlot();
        AddItemAppearances();

        // hater: perks
        AddPerks();
        AddCharacterPerks();
        AddCharacterQueuedPerks();
        AddCharacterPrestigePerks();

        // hater: force talent resets
        LoadCharacterResetFlags();
        // hater: soul shards
        AddSoulShards();
        AddPlayerSoulShards();

        // hater: world tiers
        AddWorldTierUnlocks();
        AddStartingZones();

        AddRaidRotation();
    }

    void GetCharacters()
    {
        QueryResult query = CharacterDatabase.Query("SELECT guid, account FROM characters");

        if (!query)
            return;

        do
        {
            Field* field = query->Fetch();
            PlayerCharacterMap[field[1].Get<uint32>()].push_back(ObjectGuid::Create<HighGuid::Player>(field[0].Get<uint32>()));

        } while (query->NextRow());
    }

    void GetConfig()
    {
        QueryResult query = WorldDatabase.Query("SELECT * FROM forge_config");

        if (!query)
            return;

        do
        {
            Field* field = query->Fetch();
            CONFIG[field[0].Get<std::string>()] = field[1].Get<uint32>();

        } while (query->NextRow());
    }

    void GetMaxLevelQuests()
    {
        QueryResult query = WorldDatabase.Query("SELECT spellid FROM forge_prestige_ignored_spells");

        if (!query)
            return;

        do
        {
            Field* field = query->Fetch();
            PRESTIGE_IGNORE_SPELLS.push_back(field[0].Get<uint32>());

        } while (query->NextRow());
    }

    void UpdateForgeSpecInternal(Player* player, CharacterDatabaseTransaction& trans, ForgeCharacterSpec*& spec)
    {
        ObjectGuid charId = player->GetGUID();
        uint32 actId = player->GetSession()->GetAccountId();
        CharacterSpecs[charId][spec->Id] = spec;

        // check for account wide info, apply to other specs for all other characters of the account in the cache.
        for (auto& actChr : PlayerCharacterMap[actId])
            for (auto& sp : CharacterSpecs[actChr])
            {
                for (auto& ts : spec->PointsSpent)
                {
                    if (TalentTabs[ts.first]->TalentType == ACCOUNT_WIDE_TYPE)
                        CharacterSpecs[charId][sp.first]->PointsSpent[ts.first] = spec->PointsSpent[ts.first];
                }

                for (auto& tals : spec->Talents)
                {
                    if (tals.first > 0) {
                        if (TalentTabs[tals.first]) {
                            if (TalentTabs[tals.first]->TalentType == ACCOUNT_WIDE_TYPE)
                                for (auto& tal : tals.second)
                                    sp.second->Talents[tals.first][tal.first] = tal.second;
                        }
                    }
                }
            }

        auto activeSpecItt = CharacterActiveSpecs.find(charId);

        if (activeSpecItt != CharacterActiveSpecs.end() && spec->Active && activeSpecItt->second != spec->Id)
        {
            ForgeCharacterSpec* activeSpec;
            if (TryGetCharacterSpec(player, activeSpecItt->second, activeSpec))
            {
                activeSpec->Active = false;
                AddCharSpecUpdateToTransaction(actId, trans, activeSpec);
            }
        }

        if (spec->Active)
            CharacterActiveSpecs[charId] = spec->Id;

        AddCharSpecUpdateToTransaction(actId, trans, spec);
    }

    void AddCharSpecUpdateToTransaction(uint32 accountId, CharacterDatabaseTransaction& trans, ForgeCharacterSpec*& spec)
    {
        auto guid = spec->CharacterGuid.GetCounter();
        trans->Append("INSERT INTO `forge_character_specs` (`id`,`guid`,`name`,`description`,`active`,`spellicon`,`visability`,`charSpec`) VALUES ({},{},\"{}\",\"{}\",{},{},{},{}) ON DUPLICATE KEY UPDATE `name` = \"{}\", `description` = \"{}\", `active` = {}, `spellicon` = {}, `visability` = {}, `charSpec` = {}",
            spec->Id, guid, spec->Name, spec->Description, spec->Active, spec->SpellIconId, (int)spec->Visability, spec->CharacterSpecTabId,
            spec->Name, spec->Description, spec->Active, spec->SpellIconId, (int)spec->Visability, spec->CharacterSpecTabId);

        for (auto& kvp : spec->PointsSpent)
        {
            if (TalentTabs[kvp.first]->TalentType != ACCOUNT_WIDE_TYPE)
                trans->Append("INSERT INTO forge_character_talents_spent(`guid`,`spec`,`tabid`,`spent`) VALUES({}, {}, {}, {}) ON DUPLICATE KEY UPDATE spent = {}",
                    guid, spec->Id, kvp.first, kvp.second,
                    kvp.second);
            else
                trans->Append("INSERT INTO forge_character_talents_spent(`guid`,`spec`,`tabid`,`spent`) VALUES({}, {}, {}, {}) ON DUPLICATE KEY UPDATE spent = {}",
                    accountId, ACCOUNT_WIDE_KEY, kvp.first, kvp.second,
                    kvp.second);
        }
    }

    void UpdateCharacterTalentInternal(uint32 account, uint32 charId, CharacterDatabaseTransaction& trans, uint32 spec, uint32 spellId, uint32 tabId, uint8 known)
    {
        if (TalentTabs[tabId]->TalentType != ACCOUNT_WIDE_TYPE)
            trans->Append("INSERT INTO `forge_character_talents` (`guid`,`spec`,`spellid`,`tabId`,`currentrank`) VALUES ({},{},{},{},{}) ON DUPLICATE KEY UPDATE `currentrank` = {}", charId, spec, spellId, tabId, known, known);
        else
            trans->Append("INSERT INTO `forge_character_talents` (`guid`,`spec`,`spellid`,`tabId`,`currentrank`) VALUES ({},{},{},{},{}) ON DUPLICATE KEY UPDATE `currentrank` = {}", account, ACCOUNT_WIDE_KEY, spellId, tabId, known, known);
    }

    void ForgetCharacterPerkInternal(uint32 charId, uint32 spec, uint32 spellId) {
        // TODO trans->Append("DELETE FROM character_perks WHERE spellId = {} and specId = {}", spellId, spec);
    }

    void AddCharacterXmogSets()
    {
        LOG_INFO("server.load", "Loading character xmog sets...");
        QueryResult xmogSets = CharacterDatabase.Query("SELECT * FROM `forge_character_transmogsets`");

        if (!xmogSets)
            return;

        XmogSets.clear();

        do
        {
            Field* xmogSet = xmogSets->Fetch();
            auto guid = xmogSet[0].Get<uint32>();
            auto setid = xmogSet[1].Get<uint32>();
            std::string name = xmogSet[2].Get<std::string>();
            uint32 head = xmogSet[3].Get<uint32>();
            uint32 shoulders = xmogSet[4].Get<uint32>();
            uint32 shirt = xmogSet[5].Get<uint32>();
            uint32 chest = xmogSet[6].Get<uint32>();
            uint32 waist = xmogSet[7].Get<uint32>();
            uint32 legs = xmogSet[8].Get<uint32>();
            uint32 feet = xmogSet[9].Get<uint32>();
            uint32 wrists = xmogSet[10].Get<uint32>();
            uint32 hands = xmogSet[11].Get<uint32>();
            uint32 back = xmogSet[12].Get<uint32>();
            uint32 mh = xmogSet[13].Get<uint32>();
            uint32 oh = xmogSet[14].Get<uint32>();
            uint32 ranged = xmogSet[15].Get<uint32>();
            uint32 tabard = xmogSet[16].Get<uint32>();

            ForgeCharacterXmog* set = new ForgeCharacterXmog();
            set->name = name;
            set->slottedItems[EQUIPMENT_SLOT_HEAD] = head;
            set->slottedItems[EQUIPMENT_SLOT_SHOULDERS] = shoulders;
            set->slottedItems[EQUIPMENT_SLOT_BODY] = shirt;
            set->slottedItems[EQUIPMENT_SLOT_CHEST] = chest;
            set->slottedItems[EQUIPMENT_SLOT_WAIST] = waist;
            set->slottedItems[EQUIPMENT_SLOT_LEGS] = legs;
            set->slottedItems[EQUIPMENT_SLOT_FEET] = feet;
            set->slottedItems[EQUIPMENT_SLOT_WRISTS] = wrists;
            set->slottedItems[EQUIPMENT_SLOT_HANDS] = hands;
            set->slottedItems[EQUIPMENT_SLOT_BACK] = back;
            set->slottedItems[EQUIPMENT_SLOT_MAINHAND] = mh;
            set->slottedItems[EQUIPMENT_SLOT_OFFHAND] = oh;
            set->slottedItems[EQUIPMENT_SLOT_RANGED] = ranged;
            set->slottedItems[EQUIPMENT_SLOT_TABARD] = tabard;

            XmogSets[guid][setid] = set;
        } while (xmogSets->NextRow());
    }

    void SaveXmogSetInternal(Player* player, uint32 set, ForgeCharacterXmog* xmog) {
        auto trans = CharacterDatabase.BeginTransaction();
        trans->Append("INSERT INTO acore_characters.forge_character_transmogsets (guid, setid, setname, head, shoulders, shirt, chest, waist, legs, feet, wrists, hands, back, mh, oh, ranged, tabard) VALUES({}, {}, '{}', {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}) on duplicate key update setname = '{}' , head = {}, shoulders = {}, shirt = {}, chest = {}, waist = {}, legs = {}, feet = {}, wrists = {}, hands = {}, back = {}, mh = {}, oh = {}, ranged = {}, tabard = {}",
            player->GetGUID().GetCounter(), set, xmog->name, xmog->slottedItems[EQUIPMENT_SLOT_HEAD], xmog->slottedItems[EQUIPMENT_SLOT_SHOULDERS],
            xmog->slottedItems[EQUIPMENT_SLOT_BODY], xmog->slottedItems[EQUIPMENT_SLOT_CHEST], xmog->slottedItems[EQUIPMENT_SLOT_WAIST],
            xmog->slottedItems[EQUIPMENT_SLOT_LEGS], xmog->slottedItems[EQUIPMENT_SLOT_FEET], xmog->slottedItems[EQUIPMENT_SLOT_WRISTS],
            xmog->slottedItems[EQUIPMENT_SLOT_HANDS], xmog->slottedItems[EQUIPMENT_SLOT_BACK], xmog->slottedItems[EQUIPMENT_SLOT_MAINHAND],
            xmog->slottedItems[EQUIPMENT_SLOT_OFFHAND], xmog->slottedItems[EQUIPMENT_SLOT_RANGED], xmog->slottedItems[EQUIPMENT_SLOT_TABARD],
            xmog->name, xmog->slottedItems[EQUIPMENT_SLOT_HEAD], xmog->slottedItems[EQUIPMENT_SLOT_SHOULDERS],
            xmog->slottedItems[EQUIPMENT_SLOT_BODY], xmog->slottedItems[EQUIPMENT_SLOT_CHEST], xmog->slottedItems[EQUIPMENT_SLOT_WAIST],
            xmog->slottedItems[EQUIPMENT_SLOT_LEGS], xmog->slottedItems[EQUIPMENT_SLOT_FEET], xmog->slottedItems[EQUIPMENT_SLOT_WRISTS],
            xmog->slottedItems[EQUIPMENT_SLOT_HANDS], xmog->slottedItems[EQUIPMENT_SLOT_BACK], xmog->slottedItems[EQUIPMENT_SLOT_MAINHAND],
            xmog->slottedItems[EQUIPMENT_SLOT_OFFHAND], xmog->slottedItems[EQUIPMENT_SLOT_RANGED], xmog->slottedItems[EQUIPMENT_SLOT_TABARD]);
        CharacterDatabase.CommitTransaction(trans);
    }

    uint8 FirstOpenXmogSlot(Player* player) {
        auto playerSets = XmogSets[player->GetGUID().GetCounter()];
        int i = 0;
        for (auto it = playerSets.cbegin(), end = playerSets.cend();
            it != end && i == it->first; ++it, ++i)
        {
        }
        return i;
    }

    void AddTalentTrees()
    {
        QueryResult talentTab = WorldDatabase.Query("SELECT * FROM forge_talent_tabs order by `id` asc");

        if (!talentTab)
            return;

        _cacheClassNodeToClassTree.clear();
        _cacheClassNodeToSpell.clear();
        _playerClassFirstSpec.clear();

        do
        {
            Field* talentFields = talentTab->Fetch();
            ForgeTalentTab* newTab = new ForgeTalentTab();
            newTab->Id = talentFields[0].Get<uint32>();
            newTab->ClassMask = talentFields[1].Get<uint32>();
            newTab->RaceMask = talentFields[2].Get<uint32>();
            newTab->Name = talentFields[3].Get<std::string>();
            newTab->SpellIconId = talentFields[4].Get<uint32>();
            newTab->Background = talentFields[5].Get<std::string>();
            newTab->Description = talentFields[6].Get<std::string>();
            newTab->Role = talentFields[7].Get<uint8>();
            newTab->SpellString = talentFields[8].Get<std::string>();
            newTab->TalentType = (CharacterPointType)talentFields[9].Get<uint8>();
            newTab->TabIndex = talentFields[10].Get<uint32>();

            for (auto& race : RaceAndClassTabMap)
            {
                auto bit = (newTab->RaceMask & (1 << (race.first - 1)));

                if (newTab->RaceMask != 0 && bit == 0)
                    continue;

                for (const auto& wowClass : race.second)
                {
                    auto classBit = (newTab->ClassMask & (1 << (wowClass.first - 1)));

                    if (classBit != 0 || newTab->ClassMask == 0)
                    {
                        auto firstSpec = _playerClassFirstSpec.find(classBit);
                        if (firstSpec != _playerClassFirstSpec.end()) {
                            if (newTab->Id < firstSpec->second)
                                _playerClassFirstSpec[classBit] = newTab->Id;
                        }
                        else
                            _playerClassFirstSpec[classBit] = newTab->Id;

                        RaceAndClassTabMap[race.first][wowClass.first].insert(newTab->Id);
                        SpellToTalentTabMap[newTab->SpellIconId] = newTab->Id;
                        TalentTabToSpellMap[newTab->Id] = newTab->SpellIconId;
                        CharacterPointTypeToTalentTabIds[newTab->TalentType].insert(newTab->Id);
                        if (newTab->TalentType == CharacterPointType::CLASS_TREE) {
                            _cacheClassNodeToSpell[wowClass.first] = {};
                            _cacheClassNodeToClassTree[wowClass.first] = newTab->Id;
                        }
                    }
                }
            }

            TalentTabs[newTab->Id] = newTab;
        } while (talentTab->NextRow());

    }

    void AddTalentsToTrees()
    {
        QueryResult talents = WorldDatabase.Query("SELECT * FROM forge_talents order by `talentTabId` asc, `rowIndex` desc, `columnIndex` asc");

        _cacheTreeMetaData.clear();
        _cacheSpecNodeToSpell.clear();
        _cacheClassNodeToSpell.clear();

        if (!talents)
            return;

        int i = 1;
        auto prevTab = -1;
        do
        {
            Field* talentFields = talents->Fetch();
            ForgeTalent* newTalent = new ForgeTalent();
            newTalent->SpellId = talentFields[0].Get<uint32>();
            newTalent->TalentTabId = talentFields[1].Get<uint32>();
            newTalent->ColumnIndex = talentFields[2].Get<uint32>();
            newTalent->RowIndex = talentFields[3].Get<uint32>();
            newTalent->RankCost = talentFields[4].Get<uint8>();
            newTalent->RequiredLevel = talentFields[5].Get<uint8>();
            newTalent->TalentType = (CharacterPointType)talentFields[6].Get<uint8>();
            newTalent->NumberOfRanks = talentFields[7].Get<uint8>();
            newTalent->PreReqType = (PereqReqirementType)talentFields[8].Get<uint8>();
            newTalent->TabPointReq = talentFields[9].Get<uint16>();
            newTalent->nodeType = NodeType(talentFields[10].Get<uint8>());

            if (prevTab != newTalent->TalentTabId) {
                prevTab = newTalent->TalentTabId;
                i = 1;
            }

            newTalent->nodeIndex = i++;
            if (newTalent->TalentType != CharacterPointType::CLASS_TREE)
                _cacheSpecNodeToSpell[newTalent->TalentTabId][newTalent->nodeIndex] = newTalent->SpellId;
            else {
                auto demaskClass = (TalentTabs[newTalent->TalentTabId]->ClassMask >> 1) + 1;
                _cacheClassNodeToSpell[demaskClass][newTalent->nodeIndex] = newTalent->SpellId;
            }

            auto tabItt = TalentTabs.find(newTalent->TalentTabId);

            if (tabItt == TalentTabs.end())
            {
                LOG_ERROR("FORGE.ForgeCache", "Error loading talents, invalid tab id: " + std::to_string(newTalent->TalentTabId));
            }
            else
                tabItt->second->Talents[newTalent->SpellId] = newTalent;

            // get treemeta from struct
            auto found = _cacheTreeMetaData.find(newTalent->TalentTabId);
            TreeMetaData* data;

            if (found == _cacheTreeMetaData.end()) {
                TreeMetaData* tree = new TreeMetaData();
                tree->MaxXDim = newTalent->ColumnIndex;
                tree->MaxYDim = newTalent->RowIndex;
                tree->TabId = newTalent->TalentTabId;
                _cacheTreeMetaData[tree->TabId] = tree;
                data = _cacheTreeMetaData[tree->TabId];
            }
            else {
                if (newTalent->RowIndex > found->second->MaxYDim)
                    found->second->MaxYDim = newTalent->RowIndex;
                if (newTalent->ColumnIndex > found->second->MaxXDim)
                    found->second->MaxXDim = newTalent->ColumnIndex;

                data = found->second;
            }
            NodeMetaData* node = new NodeMetaData();
            node->spellId = newTalent->SpellId;
            node->pointReq = newTalent->TabPointReq;
            node->col = newTalent->ColumnIndex;
            node->row = newTalent->RowIndex;
            node->nodeIndex = newTalent->nodeIndex;

            data->nodes[node->row][node->col] = node;
            data->nodeLocation[node->spellId] = node;
        } while (talents->NextRow());

        // load meta data
    }

    void AddTalentPrereqs()
    {
        QueryResult preReqTalents = WorldDatabase.Query("SELECT * FROM forge_talent_prereq");

        if (!preReqTalents)
            return;

        do
        {
            Field* talentFields = preReqTalents->Fetch();
            ForgeTalentPrereq* newTalent = new ForgeTalentPrereq();
            newTalent->reqId = talentFields[0].Get<uint32>();
            newTalent->Talent = talentFields[3].Get<uint32>();
            newTalent->TalentTabId = talentFields[4].Get<uint32>();
            newTalent->RequiredRank = talentFields[5].Get<uint32>();

            auto reqdSpellId = talentFields[1].Get<uint32>();
            auto reqSpelltab = talentFields[2].Get<uint32>();

            if (!TalentTabs.empty()) {
                auto tab = TalentTabs.find(reqSpelltab);
                if (tab != TalentTabs.end()) {
                    auto talent = tab->second->Talents.find(reqdSpellId);
                    if (talent != tab->second->Talents.end()) {
                        ForgeTalent* lt = talent->second;
                        if (lt != nullptr)
                            lt->Prereqs.push_back(newTalent);
                        else
                            LOG_ERROR("FORGE.ForgeCache", "Error loading AddTalentPrereqs, invaild req id: " + std::to_string(newTalent->reqId));

                        auto found = _cacheTreeMetaData.find(reqSpelltab);
                        if (found != _cacheTreeMetaData.end()) {
                            TreeMetaData* tree = found->second;
                            NodeMetaData* node = tree->nodeLocation[newTalent->Talent];
                            node->unlocks.push_back(tree->nodeLocation[reqdSpellId]);
                            tree->nodeLocation[newTalent->Talent] = node;
                            tree->nodes[node->row][node->col] = node;
                        }
                        else
                            LOG_ERROR("FORGE.ForgeCache", "Prereq cannot be mapped to existing talent meta data.");
                    }
                    else {
                        LOG_ERROR("FORGE.ForgeCache", "Prereq spell not found in tab.");
                    }
                }
                else {
                    LOG_ERROR("FORGE.ForgeCache", "Prereq spell tab not found.");
                }
            }
            else {
                LOG_ERROR("FORGE.ForgeCache", "Talent tab store is empty.");
            }
        } while (preReqTalents->NextRow());
    }

    void AddTalentChoiceNodes()
    {
        QueryResult exclTalents = WorldDatabase.Query("SELECT * FROM forge_talent_choice_nodes");

        _choiceNodes.clear();
        _choiceNodesRev.clear();
        _choiceNodeIndexLookup.clear();

        if (!exclTalents)
            return;

        do
        {
            Field* talentFields = exclTalents->Fetch();
            uint32 choiceNodeId = talentFields[0].Get<uint32>();
            uint32 talentTabId = talentFields[1].Get<uint32>();
            uint8 choiceIndex = talentFields[2].Get<uint8>();
            uint32 spellChoice = talentFields[3].Get<uint32>();

            ForgeTalentChoice* choice = new ForgeTalentChoice();
            choice->active = false;
            choice->spellId = spellChoice;

            _choiceNodes[choiceNodeId].push_back(spellChoice);
            _choiceNodesRev[spellChoice] = choiceNodeId;
            _choiceNodeIndexLookup[choiceIndex] = spellChoice;

            ForgeTalent* lt = TalentTabs[talentTabId]->Talents[choiceNodeId];
            if (lt != nullptr)
            {
                lt->Choices[choiceIndex] = choice;
            }
            else
            {
                LOG_ERROR("FORGE.ForgeCache", "Error loading AddTalentChoiceNodes, invaild choiceNodeId id: " + std::to_string(choiceNodeId));
            }

        } while (exclTalents->NextRow());
    }

    void AddCharacterChoiceNodes() {
        QueryResult choiceQuery = CharacterDatabase.Query("SELECT * FROM forge_character_node_choices");

        if (!choiceQuery)
            return;

        do
        {
            Field* fields = choiceQuery->Fetch();
            uint32 id = fields[0].Get<uint32>();
            ObjectGuid characterGuid = ObjectGuid::Create<HighGuid::Player>(id);
            uint32 specId = fields[1].Get<uint32>();
            uint32 TabId = fields[2].Get<uint32>();
            uint32 nodeId = fields[3].Get<uint32>();
            uint32 chosenSpell = fields[4].Get<uint32>();

            ForgeTalent* ft = TalentTabs[TabId]->Talents[nodeId];
            if (ft->nodeType == NodeType::CHOICE) {
                ForgeCharacterSpec* spec = CharacterSpecs[characterGuid][specId];
                spec->ChoiceNodesChosen[nodeId] = chosenSpell;
            }

        } while (choiceQuery->NextRow());
    }

    void AddTalentRanks()
    {
        QueryResult talentRanks = WorldDatabase.Query("SELECT * FROM forge_talent_ranks");

        if (!talentRanks)
            return;

        do
        {
            Field* talentFields = talentRanks->Fetch();
            uint32 talentspellId = talentFields[0].Get<uint32>();
            uint32 talentTabId = talentFields[1].Get<uint32>();
            uint32 rank = talentFields[2].Get<uint32>();
            uint32 spellId = talentFields[3].Get<uint32>();

            ForgeTalent* lt = TalentTabs[talentTabId]->Talents[talentspellId];

            if (lt != nullptr)
            {
                lt->Ranks[rank] = spellId;
                lt->RanksRev[spellId] = rank;
            }
            else
            {
                LOG_ERROR("FORGE.ForgeCache", "Error loading AddTalentRanks, invaild talentTabId id: " + std::to_string(talentTabId) + " Rank: " + std::to_string(rank) + " SpellId: " + std::to_string(spellId));
            }

        } while (talentRanks->NextRow());
    }

    void AddTalentUnlearn()
    {
        QueryResult exclTalents = WorldDatabase.Query("SELECT * FROM forge_talent_unlearn");

        if (!exclTalents)
            return;

        do
        {
            Field* talentFields = exclTalents->Fetch();
            uint32 spellId = talentFields[1].Get<uint32>();
            uint32 talentTabId = talentFields[0].Get<uint32>();
            uint32 exclusiveSpellId = talentFields[2].Get<uint32>();

            ForgeTalent* lt = TalentTabs[talentTabId]->Talents[spellId];

            if (lt != nullptr)
            {
                lt->UnlearnSpells.push_back(exclusiveSpellId);
            }
            else
            {
                LOG_ERROR("FORGE.ForgeCache", "Error loading AddTalentUnlearn, invaild talentTabId id: " + std::to_string(talentTabId) + " ExclusiveSpell: " + std::to_string(exclusiveSpellId) + " SpellId: " + std::to_string(spellId));
            }

        } while (exclTalents->NextRow());
    }

    void AddCharacterSpecs()
    {
        QueryResult charSpecs = CharacterDatabase.Query("SELECT * FROM forge_character_specs");

        if (!charSpecs)
            return;

        do
        {
            Field* specFields = charSpecs->Fetch();
            ForgeCharacterSpec* spec = new ForgeCharacterSpec();
            spec->Id = specFields[0].Get<uint32>();
            spec->CharacterGuid = ObjectGuid::Create<HighGuid::Player>(specFields[1].Get<uint32>());
            spec->Name = specFields[2].Get<std::string>();
            spec->Description = specFields[3].Get<std::string>();
            spec->Active = specFields[4].Get<bool>();
            spec->SpellIconId = specFields[5].Get<uint32>();
            spec->Visability = (SpecVisibility)specFields[6].Get<uint8>();
            spec->CharacterSpecTabId = specFields[7].Get<uint32>();

            if (spec->Active)
                CharacterActiveSpecs[spec->CharacterGuid] = spec->Id;

            CharacterSpecs[spec->CharacterGuid][spec->Id] = spec;
        } while (charSpecs->NextRow());
    }

    void AddTalentSpent()
    {
        QueryResult exclTalents = CharacterDatabase.Query("SELECT * FROM forge_character_talents_spent");

        if (!exclTalents)
            return;

        do
        {
            Field* talentFields = exclTalents->Fetch();
            uint32 id = talentFields[0].Get<uint32>();
            uint32 spec = talentFields[1].Get<uint32>();
            uint32 tabId = talentFields[2].Get<uint32>();

            if (spec != ACCOUNT_WIDE_KEY)
            {
                ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(id);

                CharacterSpecs[guid][spec]->PointsSpent[tabId] = talentFields[3].Get<uint8>();
            }
            else
            {
                auto aws = AccountWideCharacterSpecs.find(id);


                if (aws == AccountWideCharacterSpecs.end())
                    AccountWideCharacterSpecs[id] = new ForgeCharacterSpec();

                AccountWideCharacterSpecs[id]->PointsSpent[tabId] = talentFields[3].Get<uint8>();

                for (auto& ch : PlayerCharacterMap[id])
                    for (auto& spec : CharacterSpecs[ch])
                        spec.second->PointsSpent[tabId] = talentFields[3].Get<uint8>();
            }

        } while (exclTalents->NextRow());
    }

    void AddCharacterTalents()
    {
        QueryResult talentsQuery = CharacterDatabase.Query("SELECT * FROM forge_character_talents");

        if (!talentsQuery)
            return;

        do
        {
            Field* talentFields = talentsQuery->Fetch();
            uint32 id = talentFields[0].Get<uint32>();
            ObjectGuid characterGuid = ObjectGuid::Create<HighGuid::Player>(id);
            uint32 specId = talentFields[1].Get<uint32>();
            ForgeCharacterTalent* talent = new ForgeCharacterTalent();
            talent->SpellId = talentFields[2].Get<uint32>();
            talent->TabId = talentFields[3].Get<uint32>();
            talent->CurrentRank = talentFields[4].Get<uint8>();

            auto invalid = false;

            if (specId != ACCOUNT_WIDE_KEY)
            {
                ForgeTalent* ft = TalentTabs[talent->TabId]->Talents[talent->SpellId];
                ForgeCharacterSpec* spec = CharacterSpecs[characterGuid][specId];
                talent->type = ft->nodeType;
                spec->Talents[talent->TabId][talent->SpellId] = talent;
            }
            else
            {
                AccountWideCharacterSpecs[id]->Talents[talent->TabId][talent->SpellId] = talent;

                for (auto& ch : PlayerCharacterMap[id])
                    for (auto& spec : CharacterSpecs[ch])
                    {
                        ForgeTalent* ft = TalentTabs[talent->TabId]->Talents[talent->SpellId];
                        talent->type = ft->nodeType;
                        spec.second->Talents[talent->TabId][talent->SpellId] = talent;
                    }
            }

        } while (talentsQuery->NextRow());
    }

    void AddCharacterPointsFromDB()
    {
        QueryResult pointsQuery = CharacterDatabase.Query("SELECT * FROM forge_character_points");

        if (!pointsQuery)
            return;
        uint32 almostMax = UINT_MAX - 1;

        do
        {
            Field* pointsFields = pointsQuery->Fetch();
            uint32 guid = pointsFields[0].Get<uint32>();
            CharacterPointType pt = (CharacterPointType)pointsFields[1].Get<uint8>();
            ForgeCharacterPoint* cp = new ForgeCharacterPoint();
            cp->PointType = pt;
            cp->SpecId = pointsFields[2].Get<uint32>();
            cp->Sum = pointsFields[3].Get<uint32>();
            cp->Max = pointsFields[4].Get<uint32>();

            if (guid == UINT_MAX)
            {
                MaxPointDefaults[pt] = cp;
            }
            else
            {
                if (cp->SpecId == ACCOUNT_WIDE_KEY)
                {
                    AccountWidePoints[guid][pt] = cp;

                    for (auto& ch : PlayerCharacterMap[guid])
                        for (auto& spec : CharacterSpecs[ch])
                            CharacterPoints[ch][cp->PointType][spec.first] = cp;

                }
                else
                {
                    ObjectGuid og = ObjectGuid::Create<HighGuid::Player>(guid);
                    CharacterPoints[og][cp->PointType][cp->SpecId] = cp;
                }
            }

        } while (pointsQuery->NextRow());
    }

    void AddCharacterClassSpecs()
    {
        /* QueryResult specsQuery = WorldDatabase.Query("SELECT sl.DisplayName_Lang_enUS as specName, sl.SpellIconID as specIcon, src.ClassMask, src.RaceMask, sl.id FROM acore_world.db_SkillLine_12340 sl LEFT JOIN acore_world.db_SkillRaceClassInfo_12340 src ON src.SkillID = sl.id WHERE sl.CategoryID = 7 AND ClassMask IS NOT NULL AND sl.SpellIconID != 1 AND sl.SpellIconID != 0 order by src.ClassMask asc, sl.DisplayName_Lang_enUS asc");


         if (!specsQuery)
             return;

         do
         {
             Field* spec = specsQuery->Fetch();
             ClassSpecDetail* cs = new ClassSpecDetail();
             cs->Name = spec[0].Get<std::string>();
             cs->SpellIconId = spec[1].Get<uint32>();
             cs->SpecId = spec[4].Get<uint32>();

             uint32 classMask = spec[2].Get<uint32>();
             std::string raceMMask = spec[3].Get<std::string>();

             if (raceMMask == "-1")
             {
                 for (const auto& race : RACE_LIST)
                     ClassSpecDetails[race][classMask].push_back(cs);
             }
             else
             {
                 ClassSpecDetails[static_cast<uint32_t>(std::stoul(raceMMask))][classMask].push_back(cs);
             }

         } while (specsQuery->NextRow());*/
    }

    void AddPlayerSpellScaler()
    {
        QueryResult scale = WorldDatabase.Query("SELECT * FROM forge_player_spell_scale");

        if (!scale)
            return;

        do
        {
            Field* talentFields = scale->Fetch();
            uint32 id = talentFields[0].Get<uint32>();
            float data = talentFields[1].Get<float>();

            PlayerSpellScaleMap[id] = data;

        } while (scale->NextRow());
    }

    void AddSpellUnlearnFlags()
    {
        FlaggedForUnlearn.clear();

        std::unordered_map<uint32 /*guid*/, std::vector<uint32 /*spell*/>> SpellLearnedAdditionalSpells;
        QueryResult flag = WorldDatabase.Query("SELECT * FROM `forge_talent_spell_flagged_unlearn`");

        if (!flag)
            return;

        do
        {
            Field* talentFields = flag->Fetch();
            uint32 guid = talentFields[0].Get<uint32>();
            uint32 spell = talentFields[1].Get<float>();

            FlaggedForUnlearn[guid] = spell;

        } while (flag->NextRow());
    }

    void AddSpellLearnAdditionalSpells()
    {
        SpellLearnedAdditionalSpells.clear();

        std::unordered_map<uint32 /*guid*/, std::vector<uint32 /*spell*/>> SpellLearnedAdditionalSpells;
        QueryResult added = WorldDatabase.Query("SELECT * FROM `forge_talent_learn_additional_spell`");

        if (!added)
            return;

        do
        {
            Field* talentFields = added->Fetch();
            uint32 rootSpell = talentFields[0].Get<uint32>();
            uint32 addedSpell = talentFields[1].Get<float>();

            SpellLearnedAdditionalSpells[rootSpell].push_back(addedSpell);

        } while (added->NextRow());
    }

    void AddLevelClassSpellMap()
    {
        _levelClassSpellMap.clear();

        QueryResult mapQuery = WorldDatabase.Query("select * from `acore_world`.`forge_character_spec_spells` order by `class` asc, `race` asc, `level` asc, `spell` asc");

        if (!mapQuery)
            return;

        do
        {
            Field* mapFields = mapQuery->Fetch();
            uint8 pClass = mapFields[0].Get<uint8>();
            uint32 race = mapFields[1].Get<uint32>();
            uint8 level = mapFields[2].Get<uint8>();
            uint32 spell = mapFields[3].Get<uint32>();

            _levelClassSpellMap[pClass][race][level].push_back(spell);
        } while (mapQuery->NextRow());
    }

    void AddPlayerTalentLoadouts()
    {
        _playerTalentLoadouts.clear();
        _playerActiveTalentLoadouts.clear();

        QueryResult loadouts = WorldDatabase.Query("select * from `acore_characters`.`forge_character_talent_loadouts`");

        if (!loadouts)
            return;

        do
        {
            Field* loadoutsFields = loadouts->Fetch();
            uint32 guid = loadoutsFields[0].Get<uint32>();
            uint32 id = loadoutsFields[1].Get<uint32>();
            uint32 tabId = loadoutsFields[2].Get<uint32>();
            std::string name = loadoutsFields[3].Get<std::string>();
            std::string talentString = loadoutsFields[4].Get<std::string>();
            bool active = loadoutsFields[5].Get<bool>();

            PlayerLoadout* plo = new PlayerLoadout();
            plo->id = id;
            plo->name = name;
            plo->talentString = talentString;
            plo->active = active;


            _playerTalentLoadouts[guid][id] = plo;
            _playerActiveTalentLoadouts[guid] = plo;
        } while (loadouts->NextRow());
    }

    // hater: custom items
    void AddItemSlotValue() {
        _forgeItemSlotValues.clear();

        QueryResult values = WorldDatabase.Query("select * from `acore_world`.`forge_item_slot_value`");

        if (!values)
            return;

        do
        {
            Field* valuesFields = values->Fetch();
            InventoryType inv = InventoryType(valuesFields[0].Get<uint32>());
            float value = valuesFields[1].Get<float>();

            _forgeItemSlotValues[inv] = value;
        } while (values->NextRow());
    }

    void AddItemStatValue() {
        _forgeItemStatValues.clear();

        QueryResult values = WorldDatabase.Query("select * from `acore_world`.`forge_item_stat_value`");

        if (!values)
            return;

        do
        {
            Field* valuesFields = values->Fetch();
            ItemModType stat = ItemModType(valuesFields[0].Get<uint32>());
            float value = valuesFields[1].Get<float>();

            _forgeItemStatValues[stat] = value;
        } while (values->NextRow());
    }

    void AddGemSlotsForItemSlot() {
        _genItemSlotsForEquipSlot.clear();

        QueryResult values = WorldDatabase.Query("select * from `acore_world`.`gen_item_slots`");

        if (!values)
            return;

        do
        {
            Field* valuesFields = values->Fetch();
            InventoryType inv = InventoryType(valuesFields[0].Get<uint32>());
            uint8 slots = valuesFields[1].Get<uint8>();

            _genItemSlotsForEquipSlot[inv] = slots;
        } while (values->NextRow());
    }

    void AddItemStatPools() {
        _forgeItemSecondaryStatPools.clear();

        QueryResult values = WorldDatabase.Query("select * from `acore_world`.`forge_item_stat_pool`");

        if (!values)
            return;

        do
        {
            Field* valuesFields = values->Fetch();
            uint8 mainstat = valuesFields[0].Get<uint8>();
            ItemModType secondarystat = ItemModType(valuesFields[1].Get<uint32>());

            _forgeItemSecondaryStatPools[mainstat].emplace_back(secondarystat);
        } while (values->NextRow());
    }

    void AddItemAppearances() {
        _genItemNamesAndDisplays.clear();
        //std::unordered_map<ItemQualities, std::unordered_map<ItemClass, std::unordered_map<uint8 /*subclass*/, std::unordered_map<uint32 /*displayid*/, std::string /*name*/>>>>
        QueryResult values = WorldDatabase.Query("select class, subclass, InventoryType, displayid, name from `acore_world`.`item_template` where class in (2, 4) and entry < 80000");

        if (!values)
            return;

        do
        {
            Field* valuesFields = values->Fetch();
            ItemClass iClass = ItemClass(valuesFields[0].Get<uint8>());
            uint8 subclass = valuesFields[1].Get<uint8>();
            uint8 invtype = valuesFields[2].Get<uint8>();
            uint32 display = valuesFields[3].Get<uint32>();
            std::string name = valuesFields[4].Get<std::string>();

            _genItemNamesAndDisplays[iClass][subclass][invtype][display] = name;
        } while (values->NextRow());
    }

    // perks
public:

    // hater: perks
    std::string FindRollKey(ForgeCharacterSpec* spec, std::string uuid)
    {
        std::string out = "NONE";
        for (auto roll : spec->perkQueue)
            for (auto perk : roll.second)
                if (perk->uuid == uuid)
                    out = roll.first;
        return out;
    }

    void PurgePerk(Player* player, CharacterSpecPerk* perk)
    {
        player->RemoveAura(perk->spell->spellId);
    }

    Perk* GetPerk(uint32 spellId)
    {
        if (AllPerks.find(spellId) != AllPerks.end()) {
            Perk* perk = AllPerks[spellId];
            return perk;
        }
        else
            return nullptr;
    }

    Perk* GetRandomPerk(Player* player)
    {
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<> cointflip(0,1);

        ForgeCharacterSpec* spec;
        if (TryGetCharacterActiveSpec(player, spec)) {

            auto perkSet = player->getClass();
            if (spec->CharacterSpecTabId)
                perkSet = cointflip(eng) ? std::log2(spec->CharacterSpecTabId) + 1 : player->getClass();

            auto perks = Perks[perkSet];
            if (!perks.empty()) {
                std::uniform_int_distribution<> distr(0, perks.size() - 1);
                auto index = distr(eng);
                auto it = perks[index];
                return it;
            }
        }
        return nullptr;
    }

    std::vector<Perk*> GetArchetypeForPlayer(Player* player) {
        return Archetypes[player->getClass()][player->GetLevel()];
    }

    bool perkEquals(Perk a, Perk b) {
        return a.spellId == b.spellId;
    }

    uint32 CountCharacterSpecPerkOccurences(Player* player, uint8 specId, Perk* perk)
    {
        ForgeCharacterSpec* charSpec;

        if (TryGetCharacterActiveSpec(player, charSpec)) {

            uint16 count = 0;

            auto perks = charSpec->perks;
            if (!perks.empty()) {
                auto csp = perks.find(perk->spellId);
                if (csp != perks.end())
                    count += csp->second->rank;
            }

            if (!charSpec->groupPerks.empty()) {
                auto gp = charSpec->groupPerks.find(perk->mutexGroup);
                if (gp != charSpec->groupPerks.end())
                    count += 1;
            }

            for (auto roll : charSpec->perkQueue)
                for (auto cperk : roll.second)
                    if (cperk->spell->spellId == perk->spellId)
                        count += 1;

            for (auto pp : charSpec->prestigePerks)
                if (pp->spell->spellId == perk->spellId)
                    count += 1;

            return count;
        }
        else
            return -1;
    }

    void PrestigePerks(Player* player)
    {
        std::unordered_map<uint32, ForgeCharacterSpec*> charSpecs = CharacterSpecs.at(player->GetGUID());
        for (auto i : charSpecs) {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            i.second->perkQueue.clear();
            i.second->prestigePerks.clear();
            trans->Append("delete from character_prestige_perk_carryover where `guid` = {} and specId = {}", player->GetGUID().GetCounter(), i.first);
            trans->Append("delete from character_perk_selection_queue where `guid` = {} and specId = {}", player->GetGUID().GetCounter(), i.first);

            for (auto perkMap : i.second->perks) {
                auto perk = perkMap.second;
                for (auto j = 1; j <= perk->rank; j++) {
                    trans->Append("INSERT INTO character_prestige_perk_carryover (`guid`, `specId`, `uuid`, `spellId`, `rank`) VALUES ({} , {} , '{}' , {} , {} )",
                        player->GetGUID().GetCounter(), i.first, perk->uuid, perk->spell->spellId, j);

                    CharacterSpecPerk* copy = new CharacterSpecPerk();
                    copy->uuid = perk->uuid;
                    copy->spell = perk->spell;
                    copy->rank = 1;
                    i.second->prestigePerks.push_back(copy);
                }
                PurgePerk(player, perk);
            }

            trans->Append("DELETE FROM character_spec_perks WHERE `guid` = {} AND `specId` = {}",
                player->GetGUID().GetCounter(), i.first);
            CharacterDatabase.CommitTransaction(trans);
            i.second->perks.clear();
        }
    }

    CharacterSpecPerk* GetPrestigePerk(Player* player) {
        ForgeCharacterSpec* spec;
        if (TryGetCharacterActiveSpec(player, spec)) {
            if (spec->prestigePerks.size() > 0) {
                std::random_device rd;
                std::mt19937 eng(rd());
                std::uniform_int_distribution<> distr(0, spec->prestigePerks.size() - 1);
                auto index = distr(eng);
                auto it = spec->prestigePerks[index];

                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                trans->Append("DELETE FROM character_prestige_perk_carryover WHERE `guid` = {} AND `specId` = {} AND `uuid` = '{}' AND `spellId` = {} LIMIT 1"
                    , player->GetGUID().GetCounter(), spec->Id, it->uuid, it->spell->spellId, it->rank);
                CharacterDatabase.CommitTransaction(trans);
                spec->prestigePerks.erase(spec->prestigePerks.begin() + index);

                return it;
            }
            else
                return nullptr;
        }
    }

    bool PerkInQueue(ForgeCharacterSpec* spec, uint32 spellId)
    {
        std::string out;
        if (!spec->perkQueue.empty())
            for (auto perk : spec->perkQueue.begin()->second)
                if (perk->spell->spellId == spellId)
                    return true;

        return false;
    }

    void InsertNewPerksForLevelUp(Player* player, ForgeCharacterSpec* spec)
    {
        ForgeCharacterPoint* pp = GetCommonCharacterPoint(player, CharacterPointType::PRESTIGE_COUNT);
        bool prestiged = pp->Sum > 0;
        uint8 maxPerks = 3;

        auto roll = boost::uuids::random_generator()();
        auto rollKey = boost::lexical_cast<std::string>(roll);
        auto guid = player->GetGUID().GetCounter();

        if (prestiged) {
            CharacterSpecPerk* perk = GetPrestigePerk(player);
            if (perk != nullptr) {
                InsertPerkSelection(player, perk->spell, rollKey, 1);
                maxPerks--;
            }
        }

        do {
            Perk* possibility = GetRandomPerk(player);

            uint32 count = CountCharacterSpecPerkOccurences(player, player->GetActiveSpec(), possibility);
            if ((count != -1) && ((count < possibility->maxRank))
                && std::find_if(spec->perkQueue[rollKey].begin(), spec->perkQueue[rollKey].end(),
                    [&possibility](CharacterSpecPerk* perk) {return perk->spell->spellId == possibility->spellId; }) == spec->perkQueue[rollKey].end())
            {
                InsertPerkSelection(player, possibility, rollKey, 0);
                maxPerks--;
            }
        } while (0 < maxPerks);
    }

    void ApplyActivePerks(Player* player)
    {
        ForgeCharacterSpec* currentSpec;

        if (TryGetCharacterActiveSpec(player, currentSpec))
        {
                for (auto perk : currentSpec->perks) {
                    auto currentRank = perk.second->rank;
                    auto spell = perk.second->spell;

                    if (auto spellInfo = sSpellMgr->GetSpellInfo(spell->spellId)) {
                        if (auto app = player->AddAura(spellInfo->Id, player))
                            app->SetStackAmount(currentRank);
                    }
                }
        }
    }

    void RemoveActivePerks(Player* player) {
        ForgeCharacterSpec* currentSpec;

        if (TryGetCharacterActiveSpec(player, currentSpec))
        {
            for (auto perk : currentSpec->perks) {
                auto spell = perk.second->spell->spellId;
                if (auto spellInfo = sSpellMgr->GetSpellInfo(spell)) {
                    player->RemoveAura(spell);
                }
            }
        }
    }

    bool TryGetCharacterPerksByType(Player* player, uint32 specId, OUT std::vector<CharacterSpecPerk*>& specPerks)
    {
        ForgeCharacterSpec* charSpec;
        if (!TryGetCharacterActiveSpec(player, charSpec))
            return false;

        for (auto perk : charSpec->perks)
            specPerks.push_back(perk.second);

        return specPerks.size() == charSpec->perks.size();
    }

    bool TryGetAllPerks(OUT std::vector<Perk*>& perks)
    {
        for (auto perk : AllPerks)
            perks.push_back(perk.second);

        return perks.size() == AllPerks.size();
    }

    int CountPerksByType(Player* player)
    {
        // TODO GRAB NUMBER FROM CATEGORY:COUNT MAP
        int count = 0;
        ForgeCharacterSpec* spec;
        if (TryGetCharacterActiveSpec(player, spec)) {
            auto perks = spec->perks;
            if (!perks.empty())
                for (auto perk : perks)
                    count += perk.second->rank;
        }
        return count;
    }

    bool TryGetCharacterPerks(Player* player, uint32 specId, OUT std::vector<CharacterSpecPerk*>& specPerks)
    {
        ForgeCharacterSpec* charSpec;
        if (TryGetCharacterActiveSpec(player, charSpec)) {
            auto count = 0;
            for (auto perk : charSpec->perks) {
                specPerks.push_back(perk.second);
                count++;
            }

            return specPerks.size() == count;
        }
        return false;
    }

    void LearnCharacterPerkInternal(Player* player, ForgeCharacterSpec* spec, CharacterSpecPerk* perk) {
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        trans->Append("INSERT INTO character_spec_perks (`guid`, `specId`, `uuid`, `spellId`, `rank`) VALUES ({}, {}, '{}', {}, {}) ON DUPLICATE KEY UPDATE `rank` = {}",
            player->GetGUID().GetCounter(), spec->Id, perk->uuid, perk->spell->spellId, perk->rank, perk->rank);
        trans->Append("INSERT INTO character_perk_roll_history select {} as `accountId`, `guid`, `rollKey`, `specId`, `spellId`, {} as `level`, 0 as `carryover` from character_perk_selection_queue where rollkey = '{}' ON DUPLICATE KEY UPDATE `carryover` = `carryover`+1",
            player->GetSession()->GetAccountId(), player->GetLevel(), perk->uuid);
        trans->Append("DELETE FROM character_spec_perks where `guid` = {} and `specId` = {} and `rank` = 0", player->GetGUID().GetCounter(), spec->Id);

        CharacterDatabase.CommitTransaction(trans);
    }

private:

    void InsertPerkSelection(Player* player, Perk* perk, std::string rollKey, uint8 carryover)
    {
        ForgeCharacterSpec* charSpec;

        if (TryGetCharacterActiveSpec(player, charSpec)) {

            CharacterDatabase.DirectExecute("INSERT INTO character_perk_selection_queue (`guid`, `specId`, `rollkey`, `spellId`) VALUES ({}, {}, '{}', {} )",
                player->GetGUID().GetCounter(), charSpec->Id, rollKey, perk->spellId);

            CharacterSpecPerk* csp = new CharacterSpecPerk();
            csp->spell = perk;
            csp->uuid = rollKey;
            csp->rank = 1;
            csp->carryover = carryover;

            charSpec->perkQueue[rollKey].push_back(csp);
        }
    }

    void AddPerks()
    {
        Perks.clear();
        AllPerks.clear();
        LOG_INFO("server.load", "Loading all perks...");
        QueryResult perks = WorldDatabase.Query("SELECT * FROM perks ORDER BY `associatedClass` ASC");
        if (!perks) return;

        do
        {
            Field* perkFields = perks->Fetch();
            Perk* newPerk = new Perk();
            newPerk->spellId = perkFields[0].Get<uint32>();
            newPerk->maxRank = perkFields[1].Get<uint8>();
            newPerk->associatedClass = perkFields[2].Get<uint32>();
            newPerk->isAura = perkFields[3].Get<uint8>() == 0 ? false : true;
            newPerk->mutexGroup = perkFields[4].Get<uint8>();
            newPerk->tags = perkFields[5].Get<std::string>();

            auto val = newPerk->associatedClass;
            if (val > 0) {
                if (val & (1 << (CLASS_WARRIOR - 1)))
                    Perks[CLASS_WARRIOR].push_back(newPerk);

                if (val & (1 << (CLASS_PALADIN - 1)))
                    Perks[CLASS_PALADIN].push_back(newPerk);

                if (val & (1 << (CLASS_HUNTER - 1)))
                    Perks[CLASS_HUNTER].push_back(newPerk);

                if (val & (1 << (CLASS_ROGUE - 1)))
                    Perks[CLASS_ROGUE].push_back(newPerk);

                if (val & (1 << (CLASS_PRIEST - 1)))
                    Perks[CLASS_PRIEST].push_back(newPerk);

                if (val & (1 << (CLASS_DEATH_KNIGHT - 1)))
                    Perks[CLASS_DEATH_KNIGHT].push_back(newPerk);

                if (val & (1 << (CLASS_SHAMAN - 1)))
                    Perks[CLASS_SHAMAN].push_back(newPerk);

                if (val & (1 << (CLASS_MAGE - 1)))
                    Perks[CLASS_MAGE].push_back(newPerk);

                if (val & (1 << (CLASS_WARLOCK - 1)))
                    Perks[CLASS_WARLOCK].push_back(newPerk);

                if (val & (1 << (CLASS_SHAPESHIFTER - 1)))
                    Perks[CLASS_SHAPESHIFTER].push_back(newPerk);

                if (val & (1 << (CLASS_DRUID - 1)))
                    Perks[CLASS_DRUID].push_back(newPerk);
            }
            else {
                Perks[CLASS_WARRIOR].push_back(newPerk);
                Perks[CLASS_PALADIN].push_back(newPerk);
                Perks[CLASS_HUNTER].push_back(newPerk);
                Perks[CLASS_ROGUE].push_back(newPerk);
                Perks[CLASS_PRIEST].push_back(newPerk);
                Perks[CLASS_DEATH_KNIGHT].push_back(newPerk);
                Perks[CLASS_SHAMAN].push_back(newPerk);
                Perks[CLASS_MAGE].push_back(newPerk);
                Perks[CLASS_WARLOCK].push_back(newPerk);
                Perks[CLASS_SHAPESHIFTER].push_back(newPerk);
                Perks[CLASS_DRUID].push_back(newPerk);
            }

            AllPerks[newPerk->spellId] = newPerk;
        } while (perks->NextRow());
    }

    void AddCharacterPerks()
    {
        LOG_INFO("server.load", "Loading character perks...");
        QueryResult perkQuery = CharacterDatabase.Query("SELECT* FROM `character_spec_perks`");
        if (!perkQuery) return;

        do {
            Field* perkFields = perkQuery->Fetch();
            uint32 guid = perkFields[0].Get<uint64>();
            ObjectGuid characterGuid = ObjectGuid::Create<HighGuid::Player>(guid);
            uint8 specId = perkFields[1].Get<uint8>();
            uint8 type = perkFields[2].Get<uint8>();
            std::string uuid = perkFields[3].Get<std::string>();
            uint32 spellId = perkFields[4].Get<uint32>();
            uint8 rank = perkFields[5].Get<uint8>();

            CharacterSpecPerk* perk = new CharacterSpecPerk();
            if (Perk* copy = GetPerk(spellId)) {
                perk->spell = copy;
                perk->rank = rank;
                perk->uuid = uuid;

                ForgeCharacterSpec* spec = CharacterSpecs[characterGuid][specId];

                spec->perks[spellId] = perk;

                if (copy->mutexGroup > 0)
                    spec->groupPerks[copy->mutexGroup] = copy;
            }
        } while (perkQuery->NextRow());
    }

    void AddCharacterQueuedPerks()
    {
        LOG_INFO("server.load", "Loading character perk queues...");
        QueryResult queueQuery = CharacterDatabase.Query("SELECT * FROM `character_perk_selection_queue`");
        if (!queueQuery) return;

        do {
            Field* selectionFields = queueQuery->Fetch();
            uint32 guid = selectionFields[0].Get<uint64>();
            ObjectGuid characterGuid = ObjectGuid::Create<HighGuid::Player>(guid);
            uint8 specId = selectionFields[1].Get<uint8>();
            uint8 type = selectionFields[2].Get<uint8>();
            std::string rollKey = selectionFields[3].Get<std::string>();
            uint32 spellId = selectionFields[4].Get<uint32>();

            CharacterSpecPerk* perk = new CharacterSpecPerk();
            Perk* copy = GetPerk(spellId);
            perk->spell = copy;
            perk->rank = 1;
            perk->uuid = rollKey;

            ForgeCharacterSpec* spec = CharacterSpecs[characterGuid][specId];
            spec->perkQueue[rollKey].push_back(perk);
            if (copy->mutexGroup > 0)
                spec->groupPerks[copy->mutexGroup] = copy;
        } while (queueQuery->NextRow());
    }

    void AddCharacterPrestigePerks()
    {
        LOG_INFO("server.load", "Loading character prestige perks...");
        QueryResult prestigeQuery = CharacterDatabase.Query("SELECT * FROM `character_prestige_perk_carryover`");
        if (!prestigeQuery) return;

        do {
            Field* perkFields = prestigeQuery->Fetch();
            uint32 guid = perkFields[0].Get<uint64>();
            ObjectGuid characterGuid = ObjectGuid::Create<HighGuid::Player>(guid);
            uint8 specId = perkFields[1].Get<uint8>();
            uint8 type = perkFields[2].Get<uint8>();
            std::string uuid = perkFields[3].Get<std::string>();
            uint32 spellId = perkFields[4].Get<uint32>();
            uint8 rank = perkFields[5].Get<uint32>();

            CharacterSpecPerk* perk = new CharacterSpecPerk();
            perk->spell = GetPerk(spellId);
            perk->uuid = uuid;
            perk->rank = 1;

            ForgeCharacterSpec* spec = CharacterSpecs[characterGuid][specId];
            spec->prestigePerks.push_back(perk);
        } while (prestigeQuery->NextRow());
    }

    // soul shards
public:
#define MAX_SOUL_GROUPS 3
#define SOUL_SHARD_UPGRADE_THRESHOLD 2
#define SOUL_SHARD_UPGRADE_MAX 2
#define MAX_ACTIVE_SHARDS 5

    enum SoulShardQuality {
        NULLSHARD = 0,
        SHARD_COMMON = 1,
        SHARD_UNCOMMON = 2,
        SHARD_RARE = 3,
        SHARD_EPIC = 4,
        SHARD_LEGENDARY = 5,
    };

#define SHARD_GROUP_SIZE 8
    enum SoulTypeGroups {
        BEAST = CREATURE_TYPE_BEAST,
        DRAGONKIN = CREATURE_TYPE_DRAGONKIN,
        DEMON = CREATURE_TYPE_DEMON,
        ELEMENTAL = CREATURE_TYPE_ELEMENTAL,
        GIANT = CREATURE_TYPE_GIANT,
        UNDEAD = CREATURE_TYPE_UNDEAD,
        HUMANOID = CREATURE_TYPE_HUMANOID,
        MECHANICAL = CREATURE_TYPE_MECHANICAL,
    };

    enum SoulGroupBonus {
        BEAST_AURA = 9000000,
        DRAGONKIN_AURA = 9000001,
        DEMON_AURA = 9000002,
        ELEMENTAL_AURA = 9000003,
        GIANT_AURA = 9000004,
        UNDEAD_AURA = 9000005,
        HUMANOID_AURA = 9000006,
        MECHANICAL_AURA = 9000007,
    };

    struct SoulShard {
        uint32 source = 0;
        SoulShardQuality quality = NULLSHARD;
        uint8 groups[MAX_SOUL_GROUPS] = { };
        uint32 special = 0;
    };

    struct PlayerSoulShard {
        uint32 source;
        uint8 count;
        uint8 rank;
        SoulShard* shard;

        PlayerSoulShard(uint32 source, uint8 count, uint8 rank, SoulShard* shard) {
            this->source = source;
            this->count = count;
            this->rank = rank;
            this->shard = shard;
        }

        PlayerSoulShard() {
            this->source = 0;
            this->rank = 0;
            this->count = 0;
            this->shard = new SoulShard();
        }
    };

    SoulShard* GetSoulShard(uint32 source) {
        auto found = SoulShards.find(source);
        return found != SoulShards.end() ? found->second : nullptr;
    }

    void CreateSoulShardFor(CreatureTemplate const* killed) {
        auto source = killed->Entry;
        auto groupOne = killed->type;

        SoulShard* shard = new SoulShard();
        switch (killed->rank) {
        case CREATURE_ELITE_ELITE:
        case CREATURE_ELITE_RAREELITE:
            shard->quality = SHARD_UNCOMMON;
            break;
        case CREATURE_ELITE_WORLDBOSS:
            shard->quality = SHARD_RARE;
            break;
        default:
            shard->quality = SHARD_COMMON;
            break;
        }
        shard->source = source;
        shard->special = 0; // TODO: eventually map to raid
        for (int i = 0; i < MAX_SOUL_GROUPS; i++)
        {
            switch (i) {
            case 0:
                shard->groups[i] = killed->type;
                break;
            case 1:
                shard->groups[i] = 0;
                break;
            case 2:
                shard->groups[i] = 0;
                break;
            }
        }

        WorldDatabase.DirectExecute("INSERT INTO acore_world.soul_shards (source, quality, group1, group2, group3, bonus_effect) VALUES({}, {}, {}, {}, {}, {})",
            shard->source, shard->quality, shard->groups[0], shard->groups[1], shard->groups[2], shard->special);

        SoulShards[source] = shard;
    }

    void HandleSoulShard(Player* player, uint32 source)
    {
        if (source) {
            if (SoulShard* shard = GetSoulShard(source)) {
                auto account = player->GetSession()->GetAccountId();
                auto acctFound = _charSoulShards.find(account);
                if (acctFound != _charSoulShards.end()) {
                    auto shardFound = _charSoulShards[account].find(source);
                    if (shardFound != _charSoulShards[account].end()) {
                        PlayerSoulShard* pShard = _charSoulShards[account][source];
                        pShard->count += 1;
                        if (pShard->rank < pShard->shard->quality + 2) {
                            if (pShard->count > std::pow(SOUL_SHARD_UPGRADE_THRESHOLD, pShard->rank)) {
                                pShard->count = 1;
                                pShard->rank += 1;
                            }
                            _charSoulShards[account][source] = pShard;
                            HandleSoulShardAcquired(player, shard, false);
                        }
                        player->AddItem(RewardItems::SOUL_DUST, 1);
                        return;
                    }
                }
                HandleSoulShardAcquired(player, shard, true);
            }
        }
    }

    std::unordered_map<uint32 /*source*/, PlayerSoulShard*> GetPlayerSoulShards(Player* player) {
        std::unordered_map<uint32 /*source*/, PlayerSoulShard*> out;
        auto foundPlayer = _charSoulShards.find(player->GetSession()->GetAccountId());
        if (foundPlayer != _charSoulShards.end()) {
            return foundPlayer->second;
        }
        return out;
    }

    void SetSoulShardSlotFor(PlayerSoulShard* shard, uint8 slot, Player* player) {
        _charActiveShards[player->GetGUID().GetCounter()][slot] = shard;
        SaveActiveShards(player);
        RecalculateShardBonuses(player);

        SendActiveSoulShards(player);
    }

    void RecalculateShardBonuses(Player* player) {
        auto pChar = player->GetGUID().GetCounter();
        std::unordered_map<SoulTypeGroups, uint8> blank = {
            {BEAST, 0}, {DRAGONKIN, 0}, {DEMON, 0},{ELEMENTAL, 0} ,{GIANT, 0} ,{UNDEAD, 0},{HUMANOID, 0} ,{MECHANICAL, 0} };
        for (auto shard : _charActiveShards[pChar]) {
            for (int i = 0; i < MAX_SOUL_GROUPS; i++) {
                if (auto s = shard.second) {
                    auto type = SoulTypeGroups(s->shard->groups[i]);
                    if (type != NULLSHARD)
                        blank[type] += s->rank;
                }
            }
        }

        _charShardBonuses[pChar] = blank;
        ApplyShardBonuses(player);
    }

    void FillBlankSoulShards(Player* player) {
        auto pChar = player->GetGUID().GetCounter();
        for (int i = 0; i < MAX_ACTIVE_SHARDS; i++)
            _charActiveShards[pChar][i] = new PlayerSoulShard();

        SaveActiveShards(player);

        _charShardBonuses[pChar] = {};
    }

    PlayerSoulShard* PlayerHasSoulShard(Player* player, uint32 shard)
    {
        auto found = _charSoulShards.find(player->GetSession()->GetAccountId());
        if (found != _charSoulShards.end()) {
            auto fshard = found->second.find(shard);
            return fshard != found->second.end() ? fshard->second : nullptr;
        }
        return nullptr;
    }

    bool PlayerHasSoulShardActive(Player* player, uint32 shardId)
    {
        for (auto shard : _charActiveShards[player->GetGUID().GetCounter()])
            if (auto has = shard.second) {
                if (shard.second->source == shardId)
                    return true;
            }
            else
                return false;

        return false;
    }

    void SendActiveSoulShards(Player* player) {
        std::string msg = "";
        auto found = GetPlayerActiveSoulShards(player);
        std::string delim = "";
        for (int i = 0; i < MAX_ACTIVE_SHARDS; i++) {
            if (auto shard = found[i])
                msg += delim + std::to_string(found[i]->source);
            else
                msg += delim + "0";

            delim = "~";
        }
        player->SendForgeUIMsg(ForgeTopic::ACTIVE_SOULSHARDS, msg);
    }

    std::unordered_map<uint8, PlayerSoulShard*> GetPlayerActiveSoulShards(Player* player)
    {
        if (_charActiveShards[player->GetGUID().GetCounter()].empty())
            _charActiveShards[player->GetGUID().GetCounter()] = { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0} };
        return _charActiveShards[player->GetGUID().GetCounter()];
    }

    WorldLocation GetRaceStartingZone(uint32 race) {
        return _startingZones[race];
    }
public:
    struct RaidRotation {
        uint32 id;
        uint32 raid1Loc;
        uint32 raid2Loc;
        uint32 raid3Loc;
        uint32 startTime;
        uint8 sequence;
        bool active;

        RaidRotation(uint32 id, uint32 r1, uint32 r2, uint32 r3, uint32 start, uint8 sequence, bool active) {
            this->id = id;
            this->raid1Loc = r1;
            this->raid2Loc = r2;
            this->raid3Loc = r3;
            this->startTime = start;
            this->sequence = sequence;
            this->active = active;
        }
    };

    RaidRotation* _activeRaidRotation;

    enum PortalObjects {
        RAID = 1179916,
        TOWN = 1179917,
    };
private:
    std::unordered_map<uint32 /*id*/, RaidRotation*> _raidRotations;
    std::unordered_map<uint8 /*sequence*/, RaidRotation*> _raidRotationsBySequence;

    void AddRaidRotation()
    {
        LOG_INFO("server.load", "Loading raid rotation...");
        _raidRotations.clear();
        _raidRotationsBySequence.clear();

        QueryResult raidResult = WorldDatabase.Query("SELECT * FROM `forge_raid_rotation`");
        if (!raidResult) return;

        auto activeRotation = 1;
        auto maxSequence = 0;
        auto needsNewRotation = true;
        do
        {
            Field* raidFields = raidResult->Fetch();
            uint32 id = raidFields[0].Get<uint32>();
            uint32 r1 = raidFields[1].Get<uint32>();
            uint32 r2 = raidFields[2].Get<uint32>();
            uint32 r3 = raidFields[3].Get<uint32>();
            uint32 startTime = raidFields[4].Get<uint32>();
            bool active = raidFields[5].Get<bool>();
            uint8 sequence = raidFields[6].Get<uint8>();


            RaidRotation* rot = new RaidRotation(id, r1, r2, r3, startTime, sequence, active);

            _raidRotations[id] = rot;
            _raidRotationsBySequence[sequence] = rot;

            if (active) {
                activeRotation = sequence;
                if (GameTime::GetGameTime() < Seconds(startTime)) {
                    _activeRaidRotation = rot;
                    needsNewRotation = false;
                }
            }

            if (sequence > maxSequence)
                maxSequence = sequence;

        } while (raidResult->NextRow());

        if (needsNewRotation) {
            WorldDatabase.DirectExecute("UPDATE `forge_raid_rotation` set `active` = 0");
            auto newSeq = activeRotation++;
            if (newSeq > maxSequence)
                newSeq = 1;

            RaidRotation* newRot = _raidRotationsBySequence[newSeq];
            newRot->active = true;
            newRot->startTime = GameTime::GetGameTime().count();
            _raidRotationsBySequence[newSeq] = newRot;
            _activeRaidRotation = newRot;

            WorldDatabase.DirectExecute("UPDATE `forge_raid_rotation` set `active` = 1 and `timeOfStart` = {} where `ID` = {}", newRot->startTime, newRot->id);
        }
    }

    std::unordered_map<uint32 /*race*/, WorldLocation> _startingZones;
    std::unordered_map<uint32 /*source*/, SoulShard*> SoulShards;
    std::unordered_map<uint32 /*account*/, std::unordered_map<uint32 /*source*/, PlayerSoulShard*>> _charSoulShards;
    std::unordered_map<uint32 /*char*/, std::unordered_map<uint8 /*slot*/, PlayerSoulShard*>> _charActiveShards;
    std::unordered_map<uint32 /*char*/, std::unordered_map<SoulTypeGroups, uint8>> _charShardBonuses;

    std::vector<uint32> _shardGroupBonusAuras = { 0, BEAST_AURA , DRAGONKIN_AURA, DEMON_AURA , ELEMENTAL_AURA ,
        GIANT_AURA, UNDEAD_AURA, HUMANOID_AURA, 0, MECHANICAL_AURA };

    void ApplyShardBonuses(Player* player) {
        ClearShardBonuses(player);

        for (auto bonuses : _charShardBonuses[player->GetGUID().GetCounter()])
            if (bonuses.second > 0) {
                if (auto aura = _shardGroupBonusAuras[bonuses.first]) {
                    player->AddAura(aura, player);
                    if (auto app = player->GetAura(aura, player->GetGUID()))
                        app->SetStackAmount(bonuses.second);
                }
            }
    }

    void ClearShardBonuses(Player* player) {
        for (auto aura : _shardGroupBonusAuras)
            if (player->HasAura(aura))
                player->RemoveAura(aura);
    }

    void SaveActiveShards(Player* player) {
        auto pChar = player->GetGUID().GetCounter();
        auto shards = _charActiveShards[pChar];

        CharacterDatabase.DirectExecute("REPLACE INTO active_shards (`char`, shard1, shard2, shard3, shard4, shard5) VALUES({}, {}, {}, {}, {}, {})",
            player->GetGUID().GetCounter(),
            shards[0] ? shards[0]->source : 0, shards[1] ? shards[1]->source : 0,
            shards[2] ? shards[2]->source : 0, shards[3] ? shards[3]->source : 0,
            shards[4] ? shards[4]->source : 0);
    }

    void AddSoulShards()
    {
        LOG_INFO("server.load", "Loading soul shards...");
        SoulShards.clear();

        QueryResult SoulShardResult = WorldDatabase.Query("SELECT * FROM `acore_world`.`soul_shards`");
        if (!SoulShardResult) return;

        do
        {
            Field* SoulShardFields = SoulShardResult->Fetch();
            uint32 source = SoulShardFields[0].Get<uint32>();
            SoulShardQuality quality = SoulShardQuality(SoulShardFields[1].Get<uint8>());
            SoulShard* shard = new SoulShard();
            shard->source = source;
            shard->quality = quality;

            for (int i = 0; i < MAX_SOUL_GROUPS; i++) {
                shard->groups[i] = SoulShardFields[2 + i].Get<uint32>();
            }
            shard->special = SoulShardFields[5].Get<uint32>();

            SoulShards[source] = shard;
        } while (SoulShardResult->NextRow());
    }

    void AddPlayerSoulShards()
    {
        LOG_INFO("server.load", "Loading player soul shards...");
        _charSoulShards.clear();

        QueryResult SoulShardResult = WorldDatabase.Query("SELECT * FROM `acore_characters`.`character_soul_shards` order by `rank` desc");
        if (!SoulShardResult) return;
        do
        {
            Field* SoulShardFields = SoulShardResult->Fetch();
            uint32 account = SoulShardFields[0].Get<uint32>();

            uint32 source = SoulShardFields[1].Get<uint32>();
            uint32 count = SoulShardFields[2].Get<uint8>();
            uint8 rank = SoulShardFields[3].Get<uint8>();

            SoulShard* realShard = SoulShards[source];
            PlayerSoulShard* shard = new PlayerSoulShard(source, count, rank, realShard);

            _charSoulShards[account][source] = shard;
        } while (SoulShardResult->NextRow());
    }

    void HandleSoulShardAcquired(Player* player, SoulShard* shard, bool isNew)
    {
        auto account = player->GetSession()->GetAccountId();
        auto source = shard->source;
        if (isNew) {
            if (_charSoulShards[account].size() < 30) {
                PlayerSoulShard* pShard = new PlayerSoulShard(shard->source, 1, 1, shard);
                _charSoulShards[account][source] = pShard;
            }
            else {
                player->AddItem(RewardItems::SOUL_DUST, shard->quality);
                return;
            }
        }

        HandleInsertOrUpdateShard(account, _charSoulShards[account][source], isNew);
    }

    void HandleInsertOrUpdateShard(uint32 accountId, PlayerSoulShard* shard, bool isNew) {
        if (isNew)
            CharacterDatabase.DirectExecute(std::format("INSERT INTO character_soul_shards (`account`, `source`, `count`, `rank`) VALUES ({}, {}, {}, {})",
                accountId, shard->source, shard->count, shard->rank));
        else
            CharacterDatabase.DirectExecute("UPDATE character_soul_shards set `count` = {}, `rank` = {} where `account` = {} and `source` = {}",
                shard->count, shard->rank, accountId, shard->source);
    }

    void AddStartingZones()
    {
        LOG_INFO("server.load", "Loading starting zones...");
        _startingZones.clear();

        QueryResult ZoneResult = WorldDatabase.Query("SELECT * FROM `forge_starting_zone`");
        if (!ZoneResult) return;
        do
        {
            Field* ZoneFields = ZoneResult->Fetch();
            uint32 race = ZoneFields[0].Get<uint32>();
            uint32 map = ZoneFields[1].Get<uint32>();
            float x = ZoneFields[2].Get<float>();
            float y = ZoneFields[3].Get<float>();
            float z = ZoneFields[4].Get<float>();
            float o = ZoneFields[5].Get<float>();

            _startingZones[race] = WorldLocation(map, x, y, z, o);
        } while (ZoneResult->NextRow());
    }
    // world tiers
public:
    uint8 GetAccountWorldTierUnlock(Player* player) {
        auto account = player->GetSession()->GetAccountId();
        auto found = _maxWorldTierAllowableAccount.find(account);
        if (found == _maxWorldTierAllowableAccount.end()) {
            HandleUpdateAccountWorldTierUnlock(account, 1);
        }
        return _maxWorldTierAllowableAccount[account];
    }

    uint8 GetCharWorldTierUnlock(Player* player) {
        auto charId = player->GetGUID().GetCounter();
        auto found = _maxWorldTierAllowableChar.find(charId);
        if (found == _maxWorldTierAllowableChar.end()) {
            HandleUpdateCharacterWorldTierUnlock(charId, 1);
        }
        return _maxWorldTierAllowableChar[charId];
    }

    void SetAccountWorldTierUnlock(Player* player, uint8 val) {
        HandleUpdateAccountWorldTierUnlock(player->GetSession()->GetAccountId(), val);
    }

    void SetCharWorldTierUnlock(Player* player, uint8 val) {
        HandleUpdateCharacterWorldTierUnlock(player->GetGUID().GetCounter(), val);
    }

private:
    std::unordered_map<uint32 /*account*/, uint8 /*level*/> _maxWorldTierAllowableAccount;
    std::unordered_map<uint32 /*account*/, uint8 /*level*/> _maxWorldTierAllowableChar;

    void HandleUpdateAccountWorldTierUnlock(uint32 accountId, uint8 newVal) {
        _maxWorldTierAllowableAccount[accountId] = newVal;
        CharacterDatabase.DirectExecute("REPLACE INTO worldtier_unlocks_account (`account`, `unlocked`) VALUES ({},{})",
            accountId, newVal);
    }

    void HandleUpdateCharacterWorldTierUnlock(uint32 charId, uint8 newVal) {
        _maxWorldTierAllowableChar[charId] = newVal;
        CharacterDatabase.DirectExecute("REPLACE INTO worldtier_unlocks_char (`char`, `unlocked`) VALUES ({},{})",
            charId, newVal);
    }

    void AddWorldTierUnlocks()
    {
        LOG_INFO("server.load", "Loading world tier achievements...");
        _maxWorldTierAllowableAccount.clear();
        _maxWorldTierAllowableChar.clear();

        QueryResult acctUnlockResult = CharacterDatabase.Query("SELECT * FROM `acore_characters`.`worldtier_unlocks_account`");
        if (!acctUnlockResult) return;

        do
        {
            Field* acctUnlockFields = acctUnlockResult->Fetch();
            _maxWorldTierAllowableAccount[acctUnlockFields[0].Get<uint32>()] = acctUnlockFields[1].Get<uint8>();
        } while (acctUnlockResult->NextRow());

        QueryResult chartUnlockResult = CharacterDatabase.Query("SELECT * FROM `acore_characters`.`worldtier_unlocks_char`");
        if (!chartUnlockResult) return;

        do
        {
            Field* charUnlockFields = chartUnlockResult->Fetch();
            _maxWorldTierAllowableChar[charUnlockFields[0].Get<uint32>()] = charUnlockFields[1].Get<uint8>();
        } while (chartUnlockResult->NextRow());
    }

    // talent resets
public:
    bool IsFlaggedReset(uint32 guid) {
        auto flagged = std::find(FlaggedForReset.begin(), FlaggedForReset.end(), guid);
        if (flagged != FlaggedForReset.end()) {
            return true;
        }
        return false;
    }

    void ClearResetFlag(uint32 guid) {
        FlaggedForReset.erase(std::remove(FlaggedForReset.begin(), FlaggedForReset.end(), guid), FlaggedForReset.end());
        auto trans = WorldDatabase.BeginTransaction();
        trans->Append("delete from forge_talent_flagged_reset where guid = {}", guid);
        WorldDatabase.CommitTransaction(trans);
    }

private:
    void LoadCharacterResetFlags()
    {
        QueryResult flags = WorldDatabase.Query("select guid from forge_talent_flagged_reset order by guid asc");
        if (!flags)
            return;

        do
        {
            Field* flagField = flags->Fetch();
            uint32 guid = flagField[0].Get<uint32>();

            FlaggedForReset.push_back(guid);
        } while (flags->NextRow());
    }
};

#define sForgeCache ForgeCache::instance()
