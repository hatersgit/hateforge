/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AREATRIGGER_H
#define AREATRIGGER_H

#include "Object.h"
#include "AreaTriggerTemplate.h"

class AuraEffect;
class AreaTriggerAI;
class SpellInfo;
class Spell;
class Unit;

namespace G3D
{
    class Vector2;
    class Vector3;
}
namespace Movement
{
    template<typename length_type>
    class Spline;
}

struct SpellCastVisual
{
    uint32 SpellXSpellVisualID = 0;
    uint32 ScriptVisualID = 0;
};

struct ScaleCurve
{
    bool OverrideActive;
    uint32 StartTimeOffset;
    uint32 ParameterCurve;
    std::vector<Position> Points;
};
struct VisualAnim
{
    bool Field_C;
    uint32 AnimationDataID;
    uint32 AnimKitID;
    uint32 AnimProgress;
};


struct AreaTriggerDataState
{
    bool HeightIgnoresScale;
    bool Field_261;
    ScaleCurve OverrideScaleCurve;
    ScaleCurve ExtraScaleCurve;
    ScaleCurve OverrideMoveCurveX;
    ScaleCurve OverrideMoveCurveY;
    ScaleCurve OverrideMoveCurveZ;
    ObjectGuid Caster;
    uint32 Duration;
    uint32 TimeToTarget;
    uint32 TimeToTargetScale;
    uint32 TimeToTargetExtraScale;
    uint32 TimeToTargetPos;
    int32 SpellID;
    int32 SpellForVisuals;
    SpellCastVisual SpellVisual;
    float BoundsRadius2D;
    uint32 DecalPropertiesID;
    ObjectGuid CreatingEffectGUID;
    uint32 NumUnitsInside;
    uint32 NumPlayersInside;
    ObjectGuid OrbitPathTarget;
    Position RollPitchYaw;
    int32 PositionalSoundKitID;
    VisualAnim VisualAnim;
};

class AreaTrigger : public WorldObject, public GridObject<AreaTrigger>, public MovableMapObject
{
    public:
        AreaTrigger();
        ~AreaTrigger();

    public:

        void AddToWorld() override;
        void RemoveFromWorld() override;

        void AI_Initialize();
        void AI_Destroy();

        AreaTriggerAI* AI() { return _ai.get(); }

        bool IsServerSide() const { return _areaTriggerTemplate->Id.IsServerSide; }

        float GetStationaryX() const override { return _stationaryPosition.GetPositionX(); }
        float GetStationaryY() const override { return _stationaryPosition.GetPositionY(); }
        float GetStationaryZ() const override { return _stationaryPosition.GetPositionZ(); }
        float GetStationaryO() const override { return _stationaryPosition.GetOrientation(); }
        void RelocateStationaryPosition(Position const& pos) { _stationaryPosition.Relocate(pos); }

    private:
        bool Create(uint32 areaTriggerCreatePropertiesId, Unit* caster, Unit* target, SpellInfo const* spellInfo, Position const& pos, int32 duration, SpellCastVisual spellVisual, Spell* spell, AuraEffect const* aurEff);
    public:
        static AreaTrigger* CreateAreaTrigger(uint32 areaTriggerCreatePropertiesId, Unit* caster, Unit* target, SpellInfo const* spellInfo, Position const& pos, int32 duration, SpellCastVisual spellVisual, Spell* spell = nullptr, AuraEffect const* aurEff = nullptr);
        static ObjectGuid CreateNewMovementForceId(Map* map, uint32 areaTriggerId);

        [[nodiscard]] std::string const& GetAIName() { return GetCreateProperties()->scriptName; };
        void SetDisplayId(uint32 displayid);
        [[nodiscard]] uint32 GetDisplayId() const { return GetUInt32Value(GAMEOBJECT_DISPLAYID); }

