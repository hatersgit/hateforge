
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "AreaTriggerDataStore.h"
#include "CellImpl.h"
#include "Chat.h"
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

AreaTrigger::AreaTrigger() : WorldObject(false), MovableMapObject(), _aurEff(nullptr),
_duration(0), _totalDuration(0), _timeSinceCreated(0), _periodicProcTimer(0), _basePeriodicProcTimer(0),
_previousCheckOrientation(std::numeric_limits<float>::infinity()),
_isBeingRemoved(false), _isRemoved(false), _reachedDestination(false), _lastSplineIndex(0), _movementTime(0),
_areaTriggerTemplate(nullptr), _areaTriggerMiscTemplate(nullptr), _spawnId(0), _guidScriptId(0), _ai(), m_areaTriggerData(new AreaTriggerDataState())
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
        GetMap()->GetObjectsStore().Insert<AreaTrigger>(GetGUID(), this);
        WorldObject::AddToWorld();
    }
}

void AreaTrigger::RemoveFromWorld()
{
    ///- Remove the AreaTrigger from the accessor and from all lists of objects in world
    if (IsInWorld())
    {
        _isRemoved = true;

        if (Unit* caster = GetCaster())
            caster->_UnregisterAreaTrigger(this);

        // Handle removal of all units, calling OnUnitExit & deleting auras if needed
        HandleUnitEnterExit({});

        _ai->OnRemove();

        WorldObject::RemoveFromWorld();
        GetMap()->GetObjectsStore().Remove<AreaTrigger>(GetGUID());
    }
}

