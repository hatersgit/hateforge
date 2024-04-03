#pragma once

#include <string>
#include "Player.h"
#include <ForgeCache.cpp>

enum MultiClassOp {
    SET = 0,
    CLEAR = 1,

    MAXOP
};

class ForgeCommonMessage
{
public:


    static ForgeCommonMessage* get_instance()
    {
        static ForgeCommonMessage* cache;

        if (cache == nullptr)
            cache = new ForgeCommonMessage(ForgeCache::instance());

        return cache;
    }

    ForgeCommonMessage(ForgeCache*);
    ForgeCache* fc;

    void SendTalents(Player*);
    void SendTalentTreeLayout(Player*);
    void SendTalentTreeLayout(Player*, uint32);
    void SendSpecInfo(Player*);
    void SendActiveSpecInfo(Player* player);
    void SendSpecSelectInfo(Player* player);
    std::string BuildTree(Player*, CharacterPointType pointType, std::list<ForgeTalentTab*> tabs);
    void ApplyKnownForgeSpells(Player*);
    bool CanLearnTalent(Player*, uint32, uint32);
    // hater: transmog
    /*void SendCollections(Player*);
    std::string SendCollections(Player*, uint32, std::string);*/
    void SendXmogSet(Player*, uint8 /*set id*/);
    void SendXmogSets(Player*);

    // talent loadouts
    void SendLoadouts(Player*);
    std::string EncodeTalentString(Player* player);
    void DecodeTalentString(std::string talent_str);

    // hater: perks
    void SendPerks(Player*, uint8);
    void SendAllPerks(Player*);
    void SendPerkSelection(Player*, std::string);

    // hater: soulshards
    void SendSoulShards(Player*);

    std::string DoBuildPerks(std::vector<CharacterSpecPerk*> spec, Player* player);
private:
    std::string DoBuildRanks(std::unordered_map<uint32, ForgeCharacterTalent*>& spec, Player* player, std::string clientMsg, uint32 tabId);
    std::string DoBuildPerkCatalogue(std::vector<Perk*> perks);

    // hater: talent string encoding
    const std::string base64_char = "|ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
};


