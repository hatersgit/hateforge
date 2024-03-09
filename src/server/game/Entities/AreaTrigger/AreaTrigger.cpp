
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "AreaTriggerDataStore.h"
#include "CellImpl.h"
#include "Chat.h"
#include "CreatureAISelector.h"
#include "DBCStores.h"
#include "GridNotifiersImpl.h"
#include "Language.h"
#include "Log.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "PathGenerator.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Spline.h"
#include "Transport.h"
#include "Unit.h"
#include "UpdateData.h"
#include <G3D/AABox.h>
#include <span>

AreaTrigger::AreaTrigger() : WorldObject(false), MovableMapObject(), _spawnId(0), _aurEff(nullptr),
_duration(0), _totalDuration(0), _timeSinceCreated(0), _verticesUpdatePreviousOrientation(std::numeric_limits<float>::infinity()),
_isRemoved(false), _reachedDestination(true), _lastSplineIndex(0), _movementTime(0),
_areaTriggerCreateProperties(nullptr), _areaTriggerTemplate(nullptr)
{
    m_objectType |= TYPEMASK_AREATRIGGER;
    m_objectTypeId = TYPEID_AREATRIGGER;

    m_valuesCount = DYNAMICOBJECT_END;

    m_updateFlag = UPDATEFLAG_STATIONARY_POSITION | UPDATEFLAG_AREATRIGGER;
}

AreaTrigger::~AreaTrigger()
{
}

void AreaTrigger::AddToWorld()
{
    ///- Register the AreaTrigger for guid lookup and for caster
    if (!IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnAreaTriggerCreate(this);

        GetMap()->GetObjectsStore().Insert<AreaTrigger>(GetGUID(), this);
        if (_spawnId)
            GetMap()->GetAreaTriggerBySpawnIdStore().insert(std::make_pair(_spawnId, this));

        WorldObject::AddToWorld();
    }
}

void AreaTrigger::RemoveFromWorld()
{
    ///- Remove the AreaTrigger from the accessor and from all lists of objects in world
    if (IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnAreaTriggerRemove(this);

        _isRemoved = true;

        if (Unit* caster = GetCaster())
            caster->_UnregisterAreaTrigger(this);

        _ai->OnRemove();

        // Handle removal of all units, calling OnUnitExit & deleting auras if needed
        HandleUnitEnterExit({});

        WorldObject::RemoveFromWorld();


        if (_spawnId)
            Acore::Containers::MultimapErasePair(GetMap()->GetAreaTriggerBySpawnIdStore(), _spawnId, this);
        GetMap()->GetObjectsStore().Remove<AreaTrigger>(GetGUID());
    }
}

bool AreaTrigger::Create(uint32 areaTriggerCreatePropertiesId, Unit* caster, Unit* target, SpellInfo const* spellInfo, Position const& pos, int32 duration, SpellCastVisual spellVisual, Spell* spell, AuraEffect const* aurEff)
{
    _targetGuid = target ? target->GetGUID() : ObjectGuid::Empty;
    _aurEff = aurEff;

    m_areaTriggerData = new AreaTriggerDataState();

    SetMap(caster->GetMap());
    Relocate(pos);
    RelocateStationaryPosition(pos);
    if (!IsPositionValid())
    {
        LOG_ERROR("entities.areatrigger", "AreaTrigger (spellMiscId {}) not created. Invalid coordinates (X: {} Y: {})", areaTriggerCreatePropertiesId, GetPositionX(), GetPositionY());
        return false;
    }

    _areaTriggerCreateProperties = sAreaTriggerDataStore->GetAreaTriggerCreateProperties(areaTriggerCreatePropertiesId);
    if (!_areaTriggerCreateProperties)
    {
        LOG_ERROR("entities.areatrigger", "AreaTrigger (spellMiscId {}) not created. Invalid areatrigger miscid ({})", areaTriggerCreatePropertiesId, areaTriggerCreatePropertiesId);
        return false;
    }

    SetZoneScript();

    _areaTriggerTemplate = _areaTriggerCreateProperties->Template;
    Object::_Create(ObjectGuid::Create<HighGuid::AreaTrigger>(GetTemplate() ? GetTemplate()->Id.Id : 0, caster->GetMap()->GenerateLowGuid<HighGuid::AreaTrigger>()));

    if (GetTemplate())
        SetEntry(GetTemplate()->Id.Id);

    SetDuration(duration);
    SetObjectScale(1.0f);

    _shape = GetCreateProperties()->Shape;

    m_areaTriggerData->Caster = caster->GetGUID();
    m_areaTriggerData->CreatingEffectGUID = ObjectGuid::Empty;

    m_areaTriggerData->SpellID = spellInfo->Id;
    m_areaTriggerData->SpellForVisuals = spellInfo->Id;
    m_areaTriggerData->SpellVisual.SpellXSpellVisualID = spellVisual.SpellXSpellVisualID;
    m_areaTriggerData->SpellVisual.ScriptVisualID = spellVisual.ScriptVisualID;
    m_areaTriggerData->TimeToTargetScale = GetCreateProperties()->TimeToTargetScale != 0 ? GetCreateProperties()->TimeToTargetScale : m_areaTriggerData->Duration;
    m_areaTriggerData->BoundsRadius2D = GetCreateProperties()->GetMaxSearchRadius();
    m_areaTriggerData->DecalPropertiesID = GetCreateProperties()->DecalPropertiesId;

    SetScaleCurve(m_areaTriggerData->ExtraScaleCurve, GetCreateProperties()->ExtraScale);
    if (Player const* modOwner = caster->GetSpellModOwner())
    {
        float multiplier = 1.0f;
        int32 flat = 0;
        //SmodOwner->GetSpellModValues(spellInfo, SpellModOp::Radius, spell, *m_areaTriggerData->BoundsRadius2D, &flat, &multiplier);
        if (multiplier != 1.0f)
        {
            AreaTriggerScaleCurveTemplate overrideScale;
            overrideScale.Curve = multiplier;
            SetScaleCurve(m_areaTriggerData->OverrideScaleCurve, overrideScale);
        }
    }

    m_areaTriggerData->VisualAnim.AnimationDataID = GetCreateProperties()->AnimId;
    m_areaTriggerData->VisualAnim.AnimKitID = GetCreateProperties()->AnimKitId;

    if (GetTemplate() && GetTemplate()->HasFlag(AREATRIGGER_FLAG_UNK3))
        m_areaTriggerData->VisualAnim.Field_C = true;

    if (target && GetTemplate() && GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ATTACHED))
        m_movementInfo.transport.guid = target->GetGUID();

    UpdatePositionData();
    SetZoneScript();

    UpdateShape();

    uint32 timeToTarget = GetCreateProperties()->TimeToTarget != 0 ? GetCreateProperties()->TimeToTarget : m_areaTriggerData->Duration;

    if (GetCreateProperties()->OrbitInfo)
    {
        AreaTriggerOrbitInfo orbit = *GetCreateProperties()->OrbitInfo;
        if (target && GetTemplate() && GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ATTACHED))
            orbit.PathTarget = target->GetGUID();
        else
            orbit.Center = pos;

        InitOrbit(orbit, timeToTarget);
    }
    else if (GetCreateProperties()->HasSplines())
    {
        InitSplineOffsets(GetCreateProperties()->SplinePoints, timeToTarget);
    }

    // movement on transport of areatriggers on unit is handled by themself
    TransportBase* transport = m_movementInfo.transport.guid.IsEmpty() ? caster->GetTransport() : nullptr;
    if (transport)
    {
        float x, y, z, o;
        pos.GetPosition(x, y, z, o);
        transport->CalculatePassengerOffset(x, y, z, &o);
        m_movementInfo.transport.pos.Relocate(x, y, z, o);

        // This object must be added to transport before adding to map for the client to properly display it
        //transport->AddPassenger(this);
    }

    AI_Initialize();

    if (HasOrbit())
        Relocate(CalculateOrbitPosition());

    if (!GetMap()->AddATToMap(this))
    {
        // Returning false will cause the object to be deleted - remove from transport
        /*if (transport)
            transport->RemovePassenger(this);*/
        return false;
    }

    caster->_RegisterAreaTrigger(this);

    _ai->OnCreate();

    return true;
}