bool AreaTrigger::Create(uint32 spellMiscId, Unit* caster, Unit* target, SpellInfo const* spell, Position const& pos, int32 duration, std::array<uint32, 2> spellVisual, ObjectGuid const& castId, AuraEffect const* aurEff, AreaTriggerOrbitInfo* customOrbitInfo)
{
    _targetGuid = target ? target->GetGUID() : ObjectGuid::Empty;
    _aurEff = aurEff;

    SetMap(caster->GetMap());
    Relocate(pos);
    if (!IsPositionValid())
    {
        LOG_ERROR("entities.areatrigger", "AreaTrigger (spellMiscId %u) not created. Invalid coordinates (X: %f Y: %f)", spellMiscId, GetPositionX(), GetPositionY());
        return false;
    }

    _areaTriggerMiscTemplate = sAreaTriggerDataStore->GetAreaTriggerMiscTemplate(spellMiscId);
    if (!_areaTriggerMiscTemplate)
    {
        LOG_ERROR("entities.areatrigger", "AreaTrigger (spellMiscId %u) not created. Invalid areatrigger miscid (%u)", spellMiscId, spellMiscId);
        return false;
    }

    _areaTriggerTemplate = _areaTriggerMiscTemplate->Template;
    auto templateId = GetTemplate() ? GetTemplate()->Id : 0;
    Object::_Create(ObjectGuid::Create<HighGuid::AreaTrigger>(templateId, caster->GetMap()->GenerateLowGuid<HighGuid::AreaTrigger>()));

    UpdatePositionData();

    SetEntry(GetTemplate()->Id);
    SetDuration(duration);
    SetObjectScale(1.0f);

    m_areaTriggerData->Caster = caster->GetGUID();
    m_areaTriggerData->CreatingEffectGUID = castId;

    m_areaTriggerData->SpellID = spell->Id;
    m_areaTriggerData->SpellForVisuals = spell->Id;
    m_areaTriggerData->SpellVisual.SpellXSpellVisualID = spellVisual[0];
    m_areaTriggerData->SpellVisual.ScriptVisualID = spellVisual[1];
    m_areaTriggerData->TimeToTargetScale = GetMiscTemplate()->TimeToTargetScale != 0 ? GetMiscTemplate()->TimeToTargetScale : m_areaTriggerData->Duration;
    m_areaTriggerData->BoundsRadius2D = GetTemplate()->MaxSearchRadius;
    m_areaTriggerData->DecalPropertiesID = GetMiscTemplate()->DecalPropertiesId;

    if (GetMiscTemplate()->ExtraScale.Data.Structured.StartTimeOffset)
        m_areaTriggerData->ExtraScaleCurve.StartTimeOffset = GetMiscTemplate()->ExtraScale.Data.Structured.StartTimeOffset;
    if (GetMiscTemplate()->ExtraScale.Data.Structured.Points[0] || GetMiscTemplate()->ExtraScale.Data.Structured.Points[1])
    {
        Position point(GetMiscTemplate()->ExtraScale.Data.Structured.Points[0], GetMiscTemplate()->ExtraScale.Data.Structured.Points[1]);
        m_areaTriggerData->ExtraScaleCurve.Points[0] = point;
    }
    if (GetMiscTemplate()->ExtraScale.Data.Structured.Points[2] || GetMiscTemplate()->ExtraScale.Data.Structured.Points[3])
    {
        Position point(GetMiscTemplate()->ExtraScale.Data.Structured.Points[2], GetMiscTemplate()->ExtraScale.Data.Structured.Points[3]);
        m_areaTriggerData->ExtraScaleCurve.Points[1] = point;
    }
    if (GetMiscTemplate()->ExtraScale.Data.Raw[5])
        m_areaTriggerData->ExtraScaleCurve.ParameterCurve = GetMiscTemplate()->ExtraScale.Data.Raw[5];
    if (GetMiscTemplate()->ExtraScale.Data.Structured.OverrideActive)
        m_areaTriggerData->ExtraScaleCurve.OverrideActive = GetMiscTemplate()->ExtraScale.Data.Structured.OverrideActive;

    if (target && GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ATTACHED))
    {
        m_movementInfo.transport.guid = target->GetGUID();
    }

    UpdateShape();

    uint32 timeToTarget = GetMiscTemplate()->TimeToTarget != 0 ? GetMiscTemplate()->TimeToTarget : m_areaTriggerData->Duration;

    if (GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_CIRCULAR_MOVEMENT))
    {
        AreaTriggerOrbitInfo orbitInfo = customOrbitInfo ? *customOrbitInfo : GetMiscTemplate()->OrbitInfo;
        if (target && GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ATTACHED))
            orbitInfo.PathTarget = target->GetGUID();
        else
            orbitInfo.Center = pos;

        InitOrbit(orbitInfo, timeToTarget);

        if (customOrbitInfo)
            delete customOrbitInfo;
    }
    else if (GetMiscTemplate()->HasSplines())
    {
        InitSplineOffsets(GetMiscTemplate()->SplinePoints, timeToTarget);
    }
    else if (GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_CIRCULAR_MOVEMENT))
    {
        m_areaTriggerData->TimeToTarget = timeToTarget;
    }

    // movement on transport of areatriggers on unit is handled by themself
    Transport* transport = m_movementInfo.transport.guid.IsEmpty() ? caster->GetTransport() : nullptr;
    if (transport)
    {
        float x, y, z, o;
        pos.GetPosition(x, y, z, o);
        transport->CalculatePassengerOffset(x, y, z, &o);
        m_movementInfo.transport.pos.Relocate(x, y, z, o);

        // This object must be added to transport before adding to map for the client to properly display it
        transport->AddPassenger(this);
    }

    AI_Initialize();

    // Relocate areatriggers with circular movement again
    if (HasOrbit())
        Relocate(CalculateOrbitPosition());

    if (IsWorldObject())
        setActive(true);    //must before add to map to be put in world container

    if (!GetMap()->AddATToMap(this))
    {
        // Returning false will cause the object to be deleted - remove from transport
        if (transport)
            transport->RemovePassenger(this);
        return false;
    }

    caster->_RegisterAreaTrigger(this);

    _ai->OnCreate();

    return true;
}

AreaTrigger* AreaTrigger::CreateAreaTrigger(uint32 spellMiscId, Unit* caster, Unit* target, SpellInfo const* spell, Position const& pos, int32 duration, std::array<uint32, 2> spellVisual, ObjectGuid const& castId /*= ObjectGuid::Empty*/, AuraEffect const* aurEff /*= nullptr*/)
{
    AreaTrigger* at = new AreaTrigger();
    if (!at->Create(spellMiscId, caster, target, spell, pos, duration, spellVisual, castId, aurEff))
    {
        delete at;
        return nullptr;
    }

    return at;
}