        void Update(uint32 diff) override;
        void Remove();
        bool IsRemoved() const { return _isRemoved; }
        uint32 GetSpellId() const { return m_areaTriggerData->SpellID; }
        AuraEffect const* GetAuraEffect() const { return _aurEff; }
        uint32 GetTimeSinceCreated() const { return _timeSinceCreated; }

        void SetHeightIgnoresScale(bool heightIgnoresScale) { m_areaTriggerData->HeightIgnoresScale = heightIgnoresScale; }

        void SetOverrideScaleCurve(float overrideScale);
        void SetOverrideScaleCurve(std::array<DBCPosition2D, 2> const& points, Optional<uint32> startTimeOffset = {}, CurveInterpolationMode interpolation = CurveInterpolationMode::Linear);
        void ClearOverrideScaleCurve();

        void SetExtraScaleCurve(float extraScale);
        void SetExtraScaleCurve(std::array<DBCPosition2D, 2> const& points, Optional<uint32> startTimeOffset = {}, CurveInterpolationMode interpolation = CurveInterpolationMode::Linear);
        void ClearExtraScaleCurve();

        void SetOverrideMoveCurve(float x, float y, float z);
        void SetOverrideMoveCurve(std::array<DBCPosition2D, 2> const& xCurvePoints, std::array<DBCPosition2D, 2> const& yCurvePoints, std::array<DBCPosition2D, 2> const& zCurvePoints,
            Optional<uint32> startTimeOffset = {}, CurveInterpolationMode interpolation = CurveInterpolationMode::Linear);
        void ClearOverrideMoveCurve();

        uint32 GetTimeToTarget() const { return m_areaTriggerData->TimeToTarget; }
        void SetTimeToTarget(uint32 timeToTarget) { m_areaTriggerData->TimeToTarget = timeToTarget; }

        uint32 GetTimeToTargetScale() const { return m_areaTriggerData->TimeToTargetScale; }
        void SetTimeToTargetScale(uint32 timeToTargetScale) {   m_areaTriggerData->TimeToTargetScale = timeToTargetScale; }

        uint32 GetTimeToTargetExtraScale() const { return m_areaTriggerData->TimeToTargetExtraScale; }
        void SetTimeToTargetExtraScale(uint32 timeToTargetExtraScale) { m_areaTriggerData->TimeToTargetExtraScale = timeToTargetExtraScale; }
        uint32 GetTimeToTargetPos() const { return m_areaTriggerData->TimeToTargetPos; }
        void SetTimeToTargetPos(uint32 timeToTargetPos) { m_areaTriggerData->TimeToTargetPos = timeToTargetPos; }

        int32 GetDuration() const { return _duration; }
        int32 GetTotalDuration() const { return _totalDuration; }
        void SetDuration(int32 newDuration);
        void Delay(int32 delaytime) { SetDuration(GetDuration() - delaytime); }

        GuidUnorderedSet const& GetInsideUnits() const { return _insideUnits; }

        AreaTriggerCreateProperties const* GetCreateProperties() const { return _areaTriggerCreateProperties; }
        AreaTriggerTemplate const* GetTemplate() const;
        uint32 GetScriptId() const;

        ObjectGuid GetCreatorGUID() const { return GetCasterGuid(); }
        ObjectGuid GetOwnerGUID() const { return GetCasterGuid(); }
        ObjectGuid const& GetCasterGuid() const { return m_areaTriggerData->Caster; }
        Unit* GetCaster() const;
        Unit* GetTarget() const;

        uint32 GetFaction() const;