AreaTrigger* AreaTrigger::CreateAreaTrigger(uint32 areaTriggerCreatePropertiesId, Unit* caster, Unit* target, SpellInfo const* spellInfo, Position const& pos, int32 duration, SpellCastVisual spellVisual, Spell* spell /*= nullptr*/, AuraEffect const* aurEff /*= nullptr*/)
{
    AreaTrigger* at = new AreaTrigger();
    if (!at->Create(areaTriggerCreatePropertiesId, caster, target, spellInfo, pos, duration, spellVisual, spell, aurEff))
    {
        delete at;
        return nullptr;
    }

    return at;
}

ObjectGuid AreaTrigger::CreateNewMovementForceId(Map* map, uint32 areaTriggerId)
{
    return ObjectGuid::Create<HighGuid::AreaTrigger>(areaTriggerId, map->GenerateLowGuid<HighGuid::AreaTrigger>());
}

void AreaTrigger::Update(uint32 diff)
{
    WorldObject::Update(diff);
    _timeSinceCreated += diff;

    if (!IsServerSide())
    {
        // "If" order matter here, Orbit > Attached > Splines
        if (HasOverridePosition())
        {
            UpdateOverridePosition();
        }
        else if (HasOrbit())
        {
            UpdateOrbitPosition(diff);
        }
        else if (GetTemplate() && GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ATTACHED))
        {
            if (Unit* target = GetTarget())
            {
                float orientation = 0.0f;
                if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
                    if (createProperties->FacingCurveId)
                        orientation = sObjectMgr->GetCurveValueAt(createProperties->FacingCurveId, GetProgress());

                if (!GetTemplate() || !GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ABSOLUTE_ORIENTATION))
                    orientation += target->GetOrientation();

                GetMap()->AreaTriggerRelocation(this, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), orientation);
            }
        }
        else if (HasSplines())
        {
            UpdateSplinePosition(diff);
        }
        else
        {
            if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
            {
                if (createProperties->FacingCurveId)
                {
                    float orientation = sObjectMgr->GetCurveValueAt(createProperties->FacingCurveId, GetProgress());
                    if (!GetTemplate() || !GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ABSOLUTE_ORIENTATION))
                        orientation += GetStationaryO();

                    SetOrientation(orientation);
                }
            }

            UpdateShape();
        }
    }

    if (GetDuration() != -1)
    {
        if (GetDuration() > int32(diff))
            _UpdateDuration(_duration - diff);
        else
        {
            Remove(); // expired
            return;
        }
    }

    _ai->OnUpdate(diff);

    UpdateTargetList();
}

void AreaTrigger::Remove()
{
    if (IsInWorld())
    {
        AddObjectToRemoveList(); // calls RemoveFromWorld
    }
}

void AreaTrigger::SetOverrideScaleCurve(float overrideScale)
{
    SetScaleCurve(m_areaTriggerData->OverrideScaleCurve, overrideScale);
}

void AreaTrigger::SetOverrideScaleCurve(std::array<DBCPosition2D, 2> const& points, Optional<uint32> startTimeOffset, CurveInterpolationMode interpolation)
{
    SetScaleCurve(m_areaTriggerData->OverrideScaleCurve, points, startTimeOffset, interpolation);
}

void AreaTrigger::ClearOverrideScaleCurve()
{
    ClearScaleCurve(m_areaTriggerData->OverrideScaleCurve);
}

void AreaTrigger::SetExtraScaleCurve(float extraScale)
{
    SetScaleCurve(m_areaTriggerData->ExtraScaleCurve, extraScale);
}

void AreaTrigger::SetExtraScaleCurve(std::array<DBCPosition2D, 2> const& points, Optional<uint32> startTimeOffset, CurveInterpolationMode interpolation)
{
    SetScaleCurve(m_areaTriggerData->ExtraScaleCurve, points, startTimeOffset, interpolation);
}

void AreaTrigger::ClearExtraScaleCurve()
{
    ClearScaleCurve(m_areaTriggerData->ExtraScaleCurve);
}