AreaTrigger* AreaTrigger::CreateAreaTrigger(uint32 spellMiscId, Unit* caster, uint32 spellId, Position const& pos, int32 duration, float radius, float angle, uint32 timeToTarget, bool canLoop, bool counterClockwise)
{
    if (!caster)
        return nullptr;

    SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(spellId);
    if (!spellEntry)
        return nullptr;

    AreaTrigger* at = new AreaTrigger();
    AreaTriggerOrbitInfo* orbitInfo = new AreaTriggerOrbitInfo();
    orbitInfo->CanLoop = canLoop;
    orbitInfo->Center = pos;
    orbitInfo->Radius = radius;
    orbitInfo->CounterClockwise = counterClockwise;
    orbitInfo->InitialAngle = angle;
    orbitInfo->TimeToTarget = timeToTarget;

    if (!at->Create(spellMiscId, caster, caster, spellEntry, pos, duration, spellEntry->SpellVisual, ObjectGuid::Empty, nullptr, orbitInfo))
    {
        delete at;
        delete orbitInfo;
        return nullptr;
    }

    return at;
}

void AreaTrigger::Update(uint32 diff)
{
    if (_isBeingRemoved)
        return;

    WorldObject::Update(diff);
    _timeSinceCreated += diff;

    // "If" order matter here, Orbit > Attached > Splines
    if (HasOrbit())
    {
        UpdateOrbitPosition(diff);
    }
    else if (GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_ATTACHED))
    {
        if (Unit* target = GetTarget())
            GetMap()->AreaTriggerRelocation(this, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
    }
    else
        UpdateSplinePosition(diff);

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

    UpdateTargetList();

    _ai->OnUpdate(diff);

    if (_basePeriodicProcTimer)
    {
        if (_periodicProcTimer <= diff)
        {
            _ai->OnPeriodicProc();
            _periodicProcTimer = _basePeriodicProcTimer;
        }
        else
            _periodicProcTimer -= diff;
    }
}

void AreaTrigger::Remove()
{
    if (IsInWorld())
    {
        _isBeingRemoved = true;
        AddObjectToRemoveList(); // calls RemoveFromWorld
    }
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

float AreaTrigger::GetProgress() const
{
    return GetTimeSinceCreated() < GetTimeToTargetScale() ? float(GetTimeSinceCreated()) / float(GetTimeToTargetScale()) : 1.0f;
}

void AreaTrigger::UpdateTargetList()
{
    std::list<Unit*> targetList;

    switch (GetTemplate()->Type)
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
    default:
        break;
    }

    HandleUnitEnterExit(targetList);
}

void AreaTrigger::SearchUnitInSphere(std::list<Unit*>& targetList)
{
    float radius = GetTemplate()->SphereDatas.Radius;
   /* if (GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_DYNAMIC_SHAPE))
    {
        if (GetMiscTemplate() && GetMiscTemplate()->MorphCurveId)
        {
            radius = G3D::lerp(GetTemplate()->SphereDatas.Radius,
                GetTemplate()->SphereDatas.RadiusTarget,
                sDB2Manager.GetCurveValueAt(GetMiscTemplate()->MorphCurveId, GetProgress()));
        }
    }*/

    Acore::AnyUnitInObjectRangeCheck check(this, radius);
    Acore::UnitListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(this, targetList, check);
    Cell::VisitAllObjects(this, searcher, GetTemplate()->MaxSearchRadius);
}

void AreaTrigger::SearchUnitInBox(std::list<Unit*>& targetList)
{
    float extentsX = GetTemplate()->BoxDatas.Extents[0];
    float extentsY = GetTemplate()->BoxDatas.Extents[1];
    float extentsZ = GetTemplate()->BoxDatas.Extents[2];

    Acore::AnyUnitInObjectRangeCheck check(this, GetTemplate()->MaxSearchRadius);
    Acore::UnitListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(this, targetList, check);
    Cell::VisitAllObjects(this, searcher, GetTemplate()->MaxSearchRadius);

    float halfExtentsX = extentsX / 2.0f;
    float halfExtentsY = extentsY / 2.0f;
    float halfExtentsZ = extentsZ / 2.0f;

    float minX = GetPositionX() - halfExtentsX;
    float maxX = GetPositionX() + halfExtentsX;

    float minY = GetPositionY() - halfExtentsY;
    float maxY = GetPositionY() + halfExtentsY;

    float minZ = GetPositionZ() - halfExtentsZ;
    float maxZ = GetPositionZ() + halfExtentsZ;

    G3D::AABox const box({ minX, minY, minZ }, { maxX, maxY, maxZ });

    targetList.remove_if([&box](Unit* unit) -> bool
        {
            return !box.contains({ unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ() });
        });
}

