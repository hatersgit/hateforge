#pragma once

#include <string>
#include "Player.h"
#include <ForgeCache.cpp>

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

    void SendTalents(Player*, uint32);
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

    std::string EncodeTalentString(Player* player);
    void DecodeTalentString(std::string talent_str);
private:
    std::string DoBuildRanks(std::unordered_map<uint32, ForgeCharacterTalent*>& spec, Player* player, std::string clientMsg, uint32 tabId);

    // hater: talent string encoding
    const std::string base64_char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const unsigned LOADOUT_SERIALIZATION_VERSION = 1;
    const size_t version_bits = 8;    // serialization version
    const size_t spec_bits = 16;   // specialization id
    const size_t tree_bits = 128;  // C_Traits.GetTreeHash(), optionally can be 0-filled
    const size_t rank_bits = 6;    // ranks purchased if node is partially filled
    const size_t choice_bits = 2;    // choice index, 0-based
    // hardcoded value from Interface/SharedXML/ExportUtil.lua
    const size_t byte_size = 6;
};