void AreaTrigger::SetOverrideMoveCurve(float x, float y, float z)
{
    SetScaleCurve(m_areaTriggerData->OverrideMoveCurveX, x);
    SetScaleCurve(m_areaTriggerData->OverrideMoveCurveY, y);
    SetScaleCurve(m_areaTriggerData->OverrideMoveCurveZ, z);
}

void AreaTrigger::SetOverrideMoveCurve(std::array<DBCPosition2D, 2> const& xCurvePoints, std::array<DBCPosition2D, 2> const& yCurvePoints,
    std::array<DBCPosition2D, 2> const& zCurvePoints, Optional<uint32> startTimeOffset, CurveInterpolationMode interpolation)
{
    SetScaleCurve(m_areaTriggerData->OverrideMoveCurveX, xCurvePoints, startTimeOffset, interpolation);
    SetScaleCurve(m_areaTriggerData->OverrideMoveCurveY, yCurvePoints, startTimeOffset, interpolation);
    SetScaleCurve(m_areaTriggerData->OverrideMoveCurveZ, zCurvePoints, startTimeOffset, interpolation);
}

void AreaTrigger::ClearOverrideMoveCurve()
{
    ClearScaleCurve(m_areaTriggerData->OverrideMoveCurveX);
    ClearScaleCurve(m_areaTriggerData->OverrideMoveCurveY);
    ClearScaleCurve(m_areaTriggerData->OverrideMoveCurveZ);
}

void AreaTrigger::SetDuration(int32 newDuration)
{
    _duration = newDuration;
    _totalDuration = newDuration;

    // negative duration (permanent areatrigger) sent as 0
    m_areaTriggerData->Duration = std::max(newDuration, 0);
}

void AreaTrigger::_UpdateDuration(int32 newDuration)
{
    _duration = newDuration;

    // should be sent in object create packets only
    m_areaTriggerData->Duration = _duration;
}

float AreaTrigger::CalcCurrentScale()
{
    float scale = 1.0f;
    if (m_areaTriggerData->OverrideScaleCurve.OverrideActive)
        scale *= std::max(GetScaleCurveValue(m_areaTriggerData->OverrideScaleCurve, m_areaTriggerData->TimeToTargetScale), 0.000001f);
    else if (AreaTriggerCreateProperties* createProperties = GetCreateProperties())
        if (createProperties->ScaleCurveId)
            scale *= std::max(sObjectMgr->GetCurveValueAt(createProperties->ScaleCurveId, GetScaleCurveProgress(m_areaTriggerData->OverrideScaleCurve, m_areaTriggerData->TimeToTargetScale)), 0.000001f);

    scale *= std::max(GetScaleCurveValue(m_areaTriggerData->ExtraScaleCurve, m_areaTriggerData->TimeToTargetExtraScale), 0.000001f);

    return scale;
}

float AreaTrigger::GetProgress() const
{
    if (_totalDuration <= 0)
        return 1.0f;

    return std::clamp(float(GetTimeSinceCreated()) / float(GetTotalDuration()), 0.0f, 1.0f);
}

float AreaTrigger::GetScaleCurveProgress(ScaleCurve const& scaleCurve, uint32 timeTo) const
{
    if (!timeTo)
        return 0.0f;

    return std::clamp(float(GetTimeSinceCreated() - scaleCurve.StartTimeOffset) / float(timeTo), 0.0f, 1.0f);
}

float AreaTrigger::GetScaleCurveValueAtProgress(ScaleCurve const& scaleCurve, float x) const
{
    ASSERT(&scaleCurve.OverrideActive, "ScaleCurve must be active to evaluate it");

    // unpack ParameterCurve
    if (scaleCurve.ParameterCurve & 1u)
        return std::bit_cast<float>(scaleCurve.ParameterCurve & ~1u);

    std::array<DBCPosition2D, 2> points;
    for (std::size_t i = 0; i < scaleCurve.Points.size(); ++i)
        points[i] = { .X = scaleCurve.Points[i].GetPositionX(), .Y = scaleCurve.Points[i].GetPositionY() };

    CurveInterpolationMode mode = CurveInterpolationMode(scaleCurve.ParameterCurve >> 1 & 0x7);
    std::size_t pointCount = scaleCurve.ParameterCurve >> 24 & 0xFF;

    return pointCount ? sObjectMgr->GetCurveValueAt(mode, std::span(points.begin(), pointCount), x) : 0.0;
}

float AreaTrigger::GetScaleCurveValue(ScaleCurve const& scaleCurve, uint32 timeTo) const
{
    return GetScaleCurveValueAtProgress(scaleCurve, GetScaleCurveProgress(scaleCurve, timeTo));
}

void AreaTrigger::SetScaleCurve(ScaleCurve scaleCurveMutator, float constantValue)
{
    AreaTriggerScaleCurveTemplate curveTemplate;
    curveTemplate.Curve = constantValue;
    SetScaleCurve(scaleCurveMutator, curveTemplate);
}

void AreaTrigger::SetScaleCurve(ScaleCurve scaleCurveMutator, std::array<DBCPosition2D, 2> const& points,
    Optional<uint32> startTimeOffset, CurveInterpolationMode interpolation)
{
    AreaTriggerScaleCurvePointsTemplate curve;
    curve.Mode = interpolation;
    curve.Points = points;

    AreaTriggerScaleCurveTemplate curveTemplate;
    curveTemplate.StartTimeOffset = startTimeOffset.value_or(GetTimeSinceCreated());
    curveTemplate.Curve = curve;

    SetScaleCurve(scaleCurveMutator, curveTemplate);
}

void AreaTrigger::ClearScaleCurve(ScaleCurve scaleCurveMutator)
{
    SetScaleCurve(scaleCurveMutator, {});
}

