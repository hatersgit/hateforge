#include "Farm.h"
#include "Player.h"

Crop::Crop(CropBonusGroup g1, CropBonusGroup g2, uint32 reward, uint32 model, std::string name) : bonuses({g1, g2}), rewardItem(reward), model(model), name(name) {
}

CropImpl::CropImpl(Crop* crop, float mult, uint8 value) : base(crop), valueMult(mult), cropValue(value) {

}

Farm::Farm(Plot* plantedCrops[MAX_FARM_DIM][MAX_FARM_DIM], uint32 upgradeCost, Player* owner) : owner(owner), UpgradeCost(upgradeCost), LaborValue(1) {
    for (int x = 0; x < MAX_FARM_DIM; x++) {
        for (int y = 0; y < MAX_FARM_DIM; y++) {
            Plot* plot = plantedCrops[x][y];
            _plantedCrops[x][y] = plot;
            _boughtPlots[x][y] = plot->bought;
        }
    }
}

void Farm::InitBoughtPlots() {
    for (auto row : _plantedCrops) {
        auto yOffset = 0;
        for (auto col : row.second) {
            auto xOffset = 0;
            auto plot = col.second;
            if (plot->bought) {
                auto x = topLeftOfFarm->GetPositionX() + xOffset;
                auto y = topLeftOfFarm->GetPositionY() + yOffset;
                auto z = topLeftOfFarm->GetPositionZ();

                GetOwner()->GetMap()->SummonGameObject(SoilGObject, Position(x, y, z));
            }
            xOffset += 2;
        }
        yOffset += 2;
    }
}

void Farm::Update(uint32 diff) {
    if (!CanHarvest()) {
        if (m_updateTimer > m_harvestTimer) {
            SetCanHarvest();
        }
        m_updateTimer += diff;
    }
}

void Farm::Plant(uint8 x, uint8 y, CropImpl* what) {
    if (_boughtPlots[x][y])
        _plantedCrops[x][y]->plantedCrop = what;
}

void Farm::Harvest() {
    Player* owner = GetOwner();
    if (CanHarvest()) {
        auto plotsHarvested = 0;
        for (auto row : _plantedCrops)
            for (auto col : row.second) {
                auto plot = col.second;
                if (plot->bought) {
                    auto value = plot->plantedCrop->GetValue() * plot->plantedCrop->GetMult();
                    owner->AddItem(plot->plantedCrop->GetBase()->GetRewardedItem(), uint32(value));
                    plotsHarvested++;
                }
            }

        LaborValue = plotsHarvested;
    }
}