        AreaTriggerShapeInfo const& GetShape() const { return _shape; }
        float GetMaxSearchRadius() const;
        Position const& GetRollPitchYaw() const { return _rollPitchYaw; }
        Position const& GetTargetRollPitchYaw() const { return _targetRollPitchYaw; }
        void InitSplineOffsets(std::vector<Position> const& offsets, uint32 timeToTarget);
        void InitSplines(std::vector<G3D::Vector3> splinePoints, uint32 timeToTarget);
        bool HasSplines() const;
        ::Movement::Spline<int32> const& GetSpline() const { return *_spline; }
        uint32 GetElapsedTimeForMovement() const { return GetTimeSinceCreated(); }
        void InitOrbit(AreaTriggerOrbitInfo const& orbit, uint32 timeToTarget);
        bool HasOrbit() const;
        Optional<AreaTriggerOrbitInfo> const& GetOrbit() const { return _orbitInfo; }

        bool HasOverridePosition() const;
        void UpdateShape();

        AreaTriggerDataState* m_areaTriggerData;

        uint32 entry;
        uint32 map;
        float x;
        float y;
        float z;
        float radius;
        float length;
        float width;
        float height;
        float orientation;

    protected:
        void _UpdateDuration(int32 newDuration);
        float GetProgress() const;

        float CalcCurrentScale() const;

        float GetScaleCurveProgress(ScaleCurve const& scaleCurve, uint32 timeTo) const;
        float GetScaleCurveValueAtProgress(ScaleCurve const& scaleCurve, float x) const;
        float GetScaleCurveValue(ScaleCurve const& scaleCurve, uint32 timeTo) const;
        void SetScaleCurve(ScaleCurve scaleCurveMutator, float constantValue);
        void SetScaleCurve(ScaleCurve scaleCurveMutator, std::array<DBCPosition2D, 2> const& points, Optional<uint32> startTimeOffset, CurveInterpolationMode interpolation);
        void ClearScaleCurve(ScaleCurve scaleCurveMutator);
        void SetScaleCurve(ScaleCurve scaleCurveMutator, Optional<AreaTriggerScaleCurveTemplate> const& curve);

        void UpdateTargetList();
        void SearchUnits(std::vector<Unit*>& targetList, float radius, bool check3D);
        void SearchUnitInSphere(std::vector<Unit*>& targetList);
        void SearchUnitInBox(std::vector<Unit*>& targetList);
        void SearchUnitInPolygon(std::vector<Unit*>& targetList);
        void SearchUnitInCylinder(std::vector<Unit*>& targetList);
        void SearchUnitInDisk(std::vector<Unit*>& targetList);
        void SearchUnitInBoundedPlane(std::vector<Unit*>& targetList);
        bool CheckIsInPolygon2D(Position const* pos) const;
        void HandleUnitEnterExit(std::vector<Unit*> const& targetList);

        void DoActions(Unit* unit);
        void UndoActions(Unit* unit);

        void UpdatePolygonVertices();
        void UpdateOrbitPosition(uint32 diff);
        void UpdateSplinePosition(uint32 diff);
        void UpdateOverridePosition();

        Position* GetOrbitCenterPosition() const;
        Position CalculateOrbitPosition() const;

        void DebugVisualizePosition(); // Debug purpose only

        ObjectGuid::LowType _spawnId;
        ObjectGuid _targetGuid;
        AuraEffect const* _aurEff;
        Position _stationaryPosition;
        AreaTriggerShapeInfo _shape;
        int32 _duration;
        int32 _totalDuration;
        uint32 _timeSinceCreated;
        float _verticesUpdatePreviousOrientation;
        bool _isRemoved;
        Position _rollPitchYaw;
        Position _targetRollPitchYaw;
        std::vector<Position> _polygonVertices;
        std::unique_ptr<::Movement::Spline<int32>> _spline;
        bool _reachedDestination;
        int32 _lastSplineIndex;
        uint32 _movementTime;
        Optional<AreaTriggerOrbitInfo> _orbitInfo;
        AreaTriggerCreateProperties const* _areaTriggerCreateProperties;
        AreaTriggerTemplate const* _areaTriggerTemplate;
        GuidUnorderedSet _insideUnits;
        std::unique_ptr<AreaTriggerAI> _ai;
};

typedef std::list<AreaTrigger*> AreaTriggerList;

#endif