void AreaTrigger::SetScaleCurve(ScaleCurve scaleCurveMutator, Optional<AreaTriggerScaleCurveTemplate> const& curve)
{
    if (!curve)
    {
        scaleCurveMutator.OverrideActive = false;
        return;
    }

    scaleCurveMutator.OverrideActive = true;
    scaleCurveMutator.StartTimeOffset = curve->StartTimeOffset;

    Position point;
    // ParameterCurve packing information
    // (not_using_points & 1) | ((interpolation_mode & 0x7) << 1) | ((first_point_offset & 0xFFFFF) << 4) | ((point_count & 0xFF) << 24)
    //   if not_using_points is set then the entire field is simply read as a float (ignoring that lowest bit)

    if (float const* simpleFloat = std::get_if<float>(&curve->Curve))
    {
        uint32 packedCurve = std::bit_cast<uint32>(*simpleFloat);
        packedCurve |= 1;

        scaleCurveMutator.ParameterCurve = packedCurve;

        // clear points
        for (std::size_t i = 0; i < scaleCurveMutator.Points.size(); ++i)
            scaleCurveMutator.Points[i] = point;
    }
    else if (AreaTriggerScaleCurvePointsTemplate const* curvePoints = std::get_if<AreaTriggerScaleCurvePointsTemplate>(&curve->Curve))
    {
        CurveInterpolationMode mode = curvePoints->Mode;
        if (curvePoints->Points[1].X < curvePoints->Points[0].X)
            mode = CurveInterpolationMode::Constant;

        switch (mode)
        {
        case CurveInterpolationMode::CatmullRom:
            // catmullrom requires at least 4 points, impossible here
            mode = CurveInterpolationMode::Cosine;
            break;
        case CurveInterpolationMode::Bezier3:
        case CurveInterpolationMode::Bezier4:
        case CurveInterpolationMode::Bezier:
            // bezier requires more than 2 points, impossible here
            mode = CurveInterpolationMode::Linear;
            break;
        default:
            break;
        }

        uint32 pointCount = 2;
        if (mode == CurveInterpolationMode::Constant)
            pointCount = 1;

        uint32 packedCurve = (uint32(mode) << 1) | (pointCount << 24);
        scaleCurveMutator.ParameterCurve = packedCurve;

        for (std::size_t i = 0; i < curvePoints->Points.size(); ++i)
        {
            point.Relocate(curvePoints->Points[i].X, curvePoints->Points[i].Y);
            scaleCurveMutator.Points[i] = point;
        }
    }
}

void AreaTrigger::UpdateTargetList()
{
    std::vector<Unit*> targetList;

    switch (_shape.Type)
    {
    case AREATRIGGER_TYPE_SPHERE:
        SearchUnitInSphere(targetList);
        break;
    case AREATRIGGER_TYPE_BOX:
        SearchUnitInBox(targetList);
        break;
    case AREATRIGGER_TYPE_POLYGON:
        SearchUnitInPolygon(targetList);
        break;
    case AREATRIGGER_TYPE_CYLINDER:
        SearchUnitInCylinder(targetList);
        break;
    case AREATRIGGER_TYPE_DISK:
        SearchUnitInDisk(targetList);
        break;
    case AREATRIGGER_TYPE_BOUNDED_PLANE:
        SearchUnitInBoundedPlane(targetList);
        break;
    default:
        break;
    }

    //if (GetTemplate())
    //{
    //    if (ConditionContainer const* conditions = sConditionMgr->GetConditionsForAreaTrigger(GetTemplate()->Id.Id, GetTemplate()->Id.IsServerSide))
    //    {
    //        Acore::Containers::EraseIf(targetList, [conditions](Unit const* target)
    //            {
    //                return !sConditionMgr->IsObjectMeetToConditions(target, *conditions);
    //            });
    //    }
    //}

    HandleUnitEnterExit(targetList);
}

void AreaTrigger::SearchUnits(std::vector<Unit*>& targetList, float radius, bool check3D)
{
    Acore::AnyUnitInObjectRangeCheck check(this, radius);
    if (IsServerSide())
    {
        Acore::PlayerListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(this, targetList, check);
        Cell::VisitWorldObjects(this, searcher, GetMaxSearchRadius());
    }
    else
    {
        Acore::UnitListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(this, targetList, check);
        Cell::VisitAllObjects(this, searcher, GetMaxSearchRadius());
    }
}

void AreaTrigger::SearchUnitInSphere(std::vector<Unit*>& targetList)
{
    float progress = GetProgress();
    if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
        if (createProperties->MorphCurveId)
            progress = sObjectMgr->GetCurveValueAt(createProperties->MorphCurveId, progress);

    float scale = CalcCurrentScale();
    float radius = G3D::lerp(_shape.SphereDatas.Radius, _shape.SphereDatas.RadiusTarget, progress) * scale;

    SearchUnits(targetList, radius, true);
}

void AreaTrigger::SearchUnitInBox(std::vector<Unit*>& targetList)
{
    float progress = GetProgress();
    if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
        if (createProperties->MorphCurveId)
            progress = sObjectMgr->GetCurveValueAt(createProperties->MorphCurveId, progress);

    float scale = CalcCurrentScale();
    float extentsX = G3D::lerp(_shape.BoxDatas.Extents[0], _shape.BoxDatas.ExtentsTarget[0], progress) * scale;
    float extentsY = G3D::lerp(_shape.BoxDatas.Extents[1], _shape.BoxDatas.ExtentsTarget[1], progress) * scale;
    float extentsZ = G3D::lerp(_shape.BoxDatas.Extents[2], _shape.BoxDatas.ExtentsTarget[2], progress) * scale;
    float radius = std::sqrt(extentsX * extentsX + extentsY * extentsY);

    SearchUnits(targetList, radius, false);

    Position const& boxCenter = GetPosition();
    Acore::Containers::EraseIf(targetList, [boxCenter, extentsX, extentsY, extentsZ](Unit const* unit) -> bool
        {
            return !unit->IsWithinBox(boxCenter, extentsX, extentsY, extentsZ / 2);
        });
}

