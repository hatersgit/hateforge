#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
//#include <ShopCache.cpp>
//#include "Transmogrification.h"

class ForgeCacheCommands : public CommandScript
{
public:
    ForgeCacheCommands() : CommandScript("ForgeCacheCommands")
    {
       
    }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> exampleCommandTable1 =
        {
            // This command does something
            { "forge",      SEC_GAMEMASTER2_L,     true,  &ForgePoints,      "" },
            { "talent",      SEC_GAMEMASTER2_L,     true, &TalentPoints,      "" },
            { "prestige",      SEC_GAMEMASTER2_L,     true, &PrestigePoints,      "" },
            { "racial",      SEC_GAMEMASTER2_L,     true, &RacialPoints,      "" },
        };

        // Level 1 sub command
        static std::vector<ChatCommand> exampleCommandTable =
        {
            { "reloadCache",      SEC_GAMEMASTER2_L,     true,  &ReloadCache,      ""},
            { "addpoints",      SEC_GAMEMASTER2_L,     true, nullptr,      "", exampleCommandTable1},
        };

        // Root command
        static std::vector<ChatCommand> commandTable =
        {
            //  name       permission   allow console?   handler method      help      child commands table 
            { "forge",   SEC_GAMEMASTER2_L,      true,           nullptr,         "", exampleCommandTable},
            { "itemgen",    SEC_GAMEMASTER2_L, true, &GenItem,  ""}
        };

        return commandTable; // Return the root command table
    }

    static bool GenItem(ChatHandler* handler, char const* args)
    {
        if (ForgeCache::instance()->isNumber(args)) {
            Player* ply = handler->getSelectedPlayerOrSelf();

            int32 baseItem = static_cast<int32>(std::stoul(args));
            auto guidlow = sObjectMgr->GetGenerator<HighGuid::Item>().Generate();
            guidlow += guidlow < 200000 ? 200000 : 0;
            auto itemProto = sObjectMgr->CreateItemTemplate(guidlow, baseItem);
            CustomItemTemplate* custom = new CustomItemTemplate(itemProto);
            sScriptMgr->GenerateItem(custom, handler->getSelectedPlayerOrSelf(), 0, true);
            ply->AddItem(guidlow, 1);
            return true;
        }
        return false;
    }

    //          Handle + "Something" + Command
    static bool ReloadCache(ChatHandler* handler, char const* args)
    {
        ForgeCache::instance()->ReloadDB();
        //ShopCache::get_instance()->LoadCache();
        //sTransmogrification->Load();

        // ChatHandler could be Console or Player session
        handler->PSendSysMessage("Cache Reloaded");
        return true;
    }

    static bool ForgePoints(ChatHandler* handler, char const* args)
    {
        uint32 amount = static_cast<uint32>(std::stoul(args));
        ForgeCache::instance()->AddCharacterPointsToAllSpecs(handler->getSelectedPlayerOrSelf(), CharacterPointType::FORGE_SKILL_TREE, amount);
        ForgeCommonMessage::get_instance()->SendActiveSpecInfo(handler->getSelectedPlayerOrSelf());
        return true;
    }

    static bool TalentPoints(ChatHandler* handler, char const* args)
    {
        uint32 amount = static_cast<uint32>(std::stoul(args));
        ForgeCache::instance()->AddCharacterPointsToAllSpecs(handler->getSelectedPlayerOrSelf(), CharacterPointType::TALENT_TREE, amount);
        ForgeCommonMessage::get_instance()->SendActiveSpecInfo(handler->getSelectedPlayerOrSelf());
        return true;
    }

    static bool PrestigePoints(ChatHandler* handler, char const* args)
    {
        uint32 amount = static_cast<uint32>(std::stoul(args));
        ForgeCache::instance()->AddCharacterPointsToAllSpecs(handler->getSelectedPlayerOrSelf(), CharacterPointType::PRESTIGE_TREE, amount);
        ForgeCommonMessage::get_instance()->SendActiveSpecInfo(handler->getSelectedPlayerOrSelf());
        return true;
    }

    static bool RacialPoints(ChatHandler* handler, char const* args)
    {
        uint32 amount = static_cast<uint32>(std::stoul(args));
        ForgeCache::instance()->AddCharacterPointsToAllSpecs(handler->getSelectedPlayerOrSelf(), CharacterPointType::RACIAL_TREE, amount);
        ForgeCommonMessage::get_instance()->SendActiveSpecInfo(handler->getSelectedPlayerOrSelf());
        return true;
    }

private:

};