void AreaTrigger::SearchUnitInPolygon(std::list<Unit*>& targetList)
{
    Acore::AnyUnitInObjectRangeCheck check(this, GetTemplate()->MaxSearchRadius);
    Acore::UnitListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(this, targetList, check);
    Cell::VisitAllObjects(this, searcher, GetTemplate()->MaxSearchRadius);

    float height = GetTemplate()->PolygonDatas.Height;
    float minZ = GetPositionZ() - height;
    float maxZ = GetPositionZ() + height;

    targetList.remove_if([this, minZ, maxZ](Unit* unit) -> bool
        {
            return !CheckIsInPolygon2D(unit)
                || unit->GetPositionZ() < minZ
                || unit->GetPositionZ() > maxZ;
        });
}

void AreaTrigger::SearchUnitInCylinder(std::list<Unit*>& targetList)
{
    Acore::AnyUnitInObjectRangeCheck check(this, GetTemplate()->MaxSearchRadius);
    Acore::UnitListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(this, targetList, check);
    Cell::VisitAllObjects(this, searcher, GetTemplate()->MaxSearchRadius);

    float height = GetTemplate()->CylinderDatas.Height;
    float minZ = GetPositionZ() - height;
    float maxZ = GetPositionZ() + height;

    targetList.remove_if([minZ, maxZ](Unit* unit) -> bool
        {
            return unit->GetPositionZ() < minZ
                || unit->GetPositionZ() > maxZ;
        });
}

void AreaTrigger::HandleUnitEnterExit(std::list<Unit*> const& newTargetList)
{
    GuidUnorderedSet exitUnits = _insideUnits;
    _insideUnits.clear();

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

            // hater: area trigger teleports NYI
            /*if (ObjectGuid::LowType spawnId = GetSpawnId())
                if (AreaTriggerTeleport const* at = sObjectMgr->GetAreaTriggerTeleport(-int64(spawnId)))
                    player->TeleportTo(at);*/
        }

        DoActions(unit);

        _ai->OnUnitEnter(unit);
    }

    for (ObjectGuid const& exitUnitGuid : exitUnits)
    {
        if (Unit* leavingUnit = ObjectAccessor::GetUnit(*this, exitUnitGuid))
        {
            if (Player* player = leavingUnit->ToPlayer())
                if (player->isDebugAreaTriggers)
                    ChatHandler(player->GetSession()).PSendSysMessage("Exited");

            UndoActions(leavingUnit);

            _ai->OnUnitExit(leavingUnit);
        }
    }
}

AreaTriggerTemplate const* AreaTrigger::GetTemplate() const
{
    return _areaTriggerTemplate;
}

uint32 AreaTrigger::GetScriptId() const
{
    if (_guidScriptId != 0)
        return _guidScriptId;

    return GetTemplate()->ScriptId;
}

Unit* AreaTrigger::GetCaster() const
{
    return ObjectAccessor::GetUnit(*this, GetCasterGuid());
}

Unit* AreaTrigger::GetTarget() const
{
    return ObjectAccessor::GetUnit(*this, _targetGuid);
}

Position const& AreaTrigger::GetRollPitchYaw() const
{
    if (GetMiscTemplate())
        return GetMiscTemplate()->RollPitchYaw;

    return GetPosition();
}

Position const& AreaTrigger::GetTargetRollPitchYaw() const
{
    if (GetMiscTemplate())
        return GetMiscTemplate()->TargetRollPitchYaw;

    return GetPosition();
}