void AreaTrigger::SearchUnitInPolygon(std::vector<Unit*>& targetList)
{
    float progress = GetProgress();
    if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
        if (createProperties->MorphCurveId)
            progress = sObjectMgr->GetCurveValueAt(createProperties->MorphCurveId, progress);

    float height = G3D::lerp(_shape.PolygonDatas.Height, _shape.PolygonDatas.HeightTarget, progress);
    float minZ = GetPositionZ() - height;
    float maxZ = GetPositionZ() + height;

    SearchUnits(targetList, GetMaxSearchRadius(), false);

    Acore::Containers::EraseIf(targetList, [this, minZ, maxZ](Unit const* unit) -> bool
        {
            return unit->GetPositionZ() < minZ
                || unit->GetPositionZ() > maxZ
                || !CheckIsInPolygon2D(unit);
        });
}

void AreaTrigger::SearchUnitInCylinder(std::vector<Unit*>& targetList)
{
    float progress = GetProgress();
    if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
        if (createProperties->MorphCurveId)
            progress = sObjectMgr->GetCurveValueAt(createProperties->MorphCurveId, progress);

    float scale = CalcCurrentScale();
    float radius = G3D::lerp(_shape.CylinderDatas.Radius, _shape.CylinderDatas.RadiusTarget, progress) * scale;
    float height = G3D::lerp(_shape.CylinderDatas.Height, _shape.CylinderDatas.HeightTarget, progress);
    if (!m_areaTriggerData->HeightIgnoresScale)
        height *= scale;

    float minZ = GetPositionZ() - height;
    float maxZ = GetPositionZ() + height;

    SearchUnits(targetList, radius, false);

    Acore::Containers::EraseIf(targetList, [minZ, maxZ](Unit const* unit) -> bool
        {
            return unit->GetPositionZ() < minZ
                || unit->GetPositionZ() > maxZ;
        });
}

void AreaTrigger::SearchUnitInDisk(std::vector<Unit*>& targetList)
{
    float progress = GetProgress();
    if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
        if (createProperties->MorphCurveId)
            progress = sObjectMgr->GetCurveValueAt(createProperties->MorphCurveId, progress);

    float scale = CalcCurrentScale();
    float innerRadius = G3D::lerp(_shape.DiskDatas.InnerRadius, _shape.DiskDatas.InnerRadiusTarget, progress) * scale;
    float outerRadius = G3D::lerp(_shape.DiskDatas.OuterRadius, _shape.DiskDatas.OuterRadiusTarget, progress) * scale;
    float height = G3D::lerp(_shape.DiskDatas.Height, _shape.DiskDatas.HeightTarget, progress);
    if (!m_areaTriggerData->HeightIgnoresScale)
        height *= scale;

    float minZ = GetPositionZ() - height;
    float maxZ = GetPositionZ() + height;

    SearchUnits(targetList, outerRadius, false);

    Acore::Containers::EraseIf(targetList, [this, innerRadius, minZ, maxZ](Unit const* unit) -> bool
        {
            return unit->IsInDist2d(this, innerRadius) || unit->GetPositionZ() < minZ || unit->GetPositionZ() > maxZ;
        });
}

void AreaTrigger::SearchUnitInBoundedPlane(std::vector<Unit*>& targetList)
{
    float progress = GetProgress();
    if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
        if (createProperties->MorphCurveId)
            progress = sObjectMgr->GetCurveValueAt(createProperties->MorphCurveId, progress);

    float scale = CalcCurrentScale();
    float extentsX = G3D::lerp(_shape.BoundedPlaneDatas.Extents[0], _shape.BoundedPlaneDatas.ExtentsTarget[0], progress) * scale;
    float extentsY = G3D::lerp(_shape.BoundedPlaneDatas.Extents[1], _shape.BoundedPlaneDatas.ExtentsTarget[1], progress) * scale;
    float radius = std::sqrt(extentsX * extentsX + extentsY * extentsY);

    SearchUnits(targetList, radius, false);

    Position const& boxCenter = GetPosition();
    Acore::Containers::EraseIf(targetList, [boxCenter, extentsX, extentsY](Unit const* unit) -> bool
        {
            return !unit->IsWithinBox(boxCenter, extentsX, extentsY, MAP_SIZE);
        });
}

void AreaTrigger::HandleUnitEnterExit(std::vector<Unit*> const& newTargetList)
{
    GuidUnorderedSet exitUnits(std::move(_insideUnits));

    std::vector<Unit*> enteringUnits;

    for (Unit* unit : newTargetList)
    {
        if (exitUnits.erase(unit->GetGUID()) == 0) // erase(key_type) returns number of elements erased
            enteringUnits.push_back(unit);

        _insideUnits.insert(unit->GetGUID());
    }

    // Handle after _insideUnits have been reinserted so we can use GetInsideUnits() in hooks
    for (Unit* unit : enteringUnits)
    {
        if (Player* player = unit->ToPlayer())
        {
            if (player->isDebugAreaTriggers)
                ChatHandler(player->GetSession()).PSendSysMessage("Entered");

            //player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_AREA_TRIGGER_ENTER, GetEntry(), 1);
        }

        DoActions(unit);

        _ai->OnUnitEnter(unit);
    }

    for (ObjectGuid const& exitUnitGuid : exitUnits)
    {
        if (Unit* leavingUnit = ObjectAccessor::GetUnit(*this, exitUnitGuid))
        {
            if (Player* player = leavingUnit->ToPlayer())
            {
                if (player->isDebugAreaTriggers)
                    ChatHandler(player->GetSession()).PSendSysMessage("Exited");

                //player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_AREA_TRIGGER_EXIT, GetEntry(), 1);
            }

            UndoActions(leavingUnit);

            _ai->OnUnitExit(leavingUnit);
        }
    }

    m_areaTriggerData->NumUnitsInside = _insideUnits.size();
    m_areaTriggerData->NumPlayersInside =
        std::count_if(_insideUnits.begin(), _insideUnits.end(), [](ObjectGuid const& guid) { return guid.IsPlayer(); });
}

AreaTriggerTemplate const* AreaTrigger::GetTemplate() const
{
    return _areaTriggerTemplate;
}

