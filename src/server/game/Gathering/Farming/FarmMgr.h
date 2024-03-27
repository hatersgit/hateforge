class Player;
class GameObject;
class Creature;
class ZoneScript;
struct GossipMenuItems;
class Farm;
class Crop;

// class to handle player enter / leave / areatrigger / GO use events
class FarmMgr
{
public:
    FarmMgr();
    ~FarmMgr();

    static FarmMgr* instance();

    void Update(uint32 diff);

    void LoadCrops();
    void LoadPlayerFarms();

protected:
    std::unordered_map<uint32/* guid */, Farm*> _plrFarmMap;
    std::unordered_map<uint8 /*id*/, Crop*> _crops;

private:
    uint32 m_UpdateTimer;

    std::string alphaMap = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
};

#define sFarmMgr FarmMgr::instance()
