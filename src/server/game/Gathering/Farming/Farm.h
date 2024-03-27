class Player;
class GameObject;
class Creature;
class ZoneScript;
struct GossipMenuItems;
class Farm;

#define MAX_FARM_DIM 11
#define DEFAULT_FARM_DIM 3;

enum CropBonusGroup : uint8 {
    WET,
    DRY,
    SPICY,
    SWEET,
    SOUR,
    BITTER,

    ROOT,
    FLOWER,
    FRUIT,
    HERB,
    VEGETABLE,
};

class Crop
{
public:
    Crop(CropBonusGroup, CropBonusGroup, uint32, uint32, std::string);
    ~Crop() = default;

    std::pair<uint32, uint32> GetBonuses() {
        return bonuses;
    }

    uint32 GetRewardedItem() {
        return rewardItem;
    }

private:
    uint8 id;
    std::pair<CropBonusGroup, CropBonusGroup> bonuses;
    uint32 rewardItem;
    uint32 model;
    std::string name;
};

class CropImpl
{
public:
    CropImpl(Crop*, float, uint8);
    ~CropImpl() = default;

    void SetMult(float mul) {
        valueMult = mul;
    }
    float GetMult() {
        return valueMult;
    }

    void SetValue(uint8 val) {
        cropValue = val;
    }
    uint8 GetValue() {
        return cropValue;
    }

    Crop* GetBase() {
        return base;
    }

private:
    Crop* base;
    float valueMult = 1.f;
    uint8 cropValue = 1;
};

struct Plot
{
    bool bought = false;
    bool planted = false;

    std::pair<uint8, uint8> position = { 0,0 };
    CropImpl* plantedCrop = nullptr;
};

class Farm
{
public:
    Farm(Plot* _plantedCrops[MAX_FARM_DIM][MAX_FARM_DIM], uint32 upgradeCost, Player* owner);
    ~Farm() = default;

    void Update(uint32 diff);
    bool CanHarvest() {
        return canHarvest;
    }
    void SetCanHarvest() {
        canHarvest = true;
    }
    void InitBoughtPlots();
    void Plant(uint8 x, uint8 y, CropImpl* what);
    void PlantAll(std::unordered_map<uint8, std::unordered_map<uint8, CropImpl*>> map);
    void Harvest();

    Player* GetOwner() {
        return owner;
    }
private:
    uint8 width = DEFAULT_FARM_DIM;
    uint8 height = DEFAULT_FARM_DIM;

    bool _boughtPlots[MAX_FARM_DIM][MAX_FARM_DIM];
    uint32 m_harvestTimer = 1800000;
    uint32 m_updateTimer = 0;

    std::unordered_map<CropBonusGroup, uint8> _activeBonuses;

    uint8 LaborValue;
    uint32 UpgradeCost;

    bool fertilized = false;
    bool canHarvest = false;

    Player* owner;

protected:
    Position* topLeftOfFarm = new Position(-523, 3129, 154.86, 0.f);
    uint32 SoilGObject = 331;

    std::unordered_map<uint8, std::unordered_map<uint8, Plot*>> _plantedCrops{};
};