uint32 AreaTrigger::GetScriptId()
{
    auto scriptId = 0;
    if (AreaTriggerCreateProperties* createProperties = GetCreateProperties()) {
        scriptId = createProperties->ScriptId;
        if (!scriptId) {
            for (auto& [scriptID, script] : ScriptRegistry<AreaTriggerEntityScript>::ScriptPointerList) {
                if (script->GetName() == createProperties->scriptName)
                    scriptId = scriptID;
            }
            createProperties->SetScriptId(scriptId);
        }
    }

    return scriptId;
}

Unit* AreaTrigger::GetCaster() const
{
    return ObjectAccessor::GetUnit(*this, GetCasterGuid());
}

Unit* AreaTrigger::GetTarget() const
{
    return ObjectAccessor::GetUnit(*this, _targetGuid);
}

uint32 AreaTrigger::GetFaction() const
{
    if (Unit const* caster = GetCaster())
        return caster->GetFaction();

    return 0;
}

float AreaTrigger::GetMaxSearchRadius() 
{
    return m_areaTriggerData->BoundsRadius2D * CalcCurrentScale();
}

void AreaTrigger::UpdatePolygonVertices()
{
    AreaTriggerCreateProperties const* createProperties = GetCreateProperties();
    float newOrientation = GetOrientation();

    // No need to recalculate, orientation didn't change
    if (G3D::fuzzyEq(_verticesUpdatePreviousOrientation, newOrientation) && (!createProperties || createProperties->PolygonVerticesTarget.empty()))
        return;

    _polygonVertices.assign(createProperties->PolygonVertices.begin(), createProperties->PolygonVertices.end());
    if (!createProperties->PolygonVerticesTarget.empty())
    {
        float progress = GetProgress();
        if (createProperties->MorphCurveId)
            progress = sObjectMgr->GetCurveValueAt(createProperties->MorphCurveId, progress);

        for (std::size_t i = 0; i < _polygonVertices.size(); ++i)
        {
            Position& vertex = _polygonVertices[i];
            Position const& vertexTarget = createProperties->PolygonVerticesTarget[i];

            vertex.m_positionX = G3D::lerp(vertex.GetPositionX(), vertexTarget.GetPositionX(), progress);
            vertex.m_positionY = G3D::lerp(vertex.GetPositionY(), vertexTarget.GetPositionY(), progress);
        }
    }

    float angleSin = std::sin(newOrientation);
    float angleCos = std::cos(newOrientation);

    // This is needed to rotate the vertices, following orientation
    for (Position& vertice : _polygonVertices)
    {
        float x = vertice.GetPositionX() * angleCos - vertice.GetPositionY() * angleSin;
        float y = vertice.GetPositionY() * angleCos + vertice.GetPositionX() * angleSin;
        vertice.Relocate(x, y);
    }

    _verticesUpdatePreviousOrientation = newOrientation;
}

bool AreaTrigger::CheckIsInPolygon2D(Position const* pos) const
{
    float testX = pos->GetPositionX();
    float testY = pos->GetPositionY();

    //this method uses the ray tracing algorithm to determine if the point is in the polygon
    bool locatedInPolygon = false;

    for (std::size_t vertex = 0; vertex < _polygonVertices.size(); ++vertex)
    {
        std::size_t nextVertex;

        //repeat loop for all sets of points
        if (vertex == (_polygonVertices.size() - 1))
        {
            //if i is the last vertex, let j be the first vertex
            nextVertex = 0;
        }
        else
        {
            //for all-else, let j=(i+1)th vertex
            nextVertex = vertex + 1;
        }

        float vertX_i = GetPositionX() + _polygonVertices[vertex].GetPositionX();
        float vertY_i = GetPositionY() + _polygonVertices[vertex].GetPositionY();
        float vertX_j = GetPositionX() + _polygonVertices[nextVertex].GetPositionX();
        float vertY_j = GetPositionY() + _polygonVertices[nextVertex].GetPositionY();

        // following statement checks if testPoint.Y is below Y-coord of i-th vertex
        bool belowLowY = vertY_i > testY;
        // following statement checks if testPoint.Y is below Y-coord of i+1-th vertex
        bool belowHighY = vertY_j > testY;

        /* following statement is true if testPoint.Y satisfies either (only one is possible)
        -->(i).Y < testPoint.Y < (i+1).Y        OR
        -->(i).Y > testPoint.Y > (i+1).Y

        (Note)
        Both of the conditions indicate that a point is located within the edges of the Y-th coordinate
        of the (i)-th and the (i+1)- th vertices of the polygon. If neither of the above
        conditions is satisfied, then it is assured that a semi-infinite horizontal line draw
        to the right from the testpoint will NOT cross the line that connects vertices i and i+1
        of the polygon
        */
        bool withinYsEdges = belowLowY != belowHighY;

        if (withinYsEdges)
        {
            // this is the slope of the line that connects vertices i and i+1 of the polygon
            float slopeOfLine = (vertX_j - vertX_i) / (vertY_j - vertY_i);

            // this looks up the x-coord of a point lying on the above line, given its y-coord
            float pointOnLine = (slopeOfLine * (testY - vertY_i)) + vertX_i;

            //checks to see if x-coord of testPoint is smaller than the point on the line with the same y-coord
            bool isLeftToLine = testX < pointOnLine;

            if (isLeftToLine)
            {
                //this statement changes true to false (and vice-versa)
                locatedInPolygon = !locatedInPolygon;
            }//end if (isLeftToLine)
        }//end if (withinYsEdges
    }

    return locatedInPolygon;
}

bool AreaTrigger::HasOverridePosition() const
{
    return m_areaTriggerData->OverrideMoveCurveX.OverrideActive
        && m_areaTriggerData->OverrideMoveCurveY.OverrideActive
        && m_areaTriggerData->OverrideMoveCurveZ.OverrideActive;
}

void AreaTrigger::UpdateShape()
{
    if (_shape.IsPolygon())
        UpdatePolygonVertices();
}