void AreaTrigger::UpdatePolygonOrientation()
{
    float newOrientation = GetOrientation();

    // No need to recalculate, orientation didn't change
    if (G3D::fuzzyEq(_previousCheckOrientation, newOrientation))
        return;

    _polygonVertices.assign(GetTemplate()->PolygonVertices.begin(), GetTemplate()->PolygonVertices.end());

    float angleSin = std::sin(newOrientation);
    float angleCos = std::cos(newOrientation);

    // This is needed to rotate the vertices, following orientation
    for (Position& vertice : _polygonVertices)
    {
        float x = vertice.GetPositionX() * angleCos - vertice.GetPositionY() * angleSin;
        float y = vertice.GetPositionY() * angleCos + vertice.GetPositionX() * angleSin;
        vertice.Relocate(x, y);
    }

    _previousCheckOrientation = newOrientation;
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

void AreaTrigger::UpdateShape()
{
    if (GetTemplate()->IsPolygon())
        UpdatePolygonOrientation();
}

bool UnitFitToActionRequirement(Unit* unit, Unit* caster, AreaTriggerAction const& action)
{
    switch (action.TargetType)
    {
    case AREATRIGGER_ACTION_USER_FRIEND:
    {
        return caster->_IsValidAssistTarget(unit, sSpellMgr->GetSpellInfo(action.Param));
    }
    case AREATRIGGER_ACTION_USER_ENEMY:
    {
        return caster->_IsValidAttackTarget(unit, sSpellMgr->GetSpellInfo(action.Param));
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
    if (Unit* caster = GetCaster())
    {
        for (AreaTriggerAction const& action : GetTemplate()->Actions)
        {
            if (UnitFitToActionRequirement(unit, caster, action))
            {
                switch (action.ActionType)
                {
                case AREATRIGGER_ACTION_CAST:
                    caster->CastSpell(unit, action.Param, true);
                    break;
                case AREATRIGGER_ACTION_ADDAURA:
                    caster->AddAura(action.Param, unit);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void AreaTrigger::UndoActions(Unit* unit)
{
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
    _spline->init_spline(&splinePoints[0], splinePoints.size(), ::Movement::SplineBase::ModeCatmullrom);
    _spline->initLengths();

    // should be sent in object create packets only
    m_areaTriggerData->TimeToTarget = timeToTarget;
    _reachedDestination = false;
}

bool AreaTrigger::HasSplines() const
{
    return bool(_spline);
}

void AreaTrigger::UpdateTimeToTarget(uint32 timeToTarget)
{
    if (!HasSplines())
        return;

    float currentTimePercent = GetCurrentTimePercent();
    int lastPositionIndex = 0;
    float percentFromLastPoint = 0;

    _spline->computeIndex(currentTimePercent, lastPositionIndex, percentFromLastPoint);

    std::vector<G3D::Vector3> newPoints;
    newPoints.push_back(G3D::Vector3(GetPositionX(), GetPositionY(), GetPositionZ()));
    newPoints.push_back(newPoints[0]);

    for (int i = lastPositionIndex + 1; i < _spline->getPointCount(); ++i)
        newPoints.push_back(_spline->getPoint(i));

    InitSplines(newPoints, timeToTarget);
}

bool AreaTrigger::SetDestination(Position const& pos, uint32 timeToTarget)
{
    PathGenerator path(GetCaster());
    bool result = path.CalculatePath(GetPosition(), pos, true);

    if (!result || path.GetPathType() & PATHFIND_NOPATH)
        return false;

    InitSplines(path.GetPath(), timeToTarget);
    return true;
}

void AreaTrigger::SetDecalPropertiesID(uint32 decalPropertiesID)
{
    m_areaTriggerData->DecalPropertiesID = decalPropertiesID;
}

void AreaTrigger::SetSpellXSpellVisualId(uint32 spellXSpellVisualId)
{
    m_areaTriggerData->SpellVisual.SpellXSpellVisualID = spellXSpellVisualId;
}

void AreaTrigger::InitOrbit(AreaTriggerOrbitInfo const& cmi, uint32 timeToTarget)
{
    // Circular movement requires either a center position or an attached unit
    ASSERT(cmi.Center.has_value() || cmi.PathTarget.has_value());

    // should be sent in object create packets only
    m_areaTriggerData->TimeToTarget = timeToTarget;

    _orbitInfo = cmi;

    _orbitInfo->TimeToTarget = timeToTarget;
    _orbitInfo->ElapsedTimeForMovement = 0;
}

bool AreaTrigger::HasOrbit() const
{
    return _orbitInfo.has_value();
}

Position AreaTrigger::GetOrbitCenterPosition() const
{
    if (!_orbitInfo.has_value())
        return GetPosition();

    if (_orbitInfo->PathTarget.has_value())
        if (WorldObject* center = ObjectAccessor::GetWorldObject(*this, *_orbitInfo->PathTarget))
            return { center->GetPositionX(), center->GetPositionY(), center->GetPositionZ(), 0.0f};

    if (_orbitInfo->Center.has_value()) {
        Position pos = _orbitInfo->Center->GetPosition();
        return pos;
    }

    return GetPosition();
}

Position AreaTrigger::CalculateOrbitPosition() const
{
    Position centerPos = GetOrbitCenterPosition();

    AreaTriggerOrbitInfo const& cmi = *_orbitInfo;

    // AreaTrigger make exactly "Duration / TimeToTarget" loops during his life time
    float pathProgress = float(cmi.ElapsedTimeForMovement) / float(cmi.TimeToTarget);

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
    float x = centerPos.GetPositionX() + (radius * std::cos(angle));
    float y = centerPos.GetPositionY() + (radius * std::sin(angle));
    float z = centerPos.GetPositionZ() + cmi.ZOffset;

    return { x, y, z, angle };
}

void AreaTrigger::UpdateOrbitPosition(uint32 /*diff*/)
{
    if (_orbitInfo->StartDelay > GetElapsedTimeForMovement())
        return;

    _orbitInfo->ElapsedTimeForMovement = GetElapsedTimeForMovement() - _orbitInfo->StartDelay;

    Position pos = CalculateOrbitPosition();

    //GetMap()->AreaTriggerRelocation(this, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
#ifdef ACORE_DEBUG
    DebugVisualizePosition();
#endif
}

void AreaTrigger::UpdateSplinePosition(uint32 diff)
{
    if (_reachedDestination)
        return;

    if (!HasSplines())
        return;

    _movementTime += diff;

    if (_movementTime >= GetTimeToTarget())
    {
        _reachedDestination = true;
        _lastSplineIndex = int32(_spline->last());

        G3D::Vector3 lastSplinePosition = _spline->getPoint(_lastSplineIndex);
        //GetMap()->AreaTriggerRelocation(this, lastSplinePosition.x, lastSplinePosition.y, lastSplinePosition.z, GetOrientation());
#ifdef ACORE_DEBUG
        DebugVisualizePosition();
#endif

        _ai->OnSplineIndexReached(_lastSplineIndex);
        _ai->OnDestinationReached();
        return;
    }

    float currentTimePercent = GetCurrentTimePercent();

    int lastPositionIndex = 0;
    float percentFromLastPoint = 0;

    if (currentTimePercent > 0.0f)
        _spline->computeIndex(currentTimePercent, lastPositionIndex, percentFromLastPoint);

    G3D::Vector3 currentPosition;
    _spline->evaluate_percent(lastPositionIndex, percentFromLastPoint, currentPosition);

    float orientation = GetOrientation();
    if (GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_FACE_MOVEMENT_DIR))
    {
        G3D::Vector3 const& nextPoint = _spline->getPoint(lastPositionIndex + 1);
        orientation = GetAngle(nextPoint.x, nextPoint.y);
    }

    ///GetMap()->AreaTriggerRelocation(this, currentPosition.x, currentPosition.y, currentPosition.z, orientation);
#ifdef ACORE_DEBUG
    DebugVisualizePosition();
#endif

    if (_lastSplineIndex != lastPositionIndex)
    {
        _lastSplineIndex = lastPositionIndex;
        _ai->OnSplineIndexReached(_lastSplineIndex);
    }
}

float AreaTrigger::GetCurrentTimePercent()
{
    float currentTimePercent = float(_movementTime) / float(GetTimeToTarget());

    if (currentTimePercent <= 0.f)
        return 0.0f;

    //if (GetMiscTemplate() && GetMiscTemplate()->MoveCurveId)
    //{
    //    float progress = sDB2Manager.GetCurveValueAt(GetMiscTemplate()->MoveCurveId, currentTimePercent);
    //    if (progress < 0.f || progress > 1.f)
    //    {
    //        LOG_ERROR("entities.areatrigger", "AreaTrigger (Id: %u, SpellMiscId: %u) has wrong progress (%f) caused by curve calculation (MoveCurveId: %u)",
    //            GetTemplate()->Id, GetMiscTemplate()->MiscId, progress, GetMiscTemplate()->MorphCurveId);
    //    }
    //    else
    //        currentTimePercent = progress;
    //}

    // currentTimePercent must be between 0.f and 1.f
    return std::max(0.f, std::min(currentTimePercent, 1.f));
}

void AreaTrigger::DebugVisualizePosition()
{
    if (Unit* caster = GetCaster())
        if (Player* player = caster->ToPlayer())
            if (player->isDebugAreaTriggers)
                player->SummonCreature(1, *this, TEMPSUMMON_TIMED_DESPAWN, GetTimeToTarget());
}

void AreaTrigger::AI_Initialize()
{
    AI_Destroy();
    AreaTriggerAI* ai = sScriptMgr->GetAreaTriggerAI(this);
    if (!ai)
        ai = new NullAreaTriggerAI(this);

    _ai.reset(ai);
    _ai->OnInitialize();
}

void AreaTrigger::AI_Destroy()
{
    _ai.reset();
}