bool UnitFitToActionRequirement(Unit* unit, Unit* caster, AreaTriggerAction const& action)
{
    switch (action.TargetType)
    {
    case AREATRIGGER_ACTION_USER_FRIEND:
    {
        return caster->IsValidAssistTarget(unit);
    }
    case AREATRIGGER_ACTION_USER_ENEMY:
    {
        return caster->IsValidAttackTarget(unit, sSpellMgr->GetSpellInfo(action.Param));
    }
    case AREATRIGGER_ACTION_USER_RAID:
    {
        return caster->IsInRaidWith(unit);
    }
    case AREATRIGGER_ACTION_USER_PARTY:
    {
        return caster->IsInPartyWith(unit);
    }
    case AREATRIGGER_ACTION_USER_CASTER:
    {
        return unit->GetGUID() == caster->GetGUID();
    }
    case AREATRIGGER_ACTION_USER_ANY:
    default:
        break;
    }

    return true;
}

void AreaTrigger::DoActions(Unit* unit)
{
    Unit* caster = IsServerSide() ? unit : GetCaster();

    if (caster && GetTemplate())
    {
        for (AreaTriggerAction const& action : GetTemplate()->Actions)
        {
            if (IsServerSide() || UnitFitToActionRequirement(unit, caster, action))
            {
                switch (action.ActionType)
                {
                case AREATRIGGER_ACTION_CAST:
                    caster->CastSpell(unit, action.Param, TRIGGERED_FULL_MASK);
                    break;
                case AREATRIGGER_ACTION_ADDAURA:
                    caster->AddAura(action.Param, unit);
                    break;
                case AREATRIGGER_ACTION_TELEPORT:
                {
                    //if (WorldSafeLocsEntry const* safeLoc = sObjectMgr->GetWorldSafeLoc(action.Param))
                    //{
                    //    if (Player* player = caster->ToPlayer())
                    //    {
                    //        if (player->GetMapId() != safeLoc->Loc.GetMapId())
                    //        {
                    //            if (WorldSafeLocsEntry const* instanceEntrance = player->GetInstanceEntrance(safeLoc->Loc.GetMapId()))
                    //                safeLoc = instanceEntrance;
                    //        }
                    //        player->TeleportTo(safeLoc->Loc);
                    //    }
                    //}
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}

void AreaTrigger::UndoActions(Unit* unit)
{
    if (GetTemplate())
        for (AreaTriggerAction const& action : GetTemplate()->Actions)
            if (action.ActionType == AREATRIGGER_ACTION_CAST || action.ActionType == AREATRIGGER_ACTION_ADDAURA)
                unit->RemoveAurasDueToSpell(action.Param, GetCasterGuid());
}

void AreaTrigger::InitSplineOffsets(std::vector<Position> const& offsets, uint32 timeToTarget)
{
    float angleSin = std::sin(GetOrientation());
    float angleCos = std::cos(GetOrientation());

    // This is needed to rotate the spline, following caster orientation
    std::vector<G3D::Vector3> rotatedPoints;
    rotatedPoints.reserve(offsets.size());
    for (Position const& offset : offsets)
    {
        float x = GetPositionX() + (offset.GetPositionX() * angleCos - offset.GetPositionY() * angleSin);
        float y = GetPositionY() + (offset.GetPositionY() * angleCos + offset.GetPositionX() * angleSin);
        float z = GetPositionZ();

        UpdateAllowedPositionZ(x, y, z);
        z += offset.GetPositionZ();

        rotatedPoints.emplace_back(x, y, z);
    }

    InitSplines(std::move(rotatedPoints), timeToTarget);
}

void AreaTrigger::InitSplines(std::vector<G3D::Vector3> splinePoints, uint32 timeToTarget)
{
    if (splinePoints.size() < 2)
        return;

    _movementTime = 0;

    _spline = std::make_unique<::Movement::Spline<int32>>();
    _spline->init_spline(&splinePoints[0], splinePoints.size(), ::Movement::SplineBase::ModeLinear);
    _spline->initLengths();

    // should be sent in object create packets only
    m_areaTriggerData->TimeToTarget = timeToTarget;

    _reachedDestination = false;
}

bool AreaTrigger::HasSplines() const
{
    return bool(_spline);
}

void AreaTrigger::InitOrbit(AreaTriggerOrbitInfo const& orbit, uint32 timeToTarget)
{
    // Circular movement requires either a center position or an attached unit
    ASSERT(orbit.Center.has_value() || orbit.PathTarget.has_value());

    // should be sent in object create packets only
    m_areaTriggerData->TimeToTarget = timeToTarget;

    m_areaTriggerData->OrbitPathTarget = orbit.PathTarget.value_or(ObjectGuid::Empty);

    _orbitInfo = orbit;

    _orbitInfo->TimeToTarget = timeToTarget;
    _orbitInfo->ElapsedTimeForMovement = 0;
}

bool AreaTrigger::HasOrbit() const
{
    return _orbitInfo.has_value();
}

Position* AreaTrigger::GetOrbitCenterPosition() const
{
    if (!_orbitInfo)
        return nullptr;

    if (_orbitInfo->PathTarget)
        if (WorldObject* center = ObjectAccessor::GetWorldObject(*this, *_orbitInfo->PathTarget))
            return center;

    if (_orbitInfo->Center) {
        auto pos = _orbitInfo->Center->GetPosition();
        return &pos;
    }

    return nullptr;
}

Position AreaTrigger::CalculateOrbitPosition()
{
    Position const* centerPos = GetOrbitCenterPosition();
    if (!centerPos)
        return GetPosition();

    AreaTriggerCreateProperties* createProperties = GetCreateProperties();
    AreaTriggerOrbitInfo const& cmi = *_orbitInfo;

    // AreaTrigger make exactly "Duration / TimeToTarget" loops during his life time
    float pathProgress = float(cmi.ElapsedTimeForMovement) / float(cmi.TimeToTarget);
    if (createProperties && createProperties->MoveCurveId)
        pathProgress = sObjectMgr->GetCurveValueAt(createProperties->MoveCurveId, pathProgress);

    // We already made one circle and can't loop
    if (!cmi.CanLoop)
        pathProgress = std::min(1.f, pathProgress);

    float radius = cmi.Radius;
    if (G3D::fuzzyNe(cmi.BlendFromRadius, radius))
    {
        float blendCurve = (cmi.BlendFromRadius - radius) / radius;
        // 4.f Defines four quarters
        blendCurve = RoundToInterval(blendCurve, 1.f, 4.f) / 4.f;
        float blendProgress = std::min(1.f, pathProgress / blendCurve);
        radius = G3D::lerp(cmi.BlendFromRadius, cmi.Radius, blendProgress);
    }

    // Adapt Path progress depending of circle direction
    if (!cmi.CounterClockwise)
        pathProgress *= -1;

    float angle = cmi.InitialAngle + 2.f * float(M_PI) * pathProgress;
    float x = centerPos->GetPositionX() + (radius * std::cos(angle));
    float y = centerPos->GetPositionY() + (radius * std::sin(angle));
    float z = centerPos->GetPositionZ() + cmi.ZOffset;

    float orientation = 0.0f;
    if (createProperties && createProperties->FacingCurveId)
        orientation = sObjectMgr->GetCurveValueAt(createProperties->FacingCurveId, GetProgress());

    if (!GetTemplate() || !GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ABSOLUTE_ORIENTATION))
    {
        orientation += angle;
        orientation += cmi.CounterClockwise ? float(M_PI_4) : -float(M_PI_4);
    }

    return { x, y, z, orientation };
}

void AreaTrigger::UpdateOrbitPosition(uint32 /*diff*/)
{
    if (_orbitInfo->StartDelay > GetElapsedTimeForMovement())
        return;

    _orbitInfo->ElapsedTimeForMovement = GetElapsedTimeForMovement() - _orbitInfo->StartDelay;

    Position pos = CalculateOrbitPosition();

    GetMap()->AreaTriggerRelocation(this, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
#ifdef ACORE_DEBUG
    DebugVisualizePosition();
#endif
}

void AreaTrigger::UpdateSplinePosition(uint32 diff)
{
    if (_reachedDestination)
        return;

    _movementTime += diff;

    if (_movementTime >= GetTimeToTarget())
    {
        _reachedDestination = true;
        _lastSplineIndex = int32(_spline->last());

        G3D::Vector3 lastSplinePosition = _spline->getPoint(_lastSplineIndex);
        GetMap()->AreaTriggerRelocation(this, lastSplinePosition.x, lastSplinePosition.y, lastSplinePosition.z, GetOrientation());
#ifdef ACORE_DEBUG
        DebugVisualizePosition();
#endif

        _ai->OnSplineIndexReached(_lastSplineIndex);
        _ai->OnDestinationReached();
        return;
    }

    float currentTimePercent = float(_movementTime) / float(GetTimeToTarget());

    if (currentTimePercent <= 0.f)
        return;

    AreaTriggerCreateProperties const* createProperties = GetCreateProperties();
    if (createProperties && createProperties->MoveCurveId)
    {
        float progress = sObjectMgr->GetCurveValueAt(createProperties->MoveCurveId, currentTimePercent);
        if (progress < 0.f || progress > 1.f)
        {
            LOG_ERROR("entities.areatrigger", "AreaTrigger (Id: {}, AreaTriggerCreatePropertiesId: {}) has wrong progress ({}) caused by curve calculation (MoveCurveId: {})",
                GetEntry(), createProperties->Id, progress, createProperties->MoveCurveId);
    }
        else
            currentTimePercent = progress;
}

    int lastPositionIndex = 0;
    float percentFromLastPoint = 0;
    _spline->computeIndex(currentTimePercent, lastPositionIndex, percentFromLastPoint);

    G3D::Vector3 currentPosition;
    _spline->evaluate_percent(lastPositionIndex, percentFromLastPoint, currentPosition);

    float orientation = GetStationaryO();
    if (createProperties && createProperties->FacingCurveId)
        orientation += sObjectMgr->GetCurveValueAt(createProperties->FacingCurveId, GetProgress());

    if (GetTemplate() && !GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ABSOLUTE_ORIENTATION) && GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_FACE_MOVEMENT_DIR))
    {
        G3D::Vector3 derivative;
        _spline->evaluate_derivative(lastPositionIndex, percentFromLastPoint, derivative);
        if (derivative.x != 0.0f || derivative.y != 0.0f)
            orientation += std::atan2(derivative.y, derivative.x);
    }

    GetMap()->AreaTriggerRelocation(this, currentPosition.x, currentPosition.y, currentPosition.z, orientation);
#ifdef ACORE_DEBUG
    DebugVisualizePosition();
#endif

    if (_lastSplineIndex != lastPositionIndex)
    {
        _lastSplineIndex = lastPositionIndex;
        _ai->OnSplineIndexReached(_lastSplineIndex);
    }
}

void AreaTrigger::UpdateOverridePosition()
{
    float progress = GetScaleCurveProgress(m_areaTriggerData->OverrideMoveCurveX, m_areaTriggerData->TimeToTargetPos);

    float x = GetScaleCurveValueAtProgress(m_areaTriggerData->OverrideMoveCurveX, progress);
    float y = GetScaleCurveValueAtProgress(m_areaTriggerData->OverrideMoveCurveY, progress);
    float z = GetScaleCurveValueAtProgress(m_areaTriggerData->OverrideMoveCurveZ, progress);
    float orientation = GetOrientation();

    if (AreaTriggerCreateProperties const* createProperties = GetCreateProperties())
    {
        if (createProperties->FacingCurveId)
        {
            orientation = sObjectMgr->GetCurveValueAt(createProperties->FacingCurveId, GetProgress());
            if (!GetTemplate() || !GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ABSOLUTE_ORIENTATION))
                orientation += GetStationaryO();
        }
    }

    GetMap()->AreaTriggerRelocation(this, x, y, z, orientation);
}

void AreaTrigger::DebugVisualizePosition()
{
    if (Unit* caster = GetCaster())
        if (Player* player = caster->ToPlayer())
            if (player->isDebugAreaTriggers)
                player->SummonCreature((uint32)1, this->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, GetTimeToTarget());
}

void AreaTrigger::AI_Initialize()
{
    if (_ai)
        delete _ai;

    _ai = FactorySelector::SelectAreaTriggerAI(this);

    _ai->OnInitialize();
}

void AreaTrigger::AI_Destroy()
{
    delete _ai;
}
